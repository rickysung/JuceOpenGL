/*
  ==============================================================================

    ShapeVertex.h
    Created: 28 Aug 2016 4:25:16pm
    Author:  Ricky

  ==============================================================================
*/

#ifndef SHAPEVERTEX_H_INCLUDED
#define SHAPEVERTEX_H_INCLUDED
#include "WavefrontObjParser.h"

#define BACKGROUND_MAIN                     0xff000000
#define BACKGROUND_COMP                     0xff1B1B1B
#define BACKGROUND_STRK                     0xff121212
#define BACKGROUND_SUB                      0xff323232
#define TREEVIEW_SELECT                     0xff2b2b2b
#define RULER_LINE                          0xff383838
#define BOX_LINE                            0xff383838
#define GRID_LINE                           0xff666666
#define SHADOW_COLOR                        0xff343434
#define FOCUS_COLOR                         0xff33d9ff
#define EDITLOCK_COLOR                      0xff9c9c9c

struct Vertex
{
    float position[3];
    float normal[3];
    float colour[4];
    float texCoord[2];
};
struct InstancedShapeVertex
{
public:
    InstancedShapeVertex(){}
    virtual ~InstancedShapeVertex(){}
    virtual void initShape(OpenGLContext& context, float cof1, float cof2, float cof3) = 0;
    void deleteShape(OpenGLContext& context)
    {
        context.extensions.glDeleteVertexArrays(1, &shapeVAO);
        context.extensions.glDeleteBuffers(1, &shapeVBO);
        context.extensions.glDeleteBuffers(1, &shapeEBO);
        context.extensions.glDeleteBuffers(1, &shapeIBO);
    }
    GLuint shapeVAO = 0;
    int vertexSize;
    int instanceSize;
protected:
    Array<Matrix> matrices;
    Array<Vertex> vertices;
    Array<uint32> indecies;
    GLuint shapeVBO, shapeEBO, shapeIBO;
    void putData(OpenGLContext& context, Array<Vertex>& vertices, Array<uint32>& indecies)
    {
        context.extensions.glBindBuffer (GL_ARRAY_BUFFER, shapeIBO);
        context.extensions.glBufferData (GL_ARRAY_BUFFER,
                                         static_cast<GLsizeiptr> (static_cast<size_t> (matrices.size()) * sizeof (Matrix)),
                                         matrices.getRawDataPointer(), GL_STATIC_DRAW);
        
        context.extensions.glBindBuffer (GL_ARRAY_BUFFER, shapeVBO);
        context.extensions.glBufferData (GL_ARRAY_BUFFER,
                                         static_cast<GLsizeiptr> (static_cast<size_t> (vertices.size()) * sizeof (Vertex)),
                                         vertices.getRawDataPointer(), GL_STATIC_DRAW);
        
        context.extensions.glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, shapeEBO);
        context.extensions.glBufferData (GL_ELEMENT_ARRAY_BUFFER,
                                         static_cast<GLsizeiptr> (static_cast<size_t> (indecies.size()) * sizeof (juce::uint32)),
                                         indecies.getRawDataPointer(), GL_STATIC_DRAW);
    }
    void enableVertexArray(OpenGLContext& context)
    {
        glBindBuffer(GL_ARRAY_BUFFER, shapeVBO);
        context.extensions.glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), 0);
        context.extensions.glEnableVertexAttribArray (0);
        
        context.extensions.glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 3));
        context.extensions.glEnableVertexAttribArray (1);
        
        context.extensions.glVertexAttribPointer (2, 4, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 6));
        context.extensions.glEnableVertexAttribArray (2);
        
        context.extensions.glVertexAttribPointer (3, 2, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 10));
        context.extensions.glEnableVertexAttribArray (3);
        
        glBindBuffer(GL_ARRAY_BUFFER, shapeIBO);
        
        context.extensions.glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*16, (GLvoid*)0);
        context.extensions.glEnableVertexAttribArray(4);
        
        context.extensions.glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*16, (GLvoid*)(sizeof(GLfloat) * 4));
        context.extensions.glEnableVertexAttribArray(5);
        
        context.extensions.glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*16, (GLvoid*)(sizeof(GLfloat) * 8));
        context.extensions.glEnableVertexAttribArray(6);
        
        context.extensions.glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*16, (GLvoid*)(sizeof(GLfloat) * 12));
        context.extensions.glEnableVertexAttribArray(7);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
        glVertexAttribDivisor(7, 1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    void initVertexArray(OpenGLContext& context)
    {
        context.extensions.glGenVertexArrays(1, &shapeVAO);
        context.extensions.glGenBuffers (1, &shapeVBO);
        context.extensions.glGenBuffers (1, &shapeEBO);
        context.extensions.glGenBuffers (1, &shapeIBO);
        context.extensions.glBindVertexArray(shapeVAO);
        putData(context, vertices, indecies);
        enableVertexArray(context);
        context.extensions.glBindVertexArray(0);
        vertexSize = indecies.size();
    }
    Matrix getModelMatrixAtPosition(float azi, float elv, float dis)
    {
        Matrix r;
        float ca, sa, ce, se;
        ca = std::cos(azi);
        sa = std::sin(azi);
        ce = std::cos(elv);
        se = std::sin(elv);
        r = Matrix(ca, 0, -sa, 0,
                   -se*sa, ce, -ca*se, 0,
                   ce*sa, se, ce*ca, 0,
                   dis*ce*sa, dis*se, dis*ce*ca, 1);
        return r;
    }
};
struct ShapeVertex
{
    ShapeVertex(){}
    virtual ~ShapeVertex()
    {
    }
    void buildArcLine(Vector v1, Vector v2, float thickness)// p1 and p2 contains Azimuth Elevation Distance sequence
    {
        int i;
        float x1, y1, z1;
        float x2, y2, z2;
        Colour g = Colour(GRID_LINE);
        Vector p1, p2;
        Vector norm[8];
        Vertex vert;
        float a, e, d;
        int cnt = vertices.size() - 1;
        a = v1.x;
        e = v1.y;
        d = v1.z;
        x1 = d * std::cos(e) * std::sin(a);
        y1 = d * std::sin(e);
        z1 = d * std::cos(e) * std::cos(a);
        p1 = Vector(x1, y1, z1);
        a = v2.x;
        e = v2.y;
        d = v2.z;
        x2 = d * std::cos(e) * std::sin(a);
        y2 = d * std::sin(e);
        z2 = d * std::cos(e) * std::cos(a);
        p2 = Vector(x2, y2, z2);
        norm[0] = -p1/p1.length();
        norm[1] = (p1-p2)^p1;
        norm[1] /= norm[1].length();
        norm[2] = -norm[0];
        norm[3] = -norm[1];
        norm[4] = -p2/p2.length();
        norm[5] = p2^(p2-p1);
        norm[5] /= norm[5].length();
        norm[6] = -norm[4];
        norm[7] = -norm[5];
        
        for(i=0 ; i<4 ; i++)
        {
            norm[i]*=thickness;
            vert = {
                {x1, y1, z1},
                {norm[i].x, norm[i].y, norm[i].z},
                {g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},
                {0.5, 0.5}
            };
            vertices.add(vert);
        }
        for(i=4 ; i<8 ; i++)
        {
            norm[i]*=thickness;
            vert = {
                {x2, y2, z2},
                {norm[i].x, norm[i].y, norm[i].z},
                {g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},
                {0.5, 0.5}
            };
            vertices.add(vert);
        }
        indecies.add(cnt+1);
        indecies.add(cnt+2);
        indecies.add(cnt+5);
        
        indecies.add(cnt+2);
        indecies.add(cnt+5);
        indecies.add(cnt+6);
        
        indecies.add(cnt+2);
        indecies.add(cnt+3);
        indecies.add(cnt+6);
        
        indecies.add(cnt+3);
        indecies.add(cnt+6);
        indecies.add(cnt+7);
        
        indecies.add(cnt+3);
        indecies.add(cnt+4);
        indecies.add(cnt+7);
        
        indecies.add(cnt+4);
        indecies.add(cnt+7);
        indecies.add(cnt+8);
        
        indecies.add(cnt+1);
        indecies.add(cnt+4);
        indecies.add(cnt+5);
        
        indecies.add(cnt+4);
        indecies.add(cnt+5);
        indecies.add(cnt+8);
    }
    void buildStraightLine(Vector p1, Vector p2, float thickness, Colour g)
    {
        int i;
        Vector norm[8];
        Vertex vert;
        int cnt = vertices.size() - 1;
        norm[1] = (p1-p2)^(p1-Vector(-999,372,51));
        norm[1] /= norm[1].length();
        norm[0] = (p1-p2)^norm[1];
        norm[0] /= norm[0].length();
        norm[2] = -norm[0];
        norm[3] = -norm[1];
        norm[5] = (p2-Vector(-999,372,51))^(p2-p1);
        norm[5] /= norm[5].length();
        norm[4] = norm[5]^(p2-p1);
        norm[4] /= norm[4].length();
        norm[6] = -norm[4];
        norm[7] = -norm[5];
        for(i=0 ; i<4 ; i++)
        {
            norm[i]*=thickness;
            vert = {
                {p1.x, p1.y, p1.z},
                {norm[i].x, norm[i].y, norm[i].z},
                {g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},
                {0.5, 0.5}
            };
            vertices.add(vert);
        }
        for(i=4 ; i<8 ; i++)
        {
            norm[i]*=thickness;
            vert = {
                {p2.x, p2.y, p2.z},
                {norm[i].x, norm[i].y, norm[i].z},
                {g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},
                {0.5, 0.5}
            };
            vertices.add(vert);
        }
        indecies.add(cnt+1);
        indecies.add(cnt+2);
        indecies.add(cnt+5);
        
        indecies.add(cnt+2);
        indecies.add(cnt+5);
        indecies.add(cnt+6);
        
        indecies.add(cnt+2);
        indecies.add(cnt+3);
        indecies.add(cnt+6);
        
        indecies.add(cnt+3);
        indecies.add(cnt+6);
        indecies.add(cnt+7);
        
        indecies.add(cnt+3);
        indecies.add(cnt+4);
        indecies.add(cnt+7);
        
        indecies.add(cnt+4);
        indecies.add(cnt+7);
        indecies.add(cnt+8);
        
        indecies.add(cnt+1);
        indecies.add(cnt+4);
        indecies.add(cnt+5);
        
        indecies.add(cnt+4);
        indecies.add(cnt+5);
        indecies.add(cnt+8);
    }
    virtual void initShape(OpenGLContext& context, float cof1, float cof2, float cof3) = 0;
    void deleteShape(OpenGLContext& context)
    {
        context.extensions.glDeleteVertexArrays(1, &shapeVAO);
        context.extensions.glDeleteBuffers(1, &shapeVBO);
        context.extensions.glDeleteBuffers(1, &shapeEBO);
    }
    GLuint shapeVAO = 0;
    int vertexSize;
protected:
    Array<Vertex> vertices;
    Array<uint32> indecies;
    GLuint shapeVBO, shapeEBO;
    Vector getAED(Vector pos)
    {
        float dis = pos.length();
        if(pos.z == 0)
            pos.z = 1;
        if(dis == 0)
            dis = 1;
        float azi = std::atan(pos.x/pos.z);
        float elv = std::asin(pos.y/dis);
       if(pos.z<0)
       {
           if(pos.x<0)
           {
               azi-=M_PI;
           }
           else
           {
               azi+=M_PI;
           }
       }
       return Vector(azi, elv, dis);
    }
    void putData(OpenGLContext& context, Array<Vertex>& vertices, Array<uint32>& indecies)
    {
        context.extensions.glBindBuffer (GL_ARRAY_BUFFER, shapeVBO);
        context.extensions.glBufferData (GL_ARRAY_BUFFER,
                                         static_cast<GLsizeiptr> (static_cast<size_t> (vertices.size()) * sizeof (Vertex)),
                                         vertices.getRawDataPointer(), GL_STATIC_DRAW);
        
        // These will call OpenGL routines to create a buffer and store the indices in it
        context.extensions.glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, shapeEBO);
        context.extensions.glBufferData (GL_ELEMENT_ARRAY_BUFFER,
                                         static_cast<GLsizeiptr> (static_cast<size_t> (indecies.size()) * sizeof (juce::uint32)),
                                         indecies.getRawDataPointer(), GL_STATIC_DRAW);
    }
    void enableVertexArray(OpenGLContext& context)
    {
        context.extensions.glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), 0);
        context.extensions.glEnableVertexAttribArray (0);

        context.extensions.glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 3));
        context.extensions.glEnableVertexAttribArray (1);
        
        context.extensions.glVertexAttribPointer (2, 4, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 6));
        context.extensions.glEnableVertexAttribArray (2);
        
        context.extensions.glVertexAttribPointer (3, 2, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 10));
        context.extensions.glEnableVertexAttribArray (3);
    }
    void initVertexArray(OpenGLContext& context)
    {
        context.extensions.glGenVertexArrays(1, &shapeVAO);
        context.extensions.glGenBuffers (1, &shapeVBO);
        context.extensions.glGenBuffers (1, &shapeEBO);
        context.extensions.glBindVertexArray(shapeVAO);
        putData(context, vertices, indecies);
        enableVertexArray(context);
        context.extensions.glBindVertexArray(0);
        vertexSize = indecies.size();
    }
};

