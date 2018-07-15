namespace juce{
void FrameBufferImpl::initFrameBuffer()
{
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
