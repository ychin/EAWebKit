import sys
import CodeStripper
import PlatformInfo
import os
import time
import shutil

# Removes folders specific to non-active platforms from src
def RemovePlatformSpecific(src, argv):
    newsrc = set([])
    xbox = set([])
    ps3 = set([])
    wii = set([])
    srcSet = set([])
    removeSet = set([])
    xboxList = ['xenon', '360', 'Xenon', 'xbox']
    ps3List = ['ps3', 'PS3']
    psp2List = ['psp2', 'vita']
    wiiList = ['rev', 'wii', 'Wii', 'WII']
    androidList = ['android']
    iosList = ['ios', 'iphone', 'iOS', 'iPhone']
    unixList = ['unix', 'linux']
    playbookList = ['playbook']
    pcList = ['pc']
    argvList = []

    nameList = [xboxList, ps3List, psp2List, wiiList, androidList, iosList, unixList, playbookList, pcList]
    
    for list in nameList:
        keepList = False
        for element in list:
            if element.lower() in argv: #if true, we have found the list associated with the platform we want to keep after stripping
                keepList=True
                break
        if not keepList:
            argvList.extend(list) #argvList contains strings associated with the platform(s) we want to get rid of

    for x in src: #x is file or folder name
        add = 1
        for platform in argvList:
            if platform in x: #if file or folder name is for a specific platform we want to get rid of
                add = 0
                break
        # Check if we should keep this file
        if add == 0:
            for file in _alwaysIncludedFiles:
                if file in x:
                    add = 1
                    break
        # This is not a platform specific file for a platform we do not want
        if add == 1:
            srcSet.add(x)

    return srcSet

def PrintList(objList, listName):
    print 'Dumping %s:' % (listName)
    
    # Output the list
    for element in objList:
        print element

    print 'End dumping %s.' % (listName)

# Preprocess startup params (sys.argv)
def GetStartupParams():
    # We wanted to make some platforms as always-active platforms
    customArgv = []
    for arg in sys.argv:
        if '--platforms=' in arg:
            newPlatform = arg
            # Append our always-active platforms to argv[]
            for platform in _alwaysActivePlatforms:
                if not platform in arg:
                    # Append only if CodeStripper recognizes it
                    if platform in PlatformInfo.GetPlatformsAvailable():
                        newPlatform += ',' + platform
            customArgv.append(newPlatform)
        else:
            customArgv.append(arg)

    return customArgv

# Update PlatformInfo to include our defines
def UpdatePlatformInfo(x):
    availablePlatforms = PlatformInfo.GetPlatformsAvailable()
    if x.name in availablePlatforms:
        oldPlatforms = PlatformInfo.GetPlatformInfoForPlatforms([x.name])
        platformInfo = oldPlatforms[0]
        platformInfo.alwaysDefines = set.union(x.alwaysDefines, platformInfo.alwaysDefines)
        platformInfo.possibleDefines = set.union(x.possibleDefines, platformInfo.possibleDefines)
        platformInfo.possibleFolder = set.union(x.possibleFolder, platformInfo.possibleFolder)
        PlatformInfo.AddPlatform(platformInfo)
    else:
        PlatformInfo.AddPlatform(x)

# These platforms are always treated as active platforms
_alwaysActivePlatforms = ['pc', 'unix']

# These files are always included regardless of its platform is active or not
_alwaysIncludedFiles = ['testercomm_xenon_client.c', 'testercomm_xenon_host.c']

