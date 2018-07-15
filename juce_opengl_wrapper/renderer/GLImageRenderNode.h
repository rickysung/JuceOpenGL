#pragma once
namespace juce{
class GLImageRenderNode : public GLOffScreenRendererNode
{
public:
    GLImageRenderNode(OpenGLContext& glContext, int width, int height);
    void initializeUniform() override;
private:
    virtual const char* getVertexShader() override;
    virtual const char* getFragmentShader() override;
    void doDraw() override;
    ScopedPointer<OpenGLShaderProgram::Uniform> scene;
};
}
