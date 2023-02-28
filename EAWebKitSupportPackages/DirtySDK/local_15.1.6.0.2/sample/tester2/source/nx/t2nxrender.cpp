/*H*************************************************************************************/
/*!
    \File t2nxrender.cpp

    \Description
        Tester2 NX rendering

    \Copyright
        Copyright (c) Electronic Arts 2018.  ALL RIGHTS RESERVED.

    \Version    1.0 09/27/2018 (tcho) First Version
*/
/*************************************************************************************H*/
/*** Include files *********************************************************************/
#include <nn/os.h>
#include <nn/fs.h>
#include <nn/init.h>
#include <nn/vi.h>
#include <nn/gfx.h>

#if NN_GFX_IS_TARGET_NVN
    #include <nvn/nvn.h>
    #include <nvn/nvn_FuncPtrInline.h>
#endif

#if defined( NN_BUILD_TARGET_PLATFORM_OS_NN ) && defined( NN_BUILD_APISET_NX )
    #include <nv/nv_MemoryManagement.h>
#endif

#include "t2nxrender.h"
#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtysock/dirtymem.h"


/*** Defines **********************************************************************/
/*** Macros ***********************************************************************/
/*** Type Definitions *************************************************************/
/*** Variables ********************************************************************/
/*** Private Functions ************************************************************/
/*F********************************************************************************/
/*!
    \Function RenderAllocator

    \Description
        Render Memory Allocate

    \Output
        void* - allocated memory, NULL if failed

    \Version 09/28/2018 (tcho)
*/
/********************************************************************************F*/
void* RenderAllocate(size_t size, size_t alignment, void* unused)
{
    return(aligned_alloc(alignment, nn::util::align_up(size, alignment)));
}

/*F********************************************************************************/
/*!
    \Function RenderReallocate

    \Description
        Render Memory Reallocate

    \Output - none

    \Version 09/28/2018 (tcho)
*/
/********************************************************************************F*/
void *RenderReallocate(void* pAddr, size_t newSize, void*)
{
    return(realloc(pAddr, newSize));
}

/*F********************************************************************************/
/*!
    \Function RenderFree

    \Description
        Render Memory Free

    \Output - none

    \Version 09/28/2018 (tcho)
*/
/********************************************************************************F*/
void RenderFree(void *pAddr, void *pUnused)
{
    free(pAddr);
}

/*F********************************************************************************/
/*!
    \Function ReadResource

    \Description
        Read Resource File

    \Output
        bool - false for fail, true for success

    \Version 09/28/2018 (tcho)
*/
/********************************************************************************F*/
void* T2Render::ReadResource(const char* strFilename)
{
    nn::Result result;
    nn::fs::FileHandle hFile;
    nn::util::BinaryFileHeader fileHeader;
    int64_t iFileSize;
    size_t alignment;
    size_t readSize;
    void* pBuffer;
    
    result = nn::fs::OpenFile(&hFile, strFilename, nn::fs::OpenMode_Read);
    if (result.IsFailure())
    {
        NetPrintf(("t2render: cannot read resource file %s\n", strFilename));
        return(NULL);
    }

    result = nn::fs::GetFileSize(&iFileSize, hFile);
    if (result.IsFailure())
    {
        NetPrintf(("t2render: cannot get file size for file %s\n", strFilename));
        nn::fs::CloseFile(hFile);
        return(NULL);
    }

    // read the file header
    result = nn::fs::ReadFile(&readSize, hFile, 0, &fileHeader, sizeof(nn::util::BinaryFileHeader));
    if (result.IsFailure())
    {
        NetPrintf(("t2render: cannot read file header %s\n", strFilename));
        nn::fs::CloseFile(hFile);
        return(NULL);
    }

    // read the contents into a buffer
    alignment = fileHeader.GetAlignment();
    m_pMemory.AlignUp(alignment);
    pBuffer = m_pMemory.Get();
    
    result = nn::fs::ReadFile(&readSize, hFile, 0, pBuffer, (size_t)iFileSize);
    if (result.IsFailure())
    {
        NetPrintf(("t2render: cannot read file contents %s", strFilename));
        nn::fs::CloseFile(hFile);
        return(NULL);
    }
    
    m_pMemory.Advance((ptrdiff_t)iFileSize);
    nn::fs::CloseFile(hFile);

    return pBuffer;
}

/*F********************************************************************************/
/*!
    \Function InitializeFileSys

    \Description
        Initializze File System

    \Output
        bool - false for fail, true for success

    \Version 09/28/2018 (tcho)
*/
/********************************************************************************F*/
bool T2Render::InitializeFileSys()
{
    size_t iCacheSize = 0;
    nn::Result result;

    // query rom cache size
    result = nn::fs::QueryMountRomCacheSize(&iCacheSize);
    if (result.IsFailure())
    {
        NetPrintf(("t2nxrender: failed to query rom cache size\n"));
        return(false);
    }

    // allocate mout rom cache
    pMountRomCacheBuffer = DirtyMemAlloc(iCacheSize, m_iMemModule, m_iMemGroup, m_pMemGroupUserData);
    if (pMountRomCacheBuffer == NULL)
    {
        NetPrintf(("t2nxrender: failed to allocate mount rom file system cache\n"));
        return(false);
    }

    // mount rom
    result = nn::fs::MountRom("assets", pMountRomCacheBuffer, iCacheSize);
    if (result.IsFailure())
    {
        NetPrintf(("t2nxrender: failed to mount rom file system\n"));
        return(false);
    }

    return(true);
}

