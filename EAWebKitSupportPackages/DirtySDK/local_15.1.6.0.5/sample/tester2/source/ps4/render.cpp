/*H*************************************************************************************/
/*!

    \File render.cpp

    \Description
        Basic rendering for sample, lagely copied from sony sample

    \Copyright
        Copyright (c) Electronic Arts 2004.  ALL RIGHTS RESERVED.

    \Version    1.0 06/04/2013 (cvienneau) First Version
*/
/*************************************************************************************H*/

#include "render.h"
#if defined(DIRTYCODE_PS4)

// All private functions are copied from sony sample:
// C:\Program Files (x86)\SCE\ORBIS SDKs\0.930\target\samples\sample_code\graphics\api_video_out\basic\main.cpp

/* SCE CONFIDENTIAL
 * ORBIS Programmer Tool Runtime Library Release 00.930.020
 * Copyright (C) 2013 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */
/* most basic sample program for libSceVideoOut */

#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <kernel.h>
#include <video_out.h>
#include <user_service.h>
#include <gnm.h>

#define DISPLAY_BUFFER_NUM 2 // minimum 2 required
#define FLIP_RATE 0 // 0: no fliprate is set. 1: 30fps on 60Hz video output  2: 20fps
#define FLIP_MODE SCE_VIDEO_OUT_FLIP_MODE_VSYNC // or _HSYNC

#define SPEED 10 // swing speed of the bar
#define LOOP_DURATION 5 // in seconds
#define SLEEP_BEFORE_FINISH 2 // in seconds


static uint32_t s_defaultColor=0x80ffffff; // back ground color. in ARGB 
static uint32_t s_barColor=0x802234BD; // bar color. in ARGB


#define ATTRIBUTE_WIDTH 1280 // must be aligned to 64pixel
#define ATTRIBUTE_HEIGHT 720

#define LINE_INTERVAL 200


#define VIDEO_MEMORY_STACK_SIZE (1024*1024*128)


// display buffer attributes
static const int s_width=ATTRIBUTE_WIDTH;
static const int s_pitch=roundup(s_width,64); // tiling_mode_linear
static const int s_height=ATTRIBUTE_HEIGHT;
static const int s_pixelFormat=SCE_VIDEO_OUT_PIXEL_FORMAT_A8R8G8B8_SRGB;
static const int s_bpp=sizeof(uint32_t);
static const int s_barWidth=s_width/64;
static const int s_tilingMode=SCE_VIDEO_OUT_TILING_MODE_LINEAR;

static void *s_surfaceAddress[DISPLAY_BUFFER_NUM]={0};
static int64_t s_flipArgLog[DISPLAY_BUFFER_NUM];


/*
 *  simple memory allocation functions for video direct memory
 */
static uint64_t s_videoMemoryStackPointer=0;
static uint32_t s_videoMemoryStackSize=1024 * 1024 * 192;
static uint64_t s_videoMemoryStackMax;
static uint64_t s_videoMemoryStackBase=0;
static off_t s_videoMemoryOffset;

using namespace sce::Gnm;

static void *videoMalloc(int size)
{
    uint64_t offset=s_videoMemoryStackPointer;
    s_videoMemoryStackPointer+=size;

    if (s_videoMemoryStackPointer>s_videoMemoryStackMax) return NULL;

    return (void *)(offset);
}


static void videoMallocInitialize(void)
{
    int ret;
    off_t offsetOut;

    const uint32_t  shaderGpuAlignment  = 2 * 1024 * 1024;

    ret = sceKernelAllocateDirectMemory(
            0,
            SCE_KERNEL_MAIN_DMEM_SIZE,
            s_videoMemoryStackSize,
            shaderGpuAlignment,
            SCE_KERNEL_WC_GARLIC,
            &offsetOut);
    if (ret){
        assert((ret==0));
    }

    s_videoMemoryOffset=offsetOut;

    void* ptr = NULL;
    ret = sceKernelMapDirectMemory(
               &ptr,
               s_videoMemoryStackSize,
               SCE_KERNEL_PROT_CPU_READ|SCE_KERNEL_PROT_CPU_WRITE|SCE_KERNEL_PROT_GPU_READ|SCE_KERNEL_PROT_GPU_ALL,
               0, //flags
               offsetOut,
               shaderGpuAlignment);
    assert((ret>=0));


    s_videoMemoryStackBase = (uintptr_t)ptr;
    s_videoMemoryStackMax=s_videoMemoryStackBase+s_videoMemoryStackSize;

    s_videoMemoryStackPointer=s_videoMemoryStackBase;

}

