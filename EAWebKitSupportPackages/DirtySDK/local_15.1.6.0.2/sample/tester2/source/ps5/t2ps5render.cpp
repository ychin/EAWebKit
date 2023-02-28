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

/*** Include files *********************************************************************/
#include <stdlib.h>
#include <assert.h>
#include <kernel.h>
#include <agc.h>
#include <agc/core.h>
#include <video_out.h>

#include "t2ps5render.h"
#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtysock/dirtyerr.h"
#include "DirtySDK/dirtysock/dirtymem.h"

/*** Macros ****************************************************************************/

#define T2PS5RENDER_WIDTH         (3840)
#define T2PS5RENDER_HEIGHT        (2160)
#define T2PS5RENDER_BUFFER_COUNT  (2)
#define T2PS5RENDER_CONTEXT_SIZE  (1024 * 1024)

/*** Type Definitions ******************************************************************/

/* These symbols point to the headers and code of the shader binaries linked into the sample's elf.
   They are declared inside the shader code. For example, the Shader::ps_header and Shader::ps_text symbols
   were declared in the shader by putting the attribute [CxxSymbol("Shader::ps")] in front of the pixel
   shader's entry point.*/
namespace T2Shader
{
    extern char ps_header[];
    extern const char ps_text[];
    extern char gs_header[];
    extern const char gs_text[];
}

class T2Ps5Render
{
    public:
        T2Ps5Render();
        void T2Ps5RenderInit(int32_t iMemModule, int32_t iMemGroup, void *pMemGroupUserData);
        void T2Ps5RenderTerm();
        void T2Ps5RenderUpdate();

    private:
        // helper functions
        void DeallocateDirectMemory(void *pPointer);
        uint8_t* AllocateDirectMemory(sce::Agc::SizeAlign sSizeAlignment);
        void CreateScanoutBuffers();
        void CreateRenderTargets();
        void CreateShaders();
        void CreateFlipLabels();
        void CreateContexts();
        void CreateRenderTargetMask();
        void CreateViewport();
        void CreateDepthRenderTarget();
        void CreateOcclusionQueryResults();

        // rendering function
        void PrepareContext(uint32_t uBufferIndex);
        void ClearColorDepthTarget(uint32_t uBufferIndex);
        void DrawBlueTriangle(uint32_t uBufferIndex);
        void DrawRedTriangle(uint32_t uBufferIndex);

        // memory
        int32_t m_iMemModule;
        int32_t m_iMemGroup;
        void *m_pMemGroupUserData;
        uint8_t *m_pDepthRenderTargetMem;
        uint8_t *m_pContextsMem[T2PS5RENDER_BUFFER_COUNT];
        uint8_t *m_pRenderTargetMem[T2PS5RENDER_BUFFER_COUNT];

        //// video out
        int32_t m_iVideoHandle;
        int32_t m_iBufferSetIndex;

        //// rendering
        uint32_t m_uFrameCount;
        sce::Agc::Shader *m_pGeometryShader;
        sce::Agc::Shader *m_pPixelShader;
        sce::Agc::CxRenderTarget m_aRenderTargets[T2PS5RENDER_BUFFER_COUNT];
        sce::Agc::Core::BasicContext m_aContexts[T2PS5RENDER_BUFFER_COUNT];
        sce::Agc::CxRenderTargetMask m_renderTargetMask;
        sce::Agc::CxViewport m_viewport;
        sce::Agc::ShUserDataGs m_frameReg;
        sce::Agc::CxDepthRenderTarget m_depthRenderTarget;
        sce::Agc::OcclusionQueryResults *m_pQueryResults;
        sce::Agc::Label *m_aFlipLabels;
};

/*** Variables *************************************************************************/

static T2Ps5Render _T2Ps5Render_Ref;

/*** Private Functions *****************************************************************/

