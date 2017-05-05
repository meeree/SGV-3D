#ifndef __SCENE_GRAPH_H__
#define __SCENE_GRAPH_H__

#include <GL/glew.h>
#include <GL/gl.h>
#include <vector>
#include <glm/glm.hpp>

struct Vertex 
{
    glm::vec3 pos, normal;
};

struct VboIndexer 
{
    GLuint first; 
    GLsizei count;
    GLenum mode;
    bool fPureVertexDraw;
};

class Mesh 
{
private:
    bool fDrawQuery;
    std::vector<Vertex> mVertices;
    VboIndexer mIndexer;
    
    //TO-DO: Add some methods to query for info 
    static struct 
    {
        glm::vec4 min, max;
    } mInfo;
public:
    Mesh ();
    inline bool queryDraw () const {return fDrawQuery;}
    inline bool toggleDraw () {return (fDrawQuery = !fDrawQuery);}
    inline void setVertices (std::vector<Vertex> const& newVertices) 
        {mVertices = newVertices;}
    void setMinMax ();
    inline VboIndexer const& getIndexer() const {return mIndexer;}
};

//TO-DO: Consider adding a method to get the min/max of 
//a group of multiple objects
//
//Diagram:
//             NODE/BRANCH NODE
//                    |
//              TRANSFORM NODE
//                    |
//            MESH/STANDARD NODE

class Node
{
protected:
    enum eType 
    {
        TRANSFORM, PURE_BRANCH
    } mType;
    std::vector<Node*> mChildren;
    Node* mParent;
    
    virtual void intermediate (glm::mat4x4&, GLuint const&) const {};
public:
    Node () = delete; //Accounted for below 
    Node (eType const& type=PURE_BRANCH);
    inline Node* getChild (unsigned const& index) const
        {return mChildren[index];}

    void render (glm::mat4x4&, GLuint const&) const;
//    void addNode (SG*);
//    void deleteNode (unsigned const&);
//    //TO-DO: ADD SORTING 
};

class TransformNode : public Node 
{
protected:
    enum eTransformType 
    {
        PURE, MESH
    } mTransType;
    glm::mat4x4 mModMat;

    virtual void intermediate (glm::mat4x4&, GLuint const&) const override;
public:
    TransformNode () = default;
    TransformNode (glm::mat4x4 const& modMat,
            eTransformType const& type=PURE);
};

class StandardNode : public TransformNode 
{
protected:
    Mesh mMesh;

    virtual void intermediate (glm::mat4x4&, GLuint const&) const override;
public:
    StandardNode () = delete; //Accounted for below
    StandardNode (Mesh const& mesh={}, 
            glm::mat4x4 const& modMat=glm::mat4x4(1.0f));
};

class SceneGraph
{
private:
    Node* mRoot;
    GLuint mVbo;
    std::vector<Vertex> mVertices;
    std::vector<unsigned> mIndices;
public:
    //FILL ME 
};

#endif //__SCENE_GRAPH_H__
