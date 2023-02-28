#include "android_native_app_glue.h"
#include <android/log.h>
#include <unistd.h>
#include "jni_main.cpp"

extern __attribute__((weak)) void jni_testsetup(struct android_app* app);

static bool g_isForegroundState = false;

#define ANDROID_DEBUG(_text) __android_log_write(ANDROID_LOG_INFO, "AndroidConfig", _text)
#define ANDROID_CMD_INFO(_cmd) case _cmd: ANDROID_DEBUG(#_cmd); break;

static void handle_cmd(struct android_app* app, int32_t cmd)
{
    switch (cmd)
    {
    ANDROID_CMD_INFO(APP_CMD_INPUT_CHANGED);

    case APP_CMD_INIT_WINDOW:
        ANDROID_DEBUG("NATIVE APP_CMD_INIT_WINDOW");
        g_isForegroundState = true;
        break;

    ANDROID_CMD_INFO(APP_CMD_TERM_WINDOW);
    ANDROID_CMD_INFO(APP_CMD_WINDOW_RESIZED);
    ANDROID_CMD_INFO(APP_CMD_WINDOW_REDRAW_NEEDED);
    ANDROID_CMD_INFO(APP_CMD_CONTENT_RECT_CHANGED);
    ANDROID_CMD_INFO(APP_CMD_GAINED_FOCUS);
    ANDROID_CMD_INFO(APP_CMD_LOST_FOCUS);
    ANDROID_CMD_INFO(APP_CMD_CONFIG_CHANGED);
    ANDROID_CMD_INFO(APP_CMD_LOW_MEMORY);
    ANDROID_CMD_INFO(APP_CMD_START);
    ANDROID_CMD_INFO(APP_CMD_RESUME);

    case APP_CMD_SAVE_STATE:
        ANDROID_DEBUG("APP_CMD_SAVE_STATE");
        g_isForegroundState = false;
        break;

    ANDROID_CMD_INFO(APP_CMD_PAUSE);
    ANDROID_CMD_INFO(APP_CMD_STOP);
    ANDROID_CMD_INFO(APP_CMD_DESTROY);
    }
}

void android_main(struct android_app* app)
{
    JNIEnv* env; 
    const ANativeActivity* activity = app->activity;
    int getEnvResult = activity->vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
    if (getEnvResult == JNI_EDETACHED)
    {
        getEnvResult = activity->vm->AttachCurrentThread(&env, nullptr);
    }

    if (getEnvResult != JNI_OK)
    {
        raise(SIGTRAP);
    }

    app->onAppCmd = handle_cmd;

    while (true)
    {
        // Read all pending events.
        int ident;
        int events;
        struct android_poll_source* source;

        while ((ident = ALooper_pollAll(0, NULL, &events, (void**)&source)) >= 0)
        {
            // Process this event.
            if (source != NULL)
                source->process(app, source);

            // Check if we are exiting.
            if (app->destroyRequested != 0)
            {
                ANDROID_DEBUG("DESTROYING APP");
                return;
            }
        }

        if (g_isForegroundState)
        {
            if (jni_testsetup)
                jni_testsetup(app);

            jni_main(env, activity->clazz);

            break;
        }
    }

    activity->vm->DetachCurrentThread();

    // TODO set activity result?
    // TODO finish activity?
}
