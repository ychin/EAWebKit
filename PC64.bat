@setlocal
@set masterconfigFile="-masterconfigfile:.\masterconfig.xml"
@set buildFile="-buildfile:.\EAWebKit.build"
@set configurations="pc64-vc-dev-debug pc64-vc-dev-opt"
@set buildDir="-buildroot:.\Build\Win64" 
"C:\packages\Framework\3.27.00\bin\nant.exe" %masterconfigFile% %buildFile% -D:package.configs=%configurations%  %buildDir% "slnruntime"
@endlocal
@pause
