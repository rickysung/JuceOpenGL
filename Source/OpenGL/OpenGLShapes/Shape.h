/*
  ==============================================================================

    Shape.h
    Created: 28 Aug 2016 4:24:40pm
    Author:  Ricky

  ==============================================================================
*/

#ifndef SHAPE_H_INCLUDED
#define SHAPE_H_INCLUDED

#include "ShapeVertex.h"

struct Shape
{
public:
    Shape(OpenGLContext& openGLContext) : context(openGLContext)
    {
    }
    virtual ~Shape(){}
    virtual void draw () = 0;
protected:
    OpenGLContext& context;
};

struct HeadShape : public Shape
{
public:
    HeadShape(OpenGLContext& openGLContext, String& binary) : Shape(openGLContext)
    {
        shapeFile.load(binary);
        Array<Colour> colourmap = {Colour(0xFFA3A3A3), Colour(0xFFA3A3A3), Colour(0xFFA3A3A3)};
        for(int i=0 ; i<shapeFile.modelObjects.size() ; i++)
        {
            VertexArrays.add(new WavefrontVertex(shapeFile.modelObjects.getUnchecked(i)->mesh, colourmap[i]));
            VertexArrays.getLast()->initShape(context);
        }
    }
    ~HeadShape()
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

struct CursorShape : public Shape
{
public:
    CursorShape(OpenGLContext& openGLContext) : Shape(openGLContext)
    {
        cursorPointVertex.initShape(context);
    }
    ~CursorShape()
    {
        cursorPointVertex.deleteShape(context);
    }
    void setCursorPoint(Vector point)
    {
        cursorPointVertex.setPoint(context, point);
    }
    void draw () override
    {
        context.extensions.glBindVertexArray(cursorPointVertex.shapeVAO);
        glDrawElements(GL_LINE_LOOP, cursorPointVertex.vertexSize, GL_UNSIGNED_INT, 0);
        context.extensions.glBindVertexArray(0);
    }
private:
    CursorVertex cursorPointVertex;
};

struct ObjectCircleShape : public Shape
{
public:
    ObjectCircleShape(OpenGLContext& openGLContext) : Shape(openGLContext)
    {
        objectVertex.initShape(context, 1.0);
    }
    ~ObjectCircleShape()
    {
        objectVertex.deleteShape(context);
    }
    void draw () override
    {
        context.extensions.glBindVertexArray(objectVertex.shapeVAO);
        glDrawElements(GL_TRIANGLE_FAN, objectVertex.vertexSize, GL_UNSIGNED_INT, 0);
        context.extensions.glBindVertexArray(0);
    }
private:
    ObjectCircleVertex objectVertex;
};

struct AmbisonicShape : public Shape
{
public:
    AmbisonicShape(OpenGLContext& openGLContext) : Shape(openGLContext)
    {
        ambisonicVertex.initShape(context, 1.0);
    }
    ~AmbisonicShape()
    {
        ambisonicVertex.deleteShape(context);
    }
    void draw () override
    {
        context.extensions.glBindVertexArray(ambisonicVertex.shapeVAO);
        glDrawElementsInstanced(GL_TRIANGLE_FAN, ambisonicVertex.vertexSize, GL_UNSIGNED_INT, 0, ambisonicVertex.instanceSize);
        context.extensions.glBindVertexArray(0);
    }
private:
    AmbisonicPointsVertex ambisonicVertex;
};
struct FanShape : public Shape
{
public:
    FanShape(OpenGLContext& openGLContext, float horizonAngle) : Shape(openGLContext)
    {
        fanVertex.initShape(context, horizonAngle);
    }
    ~FanShape()
    {
        fanVertex.deleteShape(context);
    }
    void draw() override
    {
        context.extensions.glBindVertexArray(fanVertex.shapeVAO);
        glDrawElements(GL_TRIANGLE_FAN, fanVertex.vertexSize, GL_UNSIGNED_INT, 0);
        context.extensions.glBindVertexArray(0);
    }
    void setAngle(float ha)
    {
        if(this->horizonAngle != ha)
        {
            this->horizonAngle = ha;
            fanVertex.setAngle(context, ha);
        }
    }
private:
    FanVertex fanVertex;
    float horizonAngle = 101/180.0f*M_PI;
};

struct ShadowShape : public Shape
{
public:
    ShadowShape(OpenGLContext& openGLContext) : Shape(openGLContext)
    {
        shadowVertex.initShape(context, 1.0);
    }
    ~ShadowShape()
    {
        shadowVertex.deleteShape(context);
    }
    void draw() override
    {
        context.extensions.glBindVertexArray(shadowVertex.shapeVAO);
        glDrawElements(GL_TRIANGLE_FAN, shadowVertex.vertexSize, GL_UNSIGNED_INT, 0);
        context.extensions.glBindVertexArray(0);
    }
private:
    ShadowVertex shadowVertex;
};

struct FlatObjectShape : public Shape
{
public:
    FlatObjectShape(OpenGLContext& openGLContext) : Shape(openGLContext)
    {
        objectVertex.initShape(context, 0.15f);
    }
    FlatObjectShape(OpenGLContext& openGLContext, float thick) : Shape(openGLContext)
    {
        objectVertex.initShape(context, thick);
    }
    ~FlatObjectShape()
    {
        objectVertex.deleteShape(context);
    }
    void draw() override
    {
        context.extensions.glBindVertexArray(objectVertex.shapeVAO);
        glDrawElements(GL_TRIANGLES, objectVertex.vertexSize, GL_UNSIGNED_INT, 0);
        context.extensions.glBindVertexArray(0);
    }
private:
    FlatEllipseVertex objectVertex;
};

struct SphereShape : public Shape
{
public:
    SphereShape(OpenGLContext& openGLContext, float radius) : Shape(openGLContext)
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

struct SphereGridShape : public Shape
{
    SphereGridShape(OpenGLContext& openGLContext, float radius, float div) : Shape(openGLContext)
    {
        gridVertex.initShape(context, radius, div);
    }
    ~SphereGridShape()
    {
        gridVertex.deleteShape(context);
    }
    void draw () override
    {
        context.extensions.glBindVertexArray(gridVertex.shapeVAO);
        glDrawElements(GL_LINES, gridVertex.vertexSize, GL_UNSIGNED_INT, 0);
        context.extensions.glBindVertexArray(0);
    }
private:
    SphereGridVertex gridVertex;
};

struct HeadOrientShape : public Shape
{
public:
    HeadOrientShape(OpenGLContext& openGLContext, float length) : Shape(openGLContext)
    {
        HeadVector.initShape(context, 1.3f, Colour(Colours::white), Vector(0, 0, length));
        NoseVector.initShape(context, 1.3f, Colour(Colours::white), Vector(0, length, 0));
    }
    ~HeadOrientShape()
    {
        HeadVector.deleteShape(context);
        NoseVector.deleteShape(context);
    }
    void draw() override
    {
        context.extensions.glBindVertexArray(HeadVector.shapeVAO);
        glDrawElements(GL_TRIANGLES, HeadVector.vertexSize, GL_UNSIGNED_INT, 0);
        
        context.extensions.glBindVertexArray(NoseVector.shapeVAO);
        glDrawElements(GL_TRIANGLES, NoseVector.vertexSize, GL_UNSIGNED_INT, 0);
    }
private:
    LineVertex HeadVector;
    LineVertex NoseVector;
};

struct UVSphereGridShape : public Shape
{
    UVSphereGridShape(OpenGLContext& openGLContext, float rad) : Shape(openGLContext)
    {
        uvsphere.initShape(context, rad);
    }
    ~UVSphereGridShape()
    {
        uvsphere.deleteShape(context);
    }
    void draw () override
    {
        context.extensions.glBindVertexArray(uvsphere.shapeVAO);
        glDrawElements(GL_TRIANGLES, uvsphere.vertexSize, GL_UNSIGNED_INT, 0);
        context.extensions.glBindVertexArray(0);
    }
    UVSphereVertex uvsphere;
};

struct MapViewGridShape : public Shape
{
public:
    MapViewGridShape(OpenGLContext& openGLContext, float diff, int div) : Shape(openGLContext), division(div)
    {
        int i;
        gridEllipses = new EllipseVertex[division];
        for(i=0 ; i<division ; i++)
        {
            gridEllipses[i].initShape(context, diff*(i+1));
        }
    }
    ~MapViewGridShape()
    {
        int i;
        for(i=0 ; i<division ; i++)
        {
            gridEllipses[i].deleteShape(context);
        }
        delete[] gridEllipses;
    }
    void draw () override
    {
        int i;
        for(i=0 ; i<division ; i++)
        {
            context.extensions.glBindVertexArray(gridEllipses[i].shapeVAO);
            glDrawElements(GL_TRIANGLES, gridEllipses[i].vertexSize, GL_UNSIGNED_INT, 0);
        }
        context.extensions.glBindVertexArray(0);
    }
private:
    const int division;
    EllipseVertex* gridEllipses;
};

struct SpeakerShape : public Shape
{
public:
    SpeakerShape(OpenGLContext& openGLContext) : Shape(openGLContext)
    {
        cubeVertex.initShape(context, 0.64f, 0.8f);
    }
    ~SpeakerShape()
    {
        cubeVertex.deleteShape(context);
    }
    void draw() override
    {
        context.extensions.glBindVertexArray(cubeVertex.shapeVAO);
        glDrawElements(GL_TRIANGLES, cubeVertex.vertexSize, GL_UNSIGNED_INT, 0);
        context.extensions.glBindVertexArray(0);
    }
private:
    RectangleVertex cubeVertex;
};

struct FlatSpeakerShape : public Shape
{
public:
    FlatSpeakerShape(OpenGLContext& openGLContext) : Shape(openGLContext)
    {
        flatRectangleVertex.initShape(context, 0.64f, 0.8f);
    }
    ~FlatSpeakerShape()
    {
        flatRectangleVertex.deleteShape(context);
    }
    void draw() override
    {
        context.extensions.glBindVertexArray(flatRectangleVertex.shapeVAO);
        glDrawElements(GL_TRIANGLES, flatRectangleVertex.vertexSize, GL_UNSIGNED_INT, 0);
        context.extensions.glBindVertexArray(0);
    }
private:
    FlatRectangleVertex flatRectangleVertex;
};

struct RectangleShape : public Shape
{
    RectangleShape(OpenGLContext& openGLContext, float width = 2.0f, float height = 2.0f) : Shape(openGLContext)
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

#endif  // SHAPE_H_INCLUDED
