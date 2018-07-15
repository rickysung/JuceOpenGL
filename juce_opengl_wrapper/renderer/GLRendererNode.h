//
//  OpenGLDrawer.h
//  GaudiReaper
//
//  Created by Ricky on 2016. 8. 13..
//
//

#pragma once
namespace juce{
    
#define CREATE_UNIFORM(var) var = createUniform(#var)

class GLRendererNode
{
public:
    GLRendererNode(OpenGLContext& openGLContext,
                    int screenWidth,
                    int screenHeight,
                    int renderTargetNum) :
    context(openGLContext),
    framebuffer(renderTargetNum, false),
    width(screenWidth),
    height(screenHeight),
    viewportX(0), viewportY(0), viewportWidth(screenWidth), viewportHeight(screenHeight),
    outputTextureNum(renderTargetNum)
    {}
    virtual ~GLRendererNode(){}
    OpenGLShaderProgram::Uniform* createUniform (const char* uniformName)
    {
        if (context.extensions.glGetUniformLocation (shaderProgram->getProgramID(), uniformName) < 0)
            return nullptr;
        return new OpenGLShaderProgram::Uniform (*shaderProgram, uniformName);
    }
    void initializeContext()
    {
        initializeShader();
        initializeShape();
        initializeUniform();
        framebuffer.initialise(context, width, height);
//        initializeBuffer();
    }
    virtual void initializeShape()
    {
        screenShape = new RectangleShape(context);
    }
    virtual void initializeUniform() = 0;
    virtual void initializeBuffer() = 0;
    void initializeShader()
    {
        const char* vs = getVertexShader();
        const char* fs = getFragmentShader();
        String statusText;
        shaderProgram = new OpenGLShaderProgram(context);
        if (shaderProgram->addVertexShader (vs)
            && shaderProgram->addFragmentShader (fs)
            && shaderProgram->link())
        {
            statusText = "GLSL: v" + String (OpenGLShaderProgram::getLanguageVersion(), 2);
        }
        else
        {
            statusText = shaderProgram->getLastError();
        }
        shaderProgram->use();
    }
    void genDoubleFrameBuffer()
    {
        glGenFramebuffers(2, pingpongFBO);
        glGenTextures(2, pingpongBuffer);
        for (unsigned int i = 0; i < 2; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
            glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0);
        }
    }
    virtual GLuint getTexture(int idx)
    {
        return framebuffer.getTextureID(idx);
    }
    void draw()
    {
        preDraw();
        doDraw();
        postDraw();
    }
    virtual void shutDownContext()
    {
        framebuffer.release();
    }
    virtual void setScreenWidthHeight(int width, int height)
    {
        viewportX = viewportY = 0;
        viewportWidth = width;
        viewportHeight = height;
        framebuffer.resize(width, height);
    }
protected:
    OpenGLContext& context;
    GLFrameBuffer framebuffer;
    ScopedPointer<OpenGLShaderProgram> shaderProgram;
    GLuint msframeBuffer, msrenderBuffer, renderBuffer;
    GLuint pingpongFBO[2];
    GLuint pingpongBuffer[2];
    virtual const char* getVertexShader() = 0;
    virtual const char* getFragmentShader() = 0;
    virtual void preDraw() = 0;
    virtual void doDraw() = 0;
    virtual void postDraw() = 0;
    ScopedPointer<RectangleShape> screenShape;
    int width;
    int height;
    int viewportX, viewportY;
    int viewportWidth, viewportHeight;
    const int outputTextureNum;
};
}
