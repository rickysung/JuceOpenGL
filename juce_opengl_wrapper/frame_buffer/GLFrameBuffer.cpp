namespace juce {

bool GLFrameBuffer::initialise(OpenGLContext &context, int width, int height)
{
    jassert (context.isActive()); // The context must be active when creating a framebuffer!
    
    pimpl.release();
    if(needMultisampling)
        pimpl.reset(new MultiSampleFrameBufferImpl(context, width, height, renderTargetNum, false, false));
    else
        pimpl.reset(new FrameBufferImpl (context, width, height, renderTargetNum, false, false));
    if (! pimpl->createdOk())
        pimpl = nullptr;
    
    return pimpl != nullptr;
}

bool GLFrameBuffer::makeCurrentRenderingTarget()
{
    if (pimpl == nullptr)
        return false;
    pimpl->bind();
    return true;
}

void GLFrameBuffer::releaseAsRenderingTarget()
{
    if (pimpl != nullptr)
        pimpl->unbind();
}

}
