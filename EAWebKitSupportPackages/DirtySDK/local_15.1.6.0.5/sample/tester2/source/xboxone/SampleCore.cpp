/*H********************************************************************************/
/*!
    \File Sample.cpp

    \Description
        Provides functionality to start the sample

    \Copyright
        Copyright (c) 2005 Electronic Arts Inc.

    \Version 11/01/2012 (akirchner) First Version
*/
/********************************************************************************H*/

/*** Include files ****************************************************************/
#include "DirtySDK/platform.h"

#include <string>
#include <sstream>

#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtysock/netconn.h"

#include "SampleCore.h"

#include "DirtySDK/dirtysock/dirtymem.h"
#include "EASystemEventMessageDispatcher\SystemEventMessageDispatcher.h"

#include <d3dcompiler_x.h>

using namespace Platform;

using namespace Windows::Data;
using namespace Windows::Foundation;
using namespace Windows::Storage::Streams;

using namespace Windows::ApplicationModel::Core;
using namespace Windows::UI::Core;
using namespace Windows::ApplicationModel::Activation;


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
    \Function Sample::Initialize

    \Description
        Implements Initialize from IFrameworkView. Activates application view.
        Subscribes to Suspending and Resuming callback

    \Input  applicationView   Windows::ApplicationModel::Core::CoreApplicationView

    \Version 16/05/2013 (tcho)
/*/
/********************************************************************************F*/
void Sample::Initialize( Windows::ApplicationModel::Core::CoreApplicationView^ applicationView )
{
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
    for(i = eacupUserList.begin(); i != eacupUserList.end(); i++)
    {
        const EA::User::IEAUser *pLocalUser = *i;
        pLocalUser->AddRef();
        mpEAmsghdlr->AddLocalUser(pLocalUser);
    }

    // give life to NetConn
    NetConnIdle();

    applicationView->Activated += ref new Windows::Foundation::TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>([](CoreApplicationView^, IActivatedEventArgs^ args) {
            Windows::UI::Core::CoreWindow::GetForCurrentThread()->Activate();
        });
    CoreApplication::Suspending += ref new Windows::Foundation::EventHandler< Windows::ApplicationModel::SuspendingEventArgs^ >(this, &Sample::OnSuspending);
    CoreApplication::Resuming += ref new Windows::Foundation::EventHandler< Platform::Object^>(this, &Sample::OnResuming); 
}

/*F********************************************************************************/
/*!
    \Function Sample::SetWindow

    \Description
        Implements SetWindow from IFrameworkView. (Doesn't do anything)

    \Input  window   Windows::UI::Core::CoreWindow

    \Version 16/05/2013 (tcho)
/*/
/********************************************************************************F*/
void Sample::SetWindow( Windows::UI::Core::CoreWindow^ /*window*/ )
{
    NetPrintf(("SampleCore: IFrameworkView::SetWindow\n"));
}

/*F********************************************************************************/
/*!
    \Function Sample::Load

    \Description
        Implements Load from IFrameworkView. (Doesn't do anything)

    \Input  str  Platform::String

    \Version 16/05/2013 (tcho)
/*/
/********************************************************************************F*/
void Sample::Load( Platform::String^ /*str*/ )
{
    NetPrintf(("SampleCore: IFrameworkView::Load\n"));
}

/*F********************************************************************************/
/*!
    \Function Sample::Run

    \Description
        Implements Run from IFrameworkView.

    \Version 16/05/2013 (tcho)
/*/
/********************************************************************************F*/
void Sample::Run()
{
    NetPrintf(("SampleCore: IFrameworkView::Run\n"));

    // initialize Renderer
    mRenderer.Initialize();

    // give the event dispatcher time to get the user events
    for (int32_t iTimeout = NetTick()+5*1000 ; ; )
    {
        Windows::UI::Core::CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(Windows::UI::Core::CoreProcessEventsOption::ProcessAllIfPresent);

        mpEasmd->Tick();
        mpEacup->Tick();

        NetConnIdle();

        // check for user being paired
        if (mpEacup->GetUsers().size() > 0)
        {
            break;
        }

        // check for timeout
        if (iTimeout < (signed)NetTick())
        {
            break;
        }

        // give time to other threads
        NetConnSleep(16);
    }

    ConnectT2();

    for (;;)
    {
        Windows::UI::Core::CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(Windows::UI::Core::CoreProcessEventsOption::ProcessAllIfPresent);

        mpEasmd->Tick();
        mpEacup->Tick();

        // give time for other threads
        NetConnSleep(16);

        // render
        mRenderer.Render();

        // update (must be call periodically if not T2 Host does not respond)
        UpdateT2();

        // if we are in a -xbl for any reason disconnect and reconnect (happens on resume)
        if (NetConnStatus('conn',0, NULL, 0) == '-xbl')
        {
            NetConnDisconnect();
            UpdateT2();
            ConnectT2();
        }
    }
}