/*F********************************************************************************/
/*!
    \Function InitializeGraphicAllocator

    \Description
        Initialize Memory Allocator

    \Output none

    \Version 09/28/2018 (tcho)
*/
/********************************************************************************F*/
void T2Render::InitializeGraphicAllocator()
{
    #if defined(NN_BUILD_TARGET_PLATFORM_OS_NN) && defined(NN_BUILD_APISET_NX)  

        // initialize memory resources for the graphics system.
        nv::SetGraphicsAllocator(RenderAllocate, RenderFree, RenderReallocate, NULL);
        nv::InitializeGraphics(DirtyMemAlloc(T2NXRENDER_GRAPHIC_MEM_SIZE, m_iMemModule, m_iMemGroup, m_pMemGroupUserData), T2NXRENDER_GRAPHIC_MEM_SIZE);

        // set the memory allocator for the graphics developer tools and the debug layer.
        nv::SetGraphicsDevtoolsAllocator(RenderAllocate, RenderFree, RenderReallocate, NULL);

    #endif
}

/*F********************************************************************************/
/*!
    \Function InitializeLayer

    \Description
        Initialize Display Layer

    \Output
        bool - false for fail, true for success

    \Version 09/28/2018 (tcho)
*/
/********************************************************************************F*/
bool T2Render::InitializeLayer()
{
    nn::Result result;

    nn::vi::Initialize();
    result = nn::vi::OpenDefaultDisplay(&m_pDisplay);
    if (result.IsFailure())
    {
        NetPrintf(("t2nxrender: failed to open default display\n"));
        return(false);
    }

    result = nn::vi::CreateLayer(&m_pLayer, m_pDisplay);
    if (result.IsFailure())
    {
        NetPrintf(("t2nxrender: failed to create display layer\n"));
        return(false);
    }

    return(true);
}

/*F********************************************************************************/
/*!
    \Function InitializeDevice

    \Description
        Initialize graphics device 

    \Output none

    \Version 09/28/2018 (tcho)
*/
/********************************************************************************F*/
void T2Render::InitializeDevice()
{
    nn::gfx::Device::InfoType deviceInfo;

    deviceInfo.SetDefault();
    deviceInfo.SetDebugMode(nn::gfx::DebugMode_Enable);
    deviceInfo.SetApiVersion(nn::gfx::ApiMajorVersion, nn::gfx::ApiMinorVersion);
    mDevice.Initialize(deviceInfo);

#if NN_GFX_IS_TARGET_NVN
    nn::gfx::Device::DataType& deviceData = nn::gfx::AccessorToData(mDevice);
    nvnDeviceGetInteger(deviceData.pNvnDevice, NVN_DEVICE_INFO_RESERVED_TEXTURE_DESCRIPTORS, &m_iTextureDescBaseIndex);
    nvnDeviceGetInteger(deviceData.pNvnDevice, NVN_DEVICE_INFO_RESERVED_SAMPLER_DESCRIPTORS, &m_iSamplerDescBaseIndex);
#endif
}

/*F********************************************************************************/
/*!
    \Function InitializeMemoryPool

    \Description
        Initialize memory pool

    \Output none

    \Version 09/28/2018 (tcho)
*/
/********************************************************************************F*/
void T2Render::InitializeMemoryPool()
{
    nn::gfx::MemoryPool::InfoType mempoolInfo;
    int32_t aMemPoolProperty[MEMPOOL_TYPEEND] =
    {
        nn::gfx::MemoryPoolProperty_CpuUncached | nn::gfx::MemoryPoolProperty_GpuCached,
        nn::gfx::MemoryPoolProperty_CpuInvisible | nn::gfx::MemoryPoolProperty_GpuCached | nn::gfx::MemoryPoolProperty_Compressible
    };

    for (int32_t iMempoolIndex = 0; iMempoolIndex < MEMPOOL_TYPEEND; ++iMempoolIndex)
    {
        void* pPoolMemoryAligned;
        size_t memoryPoolSizeAligned;
        int32_t iAlignment;
        int32_t iGranularity;

        mempoolInfo.SetDefault();
        mempoolInfo.SetMemoryPoolProperty(aMemPoolProperty[iMempoolIndex]);

        iAlignment = (int32_t)nn::gfx::MemoryPool::GetPoolMemoryAlignment(&mDevice, mempoolInfo);
        iGranularity = (int32_t)nn::gfx::MemoryPool::GetPoolMemorySizeGranularity(&mDevice, mempoolInfo);
        m_apMemPool[iMempoolIndex] = DirtyMemAlloc(g_aT2RenderMempoolSize[iMempoolIndex], m_iMemModule, m_iMemGroup, m_pMemGroupUserData);
        pPoolMemoryAligned = nn::util::BytePtr(m_apMemPool[iMempoolIndex]).AlignUp(iAlignment).Get();
        memoryPoolSizeAligned = nn::util::align_down(g_aT2RenderMempoolSize[iMempoolIndex], iGranularity);
        mempoolInfo.SetPoolMemory(pPoolMemoryAligned, memoryPoolSizeAligned);

        m_aMemPool[iMempoolIndex].Initialize(&mDevice, mempoolInfo);
        m_aMemPoolOffset[iMempoolIndex] = 0;
    }
}