try:
    UpdatePlatformInfo(PlatformInfo.PlatformInfo(
        name = 'pc',
        alwaysDefines = ['DIRTYCODE_PC'],
        possibleDefines = [],
        possibleFolder = []
        ))
    UpdatePlatformInfo(PlatformInfo.PlatformInfo(
        name = 'iphone',
        alwaysDefines = ['DIRTYCODE_APPLEIOS'],
        possibleDefines = [],
        possibleFolder = []
        ))
    UpdatePlatformInfo(PlatformInfo.PlatformInfo(
        name = 'android',
        alwaysDefines = ['DIRTYCODE_ANDROID'],
        possibleDefines = [],
        possibleFolder = []
        ))
    UpdatePlatformInfo(PlatformInfo.PlatformInfo(
        name = 'unix',
        alwaysDefines = ['DIRTYCODE_LINUX', '__linux__'],
        possibleDefines = [],
        possibleFolder = ['unix']
        ))

    # Instantiate CodeStripper    
    codeStripper = CodeStripper.CreateCodeStripper(GetStartupParams())

    # Active platform list (lowercased) specified by --platforms=
    platformList = []
    for platform in codeStripper.GetActivePlatformNames():
        platformList.append(platform.lower())
    if codeStripper.options.verbose:
        PrintList(platformList, 'Active Platforms')

    # Generate a list of folders containing source code that will be filtered as well as some
    # source folders that should be omitted entirely from the stripped package.
    includedSubDirectories = [os.path.abspath(codeStripper.NormalizePackageRelativePath(''))]
    srcFolders = codeStripper.GenerateRecursiveFolderList(includedSubDirectories)

    # Remove folders specific to non-active platforms
    srcFolders = RemovePlatformSpecific(srcFolders, platformList)
    srcFoldersCopy = srcFolders

    # Remove the build directories
    for folder in srcFoldersCopy:
        if 'build' in folder or 'BuildLog' in folder:
            srcFolders = srcFolders - set([folder])

    # DirtySDK's source/header files are not only putted under 'source'/'src'/'include' but also other folders
    # We will exclude the .c/.cpp files later if we're not including source
    srcFoldersToFilter = srcFolders
    
    # Generate a list of files that will be filtered using unifdef
    srcFilesToFilter = codeStripper.GenerateFilteredFileList(srcFoldersToFilter, codeStripper.GetDefaultSourceFileExtensions())

    # Generate a list of files that will be copied directly
    srcFilesToCopy = codeStripper.GenerateFileList(srcFolders)
    # We need to exclude the .c/.cpp files if we're not including source files
    # This is not a nice way to do that but we have no other choice
    if not codeStripper.ShouldIncludeSource():
        # Generate the .c/.cpp file list
        cppFiles = codeStripper.GenerateFilteredFileList(srcFolders, set(['.cpp', '.c']))
        # No need to filter them since we don't copy them
        srcFilesToFilter -= cppFiles
        # Exclude the source files from the copy list
        srcFilesToCopy -= cppFiles

    srcFilesToFilter = RemovePlatformSpecific(srcFilesToFilter, platformList)
    srcFilesToCopy = RemovePlatformSpecific(srcFilesToCopy, platformList)

    # psp2 uses dirtyaddrps3.c that is under ps3
    if 'psp2' in platformList and not 'ps3' in platformList:
        if codeStripper.ShouldIncludeSource():
            srcFilesToFilter.add(includedSubDirectories[0] + '\source\dirtysock\ps3\dirtyaddrps3.c')
            srcFilesToCopy.add(includedSubDirectories[0] + '\source\dirtysock\ps3\dirtyaddrps3.c')

    # Get the default set of undefines.  These undefines are determined based on the currently
    # active platforms which are specified by the user.
    undefines = codeStripper.GetDefaultUndefines()
    if codeStripper.options.verbose:
        PrintList(undefines, 'Undefines')

    # Get the default set of defines.  These defines are determined based on the currently
    # active platforms which are specified by the user.
    defines = codeStripper.GetDefaultDefines()

    # Customize defines
    if not 'xenon' in platformList:
        defines.add('TESTER_NO_XBDM')

    if codeStripper.options.verbose:
        PrintList(defines, 'Defines')

    codeStripper.CopyFiles(srcFilesToCopy) #copies DirtySDK folders to new location
    codeStripper.UnifdefFiles(srcFilesToFilter, undefines, defines) #strips the DirtySDK files
    exit(0)

except KeyboardInterrupt:

    exit(0)