/*F********************************************************************************/
/*!
    \Function Sample::Uninitialize

    \Description
        Implements Uninitialize from IFrameworkView.

    \Version 16/05/2013 (tcho)
*/
/********************************************************************************F*/
void Sample::Uninitialize()
{
    NetPrintf(("SampleCore: IFrameworkView::Uninitialize   tick = %d\n", NetTick()));

    UninitializeT2();

    // free EACUP server
    mpEacup->RemoveMessageHandler(mpEAmsghdlr, EA::Pairing::E_USER_ADDED);
    mpEacup->RemoveMessageHandler(mpEAmsghdlr, EA::Pairing::E_USER_REMOVED);
    delete mpEAmsghdlr;
    mpEacup->Shutdown();
    delete mpEacup;
    mpEasmd->DestroyInstance();
}

void Sample::OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args )
{
    NetPrintf(("SampleCore: IFrameworkView::OnActivated   tick = %d\n", NetTick()));

    CoreWindow^ window = CoreWindow::GetForCurrentThread(); 

    window->VisibilityChanged += ref new Windows::Foundation::TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>( this, &Sample::OnVisibilityChanged );
    window->Activated  += ref new Windows::Foundation::TypedEventHandler<CoreWindow^,WindowActivatedEventArgs^>( this, &Sample::OnWindowActivatedChanged );

    window->Activate();
}

void Sample::OnResuming(Platform::Object^ sender, Platform::Object^ args)
{
    NetPrintf(("SampleCore: IFrameworkView::OnResuming triggered   tick = %d\n", NetTick()));
    mRenderer.pD3DXboxPerfContext->Resume();

}

//--------------------------------------------------------------------------------------
// This event handler is called by when title window visibility is changed. 
//--------------------------------------------------------------------------------------
void Sample::OnVisibilityChanged(CoreWindow^ window, VisibilityChangedEventArgs^ args)
{
    try
    {
        if( args->Visible )
        {
            NetPrintf(("SampleCore: IFrameworkView::OnVisibilityChanged - Window gained visibility   tick = %d\n", NetTick()));
        }
        else
        {
            NetPrintf(("SampleCore: IFrameworkView::OnVisibilityChanged - Window lost visibility   tick = %d\n", NetTick()));
        }
    }
    catch( Platform::Exception^ ex )
    {
        NetPrintf(("SampleCore: IFrameworkView::OnVisibilityChanged - Exception thrown by VisibilityChangedEventArgs::Visible; Error code = 0x%x, Message = %s\n", ex->HResult, ex->Message));
    }
}

//--------------------------------------------------------------------------------------
// This event handler is called by when title window focus state is changed. 
//--------------------------------------------------------------------------------------
void Sample::OnWindowActivatedChanged(CoreWindow^ window, WindowActivatedEventArgs^ args)
{
    try
    {
        if( args->WindowActivationState == CoreWindowActivationState::CodeActivated )
        {
            NetPrintf(("SampleCore: IFrameworkView::OnWindowActivatedChanged - Window gained focus, CodeActivated   tick = %d\n", NetTick()));
        }
        else if ( args->WindowActivationState ==  CoreWindowActivationState::Deactivated )
        {
            NetPrintf(("SampleCore: IFrameworkView::OnWindowActivatedChanged - Window lost focus, Deactivated   tick = %d\n", NetTick()));
        }
        else if ( args->WindowActivationState ==  CoreWindowActivationState::PointerActivated )
        {
            NetPrintf(("SampleCore: IFrameworkView::OnWindowActivatedChanged - Window gained focus, PointerActivated   tick = %d\n", NetTick()));
        }
    }
    catch( Platform::Exception^ ex )
    {
        NetPrintf(("SampleCore: IFrameworkView::OnWindowActivatedChanged- Exception thrown by WindowActivatedEventArgs::WindowActivationState; Error code = 0x%x, Message = %s\n", ex->HResult, ex->Message));
    }

}

