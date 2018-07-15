#pragma once
namespace juce{
class GLImageRenderNode : public GLRendererNode
{
public:
    GLImageRenderNode(OpenGLContext& glContext, int width, int height);
    void initializeUniform() override;
    void initializeBuffer() override{}
    
    void BindTexture(GLuint textureID)
    {
        inputTexture = textureID;
    }
private:
    GLuint inputTexture;
    virtual const char* getVertexShader() override;
    virtual const char* getFragmentShader() override;
    void preDraw() override;
    void doDraw() override;
    void postDraw() override;
    ScopedPointer<OpenGLShaderProgram::Uniform> scene;
};
}
