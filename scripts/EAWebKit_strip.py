# Written by Arpit Baldeva
# Maintained by EAWebKit Team, Copyright Electronic Arts Inc
# Based on CodeStripper example scripts and DirtySDK_strip.py

# EAWebKit has a pretty complex package structure. It has lot of open source code as well as nested packages inside it.
# So we implement a custom script.

import os
import sys
import CodeStripper
import PlatformInfo

def printList(aList,listName):
    print 'Dumping %s:' % (listName)
    for element in aList:
        print element
        
    print 'End Dumping.'

# When stripping open source code, we always keep platforms below as active platforms. The reason is that otherwise we'll unnecessaryly strip some of the
# external open source packages from their original form.
_openSourcePlatforms = ['pc','osx', 'unix','sun']
# Modify the platform info for CodeStripper for some open source packages so we don't end up stripping their original code.
def GetOpenSourceStartupParams():
    
    customArgv = []
    for arg in sys.argv:
        if '--platforms=' in arg:
            newPlatform = arg
            for platform in _openSourcePlatforms:
                if not platform in arg:
                    if platform in PlatformInfo.GetPlatformsAvailable():
                        newPlatform += ',' + platform
            customArgv.append(newPlatform)
        else:
            customArgv.append(arg)

    return customArgv   

# We strip our nested and main package one by one. We also return the instance of CodeStripper used for any additional processing.  
# copyFilesFromSource is a list of extensions - membership means copy files with that extension
def StripPackage(startUpargs, rootDir, sourceDirList, copyFilesFromSource, excludeDirFromCopyList): 
    codeStripper = CodeStripper.CreateCodeStripper(startUpargs)
    if rootDir != '.':
        children = os.listdir(codeStripper.NormalizePackageRelativePath(rootDir))
        assert len(children) == 1, 'Found multiple package version. Curretly not supported'
        rootDir += '/'+children[0]
    
    #print rootDir
    
    #source code folders
    tempSourceDirList = []
    for sourceDir in sourceDirList:
        tempSourceDirList.append(rootDir+'/'+sourceDir)
    
    sourceDirList = tempSourceDirList
    #printList(sourceDirList,'sourceDirs')
    
    srcFolders = codeStripper.GenerateRecursiveFolderList(sourceDirList)
    excludedFolders = codeStripper.GetDefaultExcludedFolders()
    # we remove arm from here since you can have open source code that should not be stripped
    excludedFolders.remove('arm')
    srcFoldersToSkip = codeStripper.MatchingFolders(srcFolders,excludedFolders)
    srcFoldersToFilter = srcFolders - srcFoldersToSkip

    # Generate a list of files that will be filtered using unifdef
    sourceFileExtensions = codeStripper.GetDefaultSourceFileExtensions()
    sourceFileExtensions.add('.cc')
    srcFilesToFilter = codeStripper.GenerateFilteredFileList(srcFoldersToFilter, sourceFileExtensions)

    # unifdef the source files specified
    codeStripper.UnifdefFiles(srcFilesToFilter, codeStripper.GetDefaultUndefines(), codeStripper.GetDefaultDefines())

    # Copy all folders that are not considered source code folders. The files in these folders will not be modified.
    foldersToCopy = codeStripper.ExcludeFolders(codeStripper.GenerateRecursiveFolderList([rootDir]), excludeDirFromCopyList) - srcFolders
    codeStripper.CopyFolders(foldersToCopy)
    
    extraFiles = codeStripper.GenerateFilteredFileList(srcFolders - srcFoldersToSkip , copyFilesFromSource)
    codeStripper.CopyFiles(extraFiles)

    return codeStripper

codeStripper = StripPackage(sys.argv,'EAWebKitSupportPackages/EAAssertEAWebKit',['include','source'],[],[])

codeStripper = StripPackage(GetOpenSourceStartupParams(),'EAWebKitSupportPackages/cairo',['include','source'],[],[])

codeStripper = StripPackage(sys.argv,'EAWebKitSupportPackages/coreallocatorEAWebKit',['include','source'],[],[])

codeStripper = StripPackage(sys.argv,'EAWebKitSupportPackages/DirtySDKEAWebKit',['core','platform'],[],[])
codeStripper.CopyFolders(codeStripper.GenerateRecursiveFolderList(['EAWebKitSupportPackages/DirtySDKEAWebKit/local/core/project']))

