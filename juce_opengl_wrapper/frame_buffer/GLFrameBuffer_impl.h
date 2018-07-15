namespace juce{
class FrameBufferImpl
{
protected:
    virtual void resizeColorAttachment(int w, int h);
public:
    FrameBufferImpl(OpenGLContext& c, const int w, const int h, const int mrt, const bool wantsDepthBuffer, const bool wantsStencilBuffer)
    : context(c), width(w), height(h), renderTargetNum(mrt), frameBufferID(0), renderBufferID(0), hasDepthBuffer(false), hasStencilBuffer(false)
    {
        jassert(OpenGLHelpers::isContextActive());
        glGenFramebuffers(1, &frameBufferID);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
        outputTexture.resize(renderTargetNum);
        glGenTextures(renderTargetNum, outputTexture.data());
        for (GLuint i = 0; i < renderTargetNum ; i++)
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
        glGenRenderbuffers (1, &renderBufferID);
        glBindRenderbuffer (GL_RENDERBUFFER, renderBufferID);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBufferID);
        GLuint* attachments = new GLuint[renderTargetNum];
        for(GLuint i=0 ; i<renderTargetNum ; i++)
        {
            attachments[i] = GL_COLOR_ATTACHMENT0 + i;
        }
        glDrawBuffers(renderTargetNum, attachments);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        delete[] attachments;
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