void Sample::OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ args)
{
    NetPrintf(("SampleCore: IFrameworkView::OnSuspending   tick = %d\n", NetTick()));
    // calling the suspend API
    // omitting to call this will result in the sample being terminated because of an incomplete suspend.
    mRenderer.pD3DXboxPerfContext->Suspend(0);
}

void Sample::OnResourceAvailabilityChanged( Platform::Object^ sender, Platform::Object^ args)
{
    if( Windows::ApplicationModel::Core::CoreApplication::ResourceAvailability == Windows::ApplicationModel::Core::ResourceAvailability::Full )
    {
        NetPrintf(("SampleCore: IFrameworkView::OnResourceAvailabilityChanged - entered the 'Full Running' state   tick = %d\n", NetTick()));
    }
    else
    {
        NetPrintf(("SampleCore: IFrameworkView::OnResourceAvailabilityChanged - entered the 'Constrained' state   tick = %d\n", NetTick()));
    }
}


/*F********************************************************************************/
/*!
    \Function SampleFactory::CreateView

    \Description
        Create the sample View

    \Output Returns the sample view

    \Version 16/05/2013 (tcho)
*/
/********************************************************************************F*/
Windows::ApplicationModel::Core::IFrameworkView^ SampleFactory::CreateView()
{
    return ref new Sample();
}

/*F********************************************************************************/
/*!
    \Function SampleRenderer::Initialize

    \Description
        Initialize D3D Deivce and Swap Chain

    \Version 03/03/2014 (tcho)
*/
/********************************************************************************F*/
void SampleRenderer::Initialize()
{
    //Get the current window handle
    CoreWindow^ pWindow = CoreWindow::GetForCurrentThread();
    uHeight = pWindow->Bounds.Height;
    uWidth = pWindow->Bounds.Width;

    //Use to convert the back to DX11.1 versions
    ComPtr<ID3D11Device> pTempDevice;
    ComPtr<ID3D11DeviceContext> pTempDeviceContext;

    // Create the device and device context objects
    D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0, D3D11_SDK_VERSION, &pTempDevice, NULL, &pTempDeviceContext);

    //Convert Device and Context back to DX11.1
    pTempDevice.As(&pD3DDevice);
    pTempDeviceContext.As(&pD3DDeviceContext);

    pD3DDeviceContext.As(&pD3DXboxPerfContext);

    //On Xone you need the dxgi factory to create the swap chain
    ComPtr<IDXGIDevice2> pDxgiDevice;
    ComPtr<IDXGIAdapter> pDxgiAdapter;
    ComPtr<IDXGIFactory2> pDxgiFactory;

    pD3DDevice.As(&pDxgiDevice);
    pDxgiDevice->GetAdapter(&pDxgiAdapter);
    pDxgiAdapter->GetParent(__uuidof(IDXGIFactory2), &pDxgiFactory);

    //Setup swap chain description (came from MS samples)
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
    ds_memclr(&swapChainDesc, sizeof(swapChainDesc));
    swapChainDesc.Height = uHeight;
    swapChainDesc.Width = uWidth;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 2;
    swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
    swapChainDesc.SampleDesc.Count = 1;

    //Create the Swap Chain
    pDxgiFactory->CreateSwapChainForCoreWindow(pD3DDevice.Get(), reinterpret_cast<IUnknown *>(pWindow), &swapChainDesc, NULL, &pSwapChain);

    //Get Point to Back Buffer 
    ComPtr<ID3D11Texture2D> pBackbuffer;
    pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &pBackbuffer);

    // create a render target pointing to the back buffer
    pD3DDevice->CreateRenderTargetView(pBackbuffer.Get(), nullptr, &pRenderTargetView);

    //Create a ViewPort
     D3D11_VIEWPORT viewPort;
     ds_memclr(&viewPort, sizeof(viewPort));
     viewPort.TopLeftX = 0;
     viewPort.TopLeftY = 0;
     viewPort.MinDepth = 0.0f;
     viewPort.MaxDepth = 1.0f;
     viewPort.Height = pWindow->Bounds.Height;
     viewPort.Width = pWindow->Bounds.Width;

     pD3DDeviceContext->RSSetViewports(1, &viewPort);

     //Animation
     InitializePipeline();
     InitializeGraphics();
}

