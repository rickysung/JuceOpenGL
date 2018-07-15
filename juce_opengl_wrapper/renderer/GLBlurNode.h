/*
  ==============================================================================

    GaussianBlurRenderer.h
    Created: 20 Aug 2016 8:43:12pm
    Author:  Ricky

  ==============================================================================
*/

#pragma once
namespace juce{
class GLBlurNode : public GLOffScreenRendererNode
{
public:
    GLBlurNode(OpenGLContext& glContext,
                      int screenWidth,
                      int screenHeight,
                      float amount);
    
    void initializeUniform() override;
    void shutDownContext() override;
    
    void BindTexture(GLuint textureID)
    {
        inputTexture = textureID;
    }
    
private:
    virtual const char* getVertexShader() override;
    virtual const char* getFragmentShader() override;
    void doDraw() override;
    GLuint inputTexture;
    GLuint stencilTexture;
    const float Amount;
    ScopedPointer<OpenGLShaderProgram::Uniform> image;
    ScopedPointer<OpenGLShaderProgram::Uniform> length;
    ScopedPointer<OpenGLShaderProgram::Uniform> blurLength;
    ScopedPointer<OpenGLShaderProgram::Uniform> isVertical;
};

}
