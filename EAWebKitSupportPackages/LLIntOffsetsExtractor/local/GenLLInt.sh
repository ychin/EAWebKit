#!/bin/sh

export buildGlobalRoot="./../../../Build/LLInt"
export nantExe="Users/rupsharma/packages/Framework/7.04.02/bin/NAnt.exe"

export masterconfigFile="../../../masterconfig.xml"

export buildFile="-buildFile: ./LLIntOffsetsExtractor.build"
export targetVersionProp="-D:target_version=local"

export rubyLoc="./Ruby193/bin/ruby.exe"
export JSC_prefix="../../JavaScriptCore/local/JavaScriptCore"

export optOffsetsFileLoc="$JSC_prefix/DerivedSources/osx64/dev-opt"

export buildDir="./build/osx64"

ruby "$JSC_prefix/offlineasm/generate_offset_extractor.rb" "-I$optOffsetsFileLoc" "$JSC_prefix/llint/LowLevelInterpreter.asm" "$optOffsetsFileLoc/LLIntDesiredOffsets.h"

mono $nantExe -masterconfigfile:$masterconfigFile -D:config=osx-x64-clang-dev-opt -buildroot:build/osx64 -verbose> output.txt
export offsetExeLocOpt="$buildDir/LLIntOffsetsExtractor/local/osx-x64-clang-dev-opt/bin/LLIntOffsetsExtractor"

echo "Generating opt LLIntAssembly.h..."
ruby "$JSC_prefix/offlineasm/asm.rb" "-I$optOffsetsFileLoc" "$JSC_prefix/llint/LowLevelInterpreter.asm" $offsetExeLocOpt "$optOffsetsFileLoc/LLIntAssembly.h"

 
