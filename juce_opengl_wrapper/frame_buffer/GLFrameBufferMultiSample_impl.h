namespace juce{
    
class MultiSampleFrameBufferImpl : public FrameBufferImpl
{
private:
    void initFrameBuffer() override;
    void resizeColorAttachment(int w, int h) override;
    void blit();
    GLuint msFrameBufferID, msRenderBufferID;
    std::vector<GLuint> msOutputTexture;
    const int multisampleNum = 4;
public:
    MultiSampleFrameBufferImpl(OpenGLContext& c, const int w, const int h, const int mrt, const bool wantsDepthBuffer, const bool wantsStencilBuffer) : FrameBufferImpl(c, w, h, mrt, wantsDepthBuffer, wantsStencilBuffer){}
    void bind() override;
    void unbind() override;
    
};
}
