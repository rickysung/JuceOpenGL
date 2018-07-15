namespace juce{
void MultiSampleFrameBufferImpl::initFrameBuffer()
{
    glGenFramebuffers(1, &msFrameBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, msFrameBufferID);
    outputTexture.resize(renderTargetNum);
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
    
    FrameBufferImpl::initFrameBuffer();
}
void MultiSampleFrameBufferImpl::resizeColorAttachment(int w, int h)
{
    glBindFramebuffer(GL_FRAMEBUFFER, msFrameBufferID);
    for (GLuint i = 0; i < renderTargetNum ; i++)
    {
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, msOutputTexture[i]);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, multisampleNum, GL_RGBA16F, width, height, GL_TRUE);
    }
    glDeleteRenderbuffers(1, &msRenderBufferID);
    glGenRenderbuffers (1, &msRenderBufferID);
    glBindRenderbuffer (GL_RENDERBUFFER, msRenderBufferID);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, multisampleNum, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, msRenderBufferID);
    
    FrameBufferImpl::resizeColorAttachment(w, h);
    
}
void MultiSampleFrameBufferImpl::bind()
{
    context.extensions.glBindFramebuffer(GL_FRAMEBUFFER, msFrameBufferID);
}
void MultiSampleFrameBufferImpl::unbind()
{
    blit();
    context.extensions.glBindFramebuffer (GL_FRAMEBUFFER, context.getFrameBufferID());
}
void MultiSampleFrameBufferImpl::blit()
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, msFrameBufferID);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferID);
    for(GLuint i = 0 ; i<renderTargetNum ; i++)
    {
        glReadBuffer(GL_COLOR_ATTACHMENT0 + i);
        glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }
}
}