/*F*************************************************************************************/
/*!
    \Function DallocateDirectMemory

    \Description
        Deallocate Direct Memory

    \Input *pPointer  - mapped memory to be deallocated

    \Version 10/22/2019 (tcho)
*/
/*************************************************************************************F*/
void T2Ps5Render::DeallocateDirectMemory(void *pPointer)
{
    uint64_t uSize;
    int32_t iResult;
    SceKernelVirtualQueryInfo QueryInfo;

    if ((iResult = sceKernelVirtualQuery(pPointer, 0, &QueryInfo, sizeof(SceKernelVirtualQueryInfo))) != SCE_OK)
    {
        NetPrintf(("t2ps5render: sceKernelVirtualQuery error %s\n", DirtyErrGetName(iResult)));
        return;
    }

    uSize = (uint64_t)QueryInfo.end - (uint64_t)QueryInfo.start;

    if ((iResult = sceKernelReleaseDirectMemory(QueryInfo.offset, (size_t)uSize)) != SCE_OK)
    {
        NetPrintf(("t2ps5render: sceKernelReleaseDirectMemory error %s\n", DirtyErrGetName(iResult)));
        return;
    }
}

/*F*************************************************************************************/
/*!
    \Function AllocateDirectMemory

    \Description
        Allocate Direct Memory

    \Input sSizeAlignment - sce::Agc::SizeAlign structure

    \Output
        uint8_t*          - allocated memory, NULL if failed

    \Version 10/22/2019 (tcho)
*/
/*************************************************************************************F*/
uint8_t* T2Ps5Render::AllocateDirectMemory(sce::Agc::SizeAlign sSizeAlignment)
{
    const size_t sAlignment = (sSizeAlignment.m_align + 0xffff) & ~0xffff; 
    const uint64_t uSize = (sSizeAlignment.m_size + 0xffff) & ~0xffff;
    int32_t iResult;
    off_t offsetOut;
    void *pMappedPtr = NULL;

    // allocate main memory with CPU: write - back mode GPU GL2 MTYPE : cached, R / W, shared
    if ((iResult = sceKernelAllocateMainDirectMemory(uSize, sAlignment, SCE_KERNEL_MTYPE_C_SHARED, &offsetOut)) != SCE_OK)
    {
        NetPrintf(("t2ps5render: sceKernelAllocateMainDirectMemory error %s\n", DirtyErrGetName(iResult)));
        return NULL;
    }

    // map direct memory to virtual address space
    if ((iResult = sceKernelMapDirectMemory(&pMappedPtr, uSize, SCE_KERNEL_PROT_GPU_RW | SCE_KERNEL_PROT_CPU_RW, 0, offsetOut, sAlignment)) != SCE_OK)
    {
        NetPrintf(("t2ps5render: sceKernelMapDirectMemory error %s\n", DirtyErrGetName(iResult)));
        return NULL;
    }

    return((uint8_t *)pMappedPtr);
}

