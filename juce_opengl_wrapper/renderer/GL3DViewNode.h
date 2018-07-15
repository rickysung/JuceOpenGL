/*
  ==============================================================================

    NodeRenderer.h
    Created: 2 Jan 2017 9:30:33pm
    Author:  Ricky

  ==============================================================================
*/

#pragma once
namespace juce{
enum class ModelMode
{
    OBJECT_3D  = 1,
    OBJECT_2D   = 3,
    LINE    = 2,
};
class Object3D
{
public:
    Matrix getModelMatrix()
    {
        return Matrix();
    }
};
    
class GL3DViewNode : public GLRendererNode
{
public:
    GL3DViewNode(OpenGLContext& glContext,
                  int screenWidth,
                  int screenHeight) :
    GLRendererNode(glContext, screenWidth, screenHeight, 3){}
    void initializeShape() override
    {
//        objectCircleShape = new ObjectCircleShape(context);
//        fillObjectShape = new SphereShape(context, 1.0);
//        flatObjectShape = new FlatObjectShape(context);
//        ambisonicCenterShape = new FlatObjectShape(context, 0.3f);
//        objectShape = new SphereShape(context, 1.0);
//        channelShape = new ObjectCircleShape(context);
//        flatChannelShape = new FlatObjectShape(context);
//        channelLabelShape = new RectangleShape(context, 10.0f, 1.25f);
//        labelShape = new RectangleShape(context, 3.5f, 0.875f);
    }
    void initializeBuffer() override
    {
//        genMultisampleFrameBuffer();
    }
    void initializeUniformVariables()
    {
//        CREATE_UNIFORM(modelMatrix);
//        CREATE_UNIFORM(viewMatrix);
//        CREATE_UNIFORM(projectionMatrix);
//        CREATE_UNIFORM(isUniformColour);
//        CREATE_UNIFORM(bodyColour);
//        CREATE_UNIFORM(idColour);
//        CREATE_UNIFORM(modelScale);
//        CREATE_UNIFORM(modelMode);
//        CREATE_UNIFORM(lineWidth);
//        CREATE_UNIFORM(nodeIndex);
//        CREATE_UNIFORM(nodeNum);
//        CREATE_UNIFORM(labelTexture);
//        CREATE_UNIFORM(channelLabelTexture);
//        CREATE_UNIFORM(alphaMask);
//        CREATE_UNIFORM(labelOffset);
//        CREATE_UNIFORM(videoTexture);
//        CREATE_UNIFORM(stereoScopicMode);
//        CREATE_UNIFORM(isVideoMode);
//        CREATE_UNIFORM(videoType);
//        CREATE_UNIFORM(ambisonicBeams);
    }
    void shutDownContext() override
    {
//        glDeleteTextures(outputTextureNum, msOutputTexture.getRawDataPointer());
//        context.extensions.glDeleteRenderbuffers(1, &msrenderBuffer);
//        context.extensions.glDeleteFramebuffers(1, &msframeBuffer);
//        glDeleteTextures(outputTextureNum, outputTexture.getRawDataPointer());
//        context.extensions.glDeleteRenderbuffers(1, &renderBuffer);
//        context.extensions.glDeleteFramebuffers(1, &frameBuffer);
    }
    void BindAmbisonicTexture(GLuint inputTexture)
    {
        ambisonicTextureID = inputTexture;
    }
    void BindLabelTexture(GLuint inputTexture)
    {
        labelTextureID = inputTexture;
    }
    void BindChannelLabelTexture(GLuint inputTexture)
    {
        channelLabelTextureID = inputTexture;
    }
    virtual Matrix getModelMatrix(Object3D* node, float x, float y, float z)
    {
        Matrix r;
        r = Matrix(1, 0, 0, x,
                   0, 1, 0, y,
                   0, 0, 1, z,
                   0, 0, 0, 1);
        return node->getModelMatrix() * r;
    }
    virtual Matrix getTransformMatrix() = 0;
    virtual Matrix getInverseTransformMatrix() = 0;
    virtual Matrix getViewMatrix(bool isInterative) = 0;
    virtual Matrix getProjectionMatrix() = 0;
    virtual Matrix getInverseProjectionMatrix() = 0;
    virtual Matrix getInverseViewMatrix(bool isInterative) = 0;
    virtual Vector getScreenPosition(Object3D* node, float x, float y, float z) = 0;
    virtual Matrix getViewSpacePosition(Object3D* node, float x, float y, float z) = 0;
    virtual Vector getAbsolutePosition(Vector p) = 0;
    virtual void checkViewAngleChanged() = 0;
//    virtual void resizeColorAttachment(int w, int h) override
//    {
//        glBindFramebuffer(GL_FRAMEBUFFER, msframeBuffer);
//        for (GLuint i = 0; i < outputTextureNum ; i++)
//        {
//            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, msOutputTexture[i]);
//            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA16F, width, height, GL_TRUE);
//        }
//        glDeleteRenderbuffers(1, &msrenderBuffer);
//        glGenRenderbuffers (1, &msrenderBuffer);
//        glBindRenderbuffer (GL_RENDERBUFFER, msrenderBuffer);
//        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
//        glBindRenderbuffer(GL_RENDERBUFFER, 0);
//        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, msrenderBuffer);
//        
//        GLRendererNode::resizeColorAttachment(w, h);
//    }
//    virtual float getListenerYaw() { return viewModel.getListenerYaw(); }
//    virtual float getListenerPitch() { return viewModel.getListenerPitch(); }
//    virtual void setListenerYaw(float val) { viewModel.setListenerYaw(val); }
//    virtual void setListenerPitch(float val) { viewModel.setListenerPitch(val); }
    
