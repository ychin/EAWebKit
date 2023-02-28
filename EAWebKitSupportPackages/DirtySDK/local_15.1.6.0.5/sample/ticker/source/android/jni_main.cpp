#include <jni.h>
#include <stdio.h>
#include <string.h>

extern int main(int argc, char** argv);

// takes a JNIEnv* and a jobect that is a reference to android activty object
// and extracts ea-style command line arugments from the activity's intent
// bundle then passes them to external main function
int jni_main(JNIEnv* env, jobject activity)
{
    int argc = 0;
    char** argv = NULL;

    // check if intent bundle has activity arguments following EA conventions
    {
        // java: Intent intent = getIntent();
        jclass activityClass = env->GetObjectClass(activity);
        jmethodID getIntentMethodId = env->GetMethodID(activityClass, "getIntent", "()Landroid/content/Intent;");
        jobject intent = env->CallObjectMethod(activity, getIntentMethodId);
        env->DeleteLocalRef(activityClass);
        
        // java: Bundle intentBundle = intent.getExtras();
        jclass intentClass = env->GetObjectClass(intent);
        jmethodID getExtrasMethodId = env->GetMethodID(intentClass, "getExtras", "()Landroid/os/Bundle;");
        jobject intentBundle = env->CallObjectMethod(intent, getExtrasMethodId);
        env->DeleteLocalRef(intentClass);
        
        // java: if (intentBundle != null)
        if (intentBundle != NULL) // jobject is actually void* to java mem location so null jobject == null ptr
        {
            // java: int argc = extraBundle.getInt("argc");    
            jclass bundleClass = env->GetObjectClass(intentBundle);
            jmethodID getIntMethodId = env->GetMethodID(bundleClass, "getInt", "(Ljava/lang/String;)I");
            const int argcBufferSize = 16;
            char argCString[argcBufferSize];
            sprintf(argCString, "argc");
            jstring argcArgument = env->NewStringUTF(argCString);
            argc = env->CallIntMethod(intentBundle, getIntMethodId, argcArgument); // getInt returns 0 if key doesn't exist, this works for our purpose
            env->DeleteLocalRef(argcArgument);
            // java: if (argc > 0)
            if (argc > 0)
            {
                // allocate space for arguments
                argv = new char*[argc]();
                    
                // java: for (int i = 0; i < argc; ++i)
                jmethodID getStringMethodId = env->GetMethodID(bundleClass, "getString", "(Ljava/lang/String;)Ljava/lang/String;");
                for (int i = 0; i < argc; ++i)
                {
                    // java: argument = extraBundle.getString("arg" + i);
                    sprintf(argCString, "arg%d", i);
                    jstring argIndexArgument = env->NewStringUTF(argCString);
                    jstring argument = static_cast<jstring>(env->CallObjectMethod(intentBundle, getStringMethodId, argIndexArgument));
                    env->DeleteLocalRef(argIndexArgument);

                    // copy to argv
                    jboolean isCopy = JNI_FALSE;
                    const char* cStrArgument = env->GetStringUTFChars(argument, &isCopy);
                    size_t argLen = strlen(cStrArgument) + 1;
                    argv[i] = new char[argLen];
                    strcpy(argv[i], cStrArgument);
                    env->ReleaseStringUTFChars(argument, cStrArgument);
                    env->DeleteLocalRef(argument);
                }
            }

            env->DeleteLocalRef(bundleClass);
            env->DeleteLocalRef(intentBundle);
        }
    }
    
    // call platform agnositc main
    int ret = main(argc, argv); 

    // clean up argv
    for (int i = 0; i <    argc; ++i)
    {
        delete[] argv[i];
    }
    delete[] argv;
    
    return ret;
}
