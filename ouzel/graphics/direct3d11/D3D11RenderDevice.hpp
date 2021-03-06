// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_D3D11RENDERDEVICE_HPP
#define OUZEL_GRAPHICS_D3D11RENDERDEVICE_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include <atomic>
#include <map>
#include <memory>
#include <system_error>
#include <thread>
#include <vector>
#include <d3d11.h>
#include "graphics/RenderDevice.hpp"
#include "graphics/direct3d11/D3D11Texture.hpp"

namespace ouzel
{
    namespace graphics
    {
        class Direct3D11ErrorCategory: public std::error_category
        {
        public:
            const char* name() const noexcept override
            {
                return "Direct3D11";
            }

            std::string message(int condition) const override
            {
                switch (condition)
                {
                    case D3D11_ERROR_FILE_NOT_FOUND: return "D3D11_ERROR_FILE_NOT_FOUND";
                    case D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS: return "D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS";
                    case D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS: return "D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS";
                    case D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD: return "D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD";
                    case DXGI_ERROR_INVALID_CALL: return "DXGI_ERROR_INVALID_CALL";
                    case DXGI_ERROR_WAS_STILL_DRAWING: return "DXGI_ERROR_WAS_STILL_DRAWING";
                    case DXGI_ERROR_NOT_CURRENTLY_AVAILABLE: return "DXGI_ERROR_NOT_CURRENTLY_AVAILABLE";
                    case E_FAIL: return "E_FAIL";
                    case E_INVALIDARG: return "E_INVALIDARG";
                    case E_OUTOFMEMORY: return "E_OUTOFMEMORY";
                    case E_NOTIMPL: return "E_NOTIMPL";
                    default: return "Unknown error (" + std::to_string(condition) + ")";
                }
            }
        };

        extern const Direct3D11ErrorCategory direct3D11ErrorCategory;

        class D3D11RenderDevice final: public RenderDevice
        {
            friend Renderer;
        public:
            ~D3D11RenderDevice();

            std::vector<Size2<uint32_t>> getSupportedResolutions() const override;

            ID3D11Device* getDevice() const { return device; }
            ID3D11DeviceContext* getContext() const { return context; }

            ID3D11SamplerState* getSamplerState(const SamplerStateDesc& desc);
            void setFullscreen(bool newFullscreen);

        protected:
            D3D11RenderDevice(const std::function<void(const Event&)>& initCallback);

            void init(Window* newWindow,
                      const Size2<uint32_t>& newSize,
                      uint32_t newSampleCount,
                      Texture::Filter newTextureFilter,
                      uint32_t newMaxAnisotropy,
                      bool newVerticalSync,
                      bool newDepth,
                      bool newStencil,
                      bool newDebugRenderer) override;

            void process() override;
            void resizeBackBuffer(UINT newWidth, UINT newHeight);
            void uploadBuffer(ID3D11Buffer* buffer, const void* data, uint32_t dataSize);
            void generateScreenshot(const std::string& filename) override;
            void main();

            IDXGIOutput* getOutput() const;

            ID3D11Device* device = nullptr;
            ID3D11DeviceContext* context = nullptr;
            IDXGISwapChain* swapChain = nullptr;
            IDXGIAdapter* adapter = nullptr;
            ID3D11Texture2D* backBuffer = nullptr;
            ID3D11RenderTargetView* renderTargetView = nullptr;
            std::map<SamplerStateDesc, ID3D11SamplerState*> samplerStates;
            ID3D11RasterizerState* rasterizerStates[12]{nullptr};
            ID3D11Texture2D* depthStencilTexture = nullptr;
            ID3D11DepthStencilView* depthStencilView = nullptr;
            ID3D11DepthStencilState* defaultDepthStencilState = nullptr;

            UINT frameBufferWidth = 0;
            UINT frameBufferHeight = 0;

            UINT swapInterval = 0;

            std::atomic_bool running{false};
            std::thread renderThread;

            std::vector<std::unique_ptr<D3D11RenderResource>> resources;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_D3D11RENDERDEVICE_HPP
