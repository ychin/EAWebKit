/*H********************************************************************************/
/*!
    \File SampleCore.cpp

    \Description
        Provides functionality to start the sample

    \Copyright
        Copyright (c) 2005 Electronic Arts Inc.

    \Version 11/01/2012 (akirchner) First Version
             05/12/2019 Based heavily on Microsoft samples for GDK
*/
/********************************************************************************H*/

/*** Include files ****************************************************************/

#include <string>
#include <sstream>

#include <Windows.h>

#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtysock/dirtyerr.h"
#include "DirtySDK/dirtysock/netconn.h"

#include "DirtySDK/dirtysock/dirtymem.h"
#include "EASystemEventMessageDispatcher\SystemEventMessageDispatcher.h"

#include "samplecode/pch.h"
#include "samplecode/ReadData.h"
#include "samplecode/ATGColors.h"
#include "SampleCore.h"

/*** Defines **********************************************************************/
#define ENABLE_4K

using namespace DirectX;
using Microsoft::WRL::ComPtr;

/*** Type Definitions *************************************************************/
struct Vertex
{
    XMFLOAT3 pos;
    XMFLOAT3 normal;
};

EA::Pairing::EAControllerUserPairingServer *mpEacup;
EA::SEMD::SystemEventMessageDispatcher *mpEasmd;
T2IEAMessageHandler *mpEAmsghdlr;

// DirtyCoreAllocator is a bare bones memory allocator that piggy-backs on top of DirtyMem functionality
class DirtyCoreAllocator : public EA::Allocator::ICoreAllocator
{
private:
     int32_t m_iMemModule;
     int32_t m_iMemGroup;
     void    *m_pMemGroupUserData;

public:
    DirtyCoreAllocator():
    m_iMemModule(DIRTYEVENT_DISP_MEMID),
    m_iMemGroup(-1),
    m_pMemGroupUserData(NULL)
    {

    }

    ~DirtyCoreAllocator() {}

    void *Alloc(size_t size, const char *name, unsigned int flags)
    {
        if (m_iMemGroup == -1)
        {
            DirtyMemGroupQuery(&m_iMemGroup, &m_pMemGroupUserData);  
        }
        return(DirtyMemAlloc((int32_t)size, m_iMemModule, m_iMemGroup, m_pMemGroupUserData));
    }

    void *Alloc(size_t size, const char *name, unsigned int flags, unsigned int align, unsigned int alignOffset = 0)
    {
        // Since we are using DirtyMemAlloc, which does not support alignment, just call the simplified Alloc
        return(Alloc(size, name, flags));
    }

    void Free(void *block, size_t size=0)
    {
        DirtyMemFree(block, m_iMemModule, m_iMemGroup, m_pMemGroupUserData);
    }
};

ICOREALLOCATOR_INTERFACE_API EA::Allocator::ICoreAllocator *EA::Allocator::ICoreAllocator::GetDefaultAllocator()
{
    // NOTE: we can't just use another global allocator object, since we'd have an initialization race condition between the two globals.
    //   to avoid that, we use a static function var, which is init'd the first time the function is called.
    static DirtyCoreAllocator sTestAllocator; // static function members are only constructed on first function call
    return &sTestAllocator;
}

T2IEAMessageHandler::T2IEAMessageHandler()
{
    NetPrintf(("EACUP event handler: initializing\n"));
    ds_memclr(aLocalUsers, sizeof(aLocalUsers));
}

bool T2IEAMessageHandler::HandleMessage(EA::Messaging::MessageId messageId, void* pMessage)
{
    bool bSuccess = true;
    const EA::User::IEAUser *pLocalUser;
    EA::Pairing::UserMessage *pUserMessage;

    switch (messageId)
    {
        case EA::Pairing::E_USER_ADDED:
            NetPrintf(("EACUP event handler: E_USER_ADDED\n"));
            pUserMessage = (EA::Pairing::UserMessage *)pMessage;
            pLocalUser = pUserMessage->GetUser();
            pLocalUser->AddRef();
            bSuccess = AddLocalUser(pLocalUser);
            break;

        case EA::Pairing::E_USER_REMOVED:
            NetPrintf(("EACUP event handler: E_USER_REMOVED\n"));
            pUserMessage = (EA::Pairing::UserMessage *)pMessage;
            pLocalUser = pUserMessage->GetUser();
            bSuccess = RemoveLocalUser(pLocalUser);
            pLocalUser->Release();
            break;

        default:
            NetPrintf(("EACUP event handler: unsupported event (%d)\n", messageId));
            bSuccess = false;
            break;
    }

    return(bSuccess);
}

