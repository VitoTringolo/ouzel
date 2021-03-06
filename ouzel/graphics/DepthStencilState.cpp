// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "DepthStencilState.hpp"
#include "Renderer.hpp"
#include "RenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        DepthStencilState::DepthStencilState(Renderer& initRenderer):
            resource(initRenderer)
        {
        }

        DepthStencilState::DepthStencilState(Renderer& initRenderer,
                                             bool initDepthTest,
                                             bool initDepthWrite,
                                             CompareFunction initCompareFunction,
                                             bool initStencilEnabled,
                                             uint32_t initStencilReadMask,
                                             uint32_t initStencilWriteMask,
                                             const StencilDescriptor& initFrontFaceStencil,
                                             const StencilDescriptor& initBackFaceStencil):
            resource(initRenderer),
            depthTest(initDepthTest),
            depthWrite(initDepthWrite),
            compareFunction(initCompareFunction),
            stencilEnabled(initStencilEnabled),
            stencilReadMask(initStencilReadMask),
            stencilWriteMask(initStencilWriteMask),
            frontFaceStencil(initFrontFaceStencil),
            backFaceStencil(initBackFaceStencil)
        {
            initRenderer.addCommand(std::unique_ptr<Command>(new InitDepthStencilStateCommand(resource.getId(),
                                                                                              initDepthTest,
                                                                                              initDepthWrite,
                                                                                              initCompareFunction,
                                                                                              initStencilEnabled,
                                                                                              initStencilReadMask,
                                                                                              initStencilWriteMask,
                                                                                              initFrontFaceStencil,
                                                                                              initBackFaceStencil)));
        }
    } // namespace graphics
} // namespace ouzel
