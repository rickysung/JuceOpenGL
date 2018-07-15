/*
  ==============================================================================

    Shape.h
    Created: 28 Aug 2016 4:24:40pm
    Author:  Ricky

  ==============================================================================
*/

#pragma once

namespace juce{
    

struct GLShape
{
public:
    GLShape(OpenGLContext& openGLContext) : context(openGLContext)
    {
    }
    virtual ~GLShape(){}
    virtual void draw () = 0;
protected:
    OpenGLContext& context;
};

struct WavefrontShape : public GLShape
{
public:
    WavefrontShape(OpenGLContext& openGLContext, String& binary) : GLShape(openGLContext)
    {
        shapeFile.load(binary);
        Array<Colour> colourmap = {Colour(0xFFA3A3A3), Colour(0xFFA3A3A3), Colour(0xFFA3A3A3)};
        for(int i=0 ; i<shapeFile.modelObjects.size() ; i++)
        {
            VertexArrays.add(new WavefrontVertex(shapeFile.modelObjects.getUnchecked(i)->mesh, colourmap[i]));
            VertexArrays.getLast()->initShape(context);
        }
    }
    ~WavefrontShape()
    {
        for(int i=0 ; i<shapeFile.modelObjects.size() ; i++)
        {
            VertexArrays.getUnchecked(i)->deleteShape(context);
        }
    }
    void draw() override
    {
        for(int i=0 ; i<shapeFile.modelObjects.size() ; i++)
        {
            context.extensions.glBindVertexArray(VertexArrays.getUnchecked(i)->shapeVAO);
            glDrawElements(GL_TRIANGLES, VertexArrays.getUnchecked(i)->vertexSize, GL_UNSIGNED_INT, 0);
            context.extensions.glBindVertexArray(0);
        }
    }
    WavefrontObjFile shapeFile;
    OwnedArray<WavefrontVertex> VertexArrays;
};

struct SphereShape : public GLShape
{
public:
    SphereShape(OpenGLContext& openGLContext, float radius) : GLShape(openGLContext)
    {
        globeVertex.initShape(context, radius);
    }
    ~SphereShape()
    {
        globeVertex.deleteShape(context);
    }
    void draw () override
    {
        context.extensions.glBindVertexArray(globeVertex.shapeVAO);
        glDrawElements(GL_TRIANGLES, globeVertex.vertexSize, GL_UNSIGNED_INT, 0);
        context.extensions.glBindVertexArray(0);
    }
private:
    SphereVertex globeVertex;
};

struct RectangleShape : public GLShape
{
    RectangleShape(OpenGLContext& openGLContext, float width = 2.0f, float height = 2.0f) : GLShape(openGLContext)
    {
        quadVertex.initShape(context, width*0.5, height*0.5);
    }
    ~RectangleShape()
    {
        quadVertex.deleteShape(context);
    }
    void draw () override
    {
        context.extensions.glBindVertexArray(quadVertex.shapeVAO);
        glDrawElements(GL_TRIANGLES, quadVertex.vertexSize, GL_UNSIGNED_INT, 0);
        context.extensions.glBindVertexArray(0);
    }
private:
    RectangleVertex quadVertex;
};

}