bool T2IEAMessageHandler::AddLocalUser(const EA::User::IEAUser *pLocalUser)
{
    bool bSuccess = false;
    int32_t iLocalUserIndex;

    for (iLocalUserIndex = 0; iLocalUserIndex < iMaxLocalUsers; iLocalUserIndex++)
    {
        if (aLocalUsers[iLocalUserIndex] == NULL)
        {
            aLocalUsers[iLocalUserIndex] = pLocalUser;
            if (NetConnAddLocalUser(iLocalUserIndex, pLocalUser) == 0)
            {
                bSuccess = true;
            }
            break;
        }
    }

    if (!bSuccess)
    {
        NetPrintf(("E_USER_ADDED failed\n"));
    }

    return(bSuccess);
}

bool T2IEAMessageHandler::RemoveLocalUser(const EA::User::IEAUser * pLocalUser)
{
    bool bSuccess = false;
    int32_t iLocalUserIndex;

    for (iLocalUserIndex = 0; iLocalUserIndex < iMaxLocalUsers; iLocalUserIndex++)
    {
        if (aLocalUsers[iLocalUserIndex] == pLocalUser)
        {
            if (NetConnRemoveLocalUser(iLocalUserIndex, pLocalUser) == 0)
            {
                bSuccess = true;
            }
            aLocalUsers[iLocalUserIndex] = NULL;
            break;
        }
    }

    if (!bSuccess)
    {
        NetPrintf(("E_USER_REMOVED failed\n"));
    }

    return(bSuccess);
}

/*F********************************************************************************/
/*!
    \Function Sample::Constructor

    \Description
        Creates the Sample

    \Version 05/12/2019 (from Microsoft Samples)
/*/
/********************************************************************************F*/
Sample::Sample() noexcept(false)
    : uFrame(0)
    , pMappedConstantData(nullptr)
    , ConstantDataGpuAddrT(0)
    , fCurRotationAngleRad(0.0f)
{
    // Use gamma-correct rendering.
    pDeviceResources = std::make_unique<DX::DeviceResources>(DXGI_FORMAT_B8G8R8A8_UNORM_SRGB, DXGI_FORMAT_D32_FLOAT, 2,
        DX::DeviceResources::c_Enable4K_UHD);
}

/*F********************************************************************************/
/*!
    \Function Sample::Destructor

    \Description
        Ensures the sample waits for the GPU to finish with resources

    \Version 05/12/2019 (from Microsoft Samples, modified)
/*/
/********************************************************************************F*/
Sample::~Sample()
{
    pDeviceResources->WaitForGpu();
}

/*F********************************************************************************/
/*!
    \Function Sample::Initialize

    \Description
        Initializes the resources required to run.

    \Input  window   the Application window

    \Version 05/12/2019 (combination of previous version by tcho and 
                         Microsoft Samples)
/*/
/********************************************************************************F*/
void Sample::Initialize(HWND window)
{
    pDeviceResources->SetWindow(window);

    pDeviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    pDeviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    m_fenceEvent.Attach(CreateEvent(nullptr, FALSE, FALSE, nullptr));

    // instantiate system event message dispatcher
    EA::SEMD::SystemEventMessageDispatcherSettings semdSettings;
    semdSettings.useCompanionHttpd = false;
    semdSettings.useCompanionUtil = false;
    semdSettings.useSystemService = true;
    semdSettings.useUserService = true;
    EA::SEMD::SystemEventMessageDispatcher::CreateInstance(semdSettings, EA::Allocator::ICoreAllocator::GetDefaultAllocator());

    // instantiate EACUP server
    mpEasmd = EA::SEMD::SystemEventMessageDispatcher::GetInstance();
    mpEacup = new EA::Pairing::EAControllerUserPairingServer(mpEasmd, EA::Allocator::ICoreAllocator::GetDefaultAllocator());
    mpEAmsghdlr = new T2IEAMessageHandler();
    mpEacup->Init();
    mpEacup->AddMessageHandler(mpEAmsghdlr, EA::Pairing::E_USER_ADDED, false, 0);
    mpEacup->AddMessageHandler(mpEAmsghdlr, EA::Pairing::E_USER_REMOVED, false, 0);

    // initialize Tester2
    InitializeT2();

    // tell netconn about EACUP list of users
    EA::User::UserList::iterator i;
    EA::User::UserList eacupUserList = mpEacup->GetUsers();
    for (i = eacupUserList.begin(); i != eacupUserList.end(); i++)
    {
        const EA::User::IEAUser *pLocalUser = *i;
        pLocalUser->AddRef();
        mpEAmsghdlr->AddLocalUser(pLocalUser);
    }

    // give life to NetConn
    NetConnIdle();
}