struct WavefrontVertex : ShapeVertex
{
    WavefrontVertex(WavefrontObjFile::Mesh& m, Colour c) : mesh(m), colour(c)
    {
    }
    void createVertexListFromMesh (const WavefrontObjFile::Mesh& mesh)
    {
        const float scale = 0.05f;
        WavefrontObjFile::TextureCoord defaultTexCoord = { 0.5f, 0.5f };
        WavefrontObjFile::Vertex defaultNormal = { 0.5f, 0.5f, 0.5f };
        //Colour colour = Colours::lightsalmon;
        for (int i = 0; i < mesh.vertices.size(); ++i)
        {
            int triangle_index = i/3;
            int idx;
            Vector one;
            Vector two;
            Vector three;
            Vector norm;
            idx = triangle_index * 3 + i%3;
            const WavefrontObjFile::Vertex& v1 = mesh.vertices.getReference (idx);
            idx = triangle_index * 3 + (i+1)%3;
            const WavefrontObjFile::Vertex& v2 = mesh.vertices.getReference (idx>=mesh.vertices.size()?(idx-1):idx);
            idx = triangle_index * 3 + (i+2)%3;
            const WavefrontObjFile::Vertex& v3 = mesh.vertices.getReference (idx>=mesh.vertices.size()?(idx-2):idx);
            one = Vector(v1.x, v1.y, v1.z);
            two = Vector(v2.x, v2.y, v2.z);
            three = Vector(v3.x, v3.y, v3.z);
            norm = (three-one) ^ (two-one);
            if((norm+one).length()<one.length())
                norm = -norm;
            defaultNormal = {norm.x, norm.y, norm.z};
            const WavefrontObjFile::Vertex& n
            = i < mesh.normals.size() ? mesh.normals.getReference (i) : defaultNormal;
            
            const WavefrontObjFile::TextureCoord& tc
            = i < mesh.textureCoords.size() ? mesh.textureCoords.getReference (i) : defaultTexCoord;
            
            Vertex vert =
            {
                { scale * v1.x, scale * v1.y, scale * v1.z, },
                { scale * n.x, scale * n.y, scale * n.z, },
                { colour.getFloatRed(), colour.getFloatGreen(), colour.getFloatBlue(), colour.getFloatAlpha() },
                { tc.x, tc.y }
            };
            vertices.add (vert);
        }
        indecies = mesh.indices;
    }
    void initShape(OpenGLContext& context, float cof1 = 0, float cof2 = 0, float cof3 = 0) override
    {
        //WavefrontObjFile::Mesh& mesh = shapeFile.modelObjects.getUnchecked(2)->mesh;
        createVertexListFromMesh(mesh);
        initVertexArray(context);
    }
    WavefrontObjFile::Mesh& mesh;
    Colour colour;
};
struct CubeVertex : ShapeVertex
{
    void initShape(OpenGLContext& context, float cof1 = 0, float cof2 = 0, float cof3 = 0) override
    {
        Vertex vert;
        Colour g = Colour(GRID_LINE);
        int cnt = 0;
        vert = {{1, 1, -1},{0, 0, -1},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5, 0.5}};
        vertices.add(vert);
        vert = {{1, -1, -1},{0, 0, -1},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5, 0.5}};
        vertices.add(vert);
        vert = {{-1, -1, -1},{0, 0, -1},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5, 0.5}};
        vertices.add(vert);
        indecies.add(cnt++);
        indecies.add(cnt++);
        indecies.add(cnt++);
        vert = {{-1, -1, -1},{0, 0, -1},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5, 0.5}};
        vertices.add(vert);
        vert = {{-1, 1, -1},{0, 0, -1},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5, 0.5}};
        vertices.add(vert);
        vert = {{1, 1, -1},{0, 0, -1},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5, 0.5}};
        vertices.add(vert);
        indecies.add(cnt++);
        indecies.add(cnt++);
        indecies.add(cnt++);
        
        vert = {{1, 1, 1},{0, 0, 1},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5, 0.5}};
        vertices.add(vert);
        vert = {{1, -1, 1},{0, 0, 1},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5, 0.5}};
        vertices.add(vert);
        vert = {{-1, -1, 1},{0, 0, 1},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5, 0.5}};
        vertices.add(vert);
        indecies.add(cnt++);
        indecies.add(cnt++);
        indecies.add(cnt++);
        
        vert = {{-1, -1, 1},{0, 0, 1},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5, 0.5}};
        vertices.add(vert);
        vert = {{-1, 1, 1},{0, 0, 1},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5, 0.5}};
        vertices.add(vert);
        vert = {{1, 1, 1},{0, 0, 1},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5, 0.5}};
        vertices.add(vert);
        indecies.add(cnt++);
        indecies.add(cnt++);
        indecies.add(cnt++);
        
        vert = {{1, -1, 1},{0, -1, 0},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5, 0.5}};
        vertices.add(vert);
        vert = {{1, -1, -1},{0, -1, 0},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5, 0.5}};
        vertices.add(vert);
        vert = {{-1, -1, -1},{0, -1, 0},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5, 0.5}};
        vertices.add(vert);
        indecies.add(cnt++);
        indecies.add(cnt++);
        indecies.add(cnt++);
        
        vert = {{-1, -1, -1},{0, -1, 0},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5, 0.5}};
        vertices.add(vert);
        vert = {{-1, -1, 1},{0, -1, 0},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5, 0.5}};
        vertices.add(vert);
        vert = {{1, -1, 1},{0, -1, 0},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5, 0.5}};
        vertices.add(vert);
        indecies.add(cnt++);
        indecies.add(cnt++);
        indecies.add(cnt++);
        
        vert = {{1, 1, 1},{0, 1, 0},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5, 0.5}};
        vertices.add(vert);
        vert = {{1, 1, -1},{0, 1, 0},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5, 0.5}};
        vertices.add(vert);
        vert = {{-1, 1, -1},{0, 1, 0},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5, 0.5}};
        vertices.add(vert);
        indecies.add(cnt++);
        indecies.add(cnt++);
        indecies.add(cnt++);
        
        vert = {{-1, 1, -1},{0, 1, 0},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5, 0.5}};
        vertices.add(vert);
        vert = {{-1, 1, 1},{0, 1, 0},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5, 0.5}};
        vertices.add(vert);
        vert = {{1, 1, 1},{0, 1, 0},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5, 0.5}};
        vertices.add(vert);
        indecies.add(cnt++);
        indecies.add(cnt++);
        indecies.add(cnt++);
        
        vert = {{1, 1, 1},{1, 0, 0},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5, 0.5}};
        vertices.add(vert);
        vert = {{1, 1, -1},{1, 0, 0},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5, 0.5}};
        vertices.add(vert);
        vert = {{1, -1, -1},{1, 0, 0},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5, 0.5}};
        vertices.add(vert);
        indecies.add(cnt++);
        indecies.add(cnt++);
        indecies.add(cnt++);
        
        vert = {{1, -1, -1},{1, 0, 0},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5, 0.5}};
        vertices.add(vert);
        vert = {{1, -1, 1},{1, 0, 0},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5, 0.5}};
        vertices.add(vert);
        vert = {{1, 1, 1},{1, 0, 0},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5, 0.5}};
        vertices.add(vert);
        indecies.add(cnt++);
        indecies.add(cnt++);
        indecies.add(cnt++);
        
        vert = {{-1, 1, 1},{-1, 0, 0},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5, 0.5}};
        vertices.add(vert);
        vert = {{-1, 1, -1},{-1, 0, 0},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5, 0.5}};
        vertices.add(vert);
        vert = {{-1, -1, -1},{-1, 0, 0},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5, 0.5}};
        vertices.add(vert);
        indecies.add(cnt++);
        indecies.add(cnt++);
        indecies.add(cnt++);
        
        vert = {{-1, -1, -1},{-1, 0, 0},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5, 0.5}};
        vertices.add(vert);
        vert = {{-1, -1, 1},{-1, 0, 0},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5, 0.5}};
        vertices.add(vert);
        vert = {{-1, 1, 1},{-1, 0, 0},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5, 0.5}};
        vertices.add(vert);
        indecies.add(cnt++);
        indecies.add(cnt++);
        indecies.add(cnt++);
        
        initVertexArray(context);
    }
};
struct FanVertex : ShapeVertex
{
    void initShape(OpenGLContext& context, float horizonAngle, float cof2 = 0, float cof3 = 0) override
    {
        Vertex vert;
        Colour g = Colour(SHADOW_COLOR);
        int i;
        float ang;
        float x, y;
        indecies.add(0);
        vert = {{0,0,0},{0,1,0},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5,0.5}};
        vertices.add(vert);
        for(i=0 ; i<=div ; i++)
        {
            ang = i*horizonAngle/div - horizonAngle/2;
            x = std::sin(ang);
            y = std::cos(ang);
            vert = {{x,0,y},{0,1,0},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5,0.5}};
            vertices.add(vert);
            indecies.add(i+1);
        }
        initVertexArray(context);
    }
    void setAngle(OpenGLContext& context, float horizonAngle)
    {
        Vertex vert;
        Colour g = Colour(SHADOW_COLOR);
        int i;
        float x, y;
        float ang;
        context.extensions.glBindVertexArray(shapeVAO);
        context.extensions.glBindBuffer(GL_ARRAY_BUFFER, shapeVBO);
        for(i=0 ; i<=div ; i++)
        {
            ang = i*horizonAngle/div - horizonAngle/2;
            x = std::sin(ang);
            y = std::cos(ang);
            vert = {{x,0,y},{0,1,0},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5,0.5}};
            context.extensions.glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertex)*(i+1), sizeof(Vertex), &vert);
        }
        context.extensions.glBindVertexArray(0);
    }
    int div = 36;
};
struct ShadowVertex : ShapeVertex
{
    void initShape(OpenGLContext& context, float cof1 = 0, float cof2 = 0, float cof3 = 0) override
    {
        Vertex vert;
        Colour g = Colour(SHADOW_COLOR);
        int i;
        int div = 72;
        float ang;
        float x, y;
        indecies.add(0);
        vert = {{0,0,0},{0,1,0},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5,0.5}};
        vertices.add(vert);
        for(i=0 ; i<=div ; i++)
        {
            ang = i*2*M_PI/div;
            x = std::cos(ang);
            y = std::sin(ang);
            vert = {{x,0,y},{0,1,0},{g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},{0.5,0.5}};
            vertices.add(vert);
            indecies.add(i+1);
        }
        initVertexArray(context);
    }
};
struct CursorVertex : ShapeVertex
{
    void initShape(OpenGLContext& context, float cof1 = 0, float cof2 = 0, float cof3 = 0) override
    {
        Vertex vert;
        Colour g = Colour(GRID_LINE);
        vert = {
            {0, 0, 0},
            {0, 0, 1},
            {g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},
            {0.5, 0.5}
        };
        vertices.add(vert);
        indecies.add(0);
        vert = {
            {200, 0, 200},
            {0, 0, 1},
            {g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},
            {0.5, 0.5}
        };
        vertices.add(vert);
        indecies.add(1);
        vert = {
            {200, 200, 200},
            {0, 0, 1},
            {g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},
            {0.5, 0.5}
        };
        vertices.add(vert);
        indecies.add(2);
        initVertexArray(context);    }
    void setPoint(OpenGLContext& context, Vector p)
    {
        Vertex ground;
        Vertex pos;
        Colour g = Colour(GRID_LINE);
        ground = {
            {p.x, 0, p.z},
            {0, 0, 1},
            {g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},
            {0.5, 0.5}
        };
        pos = {
            {p.x, p.y, p.z},
            {0, 0, 1},
            {g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},
            {0.5, 0.5}
        };
        context.extensions.glBindVertexArray(shapeVAO);
        context.extensions.glBindBuffer(GL_ARRAY_BUFFER, shapeVBO);
        context.extensions.glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertex), sizeof(Vertex), &ground);
        context.extensions.glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertex)*2, sizeof(Vertex), &pos);
    }
};
struct LineVertex : ShapeVertex
{
    void initShape(OpenGLContext& context, float cof1 = 1, float cof2 = 0, float cof3 = 0) override
    {
        
    }
    void initShape(OpenGLContext& context, float thickness, Colour lineColour, Vector p1, Vector p2 = Vector(0,0,0))
    {
        buildStraightLine(p1, p2, thickness, lineColour);
        initVertexArray(context);
    }
};
struct SphereGridVertex : ShapeVertex
{
    void initShape(OpenGLContext& context, float cof1 = 1, float cof2 = 6, float cof3 = 0) override
    {
        Vertex vert;
        Colour g = Colour(GRID_LINE);
        int i, j;
        const int n = cof2;
        float idx[100][50] = {0,};
        float diff = M_PI/n;
        float azi, elv;
        int cnt=0;
        float u;
        float v;
        for(i = 0 ; i<=2*n ; i++)
        {
            for(j=0 ; j<=n ; j++)
            {
                elv = M_PI_2 - diff * (float)j;
                azi = M_PI - diff * (float)i;
                
                u = static_cast<float>(j)/n;
                v = static_cast<float>(i)/(2*n);
                vert = {
                    {cof1*std::cos(elv) * std::sin(azi), cof1*std::sin(elv), cof1*std::cos(elv)*std::cos(azi)},
                    {std::cos(elv) * std::sin(azi), std::cos(elv), std::cos(elv)*std::cos(azi)},
                    {g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},
                    {v, u}
                };
                vertices.add(vert);
                idx[i][j] = cnt++;
            }
        }
        for(j=0 ; j<n ; j++)
        {
            for(i=0 ; i<2*n ; i++)
            {
                indecies.add(idx[i][j]);
                indecies.add(idx[i][j+1]);
                indecies.add(idx[i][j]);
                indecies.add(idx[i+1][j]);
            }
        }
        for(j=0 ; j<n ; j++)
        {
            indecies.add(idx[i][j]);
            indecies.add(idx[i][j+1]);
        }
        for(i=0 ; i<2*n ; i++)
        {
            indecies.add(idx[i][j]);
            indecies.add(idx[i+1][j]);
        }
        initVertexArray(context);
    }
};
struct SphereVertex : ShapeVertex
{
    void initShape(OpenGLContext& context, float cof1 = 1, float cof2 = 0, float cof3 = 0) override
    {
        Vertex vert;
        Colour g = Colour(GRID_LINE);
        int i, j;
        const int n = 36;
        float idx[n*2+1][n*2] = {0,};
        float diff = M_PI/n;
        float azi, elv;
        int cnt=0;
        float u;
        float v;
        for(i = 0 ; i<=2*n ; i++)
        {
            for(j=0 ; j<=n ; j++)
            {
                elv = M_PI_2 - diff * (float)j;
                azi = -M_PI + diff * (float)i;
                
                u = static_cast<float>(j)/n;
                v = static_cast<float>(i)/(2*n);
                vert = {
                    {cof1*std::cos(elv) * std::sin(azi), cof1*std::sin(elv), cof1*std::cos(elv)*std::cos(azi)},
                    {std::cos(elv) * std::sin(azi), std::sin(elv), std::cos(elv)*std::cos(azi)},
                    {1.0, 1.0, 1.0, 1.0},
                    {v, u}
                };
                vertices.add(vert);
                idx[i][j] = cnt++;
            }
        }
        for(j=0 ; j<n ; j++)
        {
            for(i=0 ; i<2*n ; i++)
            {
                indecies.add(idx[i][j]);
                indecies.add(idx[i][j+1]);
                indecies.add(idx[i+1][j]);
                indecies.add(idx[i][j+1]);
                indecies.add(idx[i+1][j+1]);
                indecies.add(idx[i+1][j]);
            }
        }
        initVertexArray(context);
    }
};
struct AmbisonicPointsVertex : InstancedShapeVertex
{
    void initShape(OpenGLContext& context, float cof1 = 1, float cof2 = 0, float cof3 = 0) override
    {
        Vertex vert;
        Colour g = Colour(GRID_LINE);
        
        int vertexCount = 60;
        float radius = 1.0f;
        float center_x = 0.0f;
        float center_y = 0.0f;
        
        // Create a buffer for vertex data
        float buffer[120]; // (x,y) for each vertex
        int idx = 0;
        
        // Center vertex for triangle fan
        buffer[idx++] = center_x;
        buffer[idx++] = center_y;
        
        // Outer vertices of the circle
        int outerVertexCount = vertexCount - 1;
        
        for (int i = 0; i < outerVertexCount; ++i)
        {
            float percent = (i / (float) (outerVertexCount - 1));
            float rad = percent * 2*M_PI;
            
            //Vertex position
            float outer_x = center_x + radius * cos(rad);
            float outer_y = center_y + radius * sin(rad);
            
            buffer[idx++] = outer_x;
            buffer[idx++] = outer_y;
        }
        
        for(int i = 0; i < 60; i++)
        {
            vert = {
                {buffer[i*2],buffer[i*2+1],0.0f},
                {0.0f, 0.0f, 0.0f},
                {1.0, 1.0, 1.0, 1.0},
                {1.0, 1.0}
            };
            
            vertices.add(vert);
        }
        for(int idx = 0; idx<60; idx++)
        {
            indecies.add(0);
            indecies.add(idx+1);
            indecies.add(idx+2);
        }
        int n = 72;
        int cnt = 0;
        float diff = 2*M_PI/n;
        Matrix t;
        for(int i=1 ; i<n ; i++, cnt++)
        {
            t = getModelMatrixAtPosition(M_PI_2, (i)*diff, 0);
            matrices.add(t);
        }
        for(int i=1 ; i<n ; i++, cnt++)
        {
            t = getModelMatrixAtPosition((i)*diff, 0, 0);
            matrices.add(t);
        }
        instanceSize = cnt;
        initVertexArray(context);
    }
};

