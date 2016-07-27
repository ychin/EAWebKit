/*
Copyright (C) 2007, 2009, 2010, 2012 Electronic Arts, Inc.  All rights reserved.

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
// EATextEffects.h
// Created by Paul Pedriana - 2007
///////////////////////////////////////////////////////////////////////////////


#ifndef EATEXT_EATEXTEFFECTS_H
#define EATEXT_EATEXTEFFECTS_H


#include <EAText/EAText.h>
#include <EAText/EATextFont.h>
#include <EAText/internal/EATextBitmap.h>
#include <string.h>

#ifdef _MSC_VER
    #pragma warning(push)           // We have to be careful about disabling this warning. Sometimes the warning is meaningful; sometimes it isn't.
    #pragma warning(disable: 4251)  // class (some template) needs to have dll-interface to be used by clients.
#endif


namespace EA
{
    namespace Text
    {
        class OutlineFont;
        class EffectsProcessor;


        // Constants
        const uint32_t kEffectInstructionListCapacity   = 40;   /// Mas number of instructions + arguments in a program.
        const uint32_t kEffectMaxBrushSize              = 13;   /// Max brush diameter in pixels.
        const int32_t  kGlyphGutterSizeDefault          = 12;   /// Space around glyphs for blurring, outlining, offsetting, etc.
        const int32_t  kMaxExpansionDefault             = 12;   /// Max amount the space around a glyph can grow due to blurring, outlining.


        ///////////////////////////////////////////////////////////////////////
        // BrushShape
        //
        // Defines a brush shape used for brush-based drawing effects, 
        // such as glyph outline effects.
        //
        enum BrushShape
        {
            kBSRound,   /// Brush is round. This is what you usually want to use.
            kBSSquare   /// Brush is square.
        };


        ///////////////////////////////////////////////////////////////////////
        // EffectsCurveType
        //
        enum EffectsCurveType
        {
            kECTJ,      /// Curve shaped like an j in a brightness curve dialog box.
            kECTS,      /// Curve shaped like an s in a brightness curve dialog box.
            kECTF       /// Curve shaped like an f in a brightness curve dialog box.
        };


        ///////////////////////////////////////////////////////////////////////
        // EffectsTargetImage
        //
        enum EffectsTargetImage
        {
            kETINone  = 0,
            kETIFloat = 1, /// The alpha image of the most recently drawn glyph. This is like a Photoshop floating layer, with the currently drawn glyph in it.
            kETIBase  = 2  /// The composed ARGB image. This is like the Photoshop base layer, with the merged down (composited) image in it.
        };


        ///////////////////////////////////////////////////////////////////////
        // EffectsChannelMask
        //
        enum EffectsChannelMask
        {
            kECMNone  = 0,   /// No mask
            kECMAlpha = 1,   /// Alpha channel (A from ARGB)
            kECMColor = 2    /// Color channel (RGB from ARGB)
        };


        ///////////////////////////////////////////////////////////////////////
        // EffectsUserId
        //
        enum EffectsUserId
        {
            kEUIBegin,  /// If the user has a callback registered, it will be called with this id upon start of execution.
            kEUIEnd     /// If the user has a callback registered, it will be called with this id upon end of execution.
        };


        ///////////////////////////////////////////////////////////////////////
        // EffectsInstruction
        //
        // Defines the basic set of effects instructions that can be used.
        //
        enum EffectsInstruction         /// Action                              Arguments                                       Notes
        {                               /// ----------------------------------------------------------------------------------------------------------------------------------------------
            kEINone,                    /// No-op                               <none>

            // Drawing state
            kEISetExpansion,            /// Sets expansion parameters           uint32 gutter, max glyph expansion              Sets the base layer gutter (buffer size around the glyph for blurs, etc.), and sets the max pixels a glyph image can expand due to blurs, glows, etc. Must be called early in a program. Default values are kGlyphGutterSizeDefault and kMaxExpansionDefault.
            kEISetGlyphMinAlpha,        /// Sets min alpha for glyph bounds     uint32_t A (0..255)                             Sets the color below which a pixel is not considered part of a glyph. Used for calculating glyph bounds after effects are applied.
            kEISetGlyphColor,           /// Set current color                   uint32_t ARGB color                             Sets color for next glyph draw of any type (normal, blur, outline).
            kEISetGlyphOffset,          /// Move glyph                          int32_t dx, dy                                  Moves glyph draw position relative to baseline pen position.
            kEISetGlyphHSpace,          /// Add space to glyph sides            int32_t left space, right space                 Adding space on the left moves the glyph to the right relative to the pen. Adding space on the right increases the advance (moves the next glyph to the right). Usually you would need this only once for an instruction set.
            kEISetGlyphVSpace,          /// Add space to glyph sides            int32_t top space, bottom space                 Adding space on top increases the ascent and line height. Adding space on bottom decreases the descent value and increases the line height. Usually you would need this only once for an instruction set.
            kEISetGlyphSmooth,          /// Enable/disable smoothing            bool enable                                     The next glyph draw of any type (normal, blur, outline) uses this.
            kEISetGlyphBrush,           /// Set brush                           uint32_t shape, size, hardness, opacity         Usually one of enum BrushShape. Used only for drawing outline, but may be used for other purposes in the future.
            kEISetTargetImage,          /// Set filter target image             uint32_t targetImage                            Sets the image action destination: either kETIFloat or kETIBase. Default is kETIFloat. Most of the time you want to use kETIFloat.
            kEISetTargetMask,           /// Set filter target mask              uint32_t maskFlags                              Set the filter raster op so that it applies to color alpha, or both.

            // Glyph drawing actions
            kEIDrawGlyphSmearOutline,   /// Draw glyph outline smear            <none>                                          Draws an outline smear with the a pattern that matches the current brush and uses other settings.
            kEIDrawGlyphBrushOutline,   /// Draw glyph outline w/ brush         <none>                                          Draws an outline directly with the current brush and other settings.
            kEIDrawGlyph,               /// Draw glyph                          <none>                                          Draws a glyph with the current settings.

            // Image actions
            kEIClear,                   /// Clear target image                  <none>                                          Clears the target image (should always be kETIFloat), in preparation for another drawing layer.
            kEIApplyCurve,              /// Apply brightness s, j, or f curve   uint32_t curveType, float strength              Applies a brightness curve to the target image. j curve makes dark things darker, f makes lighter things lighter, s does both.
            kEIApplyBoxBlur,            /// Apply box blur                      float radius, gain, uint32_t pass count         Implements a basic box blur on the target image. Gain is a multiplier in the range of [0, infinity] which reduces (gain < 1) or increase (> 1) the blurred pixel volume. A value > 1 causes a blurred glyph to not fade away so much. A value of 1 has no effect.
            kEIMerge,                   /// Merge float down to base            <none>                                          Merges the floating layer image down to the base layer. Leaves the float layer as-is. This operation is similar to Photoshop merge layer down, but preserves the floating layer instead of making it go away. 

            // User action
            kEICallUser,                /// Call user function                  uint32_t user-defined id                        Calls user function.
        };


        ///////////////////////////////////////////////////////////////////////
        // EffectsInstructionWord
        //
        // Used to encode instructions in the instruction list. This simply
        // allows us to have arguments to instructions that are float, bool,
        // and other types.
        //
        union EffectsInstructionWord
        {
            uint32_t mBool;
            uint32_t mU32;
            int32_t  mI32;
            float    mF32;
        };
        typedef EffectsInstructionWord EIWord;


        ///////////////////////////////////////////////////////////////////////
        // EffectsBitmaps
        //
        // Effects are composed onto a temporary canvas during the process of
        // making effects. We use multiple such canvas images during compositing.
        // These images can potentially take up a lot of space if the glyph
        // size is very large. So we have a separate EffectsBitmaps class to
        // manage these images and provide a shared version that all fonts 
        // use in order to conserve memory.
        //
        struct EATEXT_API EffectsBitmaps
        {
            EffectsBitmaps();

            void BeginUse(void* pUser, EA::Allocator::ICoreAllocator* pAllocator);  /// Should be called before being used for a compositing session.
            void EndUse(void* pUser);                                               /// Should be called after being used for a compositing session.
            bool IsInUse() const;
            bool IsInUse(void* pUser);
            void Reset();                                                           /// Frees existing memory. Does not clear allocator member pointer.

            Bitmap8                  mFloatImage;                       /// Float layer glyph bitmap image. This is the effects composition source. Glyphs are drawn here before merged down to the base layer.
            Bitmap32                 mBaseImage;                        /// Base layer working bitmap image. This is the effects composition destination.
            Bitmap32                 mBlurPrecalcSumImage;              /// Used for improving blur performance by storing precalculated values.
            void*                    mpUser;                            /// The user who called BeginUse.
            int32_t                  mUserCount;                        /// Number of users who use this class. Used for knowing when the last user goes away so we can clear our memory.

        protected:
            EffectsBitmaps(const EffectsBitmaps&);
            EffectsBitmaps& operator=(const EffectsBitmaps&);
        };


        ///////////////////////////////////////////////////////////////////////
        // EffectsState
        //
        // Stores the entire effects state needed to implement effects.
        //
        struct EATEXT_API EffectsState
        {
        public:
            // Drawing parameters
            uint32_t        mGlyphMinAlpha;         /// Initialized to 5
            uint32_t        mGlyphColor;            /// Initialized to ARGB = ff000000
            int32_t         mGlyphDX;               /// Initialized to 0
            int32_t         mGlyphDY;               /// Initialized to 0
            int32_t         mGlyphLeftSpace;        /// Initialized to 0
            int32_t         mGlyphTopSpace;         /// Initialized to 0
            int32_t         mGlyphRightSpace;       /// Initialized to 0
            int32_t         mGlyphBottomSpace;      /// Initialized to 0
            int32_t         mMaxExpansion;          /// Initialized to kMaxExpansionDefault
            uint32_t        mGlyphSmooth;           /// Initialized to 1 (kSmoothEnabled).
            uint32_t        mGlyphBrushShape;       /// Initialized to kBSRound
            uint32_t        mGlyphBrushSize;        /// Initialized to 3
            float           mGlyphBrushHardness;    /// Initialized to 0.5f
            float           mGlyphBrushOpacity;     /// Initialized to 1.0f
            uint32_t        mTargetImage;           /// Initialized to kETIFloat
            uint32_t        mTargetMask;            /// Initialized to kECMAlpha | kECMColor

        public:
            struct BrushPixel
            {
                int8_t  mDX;
                int8_t  mDY;
                uint8_t mAlpha;
            };

        public:
            // State
            EIWord                  mInstructionList[kEffectInstructionListCapacity];       /// The effects instruction list. Composed of EffectsInstructions and their arguments.
            uint32_t                mInstructionListSize;                                   /// Current total count of instructions in list.
            EIWord*                 mpCurrentInstruction;                                   /// The current instruction in the effects instruction set.
            OutlineFont*            mpOutlineFont;                                          /// The current font.
            GlyphId                 mGlyphId;                                               /// The current glyph id.
            BrushPixel              mBrush[kEffectMaxBrushSize * kEffectMaxBrushSize + 1];  /// 
            int32_t                 mRequiredGutterSpace;                                   /// Space around glyphs for blurring, outlining, offsetting, etc.
            bool                    mbFloatImageClear;                                      /// True if the float image has not been written to.
            bool                    mbBaseImageClear;                                       /// True if the base image has not been written to.
            int32_t                 mImagePenX;                                             /// 
            int32_t                 mImagePenY;                                             /// 
            int32_t                 mActualGlyphLeft;                                       /// 
            int32_t                 mActualGlyphTop;                                        /// 
            int32_t                 mActualGlyphRight;                                      /// 
            int32_t                 mActualGlyphBottom;                                     /// 
            int32_t                 mGlyphExpansion;                                        /// How much extra space around the glyph is added due to blurring, glowing, etc.
            GlyphMetrics            mGlyphMetrics;                                          /// The current glyph metrics.
            static EffectsBitmaps   mEffectsBitmaps;                                        /// Shared bitmaps between all effects instances (and thus all fonts).

        public:
            EffectsState(EA::Allocator::ICoreAllocator* pCoreAllocator = NULL);
           ~EffectsState();

            void ResetParameters();
            void ResetExecution(OutlineFont* pOutlineFont, GlyphId glyphId);
            void ClearInstructionList();
            void AppendInstructionList(const EIWord* pInstructionList, uint32_t nInstructionCount);

            // Return a hash of an instruction list. Useful for quickly comparing instruction lists.
            static uint32_t HashInstructionList(const EIWord* pInstructionList, uint32_t nInstructionCount);

            // Instruction list building
            void SetExpansion(uint32_t gutterSize, uint32_t maxExpansion);
            void SetGlyphMinAlpha(uint32_t colorA);
            void SetGlyphColor(uint32_t colorARGB);
            void SetGlyphOffset(int32_t dx, int32_t dy);
            void SetGlyphHSpace(int32_t leftSpace, int32_t rightSpace);
            void SetGlyphVSpace(int32_t topSpace, int32_t bottomSpace);
            void SetGlyphSmooth(bool bEnable);
            void SetGlyphBrush(uint32_t shape, uint32_t size, float hardness, float opacity);
            void SetTargetImage(uint32_t image);
            void SetTargetMask(uint32_t mask);
            void DrawGlyphSmearOutline();
            void DrawGlyphBrushOutline();
            void DrawGlyph();
            void Clear();
            void ApplyCurve(uint32_t type, float strength);
            void ApplyBoxBlur(float radius, float gain, uint32_t passCount);
            void Merge();
            void CallUser(uint32_t id);

        protected:
            // Declare this protected so nobody external can get away with calling them. We do processing 
            // in our ctor and dtor that we want to make sure always gets executed and thus we don't want 
            // the compiler to generate a copy ctor or operator= and thus get away with not executing our ctor code.
            EffectsState(const EffectsState&);
            EffectsState& operator=(const EffectsState&);
        };


        /////////////////////////////////////////////////////////////////////////
        // EffectsPlugin
        //
        // Allows the user to provide their own plugin effects processor.
        //
        class EATEXT_API EffectsPlugin
        {
        public:
            virtual ~EffectsPlugin() { }
            virtual void ProcessEffect(uint32_t id, EffectsProcessor* pEffectsProcessor, 
                                       EffectsState* pEffectsState, void* pUserData) = 0;
        };


        /////////////////////////////////////////////////////////////////////////////
        // CreateInstructionList
        //
        // This function converts a string-based instruction list to a native 
        // instruction list. Documentation by example:
        //     SetGlyphMinAlpha 8
        //     SetGlyphColor 0xff000000
        //     SetGlyphOffset 2 2
        //     SetGlyphHSpace 1 1
        //     SetGlyphYSpace 1 1
        //     SetGlyphSmooth 1
        //     SetGlyphBrush 0 3 .5 .5
        //     SetTargetImage 0
        //     SetTargetMask 3
        //     DrawGlyphSmearOutline
        //     DrawGlyphBrushOutline
        //     DrawGlyph
        //     Clear
        //     ApplyCurve 1 0.7
        //     ApplyBoxBlur 1.5 1.3 2
        //     Merge
        //     CallUser 0x12345678
        //
        // Example usage:
        //    EffectsState es;
        //    int32_t      nErrorLine;
        //    uint32_t     count;
        // 
        //    count = CreateInstructionList("SetGlyphColor 0xffffffff\n"    // Note that we separate the statements on different lines.
        //                                  "SetGlyphSmooth 1\n"            // This isn't necessary, though the \n chars are.
        //                                  "SetGlyphBrush 0 3 0.5 0.5\n"
        //                                  "DrawGlyphSmearOutline\n"
        //                                  "ApplyCurve 1 1\n"
        //                                  "Merge\n"
        //                                  "Clear\n"
        //                                  "SetGlyphColor 0xffffffff\n"
        //                                  "SetGlyphSmooth 0\n"
        //                                  "DrawGlyph\n"
        //                                  "Merge\n", es, &nErrorLine);
        //    if(count)
        //        pFontServer->AddEffect(0x12345678, es.mInstructionList, es.mInstructionListSize);
        //    else
        //        printf("Error at line %d\n", nErrorLine);
        //
        EATEXT_API uint32_t CreateInstructionList(const char* pInstructionString, EA::Text::EIWord* pInstructionList, uint32_t instructionListCapacity, int32_t* pErrorLine);
        EATEXT_API uint32_t CreateInstructionList(const char* pInstructionString, EA::Text::EffectsState& es, int32_t* pError);


        /////////////////////////////////////////////////////////////////////////
        // EffectsProcessor
        //
        // Implements text effects.
        // Works by implementing an effecs instruction list that gets executed.
        //
        class EATEXT_API EffectsProcessor
        {
        public:
            EffectsProcessor(EA::Allocator::ICoreAllocator* pCoreAllocator = NULL);
           ~EffectsProcessor();

            void          SetAllocator(Allocator::ICoreAllocator* pCoreAllocator);
            void          SetUserCallback(EffectsPlugin* pEffectsPlugin, void* pUserData);
            void          SetGlyph();
            EffectsState* GetEffectsState();

            void          Execute(OutlineFont* pOutlineFont, GlyphId glyphId);
            void          DrawGlyphSmearOutline();
            void          DrawGlyphBrushOutline();
            void          DrawGlyph();
            void          Clear();
            void          ApplyCurve(uint32_t type, float strength);
            void          ApplyBoxBlur(float radius, float gain, uint32_t passCount);
            void          Merge();
            void          AdjustFontMetrics(FontMetrics& fontMetrics);

            // These functions are merely inline passthrough functions which call the same function in mEffectsState::mEffectsBitmaps.
            static void  BeginEffectBitmapsUse(void* pUser, EA::Allocator::ICoreAllocator* pAllocator);
            static void  EndEffectBitmapsUse(void* pUser);
            static bool  IsEffectBitmapsInUse(void* pUser);
            static void  ResetEffectBitmaps();

        protected:
            enum GlyphState
            {
                kGlyphNotDrawn,
                kGlyphDrawnMono,
                kGlyphDrawnGray
            };

            GlyphState GetCurrentGlyphState() const;
            bool       SetCurrentGlyphState(GlyphState glyphState);
            void       SetupImages();
            void       BuildBrush();

            #if EATEXT_DEBUG
                void DebugPrint();
            #endif

            EffectsProcessor(const EffectsProcessor&);
            EffectsProcessor& operator=(const EffectsProcessor&);

        protected:
            Allocator::ICoreAllocator*  mpCoreAllocator;
            EffectsState                mEffectsState;
            EffectsPlugin*              mpEffectsPlugin;
            void*                       mpUserData;
            GlyphState                  mGlyphState;
            #if EATEXT_DEBUG
                bool                    mbDebugPrint; // If enabled, prints bitmap files after each modifying step.
            #endif
        };

    } // namespace Text

} // namespace EA



#ifdef _MSC_VER
    #pragma warning(pop)
#endif


#endif // EATEXT_EATEXTEFFECTS_H