/*F*************************************************************************************/
/*!
    \Function CreateScanoutBuffers

    \Description
        Create scan out buffers

    \Version 10/22/2019 (tcho)
*/
/*************************************************************************************F*/
void T2Ps5Render::CreateScanoutBuffers()
{
    int32_t iResult;

    // select the video output which is SCE_VIDEO_OUT_BUS_TYPE_MAIN in this case aka the TV
    int32_t iVideoHandle = sceVideoOutOpen(SCE_USER_SERVICE_USER_ID_SYSTEM, SCE_VIDEO_OUT_BUS_TYPE_MAIN, 0, NULL);
    if (iVideoHandle < 0)
    {
        NetPrintf(("t2ps5render: sceVideoOutOpen error %s\n", DirtyErrGetName(iVideoHandle)));
        assert(FALSE);
    }

    /* configure the buffer format which can be extracted from the render targets 
       assuming all render targets have the same format we only need to extract it from one of the targets */
    sce::Agc::Core::RenderTargetSpec renderTargetSpec;
    if ((iResult = sce::Agc::Core::translate(&renderTargetSpec, &m_aRenderTargets[0])) != SCE_OK)
    {
        NetPrintf(("t2ps5render: RenderTargetSpec sce::Agc::Core::translate error %s\n", DirtyErrGetName(iResult)));
        assert(FALSE);
    }

    /* configure the video attributes according to our render target spec
       the video out also need to know how to interpret the color data, which is fill by our pixel shader writing 
       linear values into an sRGB render targer therefore the color data in the buffer will be sRGB encoded */
    SceVideoOutBufferAttribute2 outBfferAttribute;
    if ((iResult = sce::Agc::Core::translate(&outBfferAttribute, &renderTargetSpec, sce::Agc::Core::Colorimetry::kSrgb)) != SCE_OK)
    {
        NetPrintf(("t2ps5render: SceVideoOutBufferAttribute2 sce::Agc::Core::translate error %s\n", DirtyErrGetName(iResult)));
        assert(FALSE);
    }

    // create video buffers and set the data address to the render target's data address
    SceVideoOutBuffers *pVideoOutBuffer = (SceVideoOutBuffers *)DirtyMemAlloc(T2PS5RENDER_BUFFER_COUNT * sizeof(SceVideoOutBuffers), m_iMemModule, m_iMemGroup, m_pMemGroupUserData);
    for (uint32_t uIndex = 0; uIndex < T2PS5RENDER_BUFFER_COUNT; uIndex += 1)
    {
        ds_memclr(&pVideoOutBuffer[uIndex], sizeof(SceVideoOutBuffers));
        pVideoOutBuffer[uIndex].data = m_aRenderTargets[uIndex].getDataAddress();
    }

    // register the output buffer set with video out
    if ((iResult = sceVideoOutRegisterBuffers2(iVideoHandle, m_iBufferSetIndex, 0, pVideoOutBuffer, T2PS5RENDER_BUFFER_COUNT, &outBfferAttribute, SCE_VIDEO_OUT_BUFFER_ATTRIBUTE_CATEGORY_UNCOMPRESSED, NULL)) != SCE_OK)
    {
        NetPrintf(("t2ps5render: sceVideoOutRegisterBuffers2 error %s\n", DirtyErrGetName(iResult)));
        assert(FALSE);
    }

    // we don't need to keep the SceVideoOutBuffers strctures after we register
    DirtyMemFree(pVideoOutBuffer, m_iMemModule, m_iMemGroup, m_pMemGroupUserData);

    m_iVideoHandle = iVideoHandle;
}

/*F*************************************************************************************/
/*!
    \Function CreateRenderTargets

    \Description
        Create render targets

    \Input *pCxRenderTargets - array of cx render targets
    \Input *pTargetSpec      - render target spec
    \Input uCount            - number of cx render targets

    \Version 10/22/2019 (tcho)
*/
/*************************************************************************************F*/
void T2Ps5Render::CreateRenderTargets()
{
    // create the render target spec
    sce::Agc::Core::RenderTargetSpec renderTargetSpec;
    renderTargetSpec.init();
    renderTargetSpec.m_width = T2PS5RENDER_WIDTH;
    renderTargetSpec.m_height = T2PS5RENDER_HEIGHT;
    renderTargetSpec.m_format = { sce::Agc::Core::TypedFormat::k8_8_8_8Srgb, sce::Agc::Core::Swizzle::kRGBA_R4S4 };
    renderTargetSpec.m_tileMode = sce::Agc::CxRenderTarget::TileMode::kRenderTarget;

    // get the render target mem size
    sce::Agc::SizeAlign sRenderTargetMemSize = sce::Agc::Core::getSize(&renderTargetSpec);

    for (uint32_t uIndex = 0; uIndex < T2PS5RENDER_BUFFER_COUNT; uIndex += 1)
    {
        int32_t iResult; 
        m_pRenderTargetMem[uIndex] = AllocateDirectMemory(sRenderTargetMemSize);
        renderTargetSpec.m_dataAddress = m_pRenderTargetMem[uIndex];
        memset((void *)renderTargetSpec.m_dataAddress, 0x80, sRenderTargetMemSize.m_size);

        if ((iResult = sce::Agc::Core::initialize(&m_aRenderTargets[uIndex], &renderTargetSpec)) != SCE_OK)
        {
            NetPrintf(("t2ps5render: CxRenderTarget sce::Agc::Core::initialize error %s\n", DirtyErrGetName(iResult)));
            assert(FALSE);
        }
    }
}

