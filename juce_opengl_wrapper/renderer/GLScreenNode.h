/*
  ==============================================================================

    ScreenRenderer.h
    Created: 20 Aug 2016 8:19:03pm
    Author:  Ricky

  ==============================================================================
*/

#pragma once
namespace juce{

class GLScreenNode : public GLRendererNode
{
public:
    GLScreenNode(OpenGLContext& glContext,
                      int screenWidth,
                      int screenHeight);
    
    void initializeUniform() override;
    void BindTexture(GLuint textureID)
    {
        inputTexture = textureID;
    }
private:
    GLuint inputTexture;
    virtual const char* getVertexShader() override;
    virtual const char* getFragmentShader() override;
    void doDraw() override;
    ScopedPointer<OpenGLShaderProgram::Uniform> scene;
};

}
