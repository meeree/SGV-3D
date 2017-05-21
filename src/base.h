#ifndef __BASE_H__
#define __BASE_H__

#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex 
{
    glm::vec3 pos, normal;
};

struct Indexer 
{
    GLuint first; 
    GLsizei count;
    bool operator== (Indexer const& indexer) const
        {return first == indexer.first && count == indexer.count;}
};

struct LightGLContext //It is only necessary to store vao and shader for 
                      //ops. that don't require changing vertices/indices
{
    GLuint vao, shader;
};

struct GLContext : public LightGLContext //This is inherited to allow for casting
{
    GLuint vbo, ebo;
};

class GraphMesh
{
private:
    unsigned mContextIndex;
    Indexer mEboIndexer, mVboIndexer;
    GLenum mPrimType;
    bool fPureVertexDraw;
public:
    GraphMesh () = default;
    GraphMesh (GLContext const&, unsigned const&, std::vector<Vertex> const&, GLenum const& primType=GL_TRIANGLES);
    GraphMesh (GLContext const&, unsigned const&, std::vector<Vertex> const&, std::vector<GLuint> const&, GLenum const& primType=GL_TRIANGLES);

    //Getter/setter
    inline Indexer const& getSigIndexer () const {return fPureVertexDraw?mVboIndexer:mEboIndexer;}
    
    bool usesIndices () const {return !fPureVertexDraw;}
};

#endif //__BASE_H__
