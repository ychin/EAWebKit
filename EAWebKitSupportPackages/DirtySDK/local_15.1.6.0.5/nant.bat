@echo off

@REM Encapsulate running env (http://stackoverflow.com/questions/15656927/restore-default-working-dir-if-bat-file-is-terminated-abruptly)
setlocal

@REM Change directory to current directory of nant.bat
chdir %~dp0

..\Framework\version\bin\nant.exe %*