/* 
 * simple functions to draw to surface by cpu
 */
static void writeColor(void *offset, int x, int y, uint32_t baseColor, int withLines)
{
    int color;
    int pixel = (y * s_pitch) + x;

    color=baseColor;

    if (withLines){
        if ((x%LINE_INTERVAL==0) || (y%LINE_INTERVAL==0)) color= ~baseColor;
    }

    ((uint32_t *)offset)[pixel]= color;
}


static void drawRectByColorWithLines(void *offset, int x0, int w, int y0, int h, uint32_t color)
{
    int x, y;
    for (y = y0; y < y0+h; y++) {
        for (x = x0; x < x0+w; x++) {
            writeColor(offset, x, y, color, 1);
        }
    }
}

static void fillBufferByColorWithLines(void *offset, uint32_t color)
{
    drawRectByColorWithLines(offset, 0, s_width, 0, s_height, color);
}

// update display buffer (usually done by gpu, but this is by cpu to make this sample simple)
static void updateDisplayBuffer(int bufferIndex, int loop, int rate)
{
    static int xHist[DISPLAY_BUFFER_NUM]={0};
    static int direction= SPEED;
    static int x0=s_width/2;
    static int width=s_barWidth;

    int delta=direction*(rate+1);

    // erase bar in the past frame on the buffer
    if (xHist[bufferIndex]!=0){
        drawRectByColorWithLines(s_surfaceAddress[bufferIndex], xHist[bufferIndex], width, 0, s_height, s_defaultColor);
    }

    // swing
    x0+=delta;

    // draw bar
    drawRectByColorWithLines(s_surfaceAddress[bufferIndex], x0, width, 0, s_height, s_barColor);


    xHist[bufferIndex]=x0;

    // turn swing direction if necessary
    if (((direction>0)&&(x0+width+delta)>=s_width)
        || ((direction<0)&&(x0+delta<=0))){
        direction*= -1;
    }

}


/*
 * utility functions for libSceVideoOut
 */
// setup VideoOut and acquire handle
static int32_t initializeVideoOut(SceKernelEqueue *eqFlip)
{
    int videoOutHandle;
    int ret;

    ret=sceVideoOutOpen(SCE_USER_SERVICE_USER_ID_SYSTEM, SCE_VIDEO_OUT_BUS_TYPE_MAIN, 0, NULL);
    assert((ret>=0));

    videoOutHandle=ret;

    ret=sceKernelCreateEqueue(eqFlip, "eq to wait flip");
    assert((ret>=0));

    ret=sceVideoOutAddFlipEvent(*eqFlip, videoOutHandle, NULL);
    assert((ret>=0));


    return videoOutHandle;
}

// register display buffers
static int initializeDisplayBuffer(int32_t videoOutHandle, void **addresses, int num, int startIndex)
{
    SceVideoOutBufferAttribute attribute;
    sceVideoOutSetBufferAttribute(&attribute, 
                                  s_pixelFormat,
                                  s_tilingMode,
                                  SCE_VIDEO_OUT_ASPECT_RATIO_16_9,
                                  ATTRIBUTE_WIDTH,
                                  ATTRIBUTE_HEIGHT,
                                  s_pitch);

    // register the buffers to the slot: [startIndex..startIndex+num-1]
    int ret= sceVideoOutRegisterBuffers(videoOutHandle, startIndex, addresses, num, &attribute);
    assert((ret>=0));

    return ret;
}

// allocate direct memory for display buffers
static void allocateDisplayBuffers(void **addresses, int displayBufferNum)
{
    int i;

    uint32_t bufferSize=s_pitch*s_height*s_bpp;
    for ( i = 0; i < displayBufferNum; i++ ) {
        addresses[i]= videoMalloc(bufferSize);
        assert((addresses[i]!=NULL));
    }
}

// wait until no flip is on pending
static int waitFlipAllFinished(int videoOutHandle, SceKernelEqueue *eqFlip)
{
    int ret;

    while (sceVideoOutIsFlipPending(videoOutHandle)!=0){
        SceKernelEvent ev;
        int out;
        ret=sceKernelWaitEqueue(*eqFlip, &ev, 1, &out, 0);
        assert((ret>=0));
    }


    return 0;
}

