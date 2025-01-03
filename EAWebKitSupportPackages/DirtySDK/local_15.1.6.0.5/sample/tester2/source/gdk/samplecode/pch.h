//--------------------------------------------------------------------------------------
// pch.h
//
// Header for standard system include files.
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#pragma once

#include <winsdkver.h>
#define _WIN32_WINNT 0x0A00
#include <sdkddkver.h>

#include <EABase/eabase.h>

// Use the C++ standard templated min/max
#define NOMINMAX

// DirectX apps don't need GDI
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP

// Include <mcx.h> if you need this
#define NOMCX

// Include <winsvc.h> if you need this
#define NOSERVICE

// WinHelp is deprecated
#define NOHELP

#include <Windows.h>

#include <wrl/client.h>
#include <wrl/event.h>

#if defined(EA_PLATFORM_XBOXONE)
#include <d3d12_x.h>
#else // _GAMING_XBOX_SCARLETT
#include <d3d12_xs.h>
#endif

#define _XM_NO_XMVECTOR_OVERLOADS_

#include <DirectXMath.h>
#include <DirectXColors.h>

#if defined(EA_PLATFORM_XBOXONE)
#include "d3dx12_x.h"
#else // _GAMING_XBOX_SCARLETT
#include "d3dx12_xs.h"
#endif

#include <algorithm>
#include <exception>
#include <memory>
#include <stdexcept>

#include <assert.h>
#include <stdio.h>
#include <pix3.h>

#include <XGame.h>
#include <XSystem.h>
#include <XTaskQueue.h>

#include "GraphicsMemory.h"

namespace DX
{
    // Helper class for COM exceptions
    class com_exception : public std::exception
    {
    public:
        com_exception(HRESULT hr) : result(hr) {}

        virtual const char* what() const override
        {
            static char s_str[64] = {};
            sprintf_s(s_str, "Failure with HRESULT of %08X", static_cast<unsigned int>(result));
            return s_str;
        }

    private:
        HRESULT result;
    };

    // Helper utility converts D3D API failures into exceptions.
    inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr))
        {
            throw com_exception(hr);
        }
    }
}
