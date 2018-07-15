namespace juce{
    
class MultiSampleFrameBufferImpl : public FrameBufferImpl
{
private:
    void resizeColorAttachment(int w, int h) override;
    void blit();
    GLuint msFrameBufferID, msRenderBufferID;
    std::vector<GLuint> msOutputTexture;
    const int multisampleNum = 4;
public:
    MultiSampleFrameBufferImpl(OpenGLContext& c, const int w, const int h, const int mrt, const bool wantsDepthBuffer, const bool wantsStencilBuffer) : FrameBufferImpl(c, w, h, mrt, wantsDepthBuffer, wantsStencilBuffer)
    {
        glGenFramebuffers(1, &msFrameBufferID);
        glBindFramebuffer(GL_FRAMEBUFFER, msFrameBufferID);
        msOutputTexture.resize(renderTargetNum);
        glGenTextures(renderTargetNum, msOutputTexture.data());
        for (GLuint i = 0; i < renderTargetNum ; i++)
        {
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, msOutputTexture[i]);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, multisampleNum, GL_RGBA16F, width, height, GL_TRUE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, msOutputTexture[i], 0);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
        }
        glGenRenderbuffers (1, &msRenderBufferID);
        glBindRenderbuffer (GL_RENDERBUFFER, msRenderBufferID);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, multisampleNum, GL_DEPTH24_STENCIL8, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, msRenderBufferID);
        GLuint* attachments = new GLuint[renderTargetNum];
        for(GLuint i=0 ; i<renderTargetNum ; i++)
        {
            attachments[i] = GL_COLOR_ATTACHMENT0 + i;
        }
        glDrawBuffers(renderTargetNum, attachments);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        delete[] attachments;
    }
    ~MultiSampleFrameBufferImpl()
    {
        glDeleteTextures(renderTargetNum, msOutputTexture.data());
        if (msRenderBufferID != 0)
            context.extensions.glDeleteRenderbuffers(1, &msRenderBufferID);
        if(msFrameBufferID != 0)
            context.extensions.glDeleteFramebuffers(1, &msFrameBufferID);
    }
    void bind() override;
    void unbind() override;
};
}
