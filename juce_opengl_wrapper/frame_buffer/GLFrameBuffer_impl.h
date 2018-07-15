namespace juce{
class FrameBufferImpl
{
protected:
    virtual void initFrameBuffer();
    virtual void resizeColorAttachment(int w, int h);
public:
    FrameBufferImpl(OpenGLContext& c, const int w, const int h, const int mrt, const bool wantsDepthBuffer, const bool wantsStencilBuffer)
    : context(c), width(w), height(h), renderTargetNum(mrt), frameBufferID(0), renderBufferID(0), hasDepthBuffer(false), hasStencilBuffer(false)
    {
        jassert(OpenGLHelpers::isContextActive());
        initFrameBuffer();
    }
    virtual ~FrameBufferImpl()
    {
        if (OpenGLHelpers::isContextActive())
        {
            glDeleteTextures (renderTargetNum, outputTexture.data());
            if (renderBufferID != 0)
                context.extensions.glDeleteRenderbuffers (1, &renderBufferID);
            if (frameBufferID != 0)
                context.extensions.glDeleteFramebuffers (1, &frameBufferID);
        }
    }
    virtual void bind()
    {
        context.extensions.glBindFramebuffer (GL_FRAMEBUFFER, frameBufferID);
    }
    virtual void unbind()
    {
        context.extensions.glBindFramebuffer (GL_FRAMEBUFFER, context.getFrameBufferID());
    }
    void resize(int w, int h)
    {
        width = w;
        height = h;
        resizeColorAttachment(w, h);
    }
    bool createdOk() const
    {
        return frameBufferID != 0;
    }
    OpenGLContext& context;
    int width, height;
    const int renderTargetNum;
    GLuint frameBufferID, renderBufferID;
    std::vector<GLuint> outputTexture;
    const bool hasDepthBuffer, hasStencilBuffer;
};
}
