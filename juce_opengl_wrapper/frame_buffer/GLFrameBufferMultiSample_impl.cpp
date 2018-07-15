namespace juce{
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
