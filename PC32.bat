@setlocal
@set masterconfigFile="-masterconfigfile:.\masterconfig.xml"
@set buildFile="-buildfile:.\EAWebKit.build"
@set nantExe="C:\packages\Framework\3.27.00\bin\nant.exe"
@set configurations="pc-vc-dev-debug pc-vc-dev-opt"
@set buildDir="-buildroot:.\Build\Win" 
%nantExe%  %masterconfigFile% %buildFile% -D:package.configs=%configurations%  %buildDir% "slnruntime"
@endlocal
@pause
