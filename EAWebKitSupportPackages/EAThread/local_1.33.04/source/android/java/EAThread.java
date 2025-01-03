///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

package com.ea.EAThread;

import java.lang.Thread;
import java.lang.System;

public class EAThread
{
    // EAThread may be optionally initialized from Java. If EAThread is initialized from Java
    // it will automatically attach and detach new threads from the JVM. Allowing EAThread to do
    // these operation once per thread should be more efficient than constantly attaching and
    // detaching from the JVM when performing JNI calls. There currently isn't an associated 
    // Shutdown function that needs to be called.
    // 
    // import com.ea.EAThread.EAThread;
    //    ...
    // EAThread.Init();

    public static native void Init();

    public static void setCurrentThreadName(String name)
    {
        Thread.currentThread().setName(name);
    }
}