/*F********************************************************************************/
/*!
    \Function InitializeSwapChain

    \Description
        Initialize swap chain

    \Output none

    \Version 09/28/2018 (tcho)
*/
/********************************************************************************F*/
void T2Render::InitializeSwapChain()
{
    nn::gfx::SwapChain::InfoType swapChainInfo;

    swapChainInfo.SetDefault();
    swapChainInfo.SetLayer(m_pLayer);
    swapChainInfo.SetWidth(T2NXRENDER_WIDTH);
    swapChainInfo.SetHeight(T2NXRENDER_HEIGHT);
    swapChainInfo.SetFormat(nn::gfx::ImageFormat_R8_G8_B8_A8_UnormSrgb);
    swapChainInfo.SetBufferCount(T2NXRENDER_SCAN_BUFFER_COUNT);

    if (nn::gfx::SwapChain::IsMemoryPoolRequired)
    {
        ptrdiff_t &offset = m_aMemPoolOffset[MEMPOOL_CPUINVIS_GPUCACHED_COMPRESS];
        size_t size = mSwapChain.CalculateScanBufferSize(&mDevice, swapChainInfo);

        offset = nn::util::align_up(offset, nn::gfx::SwapChain::GetScanBufferAlignment(&mDevice, swapChainInfo));
        mSwapChain.Initialize(&mDevice, swapChainInfo, &m_aMemPool[MEMPOOL_CPUINVIS_GPUCACHED_COMPRESS], offset, size);
        offset += size;
    }
    else
    {
        mSwapChain.Initialize(&mDevice, swapChainInfo, NULL, 0, 0);
    }
}

/*F********************************************************************************/
/*!
    \Function InitializeQueue

    \Description
        Initialize queue

    \Output none

    \Version 09/28/2018 (tcho)
*/
/********************************************************************************F*/
void T2Render::InitializeQueue()
{
    nn::gfx::Queue::InfoType queueInfo;

    queueInfo.SetDefault();
    queueInfo.SetCapability(nn::gfx::QueueCapability_Graphics);
    mQueue.Initialize(&mDevice, queueInfo);
}

/*F********************************************************************************/
/*!
    \Function InitializeCommandBuffer

    \Description
        Initialize command buffer

    \Output none

    \Version 09/28/2018 (tcho)
*/
/********************************************************************************F*/
void T2Render::InitializeCommandBuffer()
{
    nn::gfx::CommandBuffer::InfoType commandBufferInfo;

    commandBufferInfo.SetDefault();
    commandBufferInfo.SetQueueCapability(nn::gfx::QueueCapability_Graphics);
    commandBufferInfo.SetCommandBufferType(nn::gfx::CommandBufferType_Direct);

    for (int32_t iCommandBuffIndex = 0; iCommandBuffIndex < T2NXRENDER_SCAN_BUFFER_COUNT; ++iCommandBuffIndex)
    {
        m_aCommandBuffer[iCommandBuffIndex].Initialize(&mDevice, commandBufferInfo);
    }
}

/*F********************************************************************************/
/*!
    \Function InitializeViewport

    \Description
        Initialize viewport

    \Output none

    \Version 09/28/2018 (tcho)
*/
/********************************************************************************F*/
void T2Render::InitializeViewport()
{
    nn::gfx::ViewportScissorState::InfoType viewportScissorInfo;
    nn::gfx::ViewportStateInfo viewportInfo;
    nn::gfx::ScissorStateInfo scissorInfo;

    viewportScissorInfo.SetDefault();
    viewportScissorInfo.SetScissorEnabled(true);
    
    viewportInfo.SetDefault();
    viewportInfo.SetWidth((float)T2NXRENDER_WIDTH);
    viewportInfo.SetHeight((float)T2NXRENDER_HEIGHT);
    
    scissorInfo.SetDefault();
    scissorInfo.SetWidth(T2NXRENDER_WIDTH);
    scissorInfo.SetHeight(T2NXRENDER_HEIGHT);

    viewportScissorInfo.SetViewportStateInfoArray(&viewportInfo, 1);
    viewportScissorInfo.SetScissorStateInfoArray(&scissorInfo, 1);
    mViewportScissor.Initialize(&mDevice, viewportScissorInfo);
}

/*F********************************************************************************/
/*!
    \Function InitializeRasterizerState

    \Description
        Initialize rasterizer state

    \Output none

    \Version 09/28/2018 (tcho)
*/
/********************************************************************************F*/
void T2Render::InitializeRasterizerState()
{
    nn::gfx::RasterizerState::InfoType rasterizerStateInfo;

    rasterizerStateInfo.SetDefault();
    rasterizerStateInfo.SetCullMode(nn::gfx::CullMode_None);
    rasterizerStateInfo.SetPrimitiveTopologyType(nn::gfx::PrimitiveTopologyType_Triangle);
    rasterizerStateInfo.SetScissorEnabled(true);
    rasterizerStateInfo.SetDepthClipEnabled(false);
    mRasterizerState.Initialize(&mDevice, rasterizerStateInfo);
}

/*F********************************************************************************/
/*!
    \Function InitializeBlendState

    \Description
        Initialize rasterizer state

    \Output none

    \Version 09/28/2018 (tcho)
*/
/********************************************************************************F*/
void T2Render::InitializeBlendState()
{
    nn::gfx::BlendState::InfoType blendStateInfo;
    nn::gfx::BlendTargetStateInfo blendTargetStateInfo;
    size_t size;

    blendStateInfo.SetDefault();
    blendTargetStateInfo.SetDefault();
    blendStateInfo.SetBlendTargetStateInfoArray(&blendTargetStateInfo, 1);

    // set memory state
    size = nn::gfx::BlendState::GetRequiredMemorySize(blendStateInfo);
    m_pMemory.AlignUp(nn::gfx::BlendState::RequiredMemoryInfo_Alignment);
    mBlendState.SetMemory(m_pMemory.Get(), size);
    m_pMemory.Advance(size);

    mBlendState.Initialize(&mDevice, blendStateInfo);
}

/*F********************************************************************************/
/*!
    \Function InitializeDepthStencilState

    \Description
        Initialize depth stencil state

    \Output none

    \Version 09/28/2018 (tcho)
*/
/********************************************************************************F*/
void T2Render::InitializeDepthStencilState()
{
    nn::gfx::DepthStencilState::InfoType depthStencilInfo;

    depthStencilInfo.SetDefault();
    depthStencilInfo.SetDepthTestEnabled(false);
    depthStencilInfo.SetDepthWriteEnabled(false);
    mDepthStencilState.Initialize(&mDevice, depthStencilInfo);
}