codeStripper = StripPackage(sys.argv,'EAWebKitSupportPackages/EABaseEAWebKit',['include'],[],[])

codeStripper = StripPackage(sys.argv,'EAWebKitSupportPackages/EAIOEAWebKit',['include','source'],[],[])

codeStripper = StripPackage(sys.argv,'EAWebKitSupportPackages/EASTLEAWebKit',['include','source'],[],[])

codeStripper = StripPackage(sys.argv,'EAWebKitSupportPackages/EATextEAWebKit',['include','source'],[],[])

codeStripper = StripPackage(GetOpenSourceStartupParams(),'EAWebKitSupportPackages/FreeTypeEAWebKit',['freetype-2.4.8/include','freetype-2.4.8/src'],[],[])

codeStripper = StripPackage(GetOpenSourceStartupParams(),'EAWebKitSupportPackages/WTF',['wtf', 'icu', 'icu_osx', 'icu_osx_10.4'],['','.html','.js','.xcconfig','.pl','.asm','.sh','.gyp','.cmd','.def','.vcproj','.vsprops','.vsprop','.pbxproj','.table','.mm','.d','.txt','.pm','.data','.make','.sln','.pri','.plist','.py','.lib','.pro','.am','.exp','.gypi','.order','.in','.pthreads-win32'],[])

codeStripper = StripPackage(GetOpenSourceStartupParams(),'EAWebKitSupportPackages/JavaScriptCore',['JavaScriptCore'],['','.html','.js','.xcconfig','.pl','.asm','.sh','.gyp','.cmd','.def','.vcproj','.vsprops','.vsprop','.pbxproj','.table','.mm','.d','.txt','.pm','.data','.make','.sln','.pri','.plist','.py','.lib','.pro','.am','.exp','.gypi','.order','.in','.pthreads-win32'],[])

codeStripper = StripPackage(GetOpenSourceStartupParams(),'EAWebKitSupportPackages/libjpegEAWebKit',['include','source'],[],[])

codeStripper = StripPackage(GetOpenSourceStartupParams(),'EAWebKitSupportPackages/libpngEAWebKit',['include','source'],[],[])

codeStripper = StripPackage(GetOpenSourceStartupParams(),'EAWebKitSupportPackages/pixman',['include','source'],['.s','.template'],[])

codeStripper = StripPackage(sys.argv,'EAWebKitSupportPackages/PPMallocEAWebKit',['include','source'],[],[])

codeStripper = StripPackage(GetOpenSourceStartupParams(),'EAWebKitSupportPackages/SQLiteEAWebKit',['include','source'],[],[])

codeStripper = StripPackage(sys.argv,'EAWebKitSupportPackages/libxml2EAWebKit',['include','source'],[],[])

codeStripper = StripPackage(GetOpenSourceStartupParams(),'EAWebKitSupportPackages/zlibEAWebKit',['include','install'],[],[])

codeStripper = StripPackage(GetOpenSourceStartupParams(),'.',['WebCore','WebKit'],['','.html','.js','.xcconfig','.pl','.asm','.sh','.gyp','.cmd','.def','.vcproj','.vsprops','.vsprop','.pbxproj','.table','.mm','.d','.txt','.pm','.data','.make','.sln','.pri','.plist','.py','.lib','.pro','.am','.exp','.gypi','.order','.in','.pthreads-win32','.nib','.aps','.rc','.idl','.strings','.qrc','.css','.png','.json','.tiff','.excludes','.y','.1','.wav','.gperf','.cmake','.gif','.re2js','.flex', '.dep', '.icc', '.symbols', '.vcprojx', '.xcodeproj', '.mm', '.includes', '.edc', '.po', '.resources'],['build','doc','distribution','eawebkitsupportpackages'])

# Copy bins
# Create a new instace of codeStripper since we modified the platform info earlier to never strip out open source code
# but we want to strip out bins
codeStripper = CodeStripper.CreateCodeStripper(sys.argv)
binFolders = codeStripper.GenerateRecursiveFolderList(['./Distribution'])
additionalExcludedFolders = set(['dev-debug'])
for arg in sys.argv:
    if 'platforms=' in arg:
        if not 'pc' in arg:
            additionalExcludedFolders.add('pc64')

binFoldersToSkip = codeStripper.MatchingFolders(binFolders, set.union(codeStripper.GetDefaultExcludedFolders(),additionalExcludedFolders))
codeStripper.CopyFolders(binFolders-binFoldersToSkip)
