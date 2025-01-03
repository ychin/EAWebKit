/*H*************************************************************************************************/
/*!

    \File sample.h

    \Description
        This implements a basic application framework for the sample

    \Notes
        None.

    \Copyright
        Copyright (c)  Electronic Arts 2013.  ALL RIGHTS RESERVED.

    \Version 1.0 

*/
/*************************************************************************************************H*/

#ifndef _sample_core_h
#define _sample_core_h

/*** Include files ****************************************************************/
#include <wrl\client.h>
#include <d3d11_x.h>
#include <DirectXColors.h>

#include "IEAController/IEAController.h"
#include "EAControllerUserPairing/EAControllerUserPairingServer.h"
#include "EAControllerUserPairing/Messages.h"

using namespace Microsoft::WRL;
using namespace DirectX;


/*** Function Prototype ***********************************************************/

//These are implemented in the T2Host.cpp
void InitializeT2();
bool UpdateT2(void);
void UninitializeT2();
void ConnectT2();


//--------------------------------------------------------------------------------------
// Name: SampleRenderer
// Desc: A class that holds the Rendering Functionality
// Note: Using DX11.1
//--------------------------------------------------------------------------------------
class SampleRenderer
{
    public:
        struct SimpleVertex
        {
            XMFLOAT3 Pos;
            XMFLOAT4 Color;
        };

        struct ConstantBuffer
        {
            XMMATRIX mWorld;
            XMMATRIX mView;
            XMMATRIX mProjection;
        };

    //Public Functions
    public:        
        void Initialize();
        void Render();
        void InitializePipeline();
        void InitializeGraphics();
    //Public Members
    public:
        ComPtr<ID3D11DeviceX> pD3DDevice;
        ComPtr<ID3D11DeviceContextX> pD3DDeviceContext;
        ComPtr<ID3DXboxPerformanceContext> pD3DXboxPerfContext;
        ComPtr<IDXGISwapChain1> pSwapChain;
        ComPtr<ID3D11RenderTargetView> pRenderTargetView;

        ComPtr<ID3D11Texture2D> pDepthStencil;
        ComPtr<ID3D11DepthStencilView> pDepthStencilView;
        ComPtr<ID3D11PixelShader> pPixelShader;
        ComPtr<ID3D11VertexShader> pVertexShader;
        ComPtr<ID3D11InputLayout> pVertexLayout;

        ComPtr<ID3D11Buffer> pVertexBuffer;
        ComPtr<ID3D11Buffer> pIndexBuffer;
        ComPtr<ID3D11Buffer> pConstantBuffer;

        XMMATRIX mWorld1;
        XMMATRIX mWorld2;
        XMMATRIX mView;
        XMMATRIX mProjection;

        UINT uHeight;
        UINT uWidth;
};

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
// Desc: Global class for this sample, derived from IFrameworkView and
//       implementing Initialize(), SetWindow, Load, Run , Uninitialize.
//       This reemoves the depenedency from the MS Sample Framework
//--------------------------------------------------------------------------------------

ref class Sample sealed : public Windows::ApplicationModel::Core::IFrameworkView
{
    internal:
        Sample() {};

    public:
    virtual ~Sample() {};

    //IFrameworkView
    public:
        virtual void Initialize(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView);
        virtual void SetWindow(Windows::UI::Core::CoreWindow^ window);
        virtual void Load(Platform::String^ entryPoint);
        virtual void Run();
        virtual void Uninitialize();

protected:

        // PLM Event landlers
        void OnActivated( Windows::ApplicationModel::Core::CoreApplicationView^ applicationViewProvider, Windows::ApplicationModel::Activation::IActivatedEventArgs^ args );
        void OnResuming( Platform::Object^ sender, Platform::Object^ args );
        void OnSuspending( Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ args );
        void OnResourceAvailabilityChanged( Platform::Object^ sender, Platform::Object^ args );
        void OnVisibilityChanged( Windows::UI::Core::CoreWindow^ window, Windows::UI::Core::VisibilityChangedEventArgs^ args );
        void OnWindowActivatedChanged( Windows::UI::Core::CoreWindow^ window, Windows::UI::Core::WindowActivatedEventArgs^ args );


    private:
        SampleRenderer mRenderer;
        EA::Pairing::EAControllerUserPairingServer *mpEacup;
        EA::SEMD::SystemEventMessageDispatcher *mpEasmd;
        T2IEAMessageHandler *mpEAmsghdlr;
};


//--------------------------------------------------------------------------------------
// Name: SampleFactory
// Desc: A factory for creating a sample. Derived from IFrameworkViewSource.
//       Implements CreateView();
//--------------------------------------------------------------------------------------

ref class SampleFactory sealed : Windows::ApplicationModel::Core::IFrameworkViewSource 
{
    internal:
        SampleFactory() {};

    public:
        virtual ~SampleFactory() {};
        virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView();
};

#endif