struct ObjectCircleVertex : ShapeVertex
{
    void initShape(OpenGLContext& context, float cof1 = 1, float cof2 = 0, float cof3 = 0) override
    {
        Vertex vert;
        Colour g = Colour(GRID_LINE);
        
        int vertexCount = 60;
        float radius = 1.0f;
        float center_x = 0.0f;
        float center_y = 0.0f;
        
        // Create a buffer for vertex data
        float buffer[120]; // (x,y) for each vertex
        int idx = 0;
        
        // Center vertex for triangle fan
        buffer[idx++] = center_x;
        buffer[idx++] = center_y;
        
        // Outer vertices of the circle
        int outerVertexCount = vertexCount-1;
        
        for (int i = 0; i < outerVertexCount; ++i)
        {
            float percent = (i / (float) (outerVertexCount-1));
            float rad = percent * 2*M_PI;
            
            //Vertex position
            float outer_x = center_x + radius * cos(rad);
            float outer_y = center_y + radius * sin(rad);
            
            buffer[idx++] = outer_x;
            buffer[idx++] = outer_y;
        }
        
        for(int i = 0; i < 60; i++)
        {
            vert = {
                {buffer[i*2],buffer[i*2+1],0.0f},
                {0.0f, 0.0f, 0.0f},
                {1.0, 1.0, 1.0, 1.0},
                {1.0, 1.0}
            };
            
            vertices.add(vert);
        }
        for(int idx = 0; idx<60; idx++)
        {
            indecies.add(0);
            indecies.add(idx+1);
            indecies.add(idx+2);
        }
        
        initVertexArray(context);
    }
};