/*F********************************************************************************/
/*!
    \Function Sample::Uninitialize

    \Description
        Uninitializes the sample.

    \Version 16/05/2013 (tcho)
*/
/********************************************************************************F*/
void Sample::Uninitialize()
{
    NetPrintf(("samplecore: uninitialize   tick = %d\n", NetTick()));

    UninitializeT2();

    // free EACUP server
    mpEacup->RemoveMessageHandler(mpEAmsghdlr, EA::Pairing::E_USER_ADDED);
    mpEacup->RemoveMessageHandler(mpEAmsghdlr, EA::Pairing::E_USER_REMOVED);
    mpEacup->Shutdown();
    delete mpEAmsghdlr;
    delete mpEacup;
    mpEasmd->DestroyInstance();

}

/*F********************************************************************************/
/*!
    \Function Sample::Tick

    \Description
        Executes the basic render loop

    \Version 05/12/2019 (from Microsoft Samples)
/*/
/********************************************************************************F*/
void Sample::Tick()
{
    PIXBeginEvent(PIX_COLOR_DEFAULT, L"Frame %I64u", uFrame);

    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();

    PIXEndEvent();
    uFrame++;
}

/*F********************************************************************************/
/*!
    \Function Sample::Update

    \Description
        Updates the world

    \Version 05/12/2019 (from Microsoft Samples)
/*/
/********************************************************************************F*/
void Sample::Update(const DX::StepTimer &timer)
{
    PIXScopedEvent(PIX_COLOR_DEFAULT, L"Update");

    float elapsedTime = float(timer.GetElapsedSeconds());

    // Update the rotation constant
    fCurRotationAngleRad += elapsedTime / 3.f;
    if (fCurRotationAngleRad >= XM_2PI)
    {
        fCurRotationAngleRad -= XM_2PI;
    }

    // Rotate the cube around the origin
    XMStoreFloat4x4(&WorldMatrix, XMMatrixRotationY(fCurRotationAngleRad));

    // Setup our lighting parameters
    aLightDirs[0] = XMFLOAT4(-0.577f, 0.577f, -0.577f, 1.0f);
    aLightDirs[1] = XMFLOAT4(0.0f, 0.0f, -1.0f, 1.0f);

    aLightColors[0] = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f); // white light
    aLightColors[1] = XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f); //teal light

    // Rotate the second light around the origin
    XMMATRIX rotate = XMMatrixRotationY(-2.0f * fCurRotationAngleRad);
    XMVECTOR aLightDir = XMLoadFloat4(&aLightDirs[1]);
    aLightDir = XMVector3Transform(aLightDir, rotate);
    XMStoreFloat4(&aLightDirs[1], aLightDir);
}

