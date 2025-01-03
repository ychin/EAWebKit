/*H*************************************************************************************************/
/*!

    \File SampleCore.h

    \Description
        This implements a basic sample class

    \Notes
        None.

    \Copyright
        Copyright (c)  Electronic Arts 2013.  ALL RIGHTS RESERVED.

    \Version 2.0 (12/12/2019, based on previous versions & Microsoft provided samples) 

*/
/*************************************************************************************************H*/

#ifndef _sample_core_h
#define _sample_core_h

/*** Include files ****************************************************************/

#include "IEAController/IEAController.h"
#include "EAControllerUserPairing/EAControllerUserPairingServer.h"
#include "EAControllerUserPairing/Messages.h"

#include "samplecode/DeviceResources.h"
#include "samplecode/StepTimer.h"


/*** Function Prototype ***********************************************************/

// these are implemented in T2Host.cpp
void InitializeT2(void);
bool UpdateT2(void);
void UninitializeT2(void);
void ConnectT2(void);

class T2IEAMessageHandler : public EA::Messaging::IHandler
{
public:
    T2IEAMessageHandler();
    bool HandleMessage(EA::Messaging::MessageId messageId, void* pMessage);
    bool AddLocalUser(const EA::User::IEAUser *pLocalUser);
    bool RemoveLocalUser(const EA::User::IEAUser * pLocalUser);

private:
    static const int32_t iMaxLocalUsers = 16;
    const EA::User::IEAUser *aLocalUsers[iMaxLocalUsers];
};

//--------------------------------------------------------------------------------------
// Name: Sample
// Desc: A basic sample implementation that creates a D3D12 device and
//       provides the update & render loop
// Note: Using DX12, taken from Microsoft provided samples
//--------------------------------------------------------------------------------------

class Sample
{
public:

    Sample() noexcept(false);
    ~Sample();

    // Initialization and management
    void Initialize(HWND window);
    void Sample::Run(Sample *pSample);

    // Messages
    void OnSuspending();
    void OnResuming();

    void Uninitialize();

private:

    struct ConstantBuffer
    {
        DirectX::XMMATRIX WorldMatrix;
        DirectX::XMMATRIX ViewMatrix;
        DirectX::XMMATRIX ProjectionMatrix;
        DirectX::XMVECTOR aLightDir[2];
        DirectX::XMVECTOR aLightColor[2];
        DirectX::XMVECTOR OutputColorT;
    };

    // We'll allocate space for several of these and they will need to be padded for alignment.
    static_assert(sizeof(ConstantBuffer) == 272, "Checking the size here.");

    // D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT < 272 < 2 * D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT
    // Create a union with the correct size and enough room for one ConstantBuffer
    union PaddedConstantBuffer
    {
        ConstantBuffer Constants;
        uint8_t aBytes[2 * D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT];
    };

    // Check the exact size of the PaddedConstantBuffer to make sure it will align properly
    static_assert(sizeof(PaddedConstantBuffer) == 2 * D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT, "PaddedConstantBuffer is not aligned properly");

    void Sample::Update(const DX::StepTimer &timer);
    void Render();

    // Basic render loop
    void Tick();

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();
 
    // Device resources.
    std::unique_ptr<DX::DeviceResources>        pDeviceResources;

    // Rendering loop timer.
    uint64_t                                    uFrame;
    DX::StepTimer                               m_timer;

    // DirectXTK objects.
    Microsoft::WRL::ComPtr<ID3D12RootSignature>  cpRootSignature;
    Microsoft::WRL::ComPtr<ID3D12PipelineState>  cpLambertPipelineState;
    Microsoft::WRL::ComPtr<ID3D12PipelineState>  cpSolidColorPipelineState;
    Microsoft::WRL::ComPtr<ID3D12Resource>       cpVertexBuffer;
    Microsoft::WRL::ComPtr<ID3D12Resource>       cpIndexBuffer;
    Microsoft::WRL::ComPtr<ID3D12Resource>       cpPerFrameConstants;
    PaddedConstantBuffer*                        pMappedConstantData;
    D3D12_GPU_VIRTUAL_ADDRESS                    ConstantDataGpuAddrT;
    D3D12_VERTEX_BUFFER_VIEW                     VertexBufferView;
    D3D12_INDEX_BUFFER_VIEW                      IndexBufferView;

    // In this simple sample, we know that there are three draw calls
    // and we will update the scene constants for each draw call.
    static const uint32_t                        uNumDrawCalls = 3;

    // A synchronization fence and an event. These members will be used
    // to synchronize the CPU with the GPU so that there will be no
    // contention for the constant buffers. 
    Microsoft::WRL::ComPtr<ID3D12Fence>          cpFence;
    Microsoft::WRL::Wrappers::Event              m_fenceEvent;

    // Index in the root parameter table
    static const uint32_t                        uRootParameterCB = 0;

    // Scene constants, updated per-frame
    float                                        fCurRotationAngleRad;

    // These computed values will be loaded into a ConstantBuffer
    // during Render
    DirectX::XMFLOAT4X4                          WorldMatrix;
    DirectX::XMFLOAT4X4                          ViewMatrix;
    DirectX::XMFLOAT4X4                          ProjectionMatrix;
    DirectX::XMFLOAT4                            aLightDirs[2];
    DirectX::XMFLOAT4                            aLightColors[2];
    DirectX::XMFLOAT4                            OutputColor;
};
#endif