/*
  ==============================================================================

    GlowRenderer.h
    Created: 12 Oct 2017 2:34:59pm
    Author:  Ricky

  ==============================================================================
*/

#ifndef GLOWRENDERER_H_INCLUDED
#define GLOWRENDERER_H_INCLUDED
#include "../OpenGLDrawer.h"

class GlowRenderer : public OpenGLDrawer
{
public:
    GlowRenderer(OpenGLContext& glContext,
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
    void resizeColorAttachment(int w, int h) override
    {
        for (unsigned int i = 0; i < 2; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
            glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        }
        OpenGLDrawer::resizeColorAttachment(w, h);
    }
private:
    virtual const char* getVertexShader() override;
    virtual const char* getFragmentShader() override;
    void preDraw() override;
    void doDraw() override;
    void postDraw() override;
    GLuint inputTexture;
    float glowArea = 0;
    float dir = 0.98f;
    GLuint stencilTexture;
    const float Amount;
    ScopedPointer<OpenGLShaderProgram::Uniform> image;
    ScopedPointer<OpenGLShaderProgram::Uniform> length;
    ScopedPointer<OpenGLShaderProgram::Uniform> blurLength;
    ScopedPointer<OpenGLShaderProgram::Uniform> isVertical;
};




#endif  // GLOWRENDERER_H_INCLUDED
