/*H*************************************************************************************/
/*!
    \File t2nxrender.h

    \Description
        Tester2 NX rendering

    \Copyright
        Copyright (c) Electronic Arts 2018.  ALL RIGHTS RESERVED.

    \Version    1.0 09/27/2018 (tcho) First Version
*/
/*************************************************************************************H*/
/*** Include files *********************************************************************/
#include <nn/util/util_BytePtr.h>
#include <nn/os.h>
#include <nn/fs.h>
#include <nn/init.h>
#include <nn/vi.h>
#include <nn/gfx.h>

/*** Defines ***************************************************************************/
#define T2NXRENDER_WIDTH                        (1280)
#define T2NXRENDER_HEIGHT                       (720)
#define T2NXRENDER_SCAN_BUFFER_COUNT            (2)
#define T2NXRENDER_GRAPHIC_MEM_SIZE             (8 * 1024 * 1024)
#define T2NXRENDER_MEMPOOL_GPUCACHE             (16 * 1024 * 1024)
#define T2NXRENDER_MEMPOOL_GPUCACHE_COMMPRESS   (20 * 1024 * 1024)

/*** Macros ****************************************************************************/
/*** Type Definitions ******************************************************************/
enum eT2RenderMemPoolType
{
    MEMPOOL_CPUUNCACHED_GPUCACHED,
    MEMPOOL_CPUINVIS_GPUCACHED_COMPRESS,
    MEMPOOL_TYPEEND
};

const size_t g_aT2RenderMempoolSize[MEMPOOL_TYPEEND] =
{
    T2NXRENDER_MEMPOOL_GPUCACHE,
    T2NXRENDER_MEMPOOL_GPUCACHE_COMMPRESS
};

class T2Render
{
    public:
        T2Render(int32_t iMemModule, int32_t iMemGroup, void *m_pMemGroupUserData);
        bool T2RenderInit();
        void T2RenderTerm();
        void T2RenderUpdate();

    private:
        void *ReadResource(const char* strFilename);
        bool InitializeFileSys();
        void InitializeGraphicAllocator();
        bool InitializeLayer();
        void InitializeDevice();
        void InitializeMemoryPool();
        void InitializeSwapChain();
        void InitializeQueue();
        void InitializeCommandBuffer();
        void InitializeViewport();
        void InitializeRasterizerState();
        void InitializeBlendState();
        void InitializeDepthStencilState();
        void InitializeVertexState();
        void InitializeVertexBuffer();
        void InitializeIndexBuffer();
        void InitializeConstantBuffer();
        void InitializeSampler();
        void InitializeBufferDescPool();
        void InitializeSamplerDescPool();
        void InitializeTextureDescPool();
        void InitializeFence();
        void InitializeSemaphore();
        void InitializeResTextureFile();
        void InitializeResShaderFile();
        void FinalizeResTextureFile();
        void FinalizeResShaderFile();
        void MakeCommand(int32_t iFrame, nn::gfx::ColorTargetView *pTarget, nn::gfx::CommandBuffer *pCommandBuffer);

    private:

        // file system cache
        void* pMountRomCacheBuffer;

        // memory
        int32_t m_iFrame;
        int32_t m_iMemModule;
        int32_t m_iMemGroup;
        void *m_pMemGroupUserData;
        nn::util::BytePtr m_pMemoryHeap;
        nn::util::BytePtr m_pMemory;
        size_t mMaxScratchMemory;
        ptrdiff_t mScratchMemoryOffset;

        // descriptor index
        int32_t m_iBufferDescBaseIndex;
        int32_t m_iTextureDescBaseIndex;
        int32_t m_iSamplerDescBaseIndex;
    
        // display layer
        nn::vi::Display* m_pDisplay;
        nn::vi::Layer* m_pLayer;

        // grahics device
        nn::gfx::Device mDevice;

        // swap chain
        nn::gfx::SwapChain mSwapChain;

        // queue
        nn::gfx::Queue mQueue;

        // command buffer
        nn::gfx::CommandBuffer m_aCommandBuffer[T2NXRENDER_SCAN_BUFFER_COUNT];

        // viewport scissor
        nn::gfx::ViewportScissorState mViewportScissor;

        // rasterizer state
        nn::gfx::RasterizerState mRasterizerState;

        // blend state
        nn::gfx::BlendState mBlendState;

        // depth stencil state
        nn::gfx::DepthStencilState mDepthStencilState;

        // vertex state 
        nn::gfx::VertexState mVertexState;

        // vertex buffer
        nn::gfx::Buffer mVertextBuffer;

        // index buffer
        nn::gfx::Buffer mIndexBuffer;

        // constant buffer
        nn::gfx::Buffer mConstantBuffer;
        size_t mConstantBufferSize;

        // sampler
        nn::gfx::Sampler mSampler;

        // buffer descriptor
        nn::gfx::DescriptorPool mBufferDescPool;

        // sampler descriptor 
        nn::gfx::DescriptorPool mSamplerDescPool;

        // texture descriptor
        nn::gfx::DescriptorPool mTextureDescPool;

        // sychronization
        nn::gfx::Fence mDisplayFence;
        nn::gfx::Semaphore mDisplaySemaphore;

        // res texture file
        nn::gfx::ResTextureFile *m_pResTextureFile;

        // res shader file
        nn::gfx::ResShaderFile* m_pResShaderFile;

        // scan buffer
        nn::gfx::ColorTargetView* m_aScanBufferViews[T2NXRENDER_SCAN_BUFFER_COUNT];

        // memory pool
        void* m_apMemPool[MEMPOOL_TYPEEND];
        ptrdiff_t m_aMemPoolOffset[MEMPOOL_TYPEEND];
        nn::gfx::MemoryPool m_aMemPool[MEMPOOL_TYPEEND];

    private:
        // triangle vertices
        const float m_aVertices[20] =
        {
            0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
            -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
            0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, -0.5f, 0.0f, 1.0f, 1.0f
        };

        // indices
        const int32_t m_aIndices[4] =
        {
            0, 1, 2, 3
        };
};
/*** Variables *************************************************************************/