/*F********************************************************************************/
/*!
    \Function InitializeVertexState

    \Description
        Initialize vertex state

    \Output none

    \Version 09/28/2018 (tcho)
*/
/********************************************************************************F*/
void T2Render::InitializeVertexState()
{
    nn::gfx::VertexBufferStateInfo bufferStateInfo;
    nn::gfx::VertexState::InfoType vertexStateInfo;
    nn::gfx::VertexAttributeStateInfo aAttribs[2];
    ptrdiff_t stride = sizeof(float) * 5;

    vertexStateInfo.SetDefault();
    
    aAttribs[0].SetDefault();
    aAttribs[0].SetBufferIndex(0);
    aAttribs[0].SetFormat(nn::gfx::AttributeFormat_32_32_32_Float);
    aAttribs[0].SetOffset(0);
    aAttribs[0].SetShaderSlot(0);

    aAttribs[1].SetDefault();
    aAttribs[1].SetBufferIndex(0);
    aAttribs[1].SetFormat(nn::gfx::AttributeFormat_32_32_32_Float);
    aAttribs[1].SetOffset(sizeof(float) * 3);
    aAttribs[1].SetShaderSlot(1);

    bufferStateInfo.SetDefault();
    bufferStateInfo.SetStride(stride);

    vertexStateInfo.SetVertexAttributeStateInfoArray(aAttribs, 2);
    vertexStateInfo.SetVertexBufferStateInfoArray(&bufferStateInfo, 1);

    size_t size = nn::gfx::VertexState::GetRequiredMemorySize(vertexStateInfo);
    m_pMemory.AlignUp(nn::gfx::VertexState::RequiredMemoryInfo_Alignment);
    mVertexState.SetMemory(m_pMemory.Get(), size);
    m_pMemory.Advance(size);

    mVertexState.Initialize(&mDevice, vertexStateInfo, NULL);
}

/*F********************************************************************************/
/*!
    \Function InitializeVertexBuffer

    \Description
        Initialize vertex buffer

    \Output none

    \Version 09/28/2018 (tcho)
*/
/********************************************************************************F*/
void T2Render::InitializeVertexBuffer()
{
    nn::gfx::Buffer::InfoType bufferInfo;
    void *pMapped;

    bufferInfo.SetDefault();
    bufferInfo.SetSize(sizeof(m_aVertices));
    bufferInfo.SetGpuAccessFlags(nn::gfx::GpuAccess_VertexBuffer);

    if (nn::gfx::Buffer::IsMemoryPoolRequired)
    {
        ptrdiff_t &offset = m_aMemPoolOffset[MEMPOOL_CPUUNCACHED_GPUCACHED];
        offset = nn::util::align_up(offset, nn::gfx::Buffer::GetBufferAlignment(&mDevice, bufferInfo));
        mVertextBuffer.Initialize(&mDevice, bufferInfo, &m_aMemPool[MEMPOOL_CPUUNCACHED_GPUCACHED], offset, bufferInfo.GetSize());
        offset += bufferInfo.GetSize();
    }
    else
    {
        mVertextBuffer.Initialize(&mDevice, bufferInfo, NULL, 0, 0);
    }

    pMapped = mVertextBuffer.Map();
    ds_memcpy(pMapped, m_aVertices, bufferInfo.GetSize());
    mVertextBuffer.Unmap();
}

/*F********************************************************************************/
/*!
    \Function InitializeIndexBuffer

    \Description
    Initialize index buffer

    \Output none

    \Version 09/28/2018 (tcho)
*/
/********************************************************************************F*/
void T2Render::InitializeIndexBuffer()
{
    nn::gfx::Buffer::InfoType bufferInfo;
    void *pMapped;
    
    bufferInfo.SetDefault();
    bufferInfo.SetSize(sizeof(m_aIndices));
    bufferInfo.SetGpuAccessFlags(nn::gfx::GpuAccess_IndexBuffer);

    if (nn::gfx::Buffer::IsMemoryPoolRequired)
    {
        ptrdiff_t &offset = m_aMemPoolOffset[MEMPOOL_CPUUNCACHED_GPUCACHED];
        offset = nn::util::align_up(offset, nn::gfx::Buffer::GetBufferAlignment(&mDevice, bufferInfo));
        mIndexBuffer.Initialize(&mDevice, bufferInfo, &m_aMemPool[MEMPOOL_CPUUNCACHED_GPUCACHED], offset, bufferInfo.GetSize());
        offset += bufferInfo.GetSize();
    }
    else
    {
        mIndexBuffer.Initialize(&mDevice, bufferInfo, NULL, 0, 0);
    }

    pMapped = mIndexBuffer.Map();
    memcpy(pMapped, m_aIndices, bufferInfo.GetSize());
    mIndexBuffer.Unmap();
}

