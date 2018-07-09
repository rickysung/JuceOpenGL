/*
  ==============================================================================

    ScreenRenderer.cpp
    Created: 20 Aug 2016 8:19:03pm
    Author:  Ricky

  ==============================================================================
*/

#include "ScreenRenderer.h"
const char* ScreenRenderer::getVertexShader()
{
    return
    "#version 330 core\n"
    "layout (location = 0) in vec4 position;\n"
    "layout (location = 3) in vec2 textureCoordIn;\n"
    "\n"
    "out vec2 TexCoords;\n"
    "out vec4 scolor;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    gl_Position = position;\n"
    "    TexCoords = textureCoordIn;\n"
    "}";
}
const char* ScreenRenderer::getFragmentShader()
{
    return
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 TexCoords;\n"
    "in vec4 scolor;\n"
    "\n"
    "uniform sampler2D scene;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   vec3 result = texture(scene, TexCoords).rgb;\n"
    "   FragColor = vec4(result, 1.0);\n"
    "}";
}
ScreenRenderer::ScreenRenderer(OpenGLContext& glContext,
                                     int screenWidth,
                                     int screenHeight) :
OpenGLDrawer(glContext, screenWidth, screenHeight, 1)
{
}
void ScreenRenderer::initializeUniform()
{
    CREATE_UNIFORM(scene);
}
void ScreenRenderer::initializeBuffer()
{
    genFrameBuffer();
}
void ScreenRenderer::preDraw()
{
    shaderProgram->use();
    const float desktopScale = (float) context.getRenderingScale();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    OpenGLHelpers::clear (Colour(BACKGROUND_COMP));// Enable depth test
    glViewport(0, 0, width*desktopScale, height*desktopScale);
    scene->set(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, inputTexture);
}
void ScreenRenderer::doDraw()
{
    screenShape->draw();
}
void ScreenRenderer::postDraw()
{
    
}
