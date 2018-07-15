/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);
    openGLContext.setOpenGLVersionRequired(juce::OpenGLContext::OpenGLVersion::openGL3_2);
    imageNode.reset(new GLImageRenderNode(openGLContext, getWidth(), getHeight()));
    screenNode.reset(new GLScreenNode(openGLContext, getWidth(), getHeight()));
}

MainComponent::~MainComponent()
{
    // This shuts down the GL system and stops the rendering calls.
    shutdownOpenGL();
}

//==============================================================================
void MainComponent::initialise()
{
    // Initialise GL objects for rendering here.
    screenNode->initializeContext();
    imageNode->initializeContext();
    image.loadImage(ImageCache::getFromMemory(BinaryData::jucelogo_png, BinaryData::jucelogo_pngSize));
}

void MainComponent::shutdown()
{
    // Free any GL objects created for rendering here.
    imageNode->shutDownContext();
    screenNode->shutDownContext();
    image.release();
}

void MainComponent::render()
{
    // This clears the context with a black background.
    OpenGLHelpers::clear (Colours::black);
    imageNode->BindTexture(image.getTextureID());
    imageNode->draw();
    screenNode->BindTexture(imageNode->getTexture(0));
    screenNode->draw();
    // Add your rendering code here...
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    // You can add your component specific drawing code here!
    // This will draw over the top of the openGL background.
}

void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}
