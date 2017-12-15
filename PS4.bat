@setlocal
@set masterconfigFile="-masterconfigfile:.\masterconfig.xml"
@set buildFile="-buildfile:.\EAWebKit.build"
@set configurations="kettle-clang-dev-debug kettle-clang-dev-opt"
@set buildDir="-buildroot:.\Build\PS4" 
"C:\packages\Framework\3.27.00\bin\nant.exe" %masterconfigFile% %buildFile% -D:package.configs=%configurations%  %buildDir% "slnruntime"
@endlocal
@pause