/*F*************************************************************************************/
/*!
    \Function CreateShaders

    \Description
        Create shaders

    \Version 10/22/2019 (tcho)
*/
/*************************************************************************************F*/
void T2Ps5Render::CreateShaders()
{
    sce::Agc::createShader(&m_pGeometryShader, T2Shader::gs_header, T2Shader::gs_text);
    sce::Agc::createShader(&m_pPixelShader, T2Shader::ps_header, T2Shader::ps_text);
}

/*F*************************************************************************************/
/*!
    \Function CreateFlipLabels

    \Description
        Create flip labels

    \Version 10/22/2019 (tcho)
*/
/*************************************************************************************F*/
void T2Ps5Render::CreateFlipLabels()
{
    sce::Agc::SizeAlign sSizeAligment;
    sSizeAligment.m_align = sce::Agc::Alignment::kLabel;
    sSizeAligment.m_size = T2PS5RENDER_BUFFER_COUNT * sizeof(sce::Agc::Label);
    m_aFlipLabels = (sce::Agc::Label *)AllocateDirectMemory(sSizeAligment);
}

/*F*************************************************************************************/
/*!
    \Function CreateContexts

    \Description
        Create contexts

    \Version 10/22/2019 (tcho)
*/
/*************************************************************************************F*/
void T2Ps5Render::CreateContexts()
{
    sce::Agc::SizeAlign sSizeAligment;
    sSizeAligment.m_size = T2PS5RENDER_CONTEXT_SIZE;
    sSizeAligment.m_align = 4;

    for (uint32_t iIndex = 0; iIndex < T2PS5RENDER_BUFFER_COUNT; iIndex += 1)
    {
        m_pContextsMem[iIndex] = AllocateDirectMemory(sSizeAligment);
        m_aContexts[iIndex].m_dcb.init(m_pContextsMem[iIndex], T2PS5RENDER_CONTEXT_SIZE, nullptr, nullptr);
        m_aContexts[iIndex].m_bdr.init(&m_aContexts[iIndex].m_dcb, &m_aContexts[iIndex].m_dcb);
        m_aContexts[iIndex].m_sb.init(256, &m_aContexts[iIndex].m_dcb, &m_aContexts[iIndex].m_dcb);
        m_aFlipLabels[iIndex].m_value = 1;
    }
}

/*F*************************************************************************************/
/*!
    \Function CreateRenderTargetMask

    \Description
        Create render target mask

    \Version 10/22/2019 (tcho)
*/
/*************************************************************************************F*/
void T2Ps5Render::CreateRenderTargetMask()
{
    m_renderTargetMask.init();
    m_renderTargetMask.setMask(0, 0xf);
}

/*F*************************************************************************************/
/*!
    \Function CreateViewport

    \Description
        Create viewport

    \Version 10/22/2019 (tcho)
*/
/*************************************************************************************F*/
void T2Ps5Render::CreateViewport()
{
    sce::Agc::Core::setViewport(&m_viewport, T2PS5RENDER_WIDTH, T2PS5RENDER_HEIGHT, 0, 0, -1.0f, 1.0f);
}

