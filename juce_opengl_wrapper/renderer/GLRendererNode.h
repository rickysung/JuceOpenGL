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
        initializeBuffer();
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
    void genMultisampleFrameBuffer()
    {
        glGenFramebuffers(1, &msframeBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, msframeBuffer);
        msOutputTexture.resize(outputTextureNum);
        glGenTextures(outputTextureNum, msOutputTexture.getRawDataPointer());
        for (GLuint i = 0; i < outputTextureNum ; i++)
        {
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, msOutputTexture[i]);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA16F, width, height, GL_TRUE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, msOutputTexture[i], 0);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
        }
        glGenRenderbuffers (1, &msrenderBuffer);
        glBindRenderbuffer (GL_RENDERBUFFER, msrenderBuffer);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, msrenderBuffer);
        GLuint* attachments = new GLuint[outputTextureNum];
        for(GLuint i=0 ; i<outputTextureNum ; i++)
        {
            attachments[i] = GL_COLOR_ATTACHMENT0 + i;
        }
        glDrawBuffers(outputTextureNum, attachments);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        delete[] attachments;
        
        genFrameBuffer();
    }
    void genFrameBuffer()
    {
        glGenFramebuffers(1, &frameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        outputTexture.resize(outputTextureNum);
        glGenTextures(outputTextureNum, outputTexture.getRawDataPointer());
        for (GLuint i = 0; i < outputTextureNum ; i++)
        {
            glBindTexture(GL_TEXTURE_2D, outputTexture[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glBindTexture(GL_TEXTURE_2D, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, outputTexture[i], 0);
        }
        glGenRenderbuffers (1, &renderBuffer);
        glBindRenderbuffer (GL_RENDERBUFFER, renderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);
        GLuint* attachments = new GLuint[outputTextureNum];
        for(GLuint i=0 ; i<outputTextureNum ; i++)
        {
            attachments[i] = GL_COLOR_ATTACHMENT0 + i;
        }
        glDrawBuffers(outputTextureNum, attachments);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        delete[] attachments;
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
    void blit()
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, msframeBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);
        
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glReadBuffer(GL_COLOR_ATTACHMENT1);
        glDrawBuffer(GL_COLOR_ATTACHMENT1);
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glReadBuffer(GL_COLOR_ATTACHMENT2);
        glDrawBuffer(GL_COLOR_ATTACHMENT2);
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }
    virtual void resizeColorAttachment(int w, int h)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        for (GLuint i = 0; i < outputTextureNum ; i++)
        {
            glBindTexture(GL_TEXTURE_2D, outputTexture[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        }
        glDeleteRenderbuffers(1, &renderBuffer);
        glGenRenderbuffers(1, &renderBuffer);
        glBindRenderbuffer (GL_RENDERBUFFER, renderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);
    }
    virtual GLuint getTexture(int idx)
    {
        return outputTexture[idx];
    }
    void draw()
    {
        preDraw();
        doDraw();
        postDraw();
    }
    virtual void shutDownContext()
    {
        glDeleteTextures(outputTextureNum, outputTexture.getRawDataPointer());
        context.extensions.glDeleteRenderbuffers(1, &renderBuffer);
        context.extensions.glDeleteFramebuffers(1, &frameBuffer);
    }
    virtual void setScreenWidthHeight(int width, int height)
    {
        this->width = width;
        this->height = height;
        viewportX = viewportY = 0;
        viewportWidth = width;
        viewportHeight = height;
        resizeColorAttachment(width, height);
    }
protected:
    OpenGLContext& context;
    ScopedPointer<OpenGLShaderProgram> shaderProgram;
    GLuint msframeBuffer, frameBuffer, msrenderBuffer, renderBuffer;
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
    
    Array<GLuint> msOutputTexture;
    Array<GLuint> outputTexture;
    const int outputTextureNum;
};
}
