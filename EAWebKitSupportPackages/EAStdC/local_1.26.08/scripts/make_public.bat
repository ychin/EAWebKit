@REM =====================================================================================================================================================================
@REM Copyright (C) Electronic Arts Inc.  All rights reserved.
@REM =====================================================================================================================================================================

@REM =====================================================================================================================================================================
@REM Path Locations 
@REM =====================================================================================================================================================================
@SET OPENSOURCE=f:\packages\opensource
@SET CODESTRIPPER=f:\packages\CodeStripper\dev
@SET NANT_EXE=f:\packages\Framework\dev\bin\nant.exe
@SET PACKAGE_NAME=EAStdC
@REM =====================================================================================================================================================================
@REM Generated from the above input 
@REM =====================================================================================================================================================================
@SET BATCH_FOLDER=%~dp0
@SET CMAKE_BIN=G:\installed\cmake-3.14.0-win64-x64\bin
@SET STRIPPED=%OPENSOURCE%\stripped
@SET EASTDC_DIR=%OPENSOURCE%\%PACKAGE_NAME%
@SET MASTERCONFIG=%OPENSOURCE%\masterconfig.xml
@SET BUILD_OUT=%EASTDC_DIR%\%PACKAGE_NAME%_build_out
@REM =====================================================================================================================================================================



@REM =====================================================================================================================================================================
@REM Sanity Check - Ensure that files that need exist on the local hard drive.
@REM =====================================================================================================================================================================
@IF NOT EXIST "%MASTERCONFIG%" (
	@ECHO Can't find "%MASTERCONFIG%"
	GOTO ERROR_HANDLER
)
@IF NOT EXIST %CMAKE_BIN%/cmake.exe (
	ECHO Can't find %CMAKE_BIN%/cmake.exe
	GOTO ERROR_HANDLER
)
@IF NOT EXIST %CMAKE_BIN%/ctest.exe (
	ECHO Can't find %CMAKE_BIN%/ctest.exe
	GOTO ERROR_HANDLER
)




@REM =====================================================================================================================================================================
@REM Run CodeStripper on all packages 
@REM =====================================================================================================================================================================
cd %CODESTRIPPER%
call %NANT_EXE% -masterconfigfile:%MASTERCONFIG% -D:codestripper_packages=%PACKAGE_NAME% -D:codestripper_platforms=pc,android,iphone,osx,kettle,capilano -D:codestripper_includesource=true -D:codestripper_outputdir=%STRIPPED%
@IF %ERRORLEVEL% NEQ 0 GOTO ERROR_HANDLER



@REM =====================================================================================================================================================================
@REM Copy stripped code into the public folder 
@REM =====================================================================================================================================================================
cd %OPENSOURCE%
xcopy %STRIPPED%\%PACKAGE_NAME%\dev %EASTDC_DIR% /i /e /s /y



@REM =====================================================================================================================================================================
@REM Remove stripped source directory
@REM =====================================================================================================================================================================
rmdir %STRIPPED% /s /q



@REM =====================================================================================================================================================================
@REM Sanitize directory
@REM =====================================================================================================================================================================
del %EASTDC_DIR%\%PACKAGE_NAME%.build
del %EASTDC_DIR%\Manifest.xml
del %EASTDC_DIR%\masterconfig.xml
del %EASTDC_DIR%\tags
del "%EASTDC_DIR%\doc\Changes.html"
del "%EASTDC_DIR%\doc\make_public.bat"
del "%EASTDC_DIR%\.p4config"
del "%EASTDC_DIR%\.gitlab-ci.yml"
rmdir %EASTDC_DIR%\config /s /q
rmdir %EASTDC_DIR%\scripts /s /q



@REM =====================================================================================================================================================================
@REM Check if the user is requesting to skip the build step
@REM =====================================================================================================================================================================
@IF "%1" == "skip_build" @GOTO SKIP_BUILD



@REM =====================================================================================================================================================================
@REM Build and Run Unit Tests 
@REM =====================================================================================================================================================================


@REM ----------------------------------------------------------------------
@REM Make the build output files directory 
@REM ----------------------------------------------------------------------
mkdir %BUILD_OUT%
cd %BUILD_OUT%

@REM ----------------------------------------------------------------------
@REM  Generate platform specific build files 
@REM ----------------------------------------------------------------------
REM  %CMAKE_BIN%/cmake.exe .. -G "Visual Studio 15 2017 Win64" -DEAMAIN_BUILD_TESTS:BOOL=ON 