struct EllipseVertex : ShapeVertex
{
    void initShape(OpenGLContext& context, float cof1 = 1, float cof2 = 0, float cof3 = 0) override
    {
        Colour g = Colour(GRID_LINE);
        int n = 180;
        int i;
        float rad = cof1;
        for(i=0 ; i<n ; i++)
        {
            buildArcLine(Vector(2*M_PI*(float)i/n, 0, rad), Vector(2*M_PI*(float)(i+1)/n, 0, rad), 1);
        }
        initVertexArray(context);
    }
};
struct UVSphereVertex : ShapeVertex
{
    void initShape(OpenGLContext& context, float cof1 = 1, float cof2 =0, float cof3 = 0) override
    {
        Colour g = Colour(GRID_LINE);
        int n = 180;
        int m = 12;
        int i, j;
        float rad = cof1;
        for(j=1 ; j<18 ; j++)
        {
            for(i=0 ; i<n ; i++)
            {
                buildArcLine(Vector(2*M_PI*(float)i/n, -M_PI_2 + M_PI * (float)j/m, rad), Vector(2*M_PI*(float)(i+1)/n, -M_PI_2 + M_PI * (float)j/m, rad), 1);
            }
        }
        for(j=0 ; j<36 ; j++)
        {
            for(i=0 ; i<n ; i++)
            {
                buildArcLine(Vector(-M_PI_2 + M_PI * (float)j/m, 2*M_PI*(float)i/n, rad), Vector(-M_PI_2 + M_PI * (float)j/m, 2*M_PI*(float)(i+1)/n, rad), 1);
            }
        }
        initVertexArray(context);
    }
};
struct FlatEllipseVertex : ShapeVertex
{
    void initShape(OpenGLContext& context, float cof1 = 1, float cof2 = 0, float cof3 = 0) override
    {
        Vertex vert;
        Colour g = Colour(GRID_LINE);
        int n=36;
        int i;
        float rad = 1.0;
        float ang;
        float thick = cof1;
        for(i=1 ; i<=n+1 ; i++)
        {
            ang = 2*M_PI*(float)(i-1)/n;
            vert = {
                {rad * std::cos(ang),rad * std::sin(ang), 0},
                {std::sin(ang), std::cos(ang),0},
                {g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},
                {0.5,0.5}};
            vertices.add(vert);
            vert = {
                {(rad-thick) * std::cos(ang),(rad-thick) * std::sin(ang), 0},
                {std::sin(ang), std::cos(ang),0},
                {g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},
                {0.5,0.5}};
            vertices.add(vert);
            indecies.add(i*2-2);
            indecies.add(i*2-1);
            indecies.add(i*2);
            indecies.add(i*2-1);
            indecies.add(i*2);
            indecies.add(i*2+1);
        }
        initVertexArray(context);
    }
};
struct RectangleVertex : ShapeVertex
{
    void initShape(OpenGLContext& context, float x=1, float y = 1, float cof3 = 0) override
    {
        Vertex vert;
        Colour g = Colour(GRID_LINE);
        vert = {
            {-x,y,0},
            {0,0,1},
            {g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},
            {0.0,1.0}};
        vertices.add(vert);
        vert = {
            {x,y,0},
            {0,0,1},
            {g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},
            {1.0,1.0}};
        vertices.add(vert);
        vert = {
            {x,-y,0},
            {0,0,1},
            {g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},
            {1.0,0.0}};
        vertices.add(vert);
        vert = {
            {-x,-y,0},
            {0,0,1},
            {g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},
            {0.0,0.0}};
        vertices.add(vert);
        indecies.add(0);
        indecies.add(1);
        indecies.add(2);
        indecies.add(0);
        indecies.add(2);
        indecies.add(3);
        initVertexArray(context);
    }
};
struct FlatRectangleVertex : ShapeVertex
{
    void initShape(OpenGLContext& context, float cof1=1, float cof2 = 1, float cof3 = 0) override
    {
        Vertex vert;
        float lineDiff = 0.82f;
        Colour g = Colour(0xFFFFFFFF);
        vert = {
            {-cof1,cof2,0},
            {0,0,1},
            {g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},
            {0.0,1.0}};
        vertices.add(vert);
        vert = {
            {cof1,cof2,0},
            {0,0,1},
            {g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},
            {1.0,1.0}};
        vertices.add(vert);
        vert = {
            {cof1,-cof2,0},
            {0,0,1},
            {g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},
            {1.0,0.0}};
        vertices.add(vert);
        vert = {
            {-cof1,-cof2,0},
            {0,0,1},
            {g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},
            {0.0,0.0}};
        vertices.add(vert);
        
        vert = {
            {-cof1*lineDiff,cof2*lineDiff,0},
            {0,0,1},
            {g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},
            {0.0,1.0}};
        vertices.add(vert);
        vert = {
            {cof1*lineDiff,cof2*lineDiff,0},
            {0,0,1},
            {g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},
            {1.0,1.0}};
        vertices.add(vert);
        vert = {
            {cof1*lineDiff,-cof2*lineDiff,0},
            {0,0,1},
            {g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},
            {1.0,0.0}};
        vertices.add(vert);
        vert = {
            {-cof1*lineDiff,-cof2*lineDiff,0},
            {0,0,1},
            {g.getFloatRed(), g.getFloatGreen(), g.getFloatBlue(), g.getFloatAlpha()},
            {0.0,0.0}};
        
        vertices.add(vert);
        
        indecies.add(0);
        indecies.add(4);
        indecies.add(3);
        
        indecies.add(0);
        indecies.add(1);
        indecies.add(4);
        
        indecies.add(4);
        indecies.add(1);
        indecies.add(5);
        
        indecies.add(3);
        indecies.add(4);
        indecies.add(7);
        
        indecies.add(3);
        indecies.add(7);
        indecies.add(2);
        
        indecies.add(2);
        indecies.add(7);
        indecies.add(6);
        
        indecies.add(5);
        indecies.add(1);
        indecies.add(2);
        
        indecies.add(5);
        indecies.add(2);
        indecies.add(6);
        
        initVertexArray(context);
    }
};
#endif  // SHAPEVERTEX_H_INCLUDED
