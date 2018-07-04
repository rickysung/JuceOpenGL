//
//  GaussianBlurRenderer.cpp
//  GaudiReaper
//
//  Created by Ricky on 2016. 8. 13..
//
//

#include "GaussianBlurRenderer.h"
const char* GaussianBlurRenderer::getVertexShader()
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
const char* GaussianBlurRenderer::getFragmentShader()
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
void GaussianBlurRenderer::initializeUniform()
{
    CREATE_UNIFORM(image);// = createUniform("image");
    CREATE_UNIFORM(blurLength);// = createUniform("blurLength");
    CREATE_UNIFORM(isVertical);// = createUniform("isVertical");
}
void GaussianBlurRenderer::initializeBuffer()
{
    genFrameBuffer();
    genDoubleFrameBuffer();
}
void GaussianBlurRenderer::shutDownContext()
{
    glDeleteTextures(outputTextureNum, outputTexture.getRawDataPointer());
    context.extensions.glDeleteRenderbuffers(1, &renderBuffer);
    context.extensions.glDeleteFramebuffers(1, &frameBuffer);
    glDeleteTextures(2, pingpongBuffer);
    context.extensions.glDeleteFramebuffers(2, pingpongFBO);
}
GaussianBlurRenderer::GaussianBlurRenderer(OpenGLContext& glContext,
                                     int screenWidth,
                                     int screenHeight,
                                     float amount) :
OpenGLDrawer(glContext, screenWidth, screenHeight, 1),
Amount(amount) {}
void GaussianBlurRenderer::preDraw()
{
    shaderProgram->use();
    blurLength->set(Amount);
    image->set(0);
    glViewport(0, 0, width, height);
}
void GaussianBlurRenderer::doDraw()
{
    glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[0]);
    OpenGLHelpers::clear (Colour::greyLevel (0.1f));// Enable depth test
    isVertical->set(true);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, inputTexture);
    screenShape->draw();

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    OpenGLHelpers::clear (Colour::greyLevel (0.1f));// Enable depth test
    isVertical->set(false);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pingpongBuffer[0]);
    screenShape->draw();
}
void GaussianBlurRenderer::postDraw()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}