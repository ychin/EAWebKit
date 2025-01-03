
package com.ea.EAIOTest;
import android.app.Activity;
import android.os.Bundle;
import android.content.Context;
import android.os.Environment;
import java.io.File;
import java.nio.ByteBuffer;
import java.nio.ShortBuffer;
import com.ea.EAIO.EAIO;
import com.ea.EAThread.EAThread;

public class EAIOTest extends Activity {
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        File fileRoot = getFilesDir();
        File externalRoot = Environment.getExternalStorageDirectory();

        EAIO.Startup(this);
        EAThread.Init();

        int ret = runEntryPoint(fileRoot.getAbsolutePath(), externalRoot.getAbsolutePath() + "/com.ea.EAIOTest");
        setResult(ret);
        
        EAIO.Shutdown();

        finish();
        System.out.println("AndroidSDK RUN COMPLETED. RETURN CODE: " + ret);
    }
    public native int runEntryPoint(String fileRoot, String externalFileRoot);
    public native void nativeGLRender(int width, int height);
    private static int sViewWidth;
    private static int sViewHeight;

    static {
        System.loadLibrary("EAIOTest");
    }
}

