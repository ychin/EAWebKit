:: for loops are weird in ms batch, seems you can just execute a single command so the actual bits
:: that fiddle with ruby and Framework have been hoisted out into LLInternal.bat, a hopefully suggestive name

@setlocal
@if [%1] == [] (
@set "platform=all"
) else (
@set "platform=%1"
)

@echo %platform%

@set "platforms=pc pc64 ps4 xboxone"

@if %platform%==pc (
    @set platforms=pc
)

@if %platform%==pc64 (
    @set platforms=pc64
)

@if %platform%==ps4 (
    @set platforms=ps4
)

@if %platform%==xboxone (
    @set platforms=xboxone
)

:: It's really hard to quit out of loops, so in successive calls of buildPlatform we check to see if a previous call died and if so
:: exit straight away back to the loop
@set "died=0"
@setlocal EnableDelayedExpansion
@for %%i in (%platforms%) do (

    @call :buildPlatform %%i
)
@pause

:buildPlatform
@if %died% == 1 (
    @exit /b
)

@set "platform=%1"

@if %platform% == "" (
    @exit /b
)


@set "buildGlobalRoot=.\..\..\..\Build\LLInt"
@set nantExe="C:\packages\Framework\3.34.00\bin\nant.exe"

@set masterconfigFile="-masterconfigfile:..\..\..\masterconfig.xml"

@set buildFile="-buildfile:.\LLIntOffsetsExtractor.build"
@set targetVersionProp="-D:target_version=local" 

@set rubyLoc=".\Ruby193\bin\ruby.exe"
@set "JSC_prefix=..\..\JavaScriptCore\local\JavaScriptCore"

@set "optOffsetsFileLoc=%JSC_prefix%\DerivedSources\%platform%\dev-opt"
@if not exist "%optOffsetsFileLoc%" (
    md "%optOffsetsFileLoc%"
)

@set "debugOffsetsFileLoc=%JSC_prefix%\DerivedSources\%platform%\dev-debug"
@if not exist "%debugOffSetsFileLoc%" (
    md "%debugOffSetsFileLoc%"
)

%rubyLoc% "%JSC_prefix%\offlineasm\generate_offset_extractor.rb" "-I%optOffsetsFileLoc%" "%JSC_prefix%\llint\LowLevelInterpreter.asm" "%optOffsetsFileLoc%\LLIntDesiredOffsets.h" 
%rubyLoc% "%JSC_prefix%\offlineasm\generate_offset_extractor.rb" "-I%debugOffsetsFileLoc%" "%JSC_prefix%\llint\LowLevelInterpreter.asm" "%debugOffsetsFileLoc%\LLIntDesiredOffsets.h" 

@set "configNameOpt=dev-opt"
@set "configNameDebug=dev-debug"

@if %platform%==pc (
    @set "buildDir=%buildGlobalRoot%\Win"
    @set "packageConfig=pc-vc-dev-debug pc-vc-dev-opt"
    @set "optConfig=pc-vc-dev-opt"
    @set "debugConfig=pc-vc-dev-debug"
)

@if %platform%==pc64 (
    @set "buildDir=%buildGlobalRoot%\Win64"
    @set "packageConfig=pc64-vc-dev-debug pc64-vc-dev-opt"
    @set "optConfig=pc64-vc-dev-opt"
    @set "debugConfig=pc64-vc-dev-debug"
)

@if %platform%==ps4 (
    @set "buildDir=%buildGlobalRoot%\ps4"
    @set "packageConfig=kettle-clang-dev-debug kettle-clang-dev-opt"
    @set "optConfig=kettle-clang-dev-opt"
    @set "debugConfig=kettle-clang-dev-debug"
)

@if %platform%==xboxone (
    @set "buildDir=%buildGlobalRoot%\xboxone"
    @set "packageConfig=capilano-vc-dev-debug capilano-vc-dev-opt"
    @set "optConfig=capilano-vc-dev-opt"
    @set "debugConfig=capilano-vc-dev-debug"
)

@set buildDirArg=-buildroot:"%buildDir%"
@echo "%buildDirArg%"

%nantExe% slnruntime	%masterconfigFile% %buildFile% %buildDirArg% -D:package.configs="%packageConfig%"
@if %errorlevel% neq 0 (
    echo "slnruntime failed, exiting..."
    @set "died=1"
    exit /b
)

