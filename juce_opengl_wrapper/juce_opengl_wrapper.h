/*
 ==============================================================================
 
 This file is part of the JUCE library.
 Copyright (c) 2017 - ROLI Ltd.
 
 JUCE is an open source library subject to commercial or open-source
 licensing.
 
 By using JUCE, you agree to the terms of both the JUCE 5 End-User License
 Agreement and JUCE 5 Privacy Policy (both updated and effective as of the
 27th April 2017).
 
 End User License Agreement: www.juce.com/juce-5-licence
 Privacy Policy: www.juce.com/juce-5-privacy-policy
 
 Or: You may also use this code under the terms of the GPL v3 (see
 www.gnu.org/licenses).
 
 JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
 EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
 DISCLAIMED.
 
 ==============================================================================
 */

/*******************************************************************************
 The block below describes the properties of this module, and is read by
 the Projucer to automatically generate project code that uses it.
 For details about the syntax and how to create or use a module, see the
 JUCE Module Format.txt file.
 
 
 BEGIN_JUCE_MODULE_DECLARATION
 
 ID:               juce_opengl_wrapper
 vendor:           rickysung
 version:          5.2.0
 name:             Custom OpenGL Wrapper classes
 description:      Classes for wrapping OpenGL APIs to make implements easy in a JUCE window.
 website:          http://www.juce.com/juce
 license:          GPL/Commercial
 
 dependencies:     juce_gui_extra juce_opengl
 OSXFrameworks:    OpenGL
 iOSFrameworks:    OpenGLES
 linuxLibs:        GL
 mingwLibs:        opengl32
 
 END_JUCE_MODULE_DECLARATION
 
 *******************************************************************************/


#pragma once
#define JUCE_GRAPHICS_WRAPPER_H_INCLUDED
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_opengl/juce_opengl.h>

//==============================================================================
//==============================================================================
namespace juce
{
    class GLVertexSet;
    class GLShape;
    class GLFrameBuffer;
    class GLRendererNode;
    class WavefrontObjFile;
    
    typedef Matrix3D<float> Matrix;
    typedef Vector3D<float> Vector;
}

#include "shape/WavefrontObjParser.h"
#include "shape/GLVertexSet.h"
#include "shape/GLShape.h"
#include "frame_buffer/GLFrameBuffer_impl.h"
#include "frame_buffer/GLFrameBufferMultiSample_impl.h"
#include "frame_buffer/GLFrameBuffer.h"
#include "renderer/GLRendererNode.h"
#include "renderer/GLImageRenderNode.h"
#include "renderer/GLScreenNode.h"
#include "renderer/GLBlurNode.h"
#include "renderer/GL3DViewNode.h"
