
package com.ea.EATextTest;
import android.app.Activity;
import android.os.Bundle;
import android.content.Context;
import android.os.Environment;
import android.os.Handler;
import java.io.File;
import java.nio.ByteBuffer;
import java.nio.ShortBuffer;
import com.ea.EAIO.EAIO;
import com.ea.EAThread.EAThread;
import android.content.Context;
import android.os.AsyncTask;

public class EATextTest extends Activity{    
Handler handler;
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        handler = new Handler();                        
    
        InitTask _initTask = new InitTask();
        _initTask.execute( this );
    
    }

    protected class InitTask extends AsyncTask<Context, Void, Integer>
    {
        @Override
        protected Integer doInBackground( Context... params ) {
          File fileRoot = getFilesDir();
          File externalRoot = Environment.getExternalStorageDirectory();

          EAIO.Startup(EATextTest.this);
          EAThread.Init();
        
          final int ret = runEntryPoint(fileRoot.getAbsolutePath(), externalRoot.getAbsolutePath() + "/com.ea.EATextTest");
          setResult(ret);

          EAIO.Shutdown();        
                        
          handler.post(new Runnable() {
              public void run() {
                finish(); 
                System.out.println("AndroidSDK RUN COMPLETED. RETURN CODE: " + ret);               
              }
          });
          return 0;
         }
    }    
                
    public native int runEntryPoint(String fileRoot, String externalFileRoot);
    public native void nativeGLRender(int width, int height);
    private static int sViewWidth;
    private static int sViewHeight;

    static {
        System.loadLibrary("EATextTest");
    }
}