/*F********************************************************************************/
/*!
    \Function InitializeConstantBuffer

    \Description
        Initialize constant buffer

    \Output none

    \Version 09/28/2018 (tcho)
*/
/********************************************************************************F*/
void T2Render::InitializeConstantBuffer()
{
    nn::gfx::Buffer::InfoType constantBufferInfo;

    constantBufferInfo.SetDefault();
    constantBufferInfo.SetGpuAccessFlags(nn::gfx::GpuAccess_ConstantBuffer); 
    mConstantBufferSize = nn::util::align_up(sizeof(float) * 4, nn::gfx::Buffer::GetBufferAlignment(&mDevice, constantBufferInfo));
    constantBufferInfo.SetSize(mConstantBufferSize * T2NXRENDER_SCAN_BUFFER_COUNT);

    if (nn::gfx::Buffer::IsMemoryPoolRequired)
    {
        ptrdiff_t &offset = m_aMemPoolOffset[MEMPOOL_CPUUNCACHED_GPUCACHED];
        offset = nn::util::align_up(offset, nn::gfx::Buffer::GetBufferAlignment(&mDevice, constantBufferInfo));
        mConstantBuffer.Initialize(&mDevice, constantBufferInfo, &m_aMemPool[MEMPOOL_CPUUNCACHED_GPUCACHED], offset, constantBufferInfo.GetSize());
        offset += constantBufferInfo.GetSize();
    }
    else
    {
        mConstantBuffer.Initialize(&mDevice, constantBufferInfo, NULL, 0, 0);
    }
}

/*F********************************************************************************/
/*!
    \Function InitializeSampler

    \Description
        Initialize sampler

    \Output none

    \Version 09/28/2018 (tcho)
*/
/********************************************************************************F*/
void T2Render::InitializeSampler()
{
    nn::gfx::Sampler::InfoType samplerInfo;

    samplerInfo.SetDefault();
    samplerInfo.SetFilterMode(nn::gfx::FilterMode_MinLinear_MagLinear_MipPoint);
    samplerInfo.SetAddressU(nn::gfx::TextureAddressMode_Mirror);
    samplerInfo.SetAddressV(nn::gfx::TextureAddressMode_Mirror);
    samplerInfo.SetAddressW(nn::gfx::TextureAddressMode_Mirror);
    mSampler.Initialize(&mDevice, samplerInfo);
}

/*F********************************************************************************/
/*!
    \Function InitializeBufferDescPool

    \Description
        Initialize buffer descriptor pool

    \Output none

    \Version 09/28/2018 (tcho)
*/
/********************************************************************************F*/
void T2Render::InitializeBufferDescPool()
{
    nn::gfx::DescriptorPool::InfoType  descPoolInfo;
    
    descPoolInfo.SetDefault();
    descPoolInfo.SetDescriptorPoolType(nn::gfx::DescriptorPoolType_BufferView);
    descPoolInfo.SetSlotCount(m_iBufferDescBaseIndex + 2);
    
    size_t size = nn::gfx::DescriptorPool::CalculateDescriptorPoolSize(&mDevice, descPoolInfo);
    ptrdiff_t &offset = m_aMemPoolOffset[MEMPOOL_CPUUNCACHED_GPUCACHED];
    offset = nn::util::align_up(offset, nn::gfx::DescriptorPool::GetDescriptorPoolAlignment(&mDevice, descPoolInfo));
    
    mBufferDescPool.Initialize(&mDevice, descPoolInfo, &m_aMemPool[MEMPOOL_CPUUNCACHED_GPUCACHED], offset, size);
    offset += size;
}

/*F********************************************************************************/
/*!
    \Function InitializeSamplerDescPool

    \Description
        Initialize sampler descriptor pool

    \Output none

    \Version 09/28/2018 (tcho)
*/
/********************************************************************************F*/
void T2Render::InitializeSamplerDescPool()
{
    nn::gfx::DescriptorPool::InfoType descPoolInfo;
    
    descPoolInfo.SetDefault();
    descPoolInfo.SetDescriptorPoolType(nn::gfx::DescriptorPoolType_Sampler);
    descPoolInfo.SetSlotCount(m_iSamplerDescBaseIndex + 1);
    
    size_t size = nn::gfx::DescriptorPool::CalculateDescriptorPoolSize(&mDevice, descPoolInfo);
    ptrdiff_t &offset = m_aMemPoolOffset[MEMPOOL_CPUUNCACHED_GPUCACHED];
    offset = nn::util::align_up(offset, nn::gfx::DescriptorPool::GetDescriptorPoolAlignment(&mDevice, descPoolInfo));
    
    mSamplerDescPool.Initialize(&mDevice, descPoolInfo, &m_aMemPool[MEMPOOL_CPUUNCACHED_GPUCACHED], offset, size);
    offset += size;
}

/*F********************************************************************************/
/*!
    \Function InitializeTextureDescPool

    \Description
        Initialize texture descriptor pool

    \Output none

    \Version 09/28/2018 (tcho)
*/
/********************************************************************************F*/
void T2Render::InitializeTextureDescPool()
{
    nn::gfx::DescriptorPool::InfoType descPoolInfo;

    descPoolInfo.SetDefault();
    descPoolInfo.SetDescriptorPoolType(nn::gfx::DescriptorPoolType_TextureView);
    descPoolInfo.SetSlotCount(m_iTextureDescBaseIndex + 1);

    size_t size = nn::gfx::DescriptorPool::CalculateDescriptorPoolSize(&mDevice, descPoolInfo);
    ptrdiff_t &offset = m_aMemPoolOffset[MEMPOOL_CPUUNCACHED_GPUCACHED];
    offset = nn::util::align_up(offset, nn::gfx::DescriptorPool::GetDescriptorPoolAlignment(&mDevice, descPoolInfo));

    mTextureDescPool.Initialize(&mDevice, descPoolInfo, &m_aMemPool[MEMPOOL_CPUUNCACHED_GPUCACHED], offset, size);
    offset += size;
}

/*F********************************************************************************/
/*!
    \Function InitializeFence

    \Description
        Initialize dsiplay fence

    \Output none

    \Version 09/28/2018 (tcho)
*/
/********************************************************************************F*/
void T2Render::InitializeFence()
{
    nn::gfx::Fence::InfoType fenceInfo;
    fenceInfo.SetDefault();
    mDisplayFence.Initialize(&mDevice, fenceInfo);
}

