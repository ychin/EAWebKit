# Copyright (C) 2015 Electronic Arts, Inc. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1.  Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
# 2.  Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
# 3.  Neither the name of Apple Inc. ("Apple") nor the names of
#     its contributors may be used to endorse or promote products derived
#     from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

make:
	@if exist "%CONFIGURATIONBUILDDIR%\buildfailed" perl -wnle "if (/XXWebCoreGeneratedXX/) { print } else { exit 1 }" "%ConfigurationBuildDir%\buildfailed"
	@if errorlevel 1 exit 1
	@echo XXWebCoreGeneratedXX > "%ConfigurationBuildDir%\buildfailed"

	perl build-generated-files.pl "%ConfigurationBuildDir%" "%WebKit_Libraries%" ea "%PlatformArchitecture%" "$(CC)"
	perl migrate-scripts.pl "%ConfigurationBuildDir%\obj%PlatformArchitecture%\WebCore\scripts"
	@cmd /C copyForwardingHeaders.cmd cairo curl
	@cmd /C copyWebCoreResourceFiles.cmd
	
clean:
	if exist "%ConfigurationBuildDir%\obj%PlatformArchitecture%\WebCore\DerivedSources" del /s /q "%ConfigurationBuildDir%\obj%PlatformArchitecture%\WebCore\DerivedSources"
	if exist "%ConfigurationBuildDir%\obj%PlatformArchitecture%\WebCore\scripts" del /s /q "%ConfigurationBuildDir%\obj%PlatformArchitecture%\WebCore\scripts"
	if exist "%ConfigurationBuildDir%\buildfailed" del "%ConfigurationBuildDir%\buildfailed"
