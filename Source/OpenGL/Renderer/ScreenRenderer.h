/*
  ==============================================================================

    ScreenRenderer.h
    Created: 20 Aug 2016 8:19:03pm
    Author:  Ricky

  ==============================================================================
*/

#ifndef SCREENRENDERER_H_INCLUDED
#define SCREENRENDERER_H_INCLUDED
#include "../OpenGLDrawer.h"


class ScreenRenderer : public OpenGLDrawer
{
public:
    ScreenRenderer(OpenGLContext& glContext,
                      int screenWidth,
                      int screenHeight);
    
    void initializeUniform() override;
    void initializeBuffer() override;
    
    void BindTexture(GLuint textureID)
    {
        inputTexture = textureID;
    }
private:
    GLuint inputTexture, inputTexture2;
    virtual const char* getVertexShader() override;
    virtual const char* getFragmentShader() override;
    void preDraw() override;
    void doDraw() override;
    void postDraw() override;
    ScopedPointer<OpenGLShaderProgram::Uniform> scene;
};




#endif  // SCREENRENDERER_H_INCLUDED
