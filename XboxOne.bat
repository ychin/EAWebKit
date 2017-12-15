@setlocal
@set masterconfigFile="-masterconfigfile:.\masterconfig.xml"
@set buildFile="-buildfile:.\EAWebKit.build"
@set configurations="capilano-vc-dev-debug capilano-vc-dev-opt"
@set buildDir="-buildroot:.\Build\XOne" 
"C:\packages\Framework\3.27.00\bin\nant.exe" %masterconfigFile% %buildFile% -D:package.configs=%configurations%  %buildDir% "slnruntime"
@endlocal
@pause
