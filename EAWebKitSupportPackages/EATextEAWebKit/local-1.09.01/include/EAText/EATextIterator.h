/*
Copyright (C) 2005, 2009, 2010, 2012 Electronic Arts, Inc.  All rights reserved.

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

///////////////////////////////////////////////////////////////////////////////
// EATextIterator.h
//
// By Paul Pedriana - 2005
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EATEXT_EATEXTITERATOR_H
#define EATEXT_EATEXTITERATOR_H


#include <EABase/eabase.h>
#include <EAText/EAText.h>
#include <EAText/EATextUnicode.h>
#include <EASTL/iterator.h>


#if defined(EA_COMPILER_NO_STANDARD_CPP_LIBRARY) || !EASTL_STD_ITERATOR_CATEGORY_ENABLED
    #define EATEXT_ITC_NS eastl
#else
    #include <iterator>
    #define EATEXT_ITC_NS std
#endif


namespace EA
{
    namespace Text
    {
        /// TextRun
        ///
        /// A text run is a string of text. With our lower level interface we work with sets of 
        /// text runs instead of a single text run, as this allows for layout of styled text such
        /// as HTML and it allows for manipulation of discontiguous text strings (which have 
        /// distinct benefits in some cases. Note that any API which works with a set of text
        /// runs can still work with a single text run such as a standalone C char array. 
        ///
        struct TextRun
        {
            const Char* mpText;
            uint32_t    mnTextSize;

            TextRun(const Char* pText = NULL, uint32_t nTextSize = 0)
                : mpText(pText), mnTextSize(nTextSize) { }

            void SetData(const Char* pText, uint32_t nTextSize)
                { mpText = pText; mnTextSize = nTextSize;  }
        };




        /// TextRunIterator
        ///
        /// A TextRunIterator is much like a regular STL iterator in its behaviour
        /// and specifications. The primary difference is that it is not templated
        /// (the only type it is useful for is Char) and that it has the container
        /// built into it instead of referring to some external container. 
        ///
        /// To consider: Make a separate TextRunArray container and have TextRunIterator
        /// refer to that container. This would result in most of our member variables
        /// being moved to TextRunArray except for mpCharCurrent and mnPosition.
        ///
        class EATEXT_API TextRunIterator
        {
        public:
            typedef EATEXT_ITC_NS::bidirectional_iterator_tag iterator_category;
            typedef Char                                      value_type;
            typedef ptrdiff_t                                 difference_type;
            typedef Char&                                     reference;
            typedef Char*                                     pointer;
            typedef TextRunIterator                           this_type;

        public:
            /// TextRunIterator
            ///
            /// Default constructor. Constructs member data to an empty but usable state.
            ///
            TextRunIterator();

            /// TextRunIterator
            ///
            /// Constructs a TextRunIterator with a TextRunArray.
            ///
            TextRunIterator(const TextRun* pTextRunArray, uint32_t nTextRunArraySize = 1, 
                            uint32_t nTextCurrentPosition = 0, uint32_t nTextStartPosition = 0, 
                            uint32_t nTextEndPosition = (uint32_t)-1);

            /// TextRunIterator
            ///
            /// Conventional copy constructor
            ///
            TextRunIterator(const TextRunIterator& tri);

            /// operator=
            ///
            /// Conventional assignment operator
            ///
            TextRunIterator& operator=(const TextRunIterator& tri);

            /// SetTextRunArray
            ///
            /// Sets the TextRunIterator to an arbitrary new TextRunArray.
            ///
            void SetTextRunArray(const TextRun* pTextRunArray, uint32_t nTextRunArraySize = 1, 
                                 uint32_t nTextCurrentPosition = 0, uint32_t nTextStartPosition = 0, 
                                 uint32_t nTextEndPosition = (uint32_t)-1);

            /// AtBegin
            ///
            /// Returns true if the iterator is at the start position.
            ///
            bool AtBegin() const;

            /// AtEnd
            ///
            /// Returns true if the iterator is at the end position.
            ///
            bool AtEnd() const;

            /// Empty
            ///
            /// Returns true if the iterator points to text runs with a size of zero.
            /// If Empty() == true, then GetSize() == 0.
            ///
            bool Empty();

            /// GetSize
            ///
            /// Returns the size of the text runs referred to by this iterator.
            /// The size is the distance from the starting position to the 
            /// ending position (nTextStartPosition to nTextEndPosition).
            ///
            uint32_t GetSize() const;

            /// operator*
            ///
            /// Returns the character currently being pointed at.
            /// Returns kCharInvalid if at end.
            ///
            Char operator*() const;

            /// operator->
            ///
            /// Returns the address of the character currently being pointed at.
            ///
            const Char* operator->() const;

            /// operator[]
            ///
            /// Returns the character at the offset relative to the user-specified
            /// starting position (nTextStartPosition). 
            ///
            Char operator[](uint32_t i) const;

            /// GetRelativeChar
            ///
            /// This is similar to operator[] except that it accepts a relative
            /// position instead of an absolute one. This function is equivalent
            /// to operator[](GetPosition() + i).
            ///
            Char GetRelativeChar(int32_t i) const;

            /// GetPosition
            ///
            /// Returns the current absolute position (relative to the user-specified
            /// starting position (nTextStartPosition)). 
            ///
            uint32_t GetPosition() const;

            /// SetPosition
            ///
            /// Sets the current absolute position, relative to the user-specified
            /// starting position (nTextStartPosition). 
            ///
            TextRunIterator& SetPosition(uint32_t i);

            /// operator++
            ///
            /// Prefix increment.
            ///
            TextRunIterator& operator++();

            /// operator++
            ///
            /// Postfix increment
            ///
            TextRunIterator operator++(int);

            /// operator--
            ///
            /// Prefix decrement.
            ///
            TextRunIterator& operator--();

            /// operator--
            ///
            /// Postfix decrement.
            ///
            TextRunIterator operator--(int);

            /// operator+=
            ///
            /// Increments the iterator by the given value, which may be 
            /// positive, zero or negative.
            ///
            TextRunIterator& operator+=(int32_t n);

            /// operator-=
            ///
            /// Decrements the iterator by the given value, which may be 
            /// positive, zero or negative.
            ///
            TextRunIterator& operator-=(int32_t n);

        protected:
            uint32_t CalculateSize() const;

            friend bool operator==(const TextRunIterator& tri1, const TextRunIterator& tri2);
            friend bool operator!=(const TextRunIterator& tri1, const TextRunIterator& tri2);

            friend bool operator==(const TextRunIterator& tri, const Char* pText);
            friend bool operator!=(const TextRunIterator& tri, const Char* pText);

            friend TextRunIterator operator+(const TextRunIterator& tri, int32_t n);
            friend TextRunIterator operator-(const TextRunIterator& tri, int32_t n);

        protected:
            const TextRun* mpArray;             /// The TextRun in the text run array we are working with.
            const TextRun* mpArrayEnd;          /// The last TextRun in the array we are working with. Note that this index follows the common convention of being one-past the last usable array item.
            const TextRun* mpArrayCurrent;      /// The TextRun we are currently iterating.
            const Char*    mpCharCurrent;       /// The Char that we are currently iterating (within mpArrayCurrent).
            uint32_t       mnStartPosition;     /// The index of the position the user wants to use as the first iterated position within the text run array.
            uint32_t       mnEndPosition;       /// The index of the last position. Follows the common convention of being one-past the last usable index.
            uint32_t       mnPosition;          /// The index of the current position, which refers to mpCharCurrent. Will be in the range of [start position, end position). 

        }; // class TextRunIterator








        inline TextRunIterator::TextRunIterator()
        {
            // Intentionally empty. Values are uninitialized.
        }


        inline TextRunIterator::TextRunIterator(const TextRun* pTextRunArray, uint32_t nTextRunArraySize, 
                               uint32_t nTextCurrentPosition, uint32_t nTextStartPosition, 
                               uint32_t nTextEndPosition)
        {
            SetTextRunArray(pTextRunArray, nTextRunArraySize, 
                            nTextCurrentPosition, nTextStartPosition, 
                            nTextEndPosition);
        }


        inline TextRunIterator::TextRunIterator(const TextRunIterator& tri)
          : mpArray(tri.mpArray), 
            mpArrayEnd(tri.mpArrayEnd), 
            mpArrayCurrent(tri.mpArrayCurrent),
            mpCharCurrent(tri.mpCharCurrent),
            mnStartPosition(tri.mnStartPosition),
            mnEndPosition(tri.mnEndPosition),
            mnPosition(tri.mnPosition)
        {
            // Nothing to do.
        }


        inline TextRunIterator& TextRunIterator::operator=(const TextRunIterator& tri)
        {
            mpArray         = tri.mpArray;
            mpArrayEnd      = tri.mpArrayEnd;
            mpArrayCurrent  = tri.mpArrayCurrent;
            mpCharCurrent   = tri.mpCharCurrent;
            mnStartPosition = tri.mnStartPosition;
            mnEndPosition   = tri.mnEndPosition;
            mnPosition      = tri.mnPosition;

            return *this;
        }


        inline void TextRunIterator::SetTextRunArray(const TextRun* pTextRunArray, uint32_t nTextRunArraySize, 
                                                     uint32_t nTextCurrentPosition, uint32_t nTextStartPosition, 
                                                     uint32_t nTextEndPosition)
        {
            mpArray    = pTextRunArray;
            mpArrayEnd = pTextRunArray + nTextRunArraySize;

            const uint32_t nSize = CalculateSize();

            // Set up mnStartPosition
            if(nTextStartPosition > nSize)
                mnStartPosition = nSize;
            else
                mnStartPosition = nTextStartPosition;

            // Set up mnEndPosition
            if(nTextEndPosition < mnStartPosition)
                mnEndPosition = mnStartPosition;
            else if(nTextEndPosition > nSize)
                mnEndPosition = nSize;
            else
                mnEndPosition = nTextEndPosition;

            // Set up mnPosition
            if(nTextCurrentPosition < mnStartPosition)
                mnPosition = mnStartPosition;
            else if(nTextCurrentPosition > mnEndPosition)
                mnPosition = mnEndPosition;
            else
                mnPosition = nTextCurrentPosition;

            // Set up mpArrayCurrent / mpCharCurrent
            SetPosition(mnPosition);
        }


        inline bool TextRunIterator::AtBegin() const
        {
            return (mnPosition == mnStartPosition);
        }


        inline bool TextRunIterator::AtEnd() const
        {
            return (mnPosition == mnEndPosition);
        }


        inline bool TextRunIterator::Empty()
        {
            return (mnStartPosition == mnEndPosition);
        }


        inline uint32_t TextRunIterator::GetSize() const
        {
            return (mnEndPosition - mnStartPosition);
        }


        inline Char TextRunIterator::operator*() const
        {
            if(mnPosition < mnEndPosition)
                return *mpCharCurrent;
            return kCharInvalid; // Note that kCharInvalid is not zero. Zero is a valid character and may be found in strings.
        }


        inline const Char* TextRunIterator::operator->() const
        {
            return mpCharCurrent;
        }


        inline Char TextRunIterator::operator[](uint32_t i) const
        {
            i += mnStartPosition; // Make i be relative to beginning of text run array.

            for(const TextRun* pCurrent = mpArray; pCurrent < mpArrayEnd; ++pCurrent)
            {
                if(i < pCurrent->mnTextSize)
                {
                    if((i - mnStartPosition) < (mnEndPosition - mnStartPosition)) // This is a faster way of saying "if((i >= mnStartPosition) && (i < mnEndPosition))"
                        return pCurrent->mpText[i];

                    break;
                }

                i -= pCurrent->mnTextSize;
            }

            return kCharInvalid; // Note that kCharInvalid is not zero. Zero is a valid character and may be found in strings.
        }


        inline Char TextRunIterator::GetRelativeChar(int32_t i) const
        {
            // This could be made more efficient via a direct implementation.
            return operator[](mnPosition + i);
        }


        inline uint32_t TextRunIterator::GetPosition() const
        {
            return mnPosition;
        }


        inline TextRunIterator& TextRunIterator::SetPosition(uint32_t i)
        {
            i += mnStartPosition; // Make i be relative to beginning of text run array.

            for(mpArrayCurrent = mpArray; mpArrayCurrent < mpArrayEnd; ++mpArrayCurrent)
            {
                if(i < mpArrayCurrent->mnTextSize)
                {
                    if((i - mnStartPosition) < (mnEndPosition - mnStartPosition)) // This is a faster way of saying "if((i >= mnStartPosition) && (i < mnEndPosition))"
                    {
                        mpCharCurrent = mpArrayCurrent->mpText + i;
                        mnPosition    = (uint32_t)(i - mnStartPosition);

                        return *this;
                    }

                    break;
                }

                i -= mpArrayCurrent->mnTextSize;
            }

            // Set the position to be the end position.
            // mpArrayCurrent = mpArrayEnd - 1;     //- for some reason freebox compiler (opt only) treats this as mpArrayCurrent = &mpArrayEnd[-1]; and throws a negative array bounds warning that can't be disabled
            mpArrayCurrent = reinterpret_cast<const TextRun*>( reinterpret_cast<uintptr_t>( mpArrayEnd ) - sizeof(TextRun) );
            mpCharCurrent  = mpArrayCurrent->mpText + mpArrayCurrent->mnTextSize;
            mnPosition     = (mnEndPosition - mnStartPosition);

            return *this;
        }


        inline TextRunIterator& TextRunIterator::operator++()
        {
            if(mnPosition < mnEndPosition) // If we can be incremented...
            {
                if(++mpCharCurrent >= (mpArrayCurrent->mpText + mpArrayCurrent->mnTextSize))
                {
                    ++mpArrayCurrent;

                    while((mpArrayCurrent < mpArrayEnd) && (mpArrayCurrent->mnTextSize == 0))
                        ++mpArrayCurrent;

                    if(mpArrayCurrent < mpArrayEnd)
                        mpCharCurrent = mpArrayCurrent->mpText;
                    else
                    {
                        // mpArrayCurrent = mpArrayEnd - 1;    //- for some reason freebox compiler (opt only) treats this as mpArrayCurrent = &mpArrayEnd[-1]; and throws a negative array bounds warning that can't be disabled
                        mpArrayCurrent = reinterpret_cast<const TextRun*>( reinterpret_cast<uintptr_t>( mpArrayEnd ) - sizeof(TextRun) );
                        mpCharCurrent  = mpArrayCurrent->mpText + mpArrayCurrent->mnTextSize;
                        mnPosition     = (mnEndPosition - mnStartPosition);

                        return *this;
                    }
                }

                ++mnPosition;
            }

            return *this;
        }


        inline TextRunIterator TextRunIterator::operator++(int)
        {
            const TextRunIterator tri(*this);
            operator++();
            return tri;
        }


        inline TextRunIterator& TextRunIterator::operator--()
        {
            if(mnPosition > mnStartPosition) // If we can be decremented...
            {
                if(--mpCharCurrent < mpArrayCurrent->mpText)
                {
                    --mpArrayCurrent;

                    while((mpArrayCurrent >= mpArray) && (mpArrayCurrent->mnTextSize == 0))
                        --mpArrayCurrent;

                    if(mpArrayCurrent >= mpArray)
                        mpCharCurrent = mpArrayCurrent->mpText + mpArrayCurrent->mnTextSize - 1;
                    else
                    {
                        mpArrayCurrent = mpArray;
                        mpCharCurrent  = mpArrayCurrent->mpText - 1;
                        mnPosition     = 0;

                        return *this;
                    }
                }

                --mnPosition;
            }

            return *this;
        }


        inline TextRunIterator TextRunIterator::operator--(int) // Postfix operator--
        {
            const TextRunIterator tri = *this;
            operator--();
            return tri;
        }


        inline TextRunIterator& TextRunIterator::operator+=(int32_t n)
        {
            // This could be made more efficient via a direct implementation.
            // We make a special case for the new position being < 0 in order to deal 
            // with wraparound issues. If the user specifies a new position of -5, 
            // we want the iterator to be placed at the beginning and not the end.
            const int32_t nNewPosition = (int32_t)mnPosition + n;

            if(nNewPosition < 0)
            {
                SetPosition(0);
                return operator--();
            }

            return SetPosition((uint32_t)nNewPosition);
        }


        inline TextRunIterator& TextRunIterator::operator-=(int32_t n)
        {
            const int32_t nNewPosition = (int32_t)mnPosition - n;

            if(nNewPosition < 0)
            {
                SetPosition(0);
                return operator--();
            }

            return SetPosition((uint32_t)nNewPosition);
        }


        inline uint32_t TextRunIterator::CalculateSize() const
        {
            uint32_t nSize = 0;

            for(const TextRun* pCurrent = mpArray; pCurrent < mpArrayEnd; ++pCurrent)
                nSize += pCurrent->mnTextSize;

            return nSize;
        }


        inline bool operator==(const TextRunIterator& tri1, const TextRunIterator& tri2)
        {
            return (tri1.mpArray        == tri2.mpArray &&
                    tri1.mpArrayEnd     == tri2.mpArrayEnd &&
                    tri1.mpArrayCurrent == tri2.mpArrayCurrent &&
                    tri1.mpCharCurrent  == tri2.mpCharCurrent);
        }


        inline bool operator!=(const TextRunIterator& tri1, const TextRunIterator& tri2)
        {
            return !operator==(tri1, tri2);
        }


        inline bool operator==(const TextRunIterator& tri, const Char* pText)
        {
            return (tri.mpCharCurrent == pText);
        }


        inline bool operator!=(const TextRunIterator& tri, const Char* pText)
        {
            return (tri.mpCharCurrent != pText);
        }


        inline TextRunIterator operator+(const TextRunIterator& tri, int32_t n)
        {
            TextRunIterator triTemp(tri);
            triTemp += n;
            return triTemp;
        }


        inline TextRunIterator operator-(const TextRunIterator& tri, int32_t n)
        {
            TextRunIterator triTemp(tri);
            triTemp -= n;
            return triTemp;
        }


    } // namespace Text


} // namespace EA


#endif // EATEXT_EATEXTITERATOR_H
