/*F*************************************************************************************/
/*!
    \Function CreateDepthRenderTarget

    \Description
        Create depth render target

    \Version 10/22/2019 (tcho)
*/
/*************************************************************************************F*/
void T2Ps5Render::CreateDepthRenderTarget()
{
    // initialize frame numbers
    m_frameReg.init();

    // seting up the depth render target spec
    sce::Agc::Core::DepthRenderTargetSpec depthRenderTargetSpec;
    depthRenderTargetSpec.init();
    depthRenderTargetSpec.m_width = T2PS5RENDER_WIDTH;
    depthRenderTargetSpec.m_height = T2PS5RENDER_HEIGHT;
    depthRenderTargetSpec.m_depthFormat = sce::Agc::CxDepthRenderTarget::DepthFormat::k32Float;

    // initialize the depth render target
    m_pDepthRenderTargetMem = AllocateDirectMemory(sce::Agc::Core::getSize(&depthRenderTargetSpec));
    depthRenderTargetSpec.m_depthReadAddress = m_pDepthRenderTargetMem;
    depthRenderTargetSpec.m_depthWriteAddress = m_pDepthRenderTargetMem;
    sce::Agc::Core::initialize(&m_depthRenderTarget, &depthRenderTargetSpec);
}

/*F*************************************************************************************/
/*!
    \Function CreateOcclusionQueryResults

    \Description
        Create occlusion query results

    \Version 10/22/2019 (tcho)
*/
/*************************************************************************************F*/
void T2Ps5Render::CreateOcclusionQueryResults()
{
    sce::Agc::SizeAlign sSizeAlignment;
    sSizeAlignment.m_size = sizeof(sce::Agc::OcclusionQueryResults);
    sSizeAlignment.m_align = sce::Agc::Alignment::kOcclusionQueryResults;
    
    m_pQueryResults = (sce::Agc::OcclusionQueryResults *)AllocateDirectMemory(sSizeAlignment);
    ds_memclr(m_pQueryResults, sSizeAlignment.m_size);
}

/*F*************************************************************************************/
/*!
    \Function PrepareContext

    \Description
        Prepare context for a particular frame

    \Input uBufferIndex - buffer index

    \Version 10/22/2019 (tcho)
*/
/*************************************************************************************F*/
void T2Ps5Render::PrepareContext(uint32_t uBufferIndex)
{
    // set the flip label to 0, which the GPU will set back to 1 when it's done.
    m_aFlipLabels[uBufferIndex].m_value = 0;

    // reset context
    m_aContexts[uBufferIndex].reset();

    // reset hardware state
    //m_aContexts[uBufferIndex].m_dcb.initializeDefaultHardwareState();

    // stall the Command Processor (CP) until the buffer is no longer being displayed.
    m_aContexts[uBufferIndex].m_dcb.waitUntilSafeForRendering(m_iVideoHandle, uBufferIndex);

    // provide context with shaders 
    m_aContexts[uBufferIndex].setShaders(nullptr, m_pGeometryShader, m_pPixelShader, sce::Agc::UcPrimitiveType::Type::kTriStrip);

    // set the context state
    m_aContexts[uBufferIndex].m_sb.setState(m_renderTargetMask);
    m_aContexts[uBufferIndex].m_sb.setState(m_viewport);
    m_aContexts[uBufferIndex].m_sb.setState(m_aRenderTargets[uBufferIndex]);
    m_aContexts[uBufferIndex].m_sb.setState(m_depthRenderTarget);
}

/*F*************************************************************************************/
/*!
    \Function PrepareContext

    \Description
        Clear color depth target

    \Input uBufferIndex - buffer index
    \Version 10/22/2019 (tcho)
*/
/*************************************************************************************F*/
void T2Ps5Render::ClearColorDepthTarget(uint32_t uBufferIndex)
{
    // disable depth test.
    m_aContexts[uBufferIndex].m_sb.setState(sce::Agc::CxDepthStencilControl().init()
        .setDepth(sce::Agc::CxDepthStencilControl::Depth::kEnable)
        .setDepthWrite(sce::Agc::CxDepthStencilControl::DepthWrite::kEnable)
        .setDepthFunction(sce::Agc::CxDepthStencilControl::DepthFunction::kAlways));

    // draw
    m_frameReg.setData(0xffffffff); // Request fullscreen grey triangle
    m_aContexts[uBufferIndex].m_dcb.setShRegisterDirect(m_frameReg.m_regs[0]);
    m_aContexts[uBufferIndex].drawIndexAuto(3);
}

