/////////////////////////////////////////////////////////////////////////////
// EAFileStreamApple
//
// Copyright (c) 2010, Electronic Arts Inc. All rights reserved.
// Created by Jaap Suter
//
/////////////////////////////////////////////////////////////////////////////


#include <EAIO/internal/Config.h>
#include <EAIO/EAFileBase.h>
#include <EAStdC/EAString.h>

#import <Foundation/Foundation.h>


namespace EA
{
namespace IO
{
    struct InitializePreMain
    {
        char16_t mTempDirectory[256];
        char16_t mHomeDirectory[256];
        char16_t mAppName[256];
        char16_t mResourcePath[256];
        
        InitializePreMain()
        : mTempDirectory()
        {
            NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
            uint32_t length = 0;
            
            NSString *td = NSTemporaryDirectory();
            
            length = static_cast<uint32_t>(td.length);
            if (length >= sizeof(mTempDirectory))
                length = sizeof(mTempDirectory) - 1;
            
            [td getCharacters:reinterpret_cast<unichar*>(mTempDirectory) range:NSMakeRange(0, length)];
            mTempDirectory[length] = 0;
        
            NSString *hd = NSHomeDirectory();
            
            length = static_cast<uint32_t>(hd.length);
            if (length >= sizeof(mHomeDirectory))
                length = sizeof(mHomeDirectory) - 1;
            
            [hd getCharacters:reinterpret_cast<unichar*>(mHomeDirectory) range:NSMakeRange(0, length)];
            mHomeDirectory[length] = 0;
            
            NSString *resPath = [[NSBundle mainBundle] resourcePath];
            
            length = static_cast<uint32_t>(resPath.length);
            if (length >= sizeof(mResourcePath))
                length = sizeof(mResourcePath) - 1;
            
            [resPath getCharacters:reinterpret_cast<unichar*>(mResourcePath) range:NSMakeRange(0, length)];
            mResourcePath[length] = 0;
            
            NSString *an = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleExecutable"];
            
            length = static_cast<uint32_t>(an.length);
            
            if (length >= sizeof(mAppName))
                length = sizeof(mAppName) - 1;
            
            [an getCharacters:reinterpret_cast<unichar*>(mAppName) range:NSMakeRange(0, length)];
            mAppName[length] = 0;
            
            [pool release];
        }
    };
    
    static InitializePreMain sInitializePreMain;

    int appleGetTempDirectory(char16_t* pDirectory, uint32_t nMaxPermittedLength)
    {
        int i = 0;
        for (; i < int(nMaxPermittedLength - 1) && sInitializePreMain.mTempDirectory[i] != 0; ++i)
            pDirectory[i] = sInitializePreMain.mTempDirectory[i];
        pDirectory[i] = 0;
        return i;
    }
    
    int appleGetTempDirectory(char* pDirectory, uint32_t nMaxPermittedLength)
    {
        return EA::StdC::Strlcpy(pDirectory, sInitializePreMain.mTempDirectory, nMaxPermittedLength);
    }

    int appleGetHomeDirectory(char16_t* pDirectory, uint32_t nMaxPermittedLength)
    {
        int i = 0;
        for (; i < int(nMaxPermittedLength - 1) && sInitializePreMain.mHomeDirectory[i] != 0; ++i)
            pDirectory[i] = sInitializePreMain.mHomeDirectory[i];
        pDirectory[i] = 0;
        return i;
    }
    
    int appleGetHomeDirectory(char* pDirectory, uint32_t nMaxPermittedLength)
    {
        return EA::StdC::Strlcpy(pDirectory, sInitializePreMain.mHomeDirectory, nMaxPermittedLength);
    }
    
    // Returns the application name; not it's file path.
    int appleGetAppName(char16_t* pAppName, uint32_t nMaxPermittedLength)
    {
        int i = 0;
        for (; i < int(nMaxPermittedLength - 1) && sInitializePreMain.mAppName[i] != 0; ++i)
            pAppName[i] = sInitializePreMain.mAppName[i];
        pAppName[i] = 0;
        return i;
    }
    
    // Returns the application name; not it's file path.
    int appleGetAppName(char* pAppName, uint32_t nMaxPermittedLength)
    {
        return EA::StdC::Strlcpy(pAppName, sInitializePreMain.mAppName, nMaxPermittedLength);
    }

    int appleGetResourceDirectory(char16_t* pDirectory, uint32_t nMaxPermittedLength)
    {
        int i = 0;
        for (; i < int(nMaxPermittedLength - 1) && sInitializePreMain.mResourcePath[i] != 0; ++i)
            pDirectory[i] = sInitializePreMain.mResourcePath[i];
        pDirectory[i] = 0;
        return i;
    }
    
    int appleGetResourceDirectory(char* pDirectory, uint32_t nMaxPermittedLength)
    {
        return EA::StdC::Strlcpy(pDirectory, sInitializePreMain.mResourcePath, nMaxPermittedLength);
    }
    
} // namespace IO

} // namespace EA
