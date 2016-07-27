/*
Copyright (C) 2009-2010 Electronic Arts, Inc.  All rights reserved.

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
// EAStreamChild.cpp
//
// Copyright (c) 2007, Electronic Arts Inc. All rights reserved.
//
// Implements a fixed-size read-only stream which is a 'child' of a parent
// stream. This is useful if you have a system whereby a single large file
// consists of smaller files or a single large database record consists of
// multiple sub-records and you want each sub-record to look like a standalone
// stream to the user.
//
// Written by Paul Pedriana
/////////////////////////////////////////////////////////////////////////////


#include <EAIO/internal/Config.h>
#include <EAIO/EAStreamChild.h>
#include EA_ASSERT_HEADER


namespace EA{

namespace IO{


StreamChild::StreamChild(IStream* pStreamParent, size_type nPosition, size_type nSize)
  : mnRefCount(0),
    mnAccessFlags(0),
    mpStreamParent(NULL),
    mnPositionParent(0),
    mnPosition(0),
    mnSize(0)
{
    if(pStreamParent)
        Open(pStreamParent, nPosition, nSize);
}


StreamChild::~StreamChild()
{
    // Nothing to do. We don't close the parent stream.
}


IStream* StreamChild::GetStream() const
{
    return mpStreamParent;
}


bool StreamChild::SetStream(IStream* pStream)
{
    mpStreamParent = pStream;
    return true;
}


int StreamChild::AddRef()
{
    return ++mnRefCount;
}


int StreamChild::Release()
{
    if(mnRefCount > 1)
        return --mnRefCount;
    delete this;
    return 0;
}


bool StreamChild::Open(IStream* pStreamParent, size_type nPosition, size_type nSize)
{
    // Question: Should we provide a means for the child stream to AddRef the parent stream?
    //           If so, then we will need to make another argument to this function which 
    //           specifies that the parent should be AddRefd. We can't unilaterally AddRef
    //           the parent, as we don't know if the parent is reference-counted.
    if(!mnAccessFlags && pStreamParent && (pStreamParent->GetAccessFlags() & kAccessFlagRead)) // If not already open and if can read...
    {
        const size_type nParentStreamSize = pStreamParent->GetSize();
        const size_type nEndPosition      = nPosition + nSize;

        if((nPosition     <  nParentStreamSize) && // If the requested span of space is entirely within the parent space...
            (nEndPosition <= nParentStreamSize) && 
            (nEndPosition >= nPosition))
        {
            mpStreamParent   = pStreamParent;
            mnAccessFlags    = kAccessFlagRead;
            mnPositionParent = nPosition;
            mnPosition       = 0;
            mnSize           = nSize;
            return true;
        }
    }
    return false;
}


bool StreamChild::Close()
{
    if(mnAccessFlags) // If open...
    {
        mnAccessFlags    = 0;
        mpStreamParent   = NULL;
        mnPositionParent = 0;
        mnPosition       = 0;
        mnSize           = 0;
    }
    return true;
}


int StreamChild::GetAccessFlags() const
{
    return mnAccessFlags;
}


int StreamChild::GetState() const
{
    if(mpStreamParent)
        return mpStreamParent->GetState();
    return 0;
}


size_type StreamChild::GetSize() const
    { return mnSize; }


bool StreamChild::SetSize(size_type)
    { return false; } // We don't allow this.


off_type StreamChild::GetPosition(PositionType positionType) const
{
    switch(positionType)
    {
        case kPositionTypeBegin:
            return (off_type)mnPosition;

        case kPositionTypeEnd:
            return (off_type)(mnPosition - mnSize);

        case kPositionTypeCurrent:
        default:
            break;
    }

    return 0; // For kPositionTypeCurrent the result is always zero for a 'get' operation.
}


bool StreamChild::SetPosition(off_type position, PositionType positionType)
{
    if(mnAccessFlags) // If open...
    {
        switch(positionType)
        {
            case kPositionTypeBegin:
                if((size_type)position < mnSize) // We assume size_type is unsigned.
                {
                    mnPosition = (size_type)position;
                    return true;
                }

            case kPositionTypeCurrent:
                return SetPosition((off_type)(mnPosition + position), kPositionTypeBegin);

            case kPositionTypeEnd:
                return SetPosition((off_type)(mnPosition + mnSize + position), kPositionTypeBegin);
        }
    }
    return false;
}


size_type StreamChild::GetAvailable() const
{ 
    return mnSize - mnPosition; 
}


size_type StreamChild::Read(void* pData, size_type nSize)
{
    if(mnAccessFlags) // If open...
    {
        // We have a potential problem here with respect to multi-threaded 
        // access to the parent. With multi-threaded usage of mpStreamParent
        // it's possible that a second thread could alter the position of 
        // the parent stream between the SetPosition and Read calls below.
        if(mpStreamParent->SetPosition((off_type)(mnPositionParent + mnPosition)))
        {
            size_type nAvailable(GetAvailable());
            if (nAvailable < nSize) // allow read to end of our range
            {
                nSize = nAvailable;
            }

            if(mpStreamParent->Read(pData, nSize) == nSize)
            {
                mnPosition += nSize;
                return nSize;
            }
        }
    }
    return kSizeTypeError;
}


bool StreamChild::Flush()
{
    // Do nothing
    return true;
}


bool StreamChild::Write(const void* pData, size_type nSize)
{
    EA_ASSERT(mnPosition <= mnSize);

    if(nSize > (mnSize - mnPosition))
       nSize = (mnSize - mnPosition);

    if(mpStreamParent->SetPosition((off_type)(mnPositionParent + mnPosition)) &&
       mpStreamParent->Write(pData, nSize))
    {
        mnPosition += nSize;
        return true;
    }

    return false;
}


} // namespace IO

} // namespace EA