// wait until incremental flipArg (specified at SubmitFlip) reaches the value
static int waitFlipArg(int videoOutHandle, SceKernelEqueue *eqFlip, int64_t flipArg)
{
    int ret;

    while (1){
        SceVideoOutFlipStatus status;
        ret=sceVideoOutGetFlipStatus(videoOutHandle, &status);
        assert((ret>=0));

        if (status.flipArg>=flipArg){
            break;
        }

        SceKernelEvent ev;
        int out;
        ret=sceKernelWaitEqueue(*eqFlip, &ev, 1, &out, 0);
        assert((ret>=0));
    }


    return 0;
}

/*** Public Functions ******************************************************************/

int32_t T2RenderInit(T2Render *pApp)
{
    // setup VideoOut and acquire handle
    int32_t iRet = pApp->iVideoOut = initializeVideoOut(&pApp->eqFlip);

    if (iRet >= 0)
    {
        // setup direct memory 
        videoMallocInitialize();

        // allocate display buffers
        allocateDisplayBuffers(s_surfaceAddress, DISPLAY_BUFFER_NUM);

        // register display buffers
        iRet = initializeDisplayBuffer(pApp->iVideoOut, s_surfaceAddress, DISPLAY_BUFFER_NUM, 0);
        if (iRet >= 0)
        {
            // status of each buffer with flipArg
            for ( pApp->ibufferIndex = 0; pApp->ibufferIndex <DISPLAY_BUFFER_NUM ; pApp->ibufferIndex++ ) {
                s_flipArgLog[pApp->ibufferIndex]= SCE_VIDEO_OUT_BUFFER_INITIAL_FLIP_ARG-1; // smaller than initial value
            }

            // prepare initial images to the display buffers
            for ( pApp->ibufferIndex = 0; pApp->ibufferIndex <DISPLAY_BUFFER_NUM ; pApp->ibufferIndex++ ) {
                fillBufferByColorWithLines(s_surfaceAddress[pApp->ibufferIndex], s_defaultColor);
            }

            iRet = sceVideoOutSetFlipRate(pApp->iVideoOut, FLIP_RATE);
            if (iRet >= 0)
            {
                pApp->ibufferIndex = 0;
                pApp->loop = 0;
                pApp->flipArg = 0;// the system sets flipArg to -1 at sceVideoOutOpen, so better avoid using -1 here.
            }
            else
            {
                NetPrintf(("T2Render: T2RenderInit sceVideoOutSetFlipRate error, %d.\n", iRet));
            }
        }
        else
        {
            NetPrintf(("T2Render: T2RenderInit initializeDisplayBuffer error, %d.\n", iRet));
        }
    }
    else
    {
        NetPrintf(("T2Render: T2RenderInit initializeVideoOut error, %d.\n", iRet));
    }

    return(iRet);
}

int32_t T2RenderUpdate(T2Render *pApp)
{
    // wait until the new rendering buffer is free from scan-out.
    // if flipArg is beyond the value of flipArgLog, it must be free
    int32_t iRet = waitFlipArg(pApp->iVideoOut, &pApp->eqFlip, s_flipArgLog[pApp->ibufferIndex] +1);
    if (iRet >= 0)
    {
        // draw to the display buffer
        updateDisplayBuffer(pApp->ibufferIndex, pApp->loop, FLIP_RATE);

        // Flush Garlic for memory base synchronization after CPU-write to WC_GARLIC before video-out
        // flushGarlic is NOT necessary if the draw is done by gc or compute shader (but need to flush L2-cache in that case)
        sce::Gnm::flushGarlic();

        // request flip to the buffer
        iRet = sceVideoOutSubmitFlip(pApp->iVideoOut, pApp->ibufferIndex, FLIP_MODE, pApp->flipArg);
        sce::Gnm::submitDone();

        if (iRet >= 0)
        {
            s_flipArgLog[pApp->ibufferIndex] = pApp->flipArg;
            // for the next loop
            pApp->flipArg++;
            // to get the next display buffer
            pApp->ibufferIndex = (pApp->ibufferIndex+1)%DISPLAY_BUFFER_NUM;
            pApp->loop++;
        }
        else
        {
            NetPrintf(("T2Render: T2RenderUpdate sceVideoOutSubmitFlip error, %d.\n", iRet));
        }
    }
    else
    {
        NetPrintf(("T2Render: T2RenderUpdate waitFlipArg error, %d.\n", iRet));
    }

    return(iRet);
}

int32_t T2RenderTerm(T2Render *pApp)
{
    // wait until all flips done
    int32_t iRet = waitFlipAllFinished(pApp->iVideoOut, &pApp->eqFlip);
    NetPrintf(("T2Render: T2RenderTerm done, %d.\n", iRet));
    return(iRet);
}

#endif //#if defined(DIRTYCODE_PS4)