/*F********************************************************************************/
/*!
    \Function Sample::Render

    \Description
        Draws the scene

    \Version 05/12/2019 (from Microsoft samples)
/*/
/********************************************************************************F*/
void Sample::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    // Check to see if the GPU is keeping up
    uint32_t uFrameIdx = pDeviceResources->GetCurrentFrameIndex();
    uint32_t uNumBackBuffers = pDeviceResources->GetBackBufferCount();
    uint64_t uCompletedValue = cpFence->GetCompletedValue();
    if ((uFrameIdx > uCompletedValue) // if frame index is reset to zero it may temporarily be smaller than the last GPU signal
        && (uFrameIdx - uCompletedValue > uNumBackBuffers))
    {
        // GPU not caught up, wait for at least one available frame
        DX::ThrowIfFailed(cpFence->SetEventOnCompletion(uFrameIdx - uNumBackBuffers, m_fenceEvent.Get()));
        WaitForSingleObjectEx(m_fenceEvent.Get(), INFINITE, FALSE);
    }

    // Prepare the command list to render a new frame.
    pDeviceResources->Prepare();
    Clear();

    ID3D12GraphicsCommandList *pCommandList = pDeviceResources->GetCommandList();
    PIXBeginEvent(pCommandList, PIX_COLOR_DEFAULT, L"Render");

    // Index into the available constant buffers based on the number
    // of draw calls. We've allocated enough for a known number of
    // draw calls per frame times the number of back buffers
    uint32_t uConstantBufferIndex = uNumDrawCalls * (uFrameIdx % uNumBackBuffers);

    // Set the root signature and pipeline state for the command list
    pCommandList->SetGraphicsRootSignature(cpRootSignature.Get());
    pCommandList->SetPipelineState(cpLambertPipelineState.Get());
    
    // Set the per-frame constants
    ConstantBuffer SceneParameters = {};

    // Shaders compiled with default row-major matrices
    SceneParameters.WorldMatrix = XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix));
    SceneParameters.ViewMatrix = XMMatrixTranspose(XMLoadFloat4x4(&ViewMatrix));
    SceneParameters.ProjectionMatrix = XMMatrixTranspose(XMLoadFloat4x4(&ProjectionMatrix));

    SceneParameters.aLightDir[0] = XMLoadFloat4(&aLightDirs[0]);
    SceneParameters.aLightDir[1] = XMLoadFloat4(&aLightDirs[1]);
    SceneParameters.aLightColor[0] = XMLoadFloat4(&aLightColors[0]);
    SceneParameters.aLightColor[1] = XMLoadFloat4(&aLightColors[1]);
    SceneParameters.OutputColorT = XMLoadFloat4(&OutputColor);

    // Set the constants for the first draw call
    memcpy(&pMappedConstantData[uConstantBufferIndex].Constants, &SceneParameters, sizeof(ConstantBuffer));

    // Bind the constants to the shader
    uint64_t baseGpuAddress = ConstantDataGpuAddrT + sizeof(PaddedConstantBuffer) * uConstantBufferIndex;
    pCommandList->SetGraphicsRootConstantBufferView(uRootParameterCB, baseGpuAddress);

    // Set up the input assembler
    pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    pCommandList->IASetVertexBuffers(0, 1, &VertexBufferView);
    pCommandList->IASetIndexBuffer(&IndexBufferView);

    // Draw the Lambert lit cube
    pCommandList->DrawIndexedInstanced(36, 1, 0, 0, 0);
    baseGpuAddress += sizeof(PaddedConstantBuffer);
    ++uConstantBufferIndex;

    // Render each light
    pCommandList->SetPipelineState(cpSolidColorPipelineState.Get());

    for (uint32_t uLightCube = 0; uLightCube < 2; ++uLightCube)
    {
        XMMATRIX LightMatrix = XMMatrixTranslationFromVector(XMVectorScale(SceneParameters.aLightDir[uLightCube], 5.0f));
        XMMATRIX LightScaleMatrix = XMMatrixScaling(0.2f, 0.2f, 0.2f);
        LightMatrix = LightScaleMatrix * LightMatrix;

        // Update the world variable to reflect the current light
        SceneParameters.WorldMatrix = XMMatrixTranspose(LightMatrix);
        SceneParameters.OutputColorT = SceneParameters.aLightColor[uLightCube];

        // Set the constants for the draw call
        memcpy(&pMappedConstantData[uConstantBufferIndex].Constants, &SceneParameters, sizeof(ConstantBuffer));

        // Bind the constants to the shader
        pCommandList->SetGraphicsRootConstantBufferView(uRootParameterCB, baseGpuAddress);

        pCommandList->DrawIndexedInstanced(36, 1, 0, 0, 0);
        baseGpuAddress += sizeof(PaddedConstantBuffer);
        ++uConstantBufferIndex;
    }

    PIXEndEvent(pCommandList);

    // Show the new frame.
    PIXBeginEvent(PIX_COLOR_DEFAULT, L"Present");
    pDeviceResources->Present();

    // GPU will signal an increasing value each frame
    pDeviceResources->GetCommandQueue()->Signal(cpFence.Get(), uFrameIdx);
    
    PIXEndEvent();
}

