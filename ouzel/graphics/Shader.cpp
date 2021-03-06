// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Shader.hpp"
#include "Renderer.hpp"
#include "RenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        Shader::Shader(Renderer& initRenderer):
            resource(initRenderer)
        {
        }

        Shader::Shader(Renderer& initRenderer,
                       const std::vector<uint8_t>& initFragmentShader,
                       const std::vector<uint8_t>& initVertexShader,
                       const std::set<Vertex::Attribute::Usage>& initVertexAttributes,
                       const std::vector<ConstantInfo>& initFragmentShaderConstantInfo,
                       const std::vector<ConstantInfo>& initVertexShaderConstantInfo,
                       uint32_t initFragmentShaderDataAlignment,
                       uint32_t initVertexShaderDataAlignment,
                       const std::string& fragmentShaderFunction,
                       const std::string& vertexShaderFunction):
            resource(initRenderer),
            vertexAttributes(initVertexAttributes)
        {
            initRenderer.addCommand(std::unique_ptr<Command>(new InitShaderCommand(resource.getId(),
                                                                                   initFragmentShader,
                                                                                   initVertexShader,
                                                                                   initVertexAttributes,
                                                                                   initFragmentShaderConstantInfo,
                                                                                   initVertexShaderConstantInfo,
                                                                                   initFragmentShaderDataAlignment,
                                                                                   initVertexShaderDataAlignment,
                                                                                   fragmentShaderFunction,
                                                                                   vertexShaderFunction)));
        }

        const std::set<Vertex::Attribute::Usage>& Shader::getVertexAttributes() const
        {
            return vertexAttributes;
        }
    } // namespace graphics
} // namespace ouzel