/*F*************************************************************************************/
/*!
    \Function DrawBlueTriangle

    \Description
        Draw blue triangle

    \Input uBufferIndex - buffer index
    \Version 10/22/2019 (tcho)
*/
/*************************************************************************************F*/
void T2Ps5Render::DrawBlueTriangle(uint32_t uBufferIndex)
{
    // enable depth test
    m_aContexts[uBufferIndex].m_sb.setState(sce::Agc::CxDepthStencilControl().init()
        .setDepth(sce::Agc::CxDepthStencilControl::Depth::kEnable)
        .setDepthWrite(sce::Agc::CxDepthStencilControl::DepthWrite::kEnable)
        .setDepthFunction(sce::Agc::CxDepthStencilControl::DepthFunction::kLessEqual));

    // draw
    m_frameReg.setData(m_uFrameCount); // Pass in frame number to drive animation.
    m_aContexts[uBufferIndex].m_dcb.setShRegisterDirect(m_frameReg.m_regs[0]);
    m_aContexts[uBufferIndex].drawIndexAuto(3);
}

/*F*************************************************************************************/
/*!
    \Function DrawRedTriangle

    \Description
        Draw red triangle

    \Input uBufferIndex - buffer index
    \Version 10/22/2019 (tcho)
*/
/*************************************************************************************F*/
void T2Ps5Render::DrawRedTriangle(uint32_t uBufferIndex)
{
    // Begin an occlusion query.
    m_aContexts[uBufferIndex].m_dcb.beginOcclusionQuery(sce::Agc::OcclusionQueryClear::kEnable, m_pQueryResults);

    // Enable perfect zpass counts to ensure we get correct occlusion query results.
    m_aContexts[uBufferIndex].m_sb.setState(sce::Agc::CxDbCountControl().init()
        .setPerfectZPassCounts(sce::Agc::CxDbCountControl::PerfectZPassCounts::kEnable));

    // Draw
    m_frameReg.setData(0xfffffffd); // Request red triangle
    m_aContexts[uBufferIndex].m_dcb.setShRegisterDirect(m_frameReg.m_regs[0]);
    m_aContexts[uBufferIndex].drawIndexAuto(3);

    // End the occlusion query.
    m_aContexts[uBufferIndex].m_dcb.endOcclusionQuery(m_pQueryResults);

    // Disable perfect zpass counts.
    m_aContexts[uBufferIndex].m_sb.setState(sce::Agc::CxDbCountControl().init()
        .setPerfectZPassCounts(sce::Agc::CxDbCountControl::PerfectZPassCounts::kDisable));
}

/*F*************************************************************************************/
/*!
    \Function T2Ps5Render

    \Description
        Constructor for T2Render

    \Input iMemModule           - mem module
    \Input iMemGroup            - mem group
    \Input *pMemGroupUserData   - mem group user data

    \Version 10/22/2019 (tcho)
*/
/*************************************************************************************F*/
T2Ps5Render::T2Ps5Render() :
    m_iBufferSetIndex(0),
    m_uFrameCount(0)
{
}
/*F*************************************************************************************/
/*!
    \Function T2Ps5RenderInit

    \Description
        T2 Render Initialize

    \Version 10/22/2019 (tcho)
*/
/*************************************************************************************F*/
void T2Ps5Render::T2Ps5RenderInit(int32_t iMemModule, int32_t iMemGroup, void *pMemGroupUserData)
{
    m_iMemModule = iMemModule;
    m_iMemGroup = iMemGroup;
    m_pMemGroupUserData = pMemGroupUserData;

    sce::Agc::init();

    CreateShaders();
    CreateRenderTargets();
    CreateFlipLabels();
    CreateScanoutBuffers();
    CreateContexts();
    CreateRenderTargetMask();
    CreateViewport();
    CreateDepthRenderTarget();
    CreateOcclusionQueryResults();
}

