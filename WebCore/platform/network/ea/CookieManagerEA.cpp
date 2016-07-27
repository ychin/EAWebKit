/*
Copyright (C) 2009, 2010, 2011, 2012, 2013, 2014 Electronic Arts, Inc.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1.  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
2.  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
3.  Neither the name of Electronic Arts, Inc. ("EA") nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ELECTRONIC ARTS AND ITS CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ELECTRONIC ARTS OR ITS CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/////////////////////////////////////////////////////////////////////////////
// CookieManagerEA.cpp
// Written/Modified by Arpit Baldeva
//
// Implements cookies for EAWebkit as per RFC 2901, 2965 and original
// Netscape cookie specifications.
/////////////////////////////////////////////////////////////////////////////

#include "config.h"
#include "CookieManagerEA.h"

#include "DateEA.h"
#include "URL.h"

#include <EAWebkit/EAWebKitTransport.h>
#include <EAWebKit/EAWebKitFileSystem.h>
#include <internal/include/EAWebKit_p.h>
#include <internal/include/EAWebKitAssert.h>
#include <internal/include/EAWebKitString.h>
#include <internal/include/EAWebKitNewDelete.h>
#include <EASTL/sort.h>
#include <eastl/fixed_vector.h>

#include <EAIO/PathString.h>

#ifdef _MSC_VER
	#define stricmp _stricmp
#else
	#define stricmp strcasecmp
#endif




///////////////////////////////////////////////////////////////////////////////
// Cookie attribute name constants
//
namespace EA
{
    namespace WebKit
    {
		// Updated the version from 1.1 to 1.2 on adding the checksum to the cookie file.
		// Updated the version from 1.2 to 1.3 on adding the httpOnly support
		const char8_t* gEAWebKitCookieFormatVersion = "CookieFormat:1.4"; 

		const char8_t* COOKIE_DELIMITER = "\n";
		
		namespace CookieManagerHelp
        {
            static const char8_t* kSetCookieHdrStr	="Set-Cookie";
            static const char8_t* kSetCookieHdr2Str	="Set-Cookie2";
            static const char8_t* kCookieHdrStr		="Cookie";
            static const char8_t* kDomainStr		="domain";
            static const char8_t* kMaxAgeStr		="Max-Age";
            static const char8_t* kExpiresStr		="expires";
            static const char8_t* kPathStr			="path";
            static const char8_t* kCommentStr		="comment";
            static const char8_t* kCommentURLStr	="commenturl";
            static const char8_t* kVersionStr		="Version";
            static const char8_t* kSecureStr		="secure";
			static const char8_t* kHttpOnlyStr		="HttpOnly";
            static const char8_t* kPortStr			="port";

            char8_t* Strtok(char8_t* pString, const char8_t* pDelimiters, char8_t** pContext)
            {
                // find point on string to resume
                char8_t* s = pString;

                if(!s)
                {
                    s = *pContext;
                    if(!s)
                        return NULL;
                }

                // Compute bit hash based on lower 5 bits of delimiter characters
                const char8_t* d = pDelimiters;
                int32_t        hash = 0;
                uint32_t       delimiterCount = 0;

                while(const char8_t c = *d++)
                {
                    hash |= (0x80000000 >> (c & 31));
                    ++delimiterCount;
                }

                // Skip delimiters
                for(;;)
                {
                    const char8_t c = *s;

                    // If we hit the end of the string, it ends solely with delimiters
                    // and there are no more tokens to get.
                    if(!c)
                    {
                        *pContext = NULL;
                        return NULL;
                    }

                    // Fast rejection against hash set
                    if((hash << (c & 31)) >= 0)
                        break;

                    // brute-force search against delimiter list
                    for(uint32_t i=0; i<delimiterCount; ++i)
                    {
                        if (pDelimiters[i] == c)    // Is it a delimiter? ...
                            goto still_delimiters;  // yes, continue the loop
                    }

                    // Not a token, so exit
                    break;

                    still_delimiters:
                    ++s;
                }

                // Mark beginning of token
                char8_t* const pToken = s;

                // Search for end of token
                while(const char8_t c = *s)
                {
                    // Fast rejection against hash set
                    if((hash << (c & 31)) < 0)
                    {
                        // Brute-force search against delimiter list
                        for(uint32_t i=0; i<delimiterCount; ++i)
                        {
                            if(pDelimiters[i] == c)
                            {
                                // This token ends with a delimiter.
                                *s = 0;                 // null-term substring
                                *pContext = (s + 1);    // restart on next byte
                                return pToken;          // return found token
                            }
                        }
                    }

                    ++s;
                }

                // We found a token but it was at the end of the string, 
                // so we null out the context and return the last token.
                *pContext = NULL;           // no more tokens
                return pToken;              // return found token
            }
			
			static bool isSubdomainOfHost(const FixedString8_128& host, const FixedString8_128& subdomain)
			{
				FixedString8_128 hostLower(host);
				hostLower.make_lower();

				EAW_ASSERT_MSG(host[0] != '.',"Not expecting a leading dot in host name");
				FixedString8_128 subdomainLower(subdomain);
				subdomainLower.make_lower();
				
				if(subdomainLower[0]=='.')
					subdomainLower = subdomainLower.erase(0,1);

				int lenDiff = hostLower.length() - subdomainLower.length();
				if(lenDiff >= -1)
				{
					int index = hostLower.find(subdomainLower);
					if (index == lenDiff)
						return true;
				}

				return false;
			}
            
            bool EndsWithDomain( const FixedString8_128& server, const FixedString8_128& domain )
            {
                //The original code from UTFInternet package talked about implying a leading dot to the server name. I dont see it. Currently this code fails if we
                //type google.com in the browser address and it sends a cookie with domain .google.com. Lets put a leading dot if start is -1
                //and allow these domains.
                int32_t start  = server.length() - domain.length();
                if(start >= 0 )
                {
                    FixedString8_128 serversDomain = server.substr(start,FixedString8_128::npos);
                    if( serversDomain.comparei(domain) == 0)
                    {
                        if( (start == 0) || (domain[0] == '.') || (server[start-1] == '.'))
                        {
                            return true;
                        }
                    }
                }
                else if (start == -1)
                {
                    FixedString8_128 serverWithDot(FixedString8_128::CtorSprintf(),".%s",server.c_str());
                    if( serverWithDot.comparei(domain) == 0)
                        return true;
                }
                return false;
            }

			bool DomainsMatch(const FixedString8_128& domain1, const FixedString8_128& domain2)
			{
				if(domain1.comparei(domain2) == 0)
					return true;

				// Some non-conforming Javascript code may send us domains not prefixed with a "."  but
				// their http servers expects the domain to be referred with a prefixed ".". It is also possible in reverse manner.
				FixedString8_128 domain1WithoutDot(domain1);
				if(domain1WithoutDot[0] == '.')
					domain1WithoutDot.erase(0,1);

				FixedString8_128 domain2WithoutDot(domain2);
				if(domain2WithoutDot[0] == '.')
					domain2WithoutDot.erase(0,1);

				if(domain1WithoutDot.comparei(domain2WithoutDot) == 0)
					return true;

				return false;
			}

            ///////////////////////////////////////////////////////////////////////////////
            // Trim
            //
            // Trim whitespace from the begining and end of a string.  Modifies the
            // original string, and returns NULL if string is NULL or consists only 
            // of whitespace.
            //
            char8_t* Trim( char8_t* str )
            {
                if( NULL == str ) 
                    return NULL;
                size_t len = strlen( str );
                while( len > 0 && isspace( str[--len] ) )
                    str[len] = '\0';
                while( *str != '\0' && isspace( *str ) )
                    *str++ = '\0';
                return '\0' == *str ? NULL : str;
            }


            ///////////////////////////////////////////////////////////////////////////////
            // GetNameValue
            //
            // Parse a name/value pair separated by '=' from pairString, setting the name
            // and value pointers to the results. Whitespace is trimmed from ends of both and
            // if either name or value doesn't exist the pointer is set to NULL. Modifies
            // the original string while parsing.
            //
            void GetNameValue( char8_t* pairString, char8_t** name, char8_t** value )
            {
                *value = NULL;
                char8_t* end = pairString ? strchr( pairString, '=' ) : 0;
                if( 0 != end )
                {
                    *end = 0;
                    *value = Trim( end + 1 );
                }
                *name = Trim( pairString );
            }

            //////////////////////////////////////////////////////////////////////////
            // ValidateDomain
            //
            // Returns true if domain from offset onward is a valid cookie domain suffix.
            // A valid cookie domain suffix contains at least one '.' that is imbedded
            // e.g. 'a.b' but not '.aa' or '.a.'.  We don't check for '..a' as those
            // host names won't resolve and therefore aren't a liability.
            //
            static bool ValidateDomain( const FixedString8_128& domain, FixedString8_128::size_type offset )
            {
                FixedString8_128::size_type len = domain.length();
                if( offset < FixedString8_128::npos && len > (offset + 1) )
                {
                    FixedString8_128::size_type dot = domain.find_first_of( '.', offset + 1 );
                    if( FixedString8_128::npos != dot && dot < (len-1) )
                        return true;
                }
                return false;

            }
            
            bool IsCookieValidForUrl (const EA::WebKit::Cookie* cookie, const WebCore::URL& url, bool ignoreHttpOnlyCookies)
            {
                FixedString8_128 host; 
                ConvertToString8(url.host(),host);

                FixedString8_128 path; 
                ConvertToString8(url.path(),path);

                FixedString8_128 scheme; 
                ConvertToString8(url.protocol(),scheme);
                
                time_t timeNow = ::time(NULL);

                //Match the domain first. 
                if(!EndsWithDomain(host, cookie->mDomain))
                    return false;

                //Now match the path
                if(path.find(cookie->mPath) != 0)
                    return false;

                //Check if the cookie expired
                if(cookie->IsCookieExpired(timeNow))
                    return false;

                //Check if the cookie is secure and if it is, what is the connection type
                if(cookie->mSecure && scheme.comparei("https") != 0)
                    return false;

				//If requested by JavaScript and the cookie is set to HttpOnly, don't send it.
				if(ignoreHttpOnlyCookies && cookie->mHttpOnly)
					return false;

                //Check if the cookie is being sent to a valid port
                if(cookie->mPorts[0])
                {
                    uint16_t urlPort(80);
                    if(url.port())
                        urlPort = url.port();
                    uint16_t index = 0;
                    for(; index < Cookie::MAX_NUM_PORTS ; ++index)
                    {
                        if(cookie->mPorts[index] == urlPort)
                            break;
                    }
                    if(index == Cookie::MAX_NUM_PORTS)
                        return false;
                }

                //Congratulations! Cookies passed all the criteria.
                return true;
            }
        }


        CookieFullTextFixedString8 Cookie::CreateCookieString() const
        {
            using namespace CookieManagerHelp;

            char expirationTime[64];
            WriteDateString(mExpirationTime, expirationTime, sizeof(expirationTime));

            CookieFullTextFixedString8 cookieStr(CookieFullTextFixedString8::CtorSprintf(),
                "%s=%s; %s=%s; %s=%s; %s=%s",
                mName.c_str(),mValue.c_str(),
                kPathStr, mPath.c_str(),
                kDomainStr, mDomain.c_str(),
                kExpiresStr, expirationTime) ;
            
            if(mSecure)
                cookieStr.append_sprintf("; %s",kSecureStr);

			if(mHttpOnly)
				cookieStr.append_sprintf("; %s",kHttpOnlyStr);

            
            if(!mComment.empty())
                cookieStr.append_sprintf("; %s=%s",kCommentStr,mComment.c_str());
            
            if(!mCommentURL.empty())
                cookieStr.append_sprintf("; %s=%s",kCommentURLStr,mCommentURL.c_str());
            
            if(mPorts[0])
            {
                cookieStr.append_sprintf("; %s=\"",kPortStr);
                for(int portIndex = 0; portIndex< Cookie::MAX_NUM_PORTS && mPorts[portIndex]; ++portIndex)
                {
                    char8_t buffer[32];
                    sprintf(buffer,"%u",mPorts[portIndex]);
                    cookieStr.append_sprintf("%s,",buffer);
                }
                cookieStr.erase(cookieStr.length()-1); //erase the last comma
                cookieStr.append_sprintf("\"");
            }

            return cookieStr;
        }

        bool Cookie::IsCookieExpired(time_t timeNow) const
        {
            return (timeNow >= mExpirationTime); // >= in case the server sends a 0 delta tick time.
        }

        CookieManager::CookieManager()
            : mParams()
            , mCookieParseBuffer(NULL)
            , mInitialized(false)
        {
        }

        CookieManager::CookieManager(const CookieManagerParameters& params, WebCore::ResourceHandleManager* /*rhm*/)
            : mParams(params)
            , mCookieParseBuffer(NULL)
            , mInitialized(false)
        { 
             
        }

        CookieManager::~CookieManager()
        {
            Shutdown();  // In case the user hasn't called it.
        }

        bool CookieManager::Init()
        {
            if(!mInitialized)
            {
                mInitialized = true;
				
				//Make sure that you don't allocate the memory if it is already allocated in the SetParameters(). This may happen if the user calls the SetCookieUsage multiple times.
				//Failure to put this check can cause a subtle leak if the max user cookie size is not the same as the default one.
				if(!mCookieParseBuffer)
					mCookieParseBuffer = new char8_t[mParams.mMaxIndividualCookieSize];//WTF::fastNewArray<char8_t>(mParams.mMaxIndividualCookieSize);
                
				ReadCookiesFromFile();
            }
            return true;
        }

        bool CookieManager::Shutdown()
        {
            if(mInitialized)
            {
                mInitialized = false;

				delete[] mCookieParseBuffer;//WTF::fastDeleteArray<char8_t> (mCookieParseBuffer);
                mCookieParseBuffer = 0;

                if(mParams.mMaxCookieCount)
                    WriteCookiesToFile();
                else
                    RemoveCookies();
                
                ClearCookies();
            }
            return true;
        }

        void CookieManager::ClearCookies()
        {
            for(CookieList::const_iterator iter = mCookies.begin(); iter != mCookies.end(); ++iter)
                delete *iter;
            
			mCookies.clear();
        }

        void CookieManager::DeleteCookiesFile()
        {
            if(!mParams.mCookieFilePath.empty())
            {
                EA::WebKit::FileSystem* fileSystem = EA::WebKit::GetFileSystem();
                fileSystem->RemoveFile(mParams.mCookieFilePath.c_str());
            }
        }

        void CookieManager::RemoveCookies()
        {
            DeleteCookiesFile();
            ClearCookies();
        }

		EA::WebKit::Cookie* CookieManager::AddCookie(const CookieFullTextFixedString8& headerValue)
		{
			Cookie* cookie = ParseCookieHeader(headerValue);
			EAW_ASSERT_MSG(cookie, "Looks like the cookie is corrupted");
			if(cookie) //Add an extra check to make sure that the pointer is valid
				mCookies.push_back(cookie);
			return cookie;
		}

        void CookieManager::ReadCookiesFromFile()
        {
            using namespace CookieManagerHelp;
            
            if(!mParams.mCookieFilePath.empty())
            {
                //Parse and cache the cookies in the memory
                EA::WebKit::FileSystem* fileSystem = EA::WebKit::GetFileSystem();
                EA::WebKit::FileSystem::FileObject fileObject = fileSystem->CreateFileObject();
                if(fileSystem->OpenFile(fileObject, mParams.mCookieFilePath.c_str(), FileSystem::kRead, FileSystem::kCDOOpenExisting))
                {
                    int64_t numBytes = 0;
                    fileSystem->GetFileSize(mParams.mCookieFilePath.c_str(),numBytes);
                    if(numBytes>0) //Have something to read
                    {
                        char* pCookieFileBuffer = new char8_t[numBytes+1];//WTF::fastNewArray<char8_t> (numBytes+1);  // +1 so we can put a \0 at the end.
                        fileSystem->ReadFile(fileObject, pCookieFileBuffer, numBytes);
                        fileSystem->CloseFile(fileObject);
                        pCookieFileBuffer[numBytes] = '\0';

                        char8_t* ctx = NULL;
						char8_t* cookieFormatVersion = NULL;
						char8_t* cookieHeader = NULL;
                        Cookie* cookie = NULL;
                        //Create Cookies from the cookie file buffer here
                        //First line is the header line which contains checksum for the file.
						char8_t* cookieChecksumFromFile = Strtok(pCookieFileBuffer, COOKIE_DELIMITER, &ctx);
						if(cookieChecksumFromFile)
						{
							uint32_t checksum = GetChecksum(ctx, (numBytes - EA::Internal::Strlen(cookieChecksumFromFile)));
							char8_t cookieChecksumData[256];
							sprintf(cookieChecksumData, "%d", checksum);

							//Second line is the header line which contains the version info for the file.
							cookieFormatVersion = Strtok(NULL, COOKIE_DELIMITER, &ctx );
							//read cookies only if the format matches and the checksum was found matched.
							if(cookieFormatVersion && EA::Internal::Stricmp(cookieFormatVersion, gEAWebKitCookieFormatVersion) == 0
								&& cookieChecksumFromFile && EA::Internal::Stricmp(cookieChecksumFromFile, cookieChecksumData) == 0)
							{
								//The file format is really simple. You have 1 cookie per line. The cookies are separated from each other by
								//a new line character.
								while( NULL != (cookieHeader = Strtok( NULL, COOKIE_DELIMITER, &ctx )) )
								{
									cookie = AddCookie(cookieHeader);
								}
							}
							else
							{
								// If the checksum fails, then delete the invalid cookie file.
								RemoveCookies();
							}
						}
						else
						{
							// Somehow even though we have a file with valid size but no checksum found (For example, if the user manually hex edited the file), Get rid of the cookies.
							RemoveCookies();
						}
						
                        delete[] pCookieFileBuffer;//WTF::fastDeleteArray<char8_t> (pCookieFileBuffer);
                        pCookieFileBuffer = NULL;
                    }
                }
                fileSystem->DestroyFileObject(fileObject);  // 2/25/09 CSidhall - Moved up one bracket for minor leak fix
            }
        }

        void CookieManager::WriteCookiesToFile()
        {
            using namespace CookieManagerHelp;
            
            if(!mParams.mCookieFilePath.empty() && mCookies.size())
            {
                EA::WebKit::FileSystem* fileSystem = EA::WebKit::GetFileSystem();
                EA::WebKit::FileSystem::FileObject fileObject = fileSystem->CreateFileObject();
                
				const char8_t* pCookieFilePath = mParams.mCookieFilePath.c_str();
				if(!fileSystem->FileExists(pCookieFilePath))
				{
					EA::IO::Path::PathString8 pathStr(pCookieFilePath);
					const EA::IO::Path::PathString8::iterator fileIter = EA::IO::Path::GetFileName(pathStr.begin(),pathStr.end());
					EA::IO::Path::PathString8 dirStr;
					dirStr.assign(pathStr.begin(),fileIter);
					if(!fileSystem->MakeDirectory(dirStr.c_str()))//Create a Directory(not the file). File creation fails without the pre-existing directory path.
					{
						EAW_ASSERT_FORMATTED(false,"Could not create directory for cookie file path %s\n",pCookieFilePath);
					}
				}
				// abaldeva: 05/23/2011. Safeguard against cookie file growing too big or if a really small size is specified.
				// Earlier, it could crash in some circumstances. 
				const size_t maxCookieFileOverhead = 256;//256 bytes
				if(fileSystem->OpenFile(fileObject, mParams.mCookieFilePath.c_str(), FileSystem::kWrite, FileSystem::kCDOCreateAlways) && maxCookieFileOverhead < mParams.mMaxCookieFileSize)
                {
					// Create the file buffer initially so as the checksum can be calculated.
					char* pCookieFileBuffer = new char8_t[mParams.mMaxCookieFileSize];
					int64_t numBytes = 0;

					strcpy(pCookieFileBuffer, gEAWebKitCookieFormatVersion);
					strcat(pCookieFileBuffer, COOKIE_DELIMITER);
					numBytes += (EA::Internal::Strlen(gEAWebKitCookieFormatVersion) + 1);

					time_t timeNow = ::time(NULL);
                    // To consider: Write the cookies in order from newest to oldest received, so that if 
                    // we have to purge some of them due to lack of space then we lose just the oldest ones.
                    for(CookieList::const_iterator iter = mCookies.begin(); iter != mCookies.end(); ++iter)
                    {
                        const Cookie* cookie = *iter;
                        //Don't write these cookies to the file. Not intended to be saved.
                        if(cookie->IsCookieExpired(timeNow) || cookie->mExpiresAtEndOfSession) 
                            continue;

                        CookieFullTextFixedString8 cookieText = cookie->CreateCookieString();
						bool hasEnoughSpaceToWriteCookie = (mParams.mMaxCookieFileSize - numBytes ) > (cookieText.length() + 1 + maxCookieFileOverhead);
						if(hasEnoughSpaceToWriteCookie)
						{
							strcat(pCookieFileBuffer, cookieText.c_str());
							strcat(pCookieFileBuffer, COOKIE_DELIMITER);
							numBytes += (cookieText.length() + 1);
						}
					}

					// Calculate the checksum for the file buffer.
					uint32_t cookieChecksum = GetChecksum(pCookieFileBuffer, numBytes);
					char8_t cookieChecksumData[maxCookieFileOverhead];

					sprintf(cookieChecksumData, "%d", cookieChecksum);

					// Write the checksum first and then version and the remaining data in the file.
					fileSystem->WriteFile(fileObject, cookieChecksumData, EA::Internal::Strlen(cookieChecksumData));
					fileSystem->WriteFile(fileObject, COOKIE_DELIMITER, 1);//strlen(COOKIE_DELIMITER) = 1

					fileSystem->WriteFile(fileObject, pCookieFileBuffer, numBytes);
                    fileSystem->CloseFile(fileObject);

					delete[] pCookieFileBuffer;//WTF::fastDeleteArray<char8_t> (pCookieFileBuffer);
					pCookieFileBuffer = NULL;
                }
                fileSystem->DestroyFileObject(fileObject);
            }
        }

        const CookieManagerParameters& CookieManager::GetParameters() const
        {
            return mParams;
        }

        void CookieManager::SetParametersAndInitialize(const CookieManagerParameters& parameters)
        {
            if(parameters.mMaxCookieCount == 0) // If we are disabling cookies...
			{
				RemoveCookies();
				mParams.mMaxCookieCount = 0;
				return;
			}

            if(parameters.mMaxCookieCount < mCookies.size())
            {
                // This is a crude way to reduce the cookie count. Would be better if we chose based on dates.
                mCookies.resize(parameters.mMaxCookieCount);
            }

            if(parameters.mMaxIndividualCookieSize != mParams.mMaxIndividualCookieSize)
            {
                if(mCookieParseBuffer)
                    delete[] mCookieParseBuffer;//WTF::fastDeleteArray<char8_t>(mCookieParseBuffer);
                
				mCookieParseBuffer = new char8_t[parameters.mMaxIndividualCookieSize];//WTF::fastNewArray<char8_t>(parameters.mMaxIndividualCookieSize);
            }

            // parameters.mMaxCookieFileSize is only used when we write cookies on shutdown.

            mParams = parameters;
			
			EAW_ASSERT_FORMATTED(parameters.mMaxCookieCount != 0, "%s", "CookieManager: No cookies!\n");
			
			//If we are already initialized, the user wants to change the way it uses cookies. So we shutdown our manager and reinitialize it 
			//with the new user parameters. I don't see the user change their cookie usage at runtime but if they do, we cover it.
			if(mInitialized)
			{
				Shutdown();
				Init();
			}
			else //We are not initialized. Initialize the manager.
			{
				Init();
			}
        }

        // This is called anytime the ResourceHandleManager has read the data. This means that
        // the headers containing the cookie information are downloaded from an http response.
        bool CookieManager::OnHeadersRead(TransportInfo* tInfo)
        {
            using namespace CookieManagerHelp;

            if(mParams.mMaxCookieCount == 0)
                return false;
			
			if(!mInitialized)
			{
				CookieManagerParameters    params;
				SetParametersAndInitialize(params);
			}

			FixedString8_128 uri;
			FixedString8_64 headerKey, headerValue;

            EA::WebKit::ConvertToString8(*GetFixedString(tInfo->mEffectiveURI), uri);    
            
            //Need to iterate through all the headers....there may be multiple Set-Cookie(2) header lines.
            for(HeaderMap::const_iterator it= GetHeaderMap(tInfo->mHeaderMapIn)->begin();it!= GetHeaderMap(tInfo->mHeaderMapIn)->end();++it)
            {
                EA::WebKit::ConvertToString8(it->first, headerKey);
                
                if(headerKey.comparei(kSetCookieHdrStr) == 0 || headerKey.comparei(kSetCookieHdr2Str) == 0)
                {
                    EA::WebKit::ConvertToString8(it->second, headerValue);
                    ProcessCookieHeader(headerValue.c_str(),uri.c_str());
                }
            }
            return true;
        }

        // This is called anytime the ResourceHandleManager is about to send data out. This means that
        // the headers containing the cookie information are ready to be sent as part of a http request.
        bool CookieManager::OnHeadersSend(TransportInfo* tInfo )
        {
            using namespace CookieManagerHelp;

            if(mParams.mMaxCookieCount == 0)
                return false;
			
			if(!mInitialized)
			{
				CookieManagerParameters    params;
				SetParametersAndInitialize(params);
			}

            FixedString8_128 url8; 
            EA::WebKit::ConvertToString8(*GetFixedString(tInfo->mEffectiveURI),url8);
			WebCore::URL url(WebCore::URL(), url8.c_str());

			FixedString8_256 cookieString = GetCookieTextForURL(url);
            
            if(!cookieString.empty())
            {
                FixedString8_64 cookieHdr8 = kCookieHdrStr;
				HeaderMap::key_type cookieHdr; 
                EA::WebKit::ConvertToString16(cookieHdr8,cookieHdr);
            
				FixedString8_64 cookieVal8;
				cookieVal8.assign(cookieString.c_str());
				HeaderMap::mapped_type cookieVal;
                EA::WebKit::ConvertToString16(cookieVal8,cookieVal);

                EA::WebKit::HeaderMap::value_type eaValue(cookieHdr, cookieVal);
                GetHeaderMap(tInfo->mHeaderMapOut)->insert(eaValue);
            }    
            return true;
        }

		FixedString8_256 CookieManager::GetCookieTextForURL(const WebCore::URL& url, bool nameAndValueOnly, bool ignoreHttpOnlyCookies)
        {
            using namespace CookieManagerHelp;

            //Some example code for debug break in a particular URL.
			//#if defined(EA_DEBUG)
			//if(url.string().find(L"toolbar.build.") != -1)
			//{
			//	int debugBreak = 0;
			//	++debugBreak;
			//}
			//#endif
			FixedString8_256 cookieString;
           
            if(mParams.mMaxCookieCount == 0 || !mInitialized)
                return cookieString; //Return Early
                        
            if (!url.host().isEmpty() && !url.path().isEmpty())
            {
                int lowestVersion = 1;

                typedef eastl::fixed_vector<const Cookie*, 8, true, EASTLAllocator> LocalMatches;
                LocalMatches matches;

                for( CookieList::const_iterator it = mCookies.begin(); it != mCookies.end(); ++it )
                {
                    const Cookie* cookie = *it;

                    if (!IsCookieValidForUrl (cookie, url, ignoreHttpOnlyCookies))
                    {
                        continue;
                    }

                    // So figure out the lowest version cookie for the cookies matching with this server. If the server did not specify a version
                    //for the cookie, this would result in the lowestVersion being set to 0.
                    if( lowestVersion > cookie->mVersion )
                        lowestVersion = cookie->mVersion;

                    matches.push_back(cookie);
                }

                if( !matches.empty() )
                {
                    //sort by path length for specificity but don't change the order otherwise
					EA::WebKit::EASTLAllocator allocator;
					eastl::stable_sort(matches.begin(),matches.end(),allocator, CookiePathSorter());

                    //Only send the version string if we received one at the first place.
                    //Not sure what number to send in case of mixed versions. Sending the lowest at the moment.
                    if( lowestVersion > 0 && !nameAndValueOnly)
                        cookieString.sprintf( "$Version=%d; ", lowestVersion );

                    bool firstCookie = true;
					for( LocalMatches::const_iterator cit = matches.begin(); matches.end() != cit; ++cit)
                    {
                        const Cookie* cookie = *cit;
                        
						if(!firstCookie)
							cookieString.append("; "); //Even though the standard says that cookies should be separated by semi-colon, all major browser vendors use semi-colon followed by a space. We do the same to deal with servers that rely on such a behavior.
						else
							firstCookie = false;

						cookieString.append( cookie->mName.c_str() ).append( "=" ).append( cookie->mValue.c_str() );

                        if(lowestVersion > 0 && !nameAndValueOnly) //Send the Domain and Path string if the version is greater than 0.
                        {
							if(!cookie->mPathDefaulted )
								cookieString.append( "; $Path=" ).append( cookie->mPath.c_str() );
							
							if(!cookie->mDomainDefaulted )
                                cookieString.append( "; $Domain=" ).append( cookie->mDomain.c_str() );

                            
                            if(cookie->mPorts[0]) //so there is a port, append all the ports in the portlist
                            {
                                cookieString.append( "; $Port=\"" );
                                for(int portIndex = 0; portIndex< Cookie::MAX_NUM_PORTS && cookie->mPorts[portIndex]; ++portIndex)
                                {
                                    char8_t buffer[32];
                                    sprintf(buffer,"%u,",cookie->mPorts[portIndex]);
                                    cookieString.append(buffer);
                                }
                                cookieString.pop_back(); //erase the last comma
                                cookieString.append( "\"" );//append the "
                            }
                        }
                    }
                }
            }

            return cookieString;
        }

        WTF::Vector<WebCore::Cookie> CookieManager::GetCookiesForURL(const WebCore::URL& url)
        {
            using namespace CookieManagerHelp;

            WTF::Vector<WebCore::Cookie> results;
            for(CookieList::const_iterator iter = mCookies.begin(); iter != mCookies.end(); ++iter)
            {
                const Cookie *cit = *iter;

                if(IsCookieValidForUrl (cit, url, false))
                {
            	    WebCore::Cookie cookie;
	                cookie.name = cit->mName.c_str();
	                cookie.value = cit->mValue.c_str();
	                cookie.domain = cit->mDomain.c_str();
	                cookie.path = cit->mPath.c_str();
	                cookie.expires = cit->mExpirationTime * 1000; // Time value in javascript is in millisec (here it is in secs).
                    cookie.secure = cit->mSecure;
                    cookie.httpOnly = cit->mHttpOnly;
					cookie.session = cit->mExpiresAtEndOfSession;

	                results.append(cookie);
                }
            }

            return results;
        }

        uint16_t CookieManager::EnumerateCookies(Cookie** cookiePtrArray, uint16_t& numCookies)
        {
            if(numCookies > mCookies.size())
                numCookies = mCookies.size();

            int index = 0;

            for(CookieList::const_iterator iter = mCookies.begin(); (iter != mCookies.end() && index<numCookies); ++iter, ++index)
                cookiePtrArray[index] = *iter;

            return numCookies;
        }

        
		uint16_t CookieManager::EnumerateCookies(char8_t** rawCookieData, uint16_t numCookiesToRead)
		{
			int index = 0;

			for(CookieList::const_iterator iter = mCookies.begin(); (iter != mCookies.end() && index< numCookiesToRead) ; ++iter, ++index)
				strcpy(rawCookieData[index],(*iter)->CreateCookieString().c_str());

			return index;
		}

		void CookieManager::deleteCookie(const WebCore::URL& url, const WTF::String& name)
		{
			using namespace CookieManagerHelp;
			FixedString8_128 host; 
			EA::WebKit::ConvertToString8(url.host(),host);

			if( !host.empty() )
			{
				CookieList::iterator it = mCookies.begin();
				while( it!= mCookies.end())
				{
					const Cookie* cookie = *it;

					if(WTF::String(cookie->mName.c_str()) ==  name 
						&& EndsWithDomain(host, cookie->mDomain) 
						&& url.path().find(cookie->mPath.c_str()) == 0)
					{
						delete cookie;
						it = mCookies.erase(it);
					}

					
				}
			}
		}
		
		void CookieManager::ProcessCookieHeader( const char8_t* pHeaderValue, const char8_t* pURI, bool ignoreHttpOnlyHeader)
        {
            if(mParams.mMaxCookieCount == 0 || !mInitialized)
                return;

            Cookie* cookie = ParseCookieHeader( pHeaderValue, pURI,ignoreHttpOnlyHeader );
            time_t timeNow = ::time(NULL);

            if( cookie && ValidateCookie( cookie, pURI ) )
            {
                // see if this cookie is replacing or removing an existing one
                for( CookieList::iterator it = mCookies.begin(); it != mCookies.end(); it++ )
                {
                    Cookie* c = *it;
					if( 0 == c->mName.compare( cookie->mName ) && CookieManagerHelp::DomainsMatch(c->mDomain,cookie->mDomain)
                        && 0 == c->mPath.compare( cookie->mPath ) )
                    {
                        //Found an existing cookie to match the new cookie. Delete existing cookie. 
                        delete *it;
                        //An existing cookie has to come from a server. 
                        //1. If the server does not set the expire/max-age attribute, it is a session cookie. Just replace the existing one.
                        //2. If the server does set the expire/max-age attribute, check if the cookie has expired (Servers do it by setting 0 as max-age
                        // or by setting a date in the past). If yes, remove it.
                        
                        if(cookie->IsCookieExpired(timeNow) && !cookie->mExpiresAtEndOfSession)
                        {
                                delete cookie;
                                mCookies.erase( it );
                        }
                        else //Just overwrite the existing cookie
                        {
                            *it = cookie;
                        }

                        return; // stop processing we are done
                    }
                }

                //So you have a new cookie now. It can come from a server or you may be reading it from the file. 
                //1. If the cookie coming from the file, mExpiresAtEndOfSession is always going to be false. So if cookie is expired, remove it. If not,
                // add it to the list.
                //2. If the cookie is coming from server with an expires/max-age attribute set, mExpiresAtEndOfSession is always going to be false. So if
                //the cookie is expired, remove it. If not, add it to the list.
                //3. If the cookie is coming from server without an expire attribute, it is a session cookie and mExpiresAtEndOfSession is going to be true.
                // In that case, you have to add the cookie to the existing list.

                if(cookie->IsCookieExpired(timeNow) && !cookie->mExpiresAtEndOfSession)
                {
                    delete cookie;
                }
                else
                {
                    // add a new cookie
                    mCookies.push_back( cookie );

                    // enforce the maximum number of cookies
                    //AJBTODO: We can code something like delete least frequently used cookie here rather than the oldest cookie.
                    if( mCookies.size() > mParams.mMaxCookieCount )
					{
						Cookie* c = mCookies.front();
						mCookies.pop_front();
						delete c;
					}
                }
                
            }
			else if(cookie) //could not validate cookie.
			{
				delete cookie;
			}

        }

        Cookie* CookieManager::ParseCookieHeader( const CookieFullTextFixedString8& headerValue, const char8_t* pURI, bool ignoreHttpOnlyHeader)
        {
            using namespace CookieManagerHelp;

            char8_t *ctx=NULL;
			char8_t *name, *value, *token;
            
            if(headerValue.length() >= mParams.mMaxIndividualCookieSize)
            {
                EAW_ASSERT_FORMATTED(false, "CookieManager: The header is too long for the cookie buffer. Required:%u,Available:%d\n", headerValue.length(), mParams.mMaxIndividualCookieSize);
                return NULL;
            }

            // copy header string into a buffer to destructively parse in place avoiding copying
            strcpy( mCookieParseBuffer, headerValue.c_str() );
            // first token pair is the cookie name/value
            GetNameValue( Strtok( mCookieParseBuffer, ";", &ctx ), &name, &value );

            if( NULL == name || name[0] == '$' )
                return NULL; // invalid name, don't bother creating cookie

            Cookie* cookie = new Cookie(name);
            //cookie->mFullCookieText = headerValue.c_str();
            if( NULL != value )
                cookie->mValue = value;

            // parse the cookie attributes
            while( NULL != (token = Strtok( NULL, ";", &ctx )) )
            {
                GetNameValue( token, &name, &value);
                if( NULL != name )
                {
                    if( 0 == stricmp( name, kDomainStr ) )
                    {
                        if( NULL != value && strlen( value ) > 2 )
                        {
                            cookie->mDomain = value;
                            cookie->mDomainDefaulted = false;
                        }

                    }
                    else if( 0 == stricmp( name, kMaxAgeStr ) )
                    {
						//	http://www.ietf.org/rfc/rfc2109.txt
						//	Max-Age=delta-seconds
						//	Optional.  The Max-Age attribute defines the lifetime of the
						//	cookie, in seconds.  The delta-seconds value is a decimal non-
						//	negative integer.  After delta-seconds seconds elapse, the client
						//	should discard the cookie.  
						
						//  A value of zero means the cookie should be discarded immediately. We don't do it right here as we figure out outside this function if the existing cookie is being discarded 
						// or this is just a new but invalid cookie (that gets discarded right away then).
						if(NULL != value)
                        {
                            uint32_t deltaTime;
                            sscanf(value,"%u",&deltaTime);
                            cookie->mExpirationTime = ::time(NULL) + deltaTime;
                        }                        

                        cookie->mExpiresAtEndOfSession = false;
                    }
                    else if( 0 == stricmp( name, kExpiresStr ) )
                    {
                        if(NULL != value)
                            cookie->mExpirationTime = ReadDateString(value);

                        cookie->mExpiresAtEndOfSession = false;
                    }
                    else if( 0 == stricmp( name, kPathStr ) )
                    {
                        if( NULL != value && strlen( value ) > 0 )
                        {
                            cookie->mPathDefaulted = false;
                            cookie->mPath = value;
                        }
                    }
                    else if( 0 == stricmp( name, kCommentStr ) )
                    {
                        if(NULL != value)
                            cookie->mComment = value;
                    }
                    else if( 0 == stricmp( name, kCommentURLStr ) )
                    {
                        if(NULL != value)
                            cookie->mCommentURL = value;
                    }
                    else if( 0 == stricmp( name, kVersionStr ) )
                    {
                        if( NULL != value)
                            sscanf(value,"%hu",&(cookie->mVersion));
                    }
                    else if( 0 == stricmp( name, kSecureStr ) )
                    {
                        cookie->mSecure = true;
                    }
					else if( 0 == stricmp( name, kHttpOnlyStr ) )
					{
						EAW_ASSERT_MSG(!ignoreHttpOnlyHeader, "HttpOnly header present when not expecting it. Fix your JavaScript.");
						if(!ignoreHttpOnlyHeader)
							cookie->mHttpOnly = true;
					}
                    else if( 0 == stricmp( name, kPortStr ) )
                    {
                        if( NULL != value)
                        {
                            eastl::string8 portsStr(value);
                            //Since we know that there is definitely going to be just 1 leading and trailing \", we can do it as follows.
                            portsStr.erase(0,1);
                            portsStr.erase(portsStr.length()-1);
                            
                            const size_t portsBufferLength = 255;
                            char ports[portsBufferLength+1];
                            if(portsStr.length() <= portsBufferLength)
                            {
                                strcpy(ports,portsStr.c_str());
                                char* ctx2 =NULL;
                                char* port =NULL;
                                port = Strtok( ports, ",", &ctx2 );
                                if(port)
                                {
                                    int portIndex = 0;
                                    sscanf(port,"%u",&(cookie->mPorts[portIndex++]));
                                    
                                    while( NULL != (port = Strtok( NULL, ",", &ctx2 )) )
                                    {
                                        if(portIndex < (Cookie::MAX_NUM_PORTS - 1))
                                            sscanf(port,"%u",&(cookie->mPorts[portIndex++]));
                                    }
                                    cookie->mPorts[portIndex] = 0; //This means the end of the ports
                                }
                            }
                            else
                            {
                                EAW_ASSERT_FORMATTED(false, "CookieManager: Not enough buffer for ports. Required:%u, Available:%d\n", portsStr.length(), portsBufferLength);
                            }

                        }
                        else //Else no ports are specified specifically. This means accept *only* the current port.
                        {
                            uint32_t currentPort(80);
							WebCore::URL url(WebCore::ParsedURLString, pURI);
                            if(url.port())
                            {
                                currentPort = url.port();
                            }
                            cookie->mPorts[0] = currentPort;
                            cookie->mPorts[1] = 0;
                        }
                    }

                }
            }

            return cookie;
            
        }

        bool CookieManager::ValidateCookie( Cookie* cookie, const char8_t* pURI )
        {
            using namespace CookieManagerHelp;

			WebCore::URL url(WebCore::URL(), pURI);

            //make sure that domain is valid
            FixedString8_128 host; 
            EA::WebKit::ConvertToString8(url.host(), host);

            if( cookie->mDomainDefaulted )
            {
				// Use the host name so for example google.com/www.google.com
				// from rfc 2109 - Domain Defaults to the request-host.  (Note that there is no dot at the beginning of request-host.)
                cookie->mDomain = host;  
				EAW_ASSERT_MSG(host[0] != '.',"Not expecting a leading dot in host name");
            }
			else
			{
				// From rfc 2965 - If an explicitly specified value does not start with a dot, the user agent supplies a leading dot.
				FixedString8_128::size_type dotLocation = cookie->mDomain.find_first_of( '.', 0 );
				if(dotLocation == FixedString8_128::npos)
				{
					EAW_ASSERT_MSG(false,"Invalid domain");
					return false;
				}
				if(dotLocation != 0)
				{
					cookie->mDomain.insert(cookie->mDomain.begin(),1,'.');
				}

			}
			// also from rfc - 
			// Domain  Defaults to the effective request-host.  (Note that because
			// there is no dot at the beginning of effective request-host,
			// the default Domain can only domain-match itself.)
			
			
			if(!ValidateDomain(cookie->mDomain,0))
			{
				EAW_ASSERT_FORMATTED(false,"Invalid domain - %s",cookie->mDomain.c_str());
				return false;
			}

			//bool status = isSubdomainOfHost("google.com",".google.com");
			//status = isSubdomainOfHost("google.com","google.com");
			//status = isSubdomainOfHost("www.google.com","google.com");
			//status = isSubdomainOfHost(".google.com","google.com");
			//bool status = isSubdomainOfHost("ad.Doubleclick.net",".doubleclick.net");
			if(!isSubdomainOfHost(host, cookie->mDomain))
			{
				if(cookie->mDomain.comparei( host ) != 0)
				{
					EAW_LOG(3,"Incorrect domain specification. Trying to set %s domain from %s host",cookie->mDomain.c_str(),host.c_str());
					return false;
				}
			}
			
            //make sure that path is valid
            FixedString8_128 path; 
            EA::WebKit::ConvertToString8(url.path(),path);
                    
            if( cookie->mPathDefaulted )
            {
                // default path is full URI path
                cookie->mPath = path; 
                cookie->mPath = cookie->mPath.substr( 0, cookie->mPath.find_last_of( '/' ) ); // substr handles npos for n
            }
            else
            {
                // validate that the request URI path is prefixed by cookie path attribute
                if( 0 != path.find( cookie->mPath ) )
                {
					EAW_LOG(3,"Incorrect path specification. Trying to set %s path from %s path",cookie->mPath.c_str(),path.c_str());
					return false;
				}
            }

            //Make sure that the port list has the port on which the request was made
            uint32_t urlPort = url.port();
            
            //urlport is 0
            if(!urlPort) 
                urlPort = 80;
            
            if(!cookie->IsValidPort(urlPort))
                return false;


            return true;
        }

		uint32_t CookieManager::GetChecksum(const char* buffer, int64_t size)
		{
			uint32_t sum = 0;

			if(size > 0) //Have something to read
			{
				int32_t  index =0;

				// We run a 8 byte loop to limit branching
				int32_t loops = size >> 3;  
				while(loops--)
				{
					sum +=buffer[index+0];    
					sum +=buffer[index+1];    
					sum +=buffer[index+2];    
					sum +=buffer[index+3];    
					sum +=buffer[index+4];    
					sum +=buffer[index+5];    
					sum +=buffer[index+6];    
					sum +=buffer[index+7];    
					index +=8;
				}

				// Deal with remainder if any in a 1 byte loop
				int32_t  remain = size & 0x07;         
				while(remain--)
				{
					sum +=buffer[index];    
					index +=1;
				}
			}
			return sum;
		}

    }

}





