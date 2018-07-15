namespace juce{
class GLFrameBuffer
{
public:
    GLFrameBuffer(int mrtNum, bool isAntialiasing) : renderTargetNum(mrtNum), needMultisampling(isAntialiasing){};
    virtual ~GLFrameBuffer(){};
    virtual bool initialise(OpenGLContext& context, int width, int height);
    void release() { pimpl = nullptr; }
    void resize(int w, int h) { pimpl->resize(w, h); }
    bool isValid() const noexcept { return pimpl.get() != nullptr; }
    
    int getWidth() const noexcept { return pimpl.get()->width; }
    int getHeight() const noexcept{ return pimpl.get()->height; }
    
    GLuint getTextureID(GLuint outputIndex) const noexcept { return pimpl->outputTexture[outputIndex]; }
    GLuint getFrameBufferID() const noexcept { return pimpl.get()->frameBufferID; }
    int getOutputTargetNum() const noexcept { return pimpl.get()->renderTargetNum; }
    bool makeCurrentRenderingTarget();
    void releaseAsRenderingTarget();
private:
    const int renderTargetNum;
    const bool needMultisampling;
    std::unique_ptr<FrameBufferImpl> pimpl;
};
}