%nantExe% visualstudio %masterconfigFile% %buildFile% %buildDirArg% -D:config="%debugConfig%"
@if %errorlevel% neq 0 (
    echo "debug build failed, exiting..."
    @set "died=1"
    exit /b
)

%nantExe% visualstudio %masterconfigFile% %buildFile% %buildDirArg% -D:config="%optConfig%"
@if %errorlevel% neq 0 (
    echo "opt build failed, exiting..."
    @set "died=1"
    exit /b
)

@set "offsetExeLocOpt=%buildDir%\LLIntOffsetsExtractor\local\%optConfig%\bin\LLIntOffsetsExtractor.exe"
@set "offsetExeLocDebug=%buildDir%\LLIntOffsetsExtractor\local\%debugConfig%\bin\LLIntOffsetsExtractor.exe"

@if %platform%==xboxone (
@set "offsetExeLocOpt=%buildDir%\LLIntOffsetsExtractor\local\%optConfig%\bin\LLIntOffsetsExtractor\LLIntOffsetsExtractor.exe"
@set "offsetExeLocDebug=%buildDir%\LLIntOffsetsExtractor\local\%debugConfig%\bin\LLIntOffsetsExtractor\LLIntOffsetsExtractor.exe"
)

@if %platform%==ps4 (
@set "offsetExeLocOpt=%buildDir%\LLIntOffsetsExtractor\local\%optConfig%\bin\LLIntOffsetsExtractor.elf"
@set "offsetExeLocDebug=%buildDir%\LLIntOffsetsExtractor\local\%debugConfig%\bin\LLIntOffsetsExtractor.elf"
)

@echo "Generating opt LLIntAssembly.h..."
%rubyLoc% "%JSC_prefix%\offlineasm\asm.rb" "-I%optOffsetsFileLoc%" "%JSC_prefix%\llint\LowLevelInterpreter.asm" %offsetExeLocOpt% "%optOffsetsFileLoc%\LLIntAssembly.h" 
@echo "Generating debug LLIntAssembly.h..."
%rubyLoc% "%JSC_prefix%\offlineasm\asm.rb" "-I%debugOffsetsFileLoc%" "%JSC_prefix%\llint\LowLevelInterpreter.asm" %offsetExeLocDebug% "%debugOffsetsFileLoc%\LLIntAssembly.h" 

:: Generate *Win.asm files for PC, PC64
@set gen_asm = false
@if %platform%==pc set gen_asm=true
@if %platform%==pc64 set gen_asm=true
@if %gen_asm%==true (
@echo "Generating opt LowLevelInterpreterWin.asm..."
%rubyLoc% "%JSC_prefix%\offlineasm\asm.rb" "-I%optOffsetsFileLoc%" "%JSC_prefix%\llint\LowLevelInterpreter.asm" %offsetExeLocOpt% "%optOffsetsFileLoc%\LowLevelInterpreterWin.asm" 
@echo "Generating debug LowLevelInterpreterWin.asm......"
%rubyLoc% "%JSC_prefix%\offlineasm\asm.rb" "-I%debugOffsetsFileLoc%" "%JSC_prefix%\llint\LowLevelInterpreter.asm" %offsetExeLocDebug% "%debugOffsetsFileLoc%\LowLevelInterpreterWin.asm" 
)
:: On XB1 the above process generates some constants that cause a C4146 error
:: The Ruby script below replaces any of the offending constants with INT_MIN
@if %platform%==xboxone (

copy "%optOffsetsFileLoc%\LLIntAssembly.h" "%optOffsetsFileLoc%\LLIntAssembly_temp.h"
%rubyLoc% "apply_int_min_fix.rb" "%optOffsetsFileLoc%\LLIntAssembly_temp.h" > "%optOffsetsFileLoc%\LLIntAssembly.h"
del "%optOffsetsFileLoc%\LLIntAssembly_temp.h"

copy "%debugOffsetsFileLoc%\LLIntAssembly.h" "%debugOffsetsFileLoc%\LLIntAssembly_temp.h"
%rubyLoc% "apply_int_min_fix.rb" "%debugOffsetsFileLoc%\LLIntAssembly_temp.h" > "%debugOffsetsFileLoc%\LLIntAssembly.h"
del "%debugOffsetsFileLoc%\LLIntAssembly_temp.h"

)

@goto:eof

@endlocal
@pause

