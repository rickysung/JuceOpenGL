
//
//  OpenGLDrawer.h
//  GaudiReaper
//
//  Created by Ricky on 2016. 8. 13..
//
//

#pragma once
namespace juce{
class GLOffScreenRendererNode : public GLRendererNode
{
public:
    GLOffScreenRendererNode(OpenGLContext& openGLContext,
                   int screenWidth,
                   int screenHeight,
                   int mrtNum,
                   bool isAntialiasing) :
    GLRendererNode(openGLContext, screenWidth, screenHeight),
    framebuffer(mrtNum, isAntialiasing)
    {}
    virtual ~GLOffScreenRendererNode(){}
    void initializeContext() override
    {
        initializeShader();
        initializeShape();
        initializeUniform();
        framebuffer.initialise(context, width, height);
    }
    virtual GLuint getTexture(int idx)
    {
        return framebuffer.getTextureID(idx);
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
private:
    void preDraw() override
    {
        shaderProgram->use();
        framebuffer.makeCurrentRenderingTarget();
        OpenGLHelpers::clear (Colour(BACKGROUND_COMP));
        glViewport(0, 0, width, height);
    }
    void postDraw() override
    {
        framebuffer.releaseAsRenderingTarget();
    }
    GLFrameBuffer framebuffer;
};
}