/*F********************************************************************************/
/*!
    \Function Sample::Clear

    \Description
        Helper method to clear the back buffers.

    \Version 05/12/2019 (from Microsoft Samples)
/*/
/********************************************************************************F*/
void Sample::Clear()
{
    ID3D12GraphicsCommandList *pCommandList = pDeviceResources->GetCommandList();
    PIXBeginEvent(pCommandList, PIX_COLOR_DEFAULT, L"Clear");

    // Clear the views.
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvDescriptor = pDeviceResources->GetRenderTargetView();
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvDescriptor = pDeviceResources->GetDepthStencilView();

    pCommandList->OMSetRenderTargets(1, &rtvDescriptor, FALSE, &dsvDescriptor);

    // Use linear clear color for gamma-correct rendering.
    pCommandList->ClearRenderTargetView(rtvDescriptor, ATG::ColorsLinear::Background, 0, nullptr);

    pCommandList->ClearDepthStencilView(dsvDescriptor, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    // Set the viewport and scissor rect.
    D3D12_VIEWPORT viewport = pDeviceResources->GetScreenViewport();
    D3D12_RECT scissorRect = pDeviceResources->GetScissorRect();
    pCommandList->RSSetViewports(1, &viewport);
    pCommandList->RSSetScissorRects(1, &scissorRect);

    PIXEndEvent(pCommandList);
}

/*F********************************************************************************/
/*!
    \Function Sample::OnSuspending

    \Description
        Message handler for suspend event

    \Version 05/12/2019 (from Microsoft Samples)
/*/
/********************************************************************************F*/
void Sample::OnSuspending()
{
    NetPrintf(("samplecore: suspending\n"));
    pDeviceResources->Suspend();
}

/*F********************************************************************************/
/*!
    \Function Sample::OnResuming

    \Description
        Message handler for resume event

    \Version 05/12/2019 (from Microsoft Samples)
/*/
/********************************************************************************F*/
void Sample::OnResuming()
{
    NetPrintf(("samplecore: resuming\n", NetTick()));
    pDeviceResources->Resume();
    m_timer.ResetElapsedTime();

    //$todo: netconn is returning -net for status after resume.
}

/*F********************************************************************************/
/*!
    \Function Sample::CreateDeviceDependentResources

    \Description
        Creates the resources that depend on the DirectX Device

    \Version 05/12/2019 (from Microsoft Samples)
/*/
/********************************************************************************F*/
void Sample::CreateDeviceDependentResources()
{
    ID3D12Device *pDevice = pDeviceResources->GetD3DDevice();

    // Create a root signature.
    std::vector<uint8_t> triangleVSBlob = DX::ReadData(L"TriangleVS.cso");

    // Xbox One best practice is to use HLSL-based root signatures to support shader precompilation.

    DX::ThrowIfFailed(
        pDevice->CreateRootSignature(0, triangleVSBlob.data(), triangleVSBlob.size(),
            IID_GRAPHICS_PPV_ARGS(cpRootSignature.ReleaseAndGetAddressOf())));

    // Create the constant buffer memory and map the CPU and GPU addresses
    {
        const D3D12_HEAP_PROPERTIES uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        size_t cbSize = uNumDrawCalls * pDeviceResources->GetBackBufferCount() * sizeof(PaddedConstantBuffer);

        const D3D12_RESOURCE_DESC ConstantBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(cbSize);
        DX::ThrowIfFailed(pDevice->CreateCommittedResource(
            &uploadHeapProperties,
            D3D12_HEAP_FLAG_NONE,
            &ConstantBufferDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_GRAPHICS_PPV_ARGS(cpPerFrameConstants.ReleaseAndGetAddressOf())));

        DX::ThrowIfFailed(cpPerFrameConstants->Map(0, nullptr, reinterpret_cast<void**>(&pMappedConstantData)));

        ConstantDataGpuAddrT = cpPerFrameConstants->GetGPUVirtualAddress();
    }

    // Load the shader blob for the vertex shader that will be shared by two pipeline state objects
    {
        // Input element descriptor also shared by two pipeline state objects
        static const D3D12_INPUT_ELEMENT_DESC aInputElementDesc[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        };

        // Create the Pipelline State Object for the Lambert pixel shader
        {
            std::vector<uint8_t> lambertPSBlob = DX::ReadData(L"LambertPS.cso");

            D3D12_GRAPHICS_PIPELINE_STATE_DESC PsoDesc = {};
            PsoDesc.InputLayout = { aInputElementDesc, _countof(aInputElementDesc) };
            PsoDesc.pRootSignature = cpRootSignature.Get();
            PsoDesc.VS = { triangleVSBlob.data(), triangleVSBlob.size() };
            PsoDesc.PS = { lambertPSBlob.data(), lambertPSBlob.size() };
            PsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
            PsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
            PsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
            PsoDesc.DSVFormat = pDeviceResources->GetDepthBufferFormat();
            PsoDesc.SampleMask = UINT_MAX;
            PsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            PsoDesc.NumRenderTargets = 1;
            PsoDesc.RTVFormats[0] = pDeviceResources->GetBackBufferFormat();
            PsoDesc.SampleDesc.Count = 1;
            DX::ThrowIfFailed(
                pDevice->CreateGraphicsPipelineState(&PsoDesc,
                    IID_GRAPHICS_PPV_ARGS(cpLambertPipelineState.ReleaseAndGetAddressOf())));
        }

        // Create the Pipeline State Object for the solid color pixel shader
        {
            std::vector<uint8_t> solidColorPSBlob = DX::ReadData(L"SolidColorPS.cso");

            D3D12_GRAPHICS_PIPELINE_STATE_DESC PsoDesc = {};
            PsoDesc.InputLayout = { aInputElementDesc, _countof(aInputElementDesc) };
            PsoDesc.pRootSignature = cpRootSignature.Get();
            PsoDesc.VS = { triangleVSBlob.data(), triangleVSBlob.size() };
            PsoDesc.PS = { solidColorPSBlob.data(), solidColorPSBlob.size() };
            PsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
            PsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
            PsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
            PsoDesc.DSVFormat = pDeviceResources->GetDepthBufferFormat();
            PsoDesc.SampleMask = UINT_MAX;
            PsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            PsoDesc.NumRenderTargets = 1;
            PsoDesc.RTVFormats[0] = pDeviceResources->GetBackBufferFormat();
            PsoDesc.SampleDesc.Count = 1;
            DX::ThrowIfFailed(
                pDevice->CreateGraphicsPipelineState(&PsoDesc,
                    IID_GRAPHICS_PPV_ARGS(cpSolidColorPipelineState.ReleaseAndGetAddressOf())));
        }
    }

    // Create the vertex buffer
    {
        static const Vertex aVertices[] =
        {
            { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
            { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
            { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
            { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },

            { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
            { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
            { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
            { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },

            { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
            { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
            { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
            { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },

            { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
            { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
            { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
            { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },

            { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
            { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
            { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
            { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },

            { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
            { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
            { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
            { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
        };

        // Note: using upload heaps to transfer static data like vert buffers is not
        // recommended. Every time the GPU needs it, the upload heap will be marshalled
        // over. An upload heap is used here because the samples did for code 
        // simplicity and because there are very few verts to actually transfer.
        const D3D12_HEAP_PROPERTIES UploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        const CD3DX12_RESOURCE_DESC ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(aVertices));
        DX::ThrowIfFailed(
            pDevice->CreateCommittedResource(&UploadHeapProperties,
                D3D12_HEAP_FLAG_NONE,
                &ResourceDesc,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_GRAPHICS_PPV_ARGS(cpVertexBuffer.ReleaseAndGetAddressOf())));

        // Copy the data to the vertex buffer.
        UINT8* pVertexDataBegin;
        CD3DX12_RANGE ReadRange(0, 0);      // We do not intend to read from this resource on the CPU.
        DX::ThrowIfFailed(
            cpVertexBuffer->Map(0, &ReadRange, reinterpret_cast<void**>(&pVertexDataBegin)));
        memcpy(pVertexDataBegin, aVertices, sizeof(aVertices));
        cpVertexBuffer->Unmap(0, nullptr);

        // Initialize the vertex buffer view.
        VertexBufferView.BufferLocation = cpVertexBuffer->GetGPUVirtualAddress();
        VertexBufferView.StrideInBytes = sizeof(Vertex);
        VertexBufferView.SizeInBytes = sizeof(aVertices);
    }

    // Create the index buffer
    {
        static const uint16_t aIndices[] =
        {
            3,1,0,
            2,1,3,

            6,4,5,
            7,4,6,

            11,9,8,
            10,9,11,

            14,12,13,
            15,12,14,

            19,17,16,
            18,17,19,

            22,20,21,
            23,20,22
        };

        const D3D12_HEAP_PROPERTIES UploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        const CD3DX12_RESOURCE_DESC ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(aIndices));
        DX::ThrowIfFailed(
            pDevice->CreateCommittedResource(&UploadHeapProperties,
                D3D12_HEAP_FLAG_NONE,
                &ResourceDesc,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_GRAPHICS_PPV_ARGS(cpIndexBuffer.ReleaseAndGetAddressOf())));

        // Copy the data to the index buffer.
        UINT8* pVertexDataBegin;
        CD3DX12_RANGE ReadRange(0, 0);      // We do not intend to read from this resource on the CPU.
        DX::ThrowIfFailed(
            cpIndexBuffer->Map(0, &ReadRange, reinterpret_cast<void**>(&pVertexDataBegin)));
        memcpy(pVertexDataBegin, aIndices, sizeof(aIndices));
        cpIndexBuffer->Unmap(0, nullptr);

        // Initialize the index buffer view.
        IndexBufferView.BufferLocation = cpIndexBuffer->GetGPUVirtualAddress();
        IndexBufferView.Format = DXGI_FORMAT_R16_UINT;
        IndexBufferView.SizeInBytes = sizeof(aIndices);
    }

    // Wait until assets have been uploaded to the GPU.
    pDeviceResources->WaitForGpu();

    // Create a fence for synchronizing between the CPU and the GPU
    DX::ThrowIfFailed(pDevice->CreateFence(pDeviceResources->GetCurrentFrameIndex(), D3D12_FENCE_FLAG_NONE,
        IID_GRAPHICS_PPV_ARGS(cpFence.ReleaseAndGetAddressOf())));

    // Start off the fence with the current frame index
    uint64_t uCurrentIdx = pDeviceResources->GetCurrentFrameIndex();
    pDeviceResources->GetCommandQueue()->Signal(cpFence.Get(), uCurrentIdx);

    // Initialize the world matrix
    XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

    // Initialize the view matrix
    static const XMVECTORF32 Eye = { 0.0f, 4.0f, -10.0f, 0.0f };
    static const XMVECTORF32 At = { 0.0f, 1.0f, 0.0f, 0.0f };
    static const XMVECTORF32 Up = { 0.0f, 1.0f, 0.0f, 0.0f };
    XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(Eye, At, Up));

    // Initialize the lighting parameters
    aLightDirs[0] = XMFLOAT4(-0.577f, 0.577f, -0.577f, 1.0f);
    aLightDirs[1] = XMFLOAT4(0.0f, 0.0f, -1.0f, 1.0f);

    aLightColors[0] = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    aLightColors[1] = XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f);

    // Initialize the scene output color
    OutputColor = XMFLOAT4(0, 0, 0, 0);
}

/*F********************************************************************************/
/*!
    \Function Sample::CreateWindowSizeDependentResources

    \Description
        Allocates all the memory resources that change on a window 
        SizeChanged event

    \Version 05/12/2019 (from Microsoft Samples)
/*/
/********************************************************************************F*/
void Sample::CreateWindowSizeDependentResources()
{
    // Initialize the projection matrix
    RECT size = pDeviceResources->GetOutputSize();
    XMMATRIX Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, float(size.right) / float(size.bottom), 0.01f, 100.0f);

    XMStoreFloat4x4(&ProjectionMatrix, Projection);
}

/*F********************************************************************************/
/*!
    \Function Sample::Run

    \Description
        Run the sample

    \Version 12/05/2019 (combination of previous version by tcho and 
        Microsoft samples)
/*/
/********************************************************************************F*/
void Sample::Run(Sample *pSample)
{
    NetPrintf(("samplecore: Run\n"));

    ConnectT2();

    // Main message loop
    MSG msg = {};
    while (WM_QUIT != msg.message)
    {
        mpEasmd->Tick();
        mpEacup->Tick();

        // give time for other threads
        NetConnSleep(16);

        // render
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // this will update and render the sample
            pSample->Tick();
        }

        // update (must be called periodically, otherwise T2 Host does not respond)
        UpdateT2();

    }
}

