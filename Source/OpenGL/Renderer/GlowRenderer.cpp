/*
  ==============================================================================

    GlowRenderer.cpp
    Created: 12 Oct 2017 2:34:59pm
    Author:  Ricky

  ==============================================================================
*/

#include "GlowRenderer.h"

const char* GlowRenderer::getVertexShader()
{
    return
    "#version 330 core\n"
    "layout (location = 0) in vec4 position;\n"
    "layout (location = 1) in vec4 normal;\n"
    "layout (location = 2) in vec4 sourceColour;\n"
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
const char* GlowRenderer::getFragmentShader()
{
    return
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 TexCoords;\n"
    "in vec4 scolor;\n"
    "\n"
    "uniform sampler2D image;\n"
    "uniform float blurLength;\n"
    "uniform bool isVertical;\n"
    "\n"
    "float weight[5] = float[](0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);\n"
    "void main()\n"
    "{\n"
    "   vec2 tex_offset = 1.0 / textureSize(image, 0);\n"
    "   vec3 result = texture(image, TexCoords).rgb * (weight[0] + blurLength);\n"
    "   if(isVertical)\n"
    "   {\n"
    "       for (int i=1; i<5; i++)"
    "       {\n"
    "           result += texture(image, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];\n"
    "           result += texture(image, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];\n"
    "       }\n"
    "   }\n"
    "   else\n"
    "   {\n"
    "       for (int i=1; i<5; i++)"
    "       {\n"
    "           result += texture(image, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];\n"
    "           result += texture(image, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];\n"
    "       }\n"
    "   }\n"
    "   FragColor = vec4(result / (blurLength + 1), 1.0);\n"
    "}";
}
GlowRenderer::GlowRenderer(OpenGLContext& glContext,
                                           int screenWidth,
                                           int screenHeight,
                                           float amount) :
OpenGLDrawer(glContext, screenWidth, screenHeight, 1),
Amount(amount)
{
    glowArea = 1;
}
void GlowRenderer::initializeUniform()
{
    CREATE_UNIFORM(image);// = createUniform("image");
    CREATE_UNIFORM(blurLength);// = createUniform("blurLength");
    CREATE_UNIFORM(isVertical);// = createUniform("isVertical");
}
void GlowRenderer::initializeBuffer()
{
    genFrameBuffer();
    genDoubleFrameBuffer();
}
void GlowRenderer::shutDownContext()
{
    glDeleteTextures(outputTextureNum, outputTexture.getRawDataPointer());
    context.extensions.glDeleteRenderbuffers(1, &renderBuffer);
    context.extensions.glDeleteFramebuffers(1, &frameBuffer);
    glDeleteTextures(2, pingpongBuffer);
    context.extensions.glDeleteFramebuffers(2, pingpongFBO);
}
void GlowRenderer::preDraw()
{
    shaderProgram->use();
    blurLength->set((float)glowArea);
    image->set(0);
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[0]);
    OpenGLHelpers::clear (Colour::greyLevel (0.1f));// Enable depth test
    isVertical->set(true);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, inputTexture);
    screenShape->draw();
}
void GlowRenderer::doDraw()
{
    int i;
    int idx = 1;
    bool vhflip = true;
    for(i=0 ; i<10 ; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[idx]);
        idx = 1 - idx;
        vhflip = !vhflip;
        OpenGLHelpers::clear (Colour::greyLevel (0.1f));// Enable depth test
        isVertical->set(vhflip);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer[idx]);
        screenShape->draw();
    }
}
void GlowRenderer::postDraw()
{
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    OpenGLHelpers::clear (Colour::greyLevel (0.1f));// Enable depth test
    isVertical->set(false);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pingpongBuffer[0]);
    screenShape->draw();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
