#include <jni.h>
#include <android/native_window_jni.h>
#include <android/log.h>
#include <android/native_activity.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <pthread.h>
#include "Core.h"

#define LOG_TAG "CubeCraft"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static EGLDisplay display = EGL_NO_DISPLAY;
static EGLSurface surface = EGL_NO_SURFACE;
static EGLContext context = EGL_NO_CONTEXT;
static Core* core = nullptr;

static pthread_t renderThread;
static bool rendering = false;

bool initEGL(ANativeWindow* window) {
    const EGLint configAttribs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_DEPTH_SIZE, 16,
            EGL_NONE
    };

    EGLint numConfigs;
    EGLConfig config;

    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        LOGE("Unable to get EGL display");
        return false;
    }

    if (!eglInitialize(display, nullptr, nullptr)) {
        LOGE("Unable to initialize EGL");
        return false;
    }

    if (!eglChooseConfig(display, configAttribs, &config, 1, &numConfigs)) {
        LOGE("Unable to choose EGL config");
        return false;
    }

    surface = eglCreateWindowSurface(display, config, window, nullptr);
    if (surface == EGL_NO_SURFACE) {
        LOGE("Unable to create EGL surface");
        return false;
    }

    const EGLint contextAttribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 3,
            EGL_NONE
    };
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
    if (context == EGL_NO_CONTEXT) {
        LOGE("Unable to create EGL context");
        return false;
    }

    if (!eglMakeCurrent(display, surface, surface, context)) {
        LOGE("Unable to make EGL context current");
        return false;
    }

    LOGI("EGL successfully initialized");
    return true;
}

void terminateEGL() {
    if (display != EGL_NO_DISPLAY) {
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

        if (context != EGL_NO_CONTEXT) {
            eglDestroyContext(display, context);
        }
        if (surface != EGL_NO_SURFACE) {
            eglDestroySurface(display, surface);
        }
        eglTerminate(display);
    }

    display = EGL_NO_DISPLAY;
    surface = EGL_NO_SURFACE;
    context = EGL_NO_CONTEXT;
}
static ANativeWindow* window = nullptr;

void* renderLoop(void* arg) {
    LOGI("Render thread started");

    if (!initEGL(window)) {
        LOGE("EGL init failed in render thread");
        return nullptr;
    }

    core = new Core();
    if (!core->init()) {
        LOGE("Core init failed");
        return nullptr;
    }

    core->resize(ANativeWindow_getWidth(window), ANativeWindow_getHeight(window));

    while (rendering) {
        core->update(0.016f);
        core->render();
        eglSwapBuffers(display, surface);

        struct timespec ts = {0, 16 * 1000 * 1000};
        nanosleep(&ts, nullptr);
    }

    delete core;
    core = nullptr;

    terminateEGL();

    LOGI("Render thread finished");
    return nullptr;
}

void onNativeWindowCreated(ANativeActivity* activity, ANativeWindow* nativeWindow) {
    LOGI("onNativeWindowCreated");

    if (window != nullptr) {
        ANativeWindow_release(window);
    }

    window = nativeWindow;
    ANativeWindow_acquire(window);

    rendering = true;
    pthread_create(&renderThread, nullptr, renderLoop, nullptr);
}

void onNativeWindowDestroyed(ANativeActivity* activity, ANativeWindow* nativeWindow) {
    LOGI("onNativeWindowDestroyed");

    rendering = false;
    pthread_join(renderThread, nullptr);

    if (window != nullptr) {
        ANativeWindow_release(window);
        window = nullptr;
    }
}

void ANativeActivity_onCreate(ANativeActivity* activity,
                              void* savedState,
                              size_t savedStateSize) {
    LOGI("ANativeActivity_onCreate called");

    activity->callbacks->onNativeWindowCreated = onNativeWindowCreated;
    activity->callbacks->onNativeWindowDestroyed = onNativeWindowDestroyed;
}
