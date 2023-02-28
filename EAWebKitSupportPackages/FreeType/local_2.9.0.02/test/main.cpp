
#include <EATest/EASTLNewOperatorGuard.inl>
#include <EAMain/EAEntryPointMain.inl>

#include <stdio.h>
#include <ft2build.h>
#include FT_FREETYPE_H      // #include <freetype/freetype.h>
#include FT_SIZES_H         // #include <freetype/ftsizes.h>
#include FT_SFNT_NAMES_H    // #include <freetype/ftsnames.h>
#include FT_TRUETYPE_IDS_H  // #include <freetype/ttnameid.h>

int EAMain(int argc, char** argv)
{
    int nErrorCount = 0;
    EA_UNUSED(argc);
    EA_UNUSED(argv);

    EA::EAMain::PlatformStartup();   

    #if defined(__GNUC__) && defined(__arm__) && defined(FT_MULFIX_ASSEMBLER) && !defined(__CC_ARM) && !defined(__ARMCC__)
        // The FreeType ARM inline asm is broken and cannot be safely used. We assert so here in this 
        // unit test in order to verify that a future FreeType package update doesn't overwrite our 
        // disabling of this asm in our version of the FreeType package.
        #error
        nErrorCount++; 
    #endif

    EA::EAMain::PlatformShutdown(nErrorCount);
    
    return nErrorCount;
}