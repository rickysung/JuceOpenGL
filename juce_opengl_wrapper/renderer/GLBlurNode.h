/*
  ==============================================================================

    GaussianBlurRenderer.h
    Created: 20 Aug 2016 8:43:12pm
    Author:  Ricky

  ==============================================================================
*/

#pragma once
namespace juce{
class GLBlurNode : public GLRendererNode
{
public:
    GLBlurNode(OpenGLContext& glContext,
                      int screenWidth,
                      int screenHeight,
                      float amount);
    
    void initializeUniform() override;
    void initializeBuffer() override;
    void shutDownContext() override;
    
    void BindTexture(GLuint textureID)
    {
        inputTexture = textureID;
    }
//    void resizeColorAttachment(int w, int h) override
//    {
//        for (unsigned int i = 0; i < 2; i++)
//        {
//            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
//            glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
//            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
//        }
//        GLRendererNode::resizeColorAttachment(w, h);
//    }
    
private:
    virtual const char* getVertexShader() override;
    virtual const char* getFragmentShader() override;
    void preDraw() override;
    void doDraw() override;
    void postDraw() override;
    GLuint inputTexture;
    GLuint stencilTexture;
    const float Amount;
    ScopedPointer<OpenGLShaderProgram::Uniform> image;
    ScopedPointer<OpenGLShaderProgram::Uniform> length;
    ScopedPointer<OpenGLShaderProgram::Uniform> blurLength;
    ScopedPointer<OpenGLShaderProgram::Uniform> isVertical;
};

}
