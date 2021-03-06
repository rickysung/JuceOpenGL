
namespace juce{
#include "GLImageRenderNode.h"
const char* GLImageRenderNode::getVertexShader()
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
const char* GLImageRenderNode::getFragmentShader()
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
    "   FragColor = vec4(0.5*result, 1.0);\n"
    "}";
}
GLImageRenderNode::GLImageRenderNode(OpenGLContext& context, int width, int height) : GLOffScreenRendererNode(context, width, height, 1, true)
{
    
}
void GLImageRenderNode::initializeUniform()
{
    CREATE_UNIFORM(scene);
}
void GLImageRenderNode::doDraw()
{
    scene->set(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, inputTexture);
    screenShape->draw();
}
}