    float getCameraYaw() { return camAzimuth; }
    float getCameraPitch() { return camElevation; }
    float getCameraDistance() { return camDistance; }
    void setCameraYaw(float val) { camAzimuth = val; }
    void setCameraPitch(float val) { camElevation = val; }
    void setCameraDistance(float val) { camDistance = val; }
    
    float getViewAngleHorizon() { return viewAngleHorizon; }
    float getViewAngleVertical() { return viewAngleVertical; }
    void setViewAngleHorizon(float val) { viewAngleHorizon = val; }
    void setViewAngleVertical(float val) { viewAngleVertical = val; }
    
protected:
    float viewAngleHorizon;
    float viewAngleVertical;
    float camAzimuth;
    float camElevation;
    float camDistance;
    float nodeSize;
    
    ScopedPointer<OpenGLShaderProgram::Uniform> projectionMatrix;
    ScopedPointer<OpenGLShaderProgram::Uniform> viewMatrix;
    ScopedPointer<OpenGLShaderProgram::Uniform> modelMatrix;
    ScopedPointer<OpenGLShaderProgram::Uniform> modelScale;
    ScopedPointer<OpenGLShaderProgram::Uniform> labelOffset;
    
    ScopedPointer<OpenGLShaderProgram::Uniform> bodyColour;
    ScopedPointer<OpenGLShaderProgram::Uniform> idColour;
    ScopedPointer<OpenGLShaderProgram::Uniform> isUniformColour;
    
    ScopedPointer<OpenGLShaderProgram::Uniform> lineWidth;
    ScopedPointer<OpenGLShaderProgram::Uniform> modelMode;
    
    ScopedPointer<OpenGLShaderProgram::Uniform> nodeIndex;
    ScopedPointer<OpenGLShaderProgram::Uniform> nodeNum;
    ScopedPointer<OpenGLShaderProgram::Uniform> labelTexture;
    ScopedPointer<OpenGLShaderProgram::Uniform> channelLabelTexture;
    
    ScopedPointer<OpenGLShaderProgram::Uniform> alphaMask;
    

    ScopedPointer<OpenGLShaderProgram::Uniform> videoTexture;
    ScopedPointer<OpenGLShaderProgram::Uniform> stereoScopicMode;
    ScopedPointer<OpenGLShaderProgram::Uniform> isVideoMode;
    ScopedPointer<OpenGLShaderProgram::Uniform> videoType;
    
    ScopedPointer<OpenGLShaderProgram::Uniform> ambisonicBeams;
    void drawShape(GLShape* shapeToDraw, float size,
                   ModelMode mode,
                   Colour iColour,
                   Colour bColour = Colour(0x00000000),
                   bool isOpacity = true)
    {
        modelScale->set(size);
        modelMode->set(static_cast<int>(mode));
        if(bColour != Colour(0x00000000))
        {
            bodyColour->set(bColour.getFloatRed(), bColour.getFloatGreen(), bColour.getFloatBlue(),
                            isOpacity == true ?
                            (bColour.getFloatAlpha() == 1.0f ? opacity : bColour.getFloatAlpha() ) : 1.0f);
            isUniformColour->set(1);
        }
        shapeToDraw->draw();
        modelMode->set(0);
        isUniformColour->set(0);
    }
    virtual float getNodeSize(Object3D*) { return nodeSize; }
    
    GLuint labelTextureID;
    GLuint channelLabelTextureID;
    GLuint ambisonicTextureID;
    
//    ScopedPointer<ObjectCircleShape> objectCircleShape;
//    ScopedPointer<SphereShape> fillObjectShape;
//    ScopedPointer<FlatObjectShape> flatObjectShape;
//    ScopedPointer<FlatObjectShape> ambisonicCenterShape;
//    ScopedPointer<SphereShape> objectShape;
//    ScopedPointer<ObjectCircleShape> channelShape;
//    ScopedPointer<FlatObjectShape> flatChannelShape;
//    ScopedPointer<RectangleShape> channelLabelShape;
//    ScopedPointer<RectangleShape> labelShape;
    GLubyte *data;
    int mx= 0, my = 0;
    bool isSelectMode = false;
    bool isChannelLabelBinded = false;
    float labelSize;
    float opacity = 0.5f;
    int currentViewAngle;
};
}