/*F********************************************************************************/
/*!
    \Function InitializeSemaphore

    \Description
        Initialize dsiplay semaphore

    \Output none

    \Version 09/28/2018 (tcho)
*/
/********************************************************************************F*/
void T2Render::InitializeSemaphore()
{
    nn::gfx::Semaphore::InfoType semaphoreInfo;
    semaphoreInfo.SetDefault();
    mDisplaySemaphore.Initialize(&mDevice, semaphoreInfo);
}

/*F********************************************************************************/
/*!
    \Function InitializeResTextureFile

    \Description
        Initialize ResTexture file

    \Output none

    \Version 09/28/2018 (tcho)
*/
/********************************************************************************F*/
void T2Render::InitializeResTextureFile()
{
    void *pResource = ReadResource("assets:/SampleTexture.bntx");
    m_pResTextureFile = nn::gfx::ResTextureFile::ResCast(pResource);
    m_pResTextureFile->Initialize(&mDevice);

    int32_t iTextCount = m_pResTextureFile->GetTextureDic()->GetCount();
    for (int32_t iTextureIndex = 0; iTextureIndex < iTextCount; ++iTextureIndex)
    {
        m_pResTextureFile->GetResTexture(iTextureIndex)->Initialize(&mDevice);
    }
}

/*F********************************************************************************/
/*!
    \Function FinalizeResTextureFile

    \Description
        Finalize ResTexture file

    \Output none

    \Version 09/28/2018 (tcho)
*/
/********************************************************************************F*/
void T2Render::FinalizeResTextureFile()
{
    int32_t iTextCount = m_pResTextureFile->GetTextureDic()->GetCount();
    
    for (int32_t iTextureIndex = 0; iTextureIndex < iTextCount; ++iTextureIndex)
    {
        m_pResTextureFile->GetResTexture(iTextureIndex)->Finalize(&mDevice);
    }
    m_pResTextureFile->Finalize(&mDevice);
}

/*F********************************************************************************/
/*!
    \Function InitializeResShaderFile

    \Description
        Initialize ResShader file

    \Output none

    \Version 09/28/2018 (tcho)
*/
/********************************************************************************F*/
void T2Render::InitializeResShaderFile()
{
    void* pResource = ReadResource("assets:/SampleShader.bnsh");
    nn::gfx::ResShaderContainer* pContainer;

    m_pResShaderFile = nn::gfx::ResShaderFile::ResCast(pResource);
    pContainer = m_pResShaderFile->GetShaderContainer();
    pContainer->Initialize(&mDevice);

    int32_t iVariationCount = pContainer->GetShaderVariationCount();
    for (int32_t iVariationIndex = 0; iVariationIndex < iVariationCount; ++iVariationIndex)
    {
        nn::gfx::ResShaderProgram* pProgram = pContainer->GetResShaderProgram(iVariationIndex);
        pProgram->Initialize(&mDevice);
    }

#if NN_GFX_IS_TARGET_NVN
    int32_t iScratchMemoryAlignment;
    ptrdiff_t &offset = m_aMemPoolOffset[MEMPOOL_CPUINVIS_GPUCACHED_COMPRESS];

    mMaxScratchMemory = nn::gfx::NvnGetMaxRecommendedScratchMemorySize(&mDevice, &m_pResShaderFile, 1);
    nvnDeviceGetInteger(mDevice.ToData()->pNvnDevice, NVN_DEVICE_INFO_SHADER_SCRATCH_MEMORY_ALIGNMENT, &iScratchMemoryAlignment);

    offset = nn::util::align_up(offset, iScratchMemoryAlignment);
    mScratchMemoryOffset = offset;
    offset += mMaxScratchMemory;
#endif
}

/*F********************************************************************************/
/*!
    \Function FinalizeResShaderFile

    \Description
        Finalize ResShader file

    \Output none

    \Version 09/28/2018 (tcho)
*/
/********************************************************************************F*/
void T2Render::FinalizeResShaderFile()
{
    nn::gfx::ResShaderContainer *pContainer = m_pResShaderFile->GetShaderContainer();
    int32_t iVariationCount = pContainer->GetShaderVariationCount();

    for (int32_t iVariationIndex = 0; iVariationIndex < iVariationCount; ++iVariationIndex)
    {
        nn::gfx::ResShaderProgram* pProgram = pContainer->GetResShaderProgram(iVariationIndex);
        pProgram->Finalize(&mDevice);
    }

    pContainer->Finalize(&mDevice);
}