/*F********************************************************************************/
/*!
    \Function SampleRenderer::InitializePipeline

    \Description
        Initialize Vertex and Pixel Shaders

    \Version 03/03/2014 (tcho)
*/
/********************************************************************************F*/
void SampleRenderer::InitializePipeline()
{
    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC depthStencilDesc;
    ds_memclr(&depthStencilDesc, sizeof(depthStencilDesc));
    depthStencilDesc.Width = uWidth;
    depthStencilDesc.Height = uHeight;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;
    
    pD3DDevice->CreateTexture2D(&depthStencilDesc, nullptr, &pDepthStencil);

    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ds_memclr(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
    depthStencilViewDesc.Format = depthStencilDesc.Format;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    pD3DDevice->CreateDepthStencilView(pDepthStencil.Get(), &depthStencilViewDesc, &pDepthStencilView);

    //Compile and Create pixel shader
    ComPtr<ID3DBlob> pPixelShaderBlob;
    D3DCompileFromFile(L"Tutorial05_PS.hlsl", NULL, NULL, "PS", "ps_4_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &pPixelShaderBlob, NULL);
    pD3DDevice->CreatePixelShader(pPixelShaderBlob->GetBufferPointer(), pPixelShaderBlob->GetBufferSize(), NULL, &pPixelShader);

    //Compile and Create vertex shader
    ComPtr<ID3DBlob> pVertexShaderBlob;
    D3DCompileFromFile(L"Tutorial05_VS.hlsl", NULL, NULL, "VS", "vs_4_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &pVertexShaderBlob, NULL);
    pD3DDevice->CreateVertexShader(pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), NULL, &pVertexShader);

    //Define and Create Input Layout
    D3D11_INPUT_ELEMENT_DESC aLayout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    UINT uElements = ARRAYSIZE(aLayout);
    pD3DDevice->CreateInputLayout(aLayout, uElements, pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), &pVertexLayout);
}

/*F********************************************************************************/
/*!
    \Function SampleRenderer::InitializeGraphics

    \Description
        Initialize Cube

    \Version 03/03/2014 (tcho)
*/
/********************************************************************************F*/
void SampleRenderer::InitializeGraphics()
{
     // Create vertex buffer
    SimpleVertex vertices[] =
    {
        { XMFLOAT3( -1.0f, 1.0f, -1.0f ), XMFLOAT4( 0.0f, 0.0f, 1.0f, 1.0f ) },
        { XMFLOAT3( 1.0f, 1.0f, -1.0f ), XMFLOAT4( 0.0f, 1.0f, 0.0f, 1.0f ) },
        { XMFLOAT3( 1.0f, 1.0f, 1.0f ), XMFLOAT4( 0.0f, 1.0f, 1.0f, 1.0f ) },
        { XMFLOAT3( -1.0f, 1.0f, 1.0f ), XMFLOAT4( 1.0f, 0.0f, 0.0f, 1.0f ) },
        { XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT4( 1.0f, 0.0f, 1.0f, 1.0f ) },
        { XMFLOAT3( 1.0f, -1.0f, -1.0f ), XMFLOAT4( 1.0f, 1.0f, 0.0f, 1.0f ) },
        { XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f ) },
        { XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f ) },
    };

    D3D11_BUFFER_DESC bufferDesc;
    ds_memclr(&bufferDesc, sizeof(bufferDesc));
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof( SimpleVertex ) * 8;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData;
    ds_memclr(&initData, sizeof(initData));
    initData.pSysMem = vertices;

    pD3DDevice->CreateBuffer(&bufferDesc, &initData, &pVertexBuffer);

    //Create Index Buffer (cube)
    WORD indices[] =
    {
        3,1,0,
        2,1,3,

        0,5,4,
        1,5,0,

        3,4,7,
        0,4,3,

        1,6,5,
        2,6,1,

        2,7,6,
        3,7,2,

        6,4,5,
        7,4,6,
    };

    //Reuse buffer descriptor
    ds_memclr(&bufferDesc, sizeof(bufferDesc));
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof( WORD ) * 36;        // 36 vertices needed for 12 triangles in a triangle list
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    initData.pSysMem = indices;

    pD3DDevice->CreateBuffer(&bufferDesc, &initData, &pIndexBuffer);

    //Set Index Buffer and primitive topology
    pD3DDeviceContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
    pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //Create Constant Buffer
     ds_memclr(&bufferDesc, sizeof(bufferDesc));
     bufferDesc.Usage = D3D11_USAGE_DEFAULT;
     bufferDesc.ByteWidth = sizeof(ConstantBuffer);
     bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
     bufferDesc.CPUAccessFlags = 0;

     pD3DDevice->CreateBuffer(&bufferDesc, NULL, &pConstantBuffer);

     //Initalize world matrix
     mWorld1 = XMMatrixIdentity();
     mWorld2 = XMMatrixIdentity();

     //Initialize View
     XMVECTOR Eye = XMVectorSet( 0.0f, 1.0f, -6.0f, 0.0f );
     XMVECTOR At = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
     XMVECTOR Up = XMVectorSet( 0.0f, -0.2f, 0.0f, 0.0f );
     mView = XMMatrixLookAtLH( Eye, At, Up );

     //Initialize Projection
     mProjection = XMMatrixPerspectiveFovLH(XM_PIDIV2, uWidth/(FLOAT)uHeight, 0.01f, 100.0f);
}

