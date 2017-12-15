#!/usr/bin/env python

# Copyright (C) 2015 Electronic Arts, Inc. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1.  Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
# 2.  Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES

# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import glob
import os
import re
import sys


# It's fragile to rely on the location of this script to find the top-level
# source directory.
#+EAWebKitChange
#09/14/2015
#Adjusted path to match ea's file layout
TOP_LEVEL_DIRECTORY = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))))
#-EAWebKitChange
WEBKIT_LIBRARIES = os.environ['WEBKIT_LIBRARIES'];

def main():
    react_to_vsprops_changes()
    react_to_webkit1_interface_changes()


def react_to_vsprops_changes():
    vsprops_directory = os.path.join(WEBKIT_LIBRARIES, 'tools', 'vsprops')
    newest_vsprops_time = mtime_of_newest_file_matching_glob(os.path.join(vsprops_directory, '*.props'))

    obj_directory = os.path.join(os.environ['CONFIGURATIONBUILDDIR'], 'obj32')

    # Visual Studio isn't smart enough to figure out it needs to rebuild these file types when
    # .vsprops files change (even if we touch wtf/Platform.h below), so we delete them to force them
    # to be rebuilt.
    for extension in ('dep', 'manifest', 'pch', 'res'):
        for filepath in glob.iglob(os.path.join(obj_directory, '*', '*.%s' % extension)):
            delete_if_older_than(filepath, newest_vsprops_time)

    # Touch wtf/Platform.h so all files will be recompiled. This is necessary
    # to pick up changes to preprocessor macros (e.g., ENABLE_*).
	#+EAWebKitChange
	#09/14/2015
	#Adjusted path to match ea's file layout
    wtf_platform_h = os.path.join(TOP_LEVEL_DIRECTORY, 'EAWebKitSupportPackages', 'WTF', 'local', 'wtf', 'Platform.h')
    touch_if_older_than(wtf_platform_h, newest_vsprops_time)
	#-EAWebKitChange

def react_to_webkit1_interface_changes():
	#+EAWebKitChange
	#09/14/2015
	#Adjusted path to match ea's file layout
    interfaces_directory = os.path.join(TOP_LEVEL_DIRECTORY, 'WebKit', 'win', 'Interfaces')
	#-EAWebKitChange
    newest_idl_time = mtime_of_newest_file_matching_glob(os.path.join(interfaces_directory, '*.idl'))
    # WebKit.idl includes all the other IDL files, so needs to be rebuilt if any IDL file changes.
    # But Visual Studio isn't smart enough to figure this out, so we touch WebKit.idl to ensure that
    # it gets rebuilt.
    touch_if_older_than(os.path.join(interfaces_directory, 'WebKit.idl'), newest_idl_time)


def mtime_of_newest_file_matching_glob(glob_pattern):
    files = glob.glob(glob_pattern)
    assert len(files), "Couldn't find any files matching glob %s" % glob_pattern
    return max(map(os.path.getmtime, files))


def delete_if_older_than(path, reference_time):
    if os.path.getmtime(path) < reference_time:
        print 'Deleting %s' % path
        os.remove(path)


def touch_if_older_than(path, reference_time):
    if os.path.getmtime(path) < reference_time:
        print 'Touching %s' % path
        os.utime(path, None)


if __name__ == '__main__':
    sys.exit(main())