/*F********************************************************************************/
/*!
    \Function MakeCommand

    \Description
        Make GPU Commands

    \Output
        int32_t                    - frame number
        nn::gfx::ColorTargetView*  - view target
        nn::gfx::CommandBuffer*    - command buffer

    \Version 09/28/2018 (tcho)
*/
/********************************************************************************F*/
void T2Render::MakeCommand(int32_t iFrame, nn::gfx::ColorTargetView *pTarget, nn::gfx::CommandBuffer *pCommandBuffer)
{
    nn::gfx::GpuAddress gpuAddress;
    ptrdiff_t &offset = m_aMemPoolOffset[MEMPOOL_CPUUNCACHED_GPUCACHED];

    offset = nn::util::align_up(offset, nn::gfx::CommandBuffer::GetCommandMemoryAlignment(&mDevice));
    pCommandBuffer->AddCommandMemory(&m_aMemPool[MEMPOOL_CPUUNCACHED_GPUCACHED], offset + (iFrame % T2NXRENDER_SCAN_BUFFER_COUNT) * 1024 * 1024, 1024 * 1024);

    m_pMemory.AlignUp(nn::gfx::CommandBuffer::GetControlMemoryAlignment(&mDevice));
    pCommandBuffer->AddControlMemory(m_pMemory.Get(), 1024);
    pCommandBuffer->Begin();

    {
        mVertextBuffer.GetGpuAddress(&gpuAddress);
        pCommandBuffer->SetVertexBuffer(0, gpuAddress, sizeof(float) * 5, sizeof(m_aVertices));
        pCommandBuffer->InvalidateMemory(nn::gfx::GpuAccess_ShaderCode | nn::gfx::GpuAccess_Descriptor);
        pCommandBuffer->SetDescriptorPool(&mBufferDescPool);
        pCommandBuffer->SetDescriptorPool(&mTextureDescPool);
        pCommandBuffer->SetDescriptorPool(&mSamplerDescPool);

        pCommandBuffer->ClearColor(pTarget, 0.0f, 1.5f, 0.2f, 1.0f, NULL);

#if NN_GFX_IS_TARGET_NVN
        nvnCommandBufferSetShaderScratchMemory(pCommandBuffer->ToData()->pNvnCommandBuffer, m_aMemPool[MEMPOOL_CPUINVIS_GPUCACHED_COMPRESS].ToData()->pNvnMemoryPool, mScratchMemoryOffset, mMaxScratchMemory);
#endif

        nn::gfx::ResShaderContainer* pContainer = m_pResShaderFile->GetShaderContainer();
        int32_t iVariation = iFrame / 32 % pContainer->GetShaderVariationCount();
        nn::gfx::ResShaderProgram* pProgram = pContainer->GetResShaderProgram(iVariation);
        nn::gfx::Shader* pShader = pProgram->GetShader();

        pCommandBuffer->SetShader(pShader, nn::gfx::ShaderStageBit_All);
        pCommandBuffer->SetRenderTargets(1, &pTarget, NULL);
        pCommandBuffer->SetViewportScissorState(&mViewportScissor);
        pCommandBuffer->SetRasterizerState(&mRasterizerState);
        pCommandBuffer->SetBlendState(&mBlendState);
        pCommandBuffer->SetDepthStencilState(&mDepthStencilState);
        pCommandBuffer->SetVertexState(&mVertexState);
        pCommandBuffer->InvalidateMemory(nn::gfx::GpuAccess_Texture | nn::gfx::GpuAccess_IndexBuffer | nn::gfx::GpuAccess_ConstantBuffer | nn::gfx::GpuAccess_VertexBuffer);

        nn::gfx::DescriptorSlot constantBufferDescriptor;
        nn::gfx::DescriptorSlot textureDescriptor;
        nn::gfx::DescriptorSlot samplerDescriptor;
        mBufferDescPool.GetDescriptorSlot(&constantBufferDescriptor, m_iBufferDescBaseIndex + (iFrame % T2NXRENDER_SCAN_BUFFER_COUNT));
        mTextureDescPool.GetDescriptorSlot(&textureDescriptor, m_iTextureDescBaseIndex);
        mSamplerDescPool.GetDescriptorSlot(&samplerDescriptor, m_iSamplerDescBaseIndex);

        int32_t iSlotMat = pShader->GetInterfaceSlot(nn::gfx::ShaderStage_Pixel, nn::gfx::ShaderInterfaceType_ConstantBuffer, "Mat");
        int32_t iSlotTex = pShader->GetInterfaceSlot(nn::gfx::ShaderStage_Pixel, nn::gfx::ShaderInterfaceType_Sampler, "tex");

        if (iSlotMat >= 0)
        {
            pCommandBuffer->SetConstantBuffer(iSlotMat, nn::gfx::ShaderStage_Pixel, constantBufferDescriptor);
        }

        if (iSlotTex >= 0)
        {
            pCommandBuffer->SetTextureAndSampler(iSlotTex, nn::gfx::ShaderStage_Pixel, textureDescriptor, samplerDescriptor);
        }

        mIndexBuffer.GetGpuAddress(&gpuAddress);
        pCommandBuffer->DrawIndexed(nn::gfx::PrimitiveTopology_TriangleStrip, nn::gfx::IndexFormat_Uint32, gpuAddress, 4, 0);
    }
    pCommandBuffer->End();
}

/*** Public Functions ************************************************************/
/*F********************************************************************************/
/*!
    \Function T2Render

    \Description
        T2 Render Constructor

    \Output none

    \Version 09/28/2018 (tcho)
*/
/********************************************************************************F*/
T2Render::T2Render(int32_t iMemModule, int32_t iMemGroup, void *m_pMemGroupUserData) :
    m_iFrame(0),
    m_iMemModule(iMemModule),
    m_iMemGroup(iMemGroup),
    m_pMemGroupUserData(m_pMemGroupUserData),
    m_pMemoryHeap(NULL),
    m_pMemory(NULL),
    mMaxScratchMemory(0),
    mScratchMemoryOffset(0),
    m_iBufferDescBaseIndex(0),
    m_iTextureDescBaseIndex(0),
    m_iSamplerDescBaseIndex(0)

{

}