/*F********************************************************************************/
/*!
    \Function SampleRenderer::Render

    \Description
        Renders a single frame

    \Version 03/03/2014 (tcho)
*/
/********************************************************************************F*/
void SampleRenderer::Render()
{
    //Prep Deivce Context for Rendering 
    pD3DDeviceContext->OMSetRenderTargets(1, pRenderTargetView.GetAddressOf(), pDepthStencilView.Get());
    pD3DDeviceContext->IASetInputLayout(pVertexLayout.Get());

    UINT uStride = sizeof(SimpleVertex);
    UINT uOffset = 0;
    pD3DDeviceContext->IASetVertexBuffers(0, 1, pVertexBuffer.GetAddressOf(), &uStride , &uOffset);

    pD3DDeviceContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
    pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); 

    //Update time
    static float t = 0.0f;
    static ULONGLONG uTimeStart = 0;
    ULONGLONG uTimeCurrent = GetTickCount64();

    if ( uTimeStart == 0)
    {
        uTimeStart = uTimeCurrent;
    }

    t = (uTimeCurrent - uTimeStart)/1000.0f;

    //Rotate 1st cube around origin
    mWorld1 = XMMatrixRotationY(t);

    //Rotate 2nd cube around origin and rotate it around the first cube.
    XMMATRIX mSpin = XMMatrixRotationZ(-t);
    XMMATRIX mOrbit = XMMatrixRotationY(-t * 2.0f);
    XMMATRIX mTranslate = XMMatrixTranslation(-4.0f, 0.0f, 0.0f);
    XMMATRIX mScale = XMMatrixScaling(0.3f, 0.3f, 0.3f);

    mWorld2 = mScale * mSpin * mTranslate * mOrbit;

    // Clear Background (Set Background Color)
    float color[4] = {0.0f, 0.8f, 0.4f, 1.0f};
    pD3DDeviceContext->ClearRenderTargetView(pRenderTargetView.Get(), color);

    //Clear depth buffer
    pD3DDeviceContext->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    //Setup Rendering Pipeline
    pD3DDeviceContext->VSSetShader(pVertexShader.Get(), NULL, 0);
    pD3DDeviceContext->VSSetConstantBuffers(0, 1, pConstantBuffer.GetAddressOf());
    pD3DDeviceContext->PSSetShader(pPixelShader.Get(), NULL, 0);

    //Setup Variables for the 1st Cube
    ConstantBuffer constantBuffer1;
    constantBuffer1.mWorld = XMMatrixTranspose(mWorld1);
    constantBuffer1.mView = XMMatrixTranspose(mView);
    constantBuffer1.mProjection = XMMatrixTranspose(mProjection);
    pD3DDeviceContext->UpdateSubresource(pConstantBuffer.Get(), 0, NULL, &constantBuffer1, 0, 0);

    //Render 1st cube
    pD3DDeviceContext->DrawIndexed(36, 0, 0);

    //Setup Variables for the 2nd Cube
    ConstantBuffer constantBuffer2;
    constantBuffer2.mWorld = XMMatrixTranspose(mWorld2);
    constantBuffer2.mView = XMMatrixTranspose(mView);
    constantBuffer2.mProjection = XMMatrixTranspose(mProjection);
    pD3DDeviceContext->UpdateSubresource(pConstantBuffer.Get(), 0, NULL, &constantBuffer2, 0, 0);

    //Render 2nd cube
    pD3DDeviceContext->DrawIndexed(36, 0, 0);

    //Present
    pSwapChain->Present(1, 0);

}

