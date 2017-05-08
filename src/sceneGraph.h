#ifndef __SCENE_GRAPH_H__
#define __SCENE_GRAPH_H__

#include <GL/glew.h>
#include <GL/gl.h>
#include <vector>
#include <glm/glm.hpp>
#include <algorithm>

//TO-DO: Add overflow test in mergeGraph and other potentially big allocation methods
//TO-DO: Add methods to check if node data is bound to the scene graph the node is trying to join

struct Vertex 
{
    glm::vec3 pos, normal;
};

struct MeshQuery 
{
    glm::vec4 min, max; //Min and max local to the mesh
    bool fDrawQuery, fPureVertexDraw;
};

class Mesh 
{
protected:
    GLenum mMode;
    MeshQuery mInfo;
    //We use this more general method that takes in iterators 
    //because it is necessary for the inherited class GraphMesh 
    void setLocalMinMax (std::vector<Vertex>::const_iterator, std::vector<Vertex>::const_iterator);
public:
    std::vector<Vertex> mVertices;
    std::vector<unsigned> mIndices;

	Mesh () = delete; //Accounted for below
    Mesh (GLenum const& mode=GL_TRIANGLES);
    Mesh (std::vector<Vertex> const&, GLenum const& mode=GL_TRIANGLES);
    Mesh (std::vector<Vertex> const&, std::vector<unsigned> const&, GLenum const& mode=GL_TRIANGLES);
    //TO-DO: Add move versions
//    Mesh (std::vector<Vertex>&&, std::vector<unsigned>&&);
    inline MeshQuery getQuery () const {return mInfo;}
    inline bool toggleDraw () {return (mInfo.fDrawQuery = !mInfo.fDrawQuery);}
    void setLocalMinMax () 
        {setLocalMinMax(mVertices.begin(), mVertices.end());}
	inline GLenum getMode () const {return mMode;}
};

class SceneGraph;

struct Indexer 
{
    GLuint first; 
    GLsizei count;
};

class GraphMesh : public Mesh
{
private:
    Indexer mVboIndexer, mEboIndexer;
public:
    GraphMesh () = default;
    inline Indexer getSigIndexer () const 
        {return mInfo.fPureVertexDraw?mVboIndexer:mEboIndexer;}
    inline Indexer getVboIndexer () const
        {return mVboIndexer;}
    inline Indexer getEboIndexer () const
        {return mEboIndexer;}
    inline void setIndexers (std::pair<Indexer, Indexer> const& indexers)
        {mVboIndexer = indexers.first; 
         mEboIndexer = indexers.second;}
    void setLocalMinMax (std::vector<Vertex> const& verts)
        {Mesh::setLocalMinMax(verts.begin()+mVboIndexer.first, verts.begin()+mVboIndexer.first+mVboIndexer.count);}
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

struct GraphQuery;

class Node
{
protected:
    enum eType 
    {
        STANDARD=0, TRANSFORM=1, PURE_BRANCH=2
    } mType;
    std::vector<Node*> mChildren;
    Node* mParent;
    virtual void intermediate (glm::mat4x4&, GraphQuery const&) const {};
public:
    Node () = delete; //Accounted for below 
    Node (eType const& type=PURE_BRANCH);
    inline Node* getChild (unsigned const& index) const
        {return mChildren[index];}
    void render (glm::mat4x4&, GraphQuery const&) const;
    virtual void setLocalMinMax (
            std::vector<Vertex> const&) {}
    inline void addChild (Node* node) 
        {mChildren.push_back(node);}
    virtual void cut (std::vector<std::pair<Indexer,Indexer>>&) {}
    virtual ~Node ();
};

class TransformNode : public Node 
{
protected:
    glm::mat4x4 mModMat;
    virtual void intermediate (glm::mat4x4&, GraphQuery const&) const override;
public:
    TransformNode () = default;
    TransformNode (glm::mat4x4 const& modMat, 
        eType const& type=TRANSFORM);
//    void render(glm::mat4x4& modMat, GraphQuery const& query) const 
//        {Node::render(modMat, query);}
};

class StandardNode : public TransformNode 
{
protected:
    GraphMesh mGraphMesh;
    virtual void intermediate (glm::mat4x4&, GraphQuery const&) const override;
public:
    StandardNode () = default;
    StandardNode (GraphMesh const& mesh, 
        glm::mat4x4 const& modMat=glm::mat4x4(1.0f));
    inline GraphMesh getGraphMesh () const {return mGraphMesh;}
    Mesh getMesh () const; //Extract a stand-alone mesh from scene graph 
    virtual void setLocalMinMax (std::vector<Vertex> const& verts) override
        {mGraphMesh.setLocalMinMax(verts);}
    virtual void cut (std::vector<std::pair<Indexer,Indexer>>& regions) final
        {regions.push_back({mGraphMesh.getVboIndexer(), mGraphMesh.getEboIndexer()});}
//    void render(glm::mat4x4& modMat, GraphQuery const& query) const 
//        {TransformNode::render(modMat, query);}
    inline void toggleDraw () {mGraphMesh.toggleDraw();}
};

struct GraphQuery
{
    bool fGlobalMinMax;
    std::pair<GLuint,glm::vec4> min, max;
    GLuint matLoc;
};

#include <iostream>
#include <typeinfo>
class SceneGraph
{
private:
    Node* mRoot;
    GLuint mVbo, mEbo;
    std::vector<Vertex> mVertices;
    std::vector<unsigned> mIndices;
    GraphQuery mInfo;
public:
    SceneGraph () = default;
    SceneGraph (Node*, GLuint const&,
            bool const& useGlobalMinMax=true, GLchar const* names[3] = (GLchar const* [3]){"min","max","modMat"});
    void setGlobalMinMax ();
    void setLocalMinMax () {mRoot->setLocalMinMax(mVertices);}
    void render (glm::mat4x4& modMat) const;
    void mergeGraph (SceneGraph&, Node*); 
    GraphMesh bindMesh (Mesh&);
    Mesh unbindMesh (GraphMesh&);
    //This should be expanded 
    //FIX ME 
    inline void setRoot (Node* node) 
        {delete mRoot; mRoot = new StandardNode{*static_cast<StandardNode*>(node)};}
    void cut (Node*);
    ~SceneGraph() {delete(mRoot);}
};

#endif //__SCENE_GRAPH_H__
