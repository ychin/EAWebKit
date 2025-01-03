package com.ea.EAIO;
import android.app.Activity;
import android.content.res.AssetManager;
import android.os.Environment;
import java.io.File;

public class EAIO
{
    // Before using any EAIO application package files it is necessary to
    // register EAIO with the Java runtime. To do so simply call this
    // startup method in your Java code on app startup and pass in the 
    // AssetManager for your Activity.
    //
    // For example you may want to call the method from you onCreate method:
    //     import com.ea.EAIO.EAIO;
    //     ...
    //     EAIO.Startup(this);    // When called from the Activity onCreate method.
    //     ...
    //     EAIO.Shutdown();

    public static void Startup(Activity activity)
    {
        File dataRoot     = Environment.getDataDirectory();
        File fileRoot     = activity.getFilesDir();
        File externalRoot = Environment.getExternalStorageDirectory();

        StartupNativeImpl(activity.getAssets(), dataRoot.getAbsolutePath(), fileRoot.getAbsolutePath(), externalRoot.getAbsolutePath());
    }


    // If you previously have called Startup, then the Shutdown method should
    // be called by your application before it exits.  This is necessary to
    // prevent leaking Java objects that were created by C++.

    public static native void Shutdown();


    // Internal function
    private static native void StartupNativeImpl(AssetManager AssetManager, String dataDir, String filesDir, String externalDir);
}
