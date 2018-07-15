#ifdef JUCE_GRAPHICS_WRAPPER_H_INCLUDED
/* When you add this cpp file to your project, you mustn't include it in a file where you've
already included any other headers - just put it inside a file on its own, possibly with your config
flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
header files that the compiler may be using.
*/
#error "Incorrect use of JUCE cpp file"
#endif
#include "juce_opengl_wrapper.h"
#include "frame_buffer/GLFrameBuffer_impl.cpp"
#include "frame_buffer/GLFrameBufferMultiSample_impl.cpp"
#include "frame_buffer/GLFrameBuffer.cpp"
#include "renderer/GLBlurNode.cpp"
#include "renderer/GLScreenNode.cpp"
#include "renderer/GLImageRenderNode.cpp"