@REM ----------------------------------------------------------------------
@REM Build the tests 
@REM ----------------------------------------------------------------------
REM  %CMAKE_BIN%/cmake.exe --build . --config Release
REM  @IF %ERRORLEVEL% NEQ 0 GOTO ERROR_HANDLER

@REM ----------------------------------------------------------------------
@REM Run the tests 
@REM ----------------------------------------------------------------------
REM  cd test
REM  %CMAKE_BIN%/ctest.exe -C Release 
REM  @IF %ERRORLEVEL% NEQ 0 GOTO ERROR_HANDLER
@REM ----------------------------------------------------------------------
cd %BATCH_FOLDER%



:SKIP_BUILD
@REM =====================================================================================================================================================================
@REM Remove temporary files 
@REM =====================================================================================================================================================================
rmdir %BUILD_OUT% /s /q


@REM =====================================================================================================================================================================
@REM Grep for keywords
@REM =====================================================================================================================================================================
cd %EASTDC_DIR%



@ECHO ================================================================================    
@ECHO        !!!! Grep commands are searching from NDA information. !!!!  
@ECHO        !!!! ENSURE THAT ALL HITS ARE ALLOWED TO BE RELEASED PUBLICALLY !!!!
@ECHO ================================================================================    
grep -irn "ps2" * 
grep -irn "ps3" * 
REM  grep -irn "ps4" * 
rem grep -irn "xenon" * 
rem grep -irn "xboxone" *
grep -inr xb . | sed s/texBuf// | sed s/radixByte// | sed s/dxBegin// | sed s/mMutexBuffer// | grep -i xb
REM  grep -irn "kettle" *
rem grep -irn "capilano" *
rem grep -irn "durango" *
grep -irn "scorpio" *
REM  grep -irn "orbis" * 
grep -irn "gamecube" * 
grep -irn "revolution" * 
REM  grep -irn "SN Systems" * 
REM  grep -irn "playstation" * 
grep -irn "nintendo" * 
REM  grep -irn "sony" * 
grep -irn "wii" * 
grep -irn "cafe" * 
grep -irn "latte[^r]" * 
grep -irn "3ds" * 
REM  grep -irn "SNC" * 
grep -irn "gpl" * 
REM grep -irn "sce" * 
REM ----
REM Look for filenames containing NDA names.
REM ----
REM dir /s/b | grep -irn "ps2" * 
REM dir /s/b | grep -irn "ps3" * 
REM dir /s/b | grep -irn "ps4" * 
REM dir /s/b | grep -irn "xenon" * 
REM dir /s/b | grep -irn "xboxone" *
dir /s/b | grep -irn "scorpio" *
REM  dir /s/b | grep -irn "kettle" *
REM dir /s/b | grep -irn "capilano" *
REM dir /s/b | grep -irn "durango" *
REM  dir /s/b | grep -irn "orbis" * 
dir /s/b | grep -irn "gamecube" * 
dir /s/b | grep -irn "revolution" * 
REM  dir /s/b | grep -irn "SN Systems" * 
REM  dir /s/b | grep -irn "playstation" * 
dir /s/b | grep -irn "nintendo" * 
REM  dir /s/b | grep -irn "sony" * 
dir /s/b | grep -irn "wii" * 
dir /s/b | grep -irn "cafe" * 
dir /s/b | grep -irn "latte[^r]" * 
dir /s/b | grep -irn "3ds" * 
REM  dir /s/b | grep -irn "SNC" * 
dir /s/b | grep -irn "gpl" * 
dir /s/b | grep -irn ".p4config" * 
@ECHO ================================================================================    
@ECHO        !!!! Grep commands are searching from NDA information. !!!!  
@ECHO        !!!! ENSURE THAT ALL HITS ARE ALLOWED TO BE RELEASED PUBLICALLY !!!!
@ECHO ================================================================================    
@GOTO COMPLETED_OK 



:ERROR_HANDLER
@ECHO MAKE_PUBLIC=ERRORS! 



:COMPLETED_OK
@ECHO MAKE_PUBLIC=OK. 



@REM =====================================================================================================================================================================
@REM Remove temporary files 
@REM =====================================================================================================================================================================
@IF EXIST "%BUILD_OUT%" (
	rmdir %BUILD_OUT% /s /q
)