/*F********************************************************************************/
/*!
    \Function T2RenderInit

    \Description
        T2 Render Initialize

    \Output none

    \Version 09/28/2018 (tcho)
*/
/********************************************************************************F*/
bool T2Render::T2RenderInit()
{
    m_pMemoryHeap.Reset(DirtyMemAlloc(1024 * 1024, m_iMemModule, m_iMemGroup, m_pMemGroupUserData));
    m_pMemory = m_pMemoryHeap;

    if (InitializeFileSys() == false)
    {
        return(false);
    }

    InitializeGraphicAllocator();
    
    if (InitializeLayer() == false)
    {
        return(false);
    }

    nn::gfx::Initialize();
    InitializeDevice();
    InitializeMemoryPool();
    InitializeSwapChain();
    InitializeQueue();
    InitializeCommandBuffer();
    InitializeViewport();
    InitializeRasterizerState();
    InitializeBlendState();
    InitializeDepthStencilState();
    InitializeVertexState();
    InitializeVertexBuffer();
    InitializeIndexBuffer();
    InitializeConstantBuffer();
    InitializeSampler();
    InitializeBufferDescPool();
    InitializeSamplerDescPool();
    InitializeTextureDescPool();
    InitializeFence();
    InitializeSemaphore();
    InitializeResTextureFile();
    InitializeResShaderFile();

    mBufferDescPool.BeginUpdate();
    {
        nn::gfx::GpuAddress gpuAddress;
        mConstantBuffer.GetGpuAddress(&gpuAddress);
        mBufferDescPool.SetBufferView(m_iBufferDescBaseIndex, gpuAddress, sizeof(float) * 4);
        gpuAddress.Offset(mConstantBufferSize);
        mBufferDescPool.SetBufferView(m_iBufferDescBaseIndex + 1, gpuAddress, sizeof(float) * 4);
    }
    mBufferDescPool.EndUpdate();

    mTextureDescPool.BeginUpdate();
    {
        mTextureDescPool.SetTextureView(m_iTextureDescBaseIndex, m_pResTextureFile->GetResTexture(0)->GetTextureView());
    }
    mTextureDescPool.EndUpdate();

    mSamplerDescPool.BeginUpdate();
    {
        mSamplerDescPool.SetSampler(m_iSamplerDescBaseIndex, &mSampler);
    }
    mSamplerDescPool.EndUpdate();

    mSwapChain.GetScanBufferViews(m_aScanBufferViews, T2NXRENDER_SCAN_BUFFER_COUNT);

    return(true);
}

/*F********************************************************************************/
/*!
    \Function T2RenderTerm

    \Description
        T2 Render Terminate

    \Output none

    \Version 09/28/2018 (tcho)
*/
/********************************************************************************F*/
void T2Render::T2RenderTerm()
{
    mQueue.Sync();

    FinalizeResShaderFile();
    FinalizeResTextureFile();

    mDisplaySemaphore.Finalize(&mDevice);
    mDisplayFence.Finalize(&mDevice);

    mSampler.Finalize(&mDevice);
    mConstantBuffer.Finalize(&mDevice);
    mIndexBuffer.Finalize(&mDevice);
    mVertextBuffer.Finalize(&mDevice);

    mRasterizerState.Finalize(&mDevice);
    mBlendState.Finalize(&mDevice);
    mDepthStencilState.Finalize(&mDevice);
    mVertexState.Finalize(&mDevice);

    mBufferDescPool.Finalize(&mDevice);
    mTextureDescPool.Finalize(&mDevice);
    mSamplerDescPool.Finalize(&mDevice);

    mViewportScissor.Finalize(&mDevice);
    for (int32_t iCommandBufferIndex = 0; iCommandBufferIndex < T2NXRENDER_SCAN_BUFFER_COUNT; ++iCommandBufferIndex)
    {
        m_aCommandBuffer[iCommandBufferIndex].Finalize(&mDevice);
    }
    mSwapChain.Finalize(&mDevice);
    mQueue.Finalize(&mDevice);
    for (int32_t iMemPoolIndex = 0; iMemPoolIndex < MEMPOOL_TYPEEND; ++iMemPoolIndex)
    {
        m_aMemPool[iMemPoolIndex].Finalize(&mDevice);
        DirtyMemFree(m_apMemPool[iMemPoolIndex], m_iMemModule, m_iMemGroup, m_pMemGroupUserData);
    }

    mDevice.Finalize();
    DirtyMemFree(m_pMemoryHeap.Get(), m_iMemModule, m_iMemGroup, m_pMemGroupUserData);

    nn::gfx::Finalize();
    nn::vi::DestroyLayer(m_pLayer);
    nn::vi::CloseDisplay(m_pDisplay);
    nn::vi::Finalize();
    nn::fs::Unmount("assets");
    DirtyMemFree(pMountRomCacheBuffer, m_iMemModule, m_iMemGroup, m_pMemGroupUserData);
}

/*F********************************************************************************/
/*!
    \Function T2RenderUpdate

    \Description
        T2 Render Initialize

    \Output none

    \Version 09/28/2018 (tcho)
*/
/********************************************************************************F*/
void T2Render::T2RenderUpdate()
{
    int32_t iNextScanBufferIndex = 0;
    nn::gfx::ColorTargetView* pTarget;
    nn::gfx::CommandBuffer* pCommandBuffer;

    mSwapChain.AcquireNextScanBufferIndex(&iNextScanBufferIndex, &mDisplaySemaphore, &mDisplayFence);
    pTarget = m_aScanBufferViews[iNextScanBufferIndex];
    pCommandBuffer = &m_aCommandBuffer[iNextScanBufferIndex];

    mQueue.SyncSemaphore(&mDisplaySemaphore);
    MakeCommand(m_iFrame, pTarget, pCommandBuffer);

    float *pConstantBuffer = nn::util::BytePtr(mConstantBuffer.Map(), (m_iFrame % T2NXRENDER_SCAN_BUFFER_COUNT) * mConstantBufferSize).Get<float>();
    {
        pConstantBuffer[0] = (m_iFrame & 0xFF) * (1.0f / 256.0f);
        pConstantBuffer[1] = 0.4f;
        pConstantBuffer[2] = 0.4f;
        pConstantBuffer[3] = 1.0f;
    }
    mConstantBuffer.Unmap();

    // execute command
    mQueue.ExecuteCommand(pCommandBuffer, NULL);

    // display result
    mQueue.Present(&mSwapChain, 1);
    mQueue.Flush();
    mDisplayFence.Sync(nn::TimeSpan::FromSeconds(1));

    ++m_iFrame;
}