/*F*************************************************************************************/
/*!
    \Function T2Ps5RenderUpdate

    \Description
        T2 Render Update

    \Version 10/22/2019 (tcho)
*/
/*************************************************************************************F*/
void T2Ps5Render::T2Ps5RenderUpdate()
{
    // First we identify the back buffer
    const uint32_t uBufferIndex = m_uFrameCount % T2PS5RENDER_BUFFER_COUNT;

    // Check if the command buffer has been fully processed, if so it's safe for us to overwrite it on the CPU.
    while (m_aFlipLabels[uBufferIndex].m_value != 1)
    {
        sceKernelUsleep(1000);
    }

    // prepate rendering context
    PrepareContext(uBufferIndex);

    // clear color depth target
    ClearColorDepthTarget(uBufferIndex);

    // draw blue triangle
    DrawBlueTriangle(uBufferIndex);

    // draw red triangle
    DrawRedTriangle(uBufferIndex);

    // submit a flip via the GPU.
    m_aContexts[uBufferIndex].m_dcb.setFlip(m_iVideoHandle, uBufferIndex, SCE_VIDEO_OUT_FLIP_MODE_VSYNC, 0);

    // fire a gpU sync event
    sce::Agc::Core::gpuSyncEvent(
        &m_aContexts[uBufferIndex].m_dcb,
        sce::Agc::Core::SyncWaitMode::kAsynchronous,
        sce::Agc::Core::SyncCacheOp::kNone,
        sce::Agc::Core::SyncLabelVisibility::kCpu,
        &m_aFlipLabels[uBufferIndex],
        1);

    // submit graphics 
    sce::Agc::submitGraphics(
        sce::Agc::GraphicsQueue::kNormal,
        m_aContexts[uBufferIndex].m_dcb.getSubmitPointer(),
        m_aContexts[uBufferIndex].m_dcb.getSubmitSize());

    // increase th frame count
    m_uFrameCount++;

    // if the application is suspendend it is stop at this point
    sce::Agc::suspendPoint();
}

/*F*************************************************************************************/
/*!
    \Function T2Ps5RenderTerm

    \Description
        T2 Render Terminate

    \Version 10/22/2019 (tcho)
*/
/*************************************************************************************F*/
void T2Ps5Render::T2Ps5RenderTerm()
{
    DeallocateDirectMemory(m_pQueryResults);
    DeallocateDirectMemory(m_depthRenderTarget.getDepthReadAddress());

    for (uint32_t iIndex = 0; iIndex < T2PS5RENDER_BUFFER_COUNT; iIndex += 1)
    {
        DeallocateDirectMemory(m_pContextsMem[iIndex]);
        DeallocateDirectMemory(m_pRenderTargetMem[iIndex]);
    }

    DeallocateDirectMemory(m_aFlipLabels);
}

/*** Public Functions *****************************************************************/

/*F*************************************************************************************/
/*!
    \Function T2RenderInit

    \Description
        T2 Render Initialize

    \Version 1/15/2020 (tcho)
*/
/*************************************************************************************F*/
void T2RenderInit(int32_t iMemModule, int32_t iMemGroup, void *pMemGroupUserData)
{
    _T2Ps5Render_Ref.T2Ps5RenderInit(iMemModule, iMemGroup, pMemGroupUserData);
}

/*F*************************************************************************************/
/*!
    \Function T2RenderUpdate

    \Description
        T2 Render Update

    \Version 10/22/2019 (tcho)
*/
/*************************************************************************************F*/
void T2RenderUpdate()
{
    _T2Ps5Render_Ref.T2Ps5RenderUpdate();
}

/*F*************************************************************************************/
/*!
    \Function T2RenderTerm

    \Description
        T2 Render Terminate

    \Version 10/22/2019 (tcho)
*/
/*************************************************************************************F*/
void T2RenderTerm()
{
    _T2Ps5Render_Ref.T2Ps5RenderTerm();
}