namespace juce{
void FrameBufferImpl::resizeColorAttachment(int w, int h)
{
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
    for (GLuint i = 0; i < renderTargetNum ; i++)
    {
        glBindTexture(GL_TEXTURE_2D, outputTexture[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
    }
    glDeleteRenderbuffers(1, &renderBufferID);
    glGenRenderbuffers(1, &renderBufferID);
    glBindRenderbuffer (GL_RENDERBUFFER, renderBufferID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBufferID);
}
}
