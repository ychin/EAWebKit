#!/bin/sh
export SOURCE_ROOT=$PWD
export SRCROOT=$PWD
export WebCore=$PWD
export InspectorScripts=$PWD/../EAWebKitSupportPackages/JavaScriptCore/local/JavaScriptCore/inspector/scripts
export WebReplayScripts=$PWD/../EAWebKitSupportPackages/JavaScriptCore/local/JavaScriptCore/replay/scripts

mkdir -p DerivedSources/WebCore &&
make -C DerivedSources/WebCore -f ../../DerivedSources.make $@
