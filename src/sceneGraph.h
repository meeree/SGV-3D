#ifndef __SCENE_GRAPH_H__
#define __SCENE_GRAPH_H__

#include <GL/glew.h>
#include <GL/gl.h>
#include <vector>
#include <glm/glm.hpp>
#include <algorithm>

//TO-DO: Add overflow test in mergeGraph and other potentially big allocation methods
//TO-DO: Add line drawing
//TO-DO: Add methods to check if node data is bound to the scene graph the node is trying to join

struct Vertex 
{
    glm::vec3 pos;
    glm::vec3 normal;
};

struct MeshQuery 
{
    glm::vec4 min, max; //Min and max local to the mesh
    glm::vec3 center;
    bool fDrawQuery, fPureVertexDraw;
};

class Mesh 
{
protected:
    GLenum mMode;
    MeshQuery mQuery;
    //We use thse more general methods that takes in iterators 
    //because it is necessary for the inherited class GraphMesh 
    void setMinMax (std::vector<Vertex>::const_iterator, std::vector<Vertex>::const_iterator);
    void setCenter (std::vector<Vertex>::const_iterator, std::vector<Vertex>::const_iterator);
public:
    std::vector<Vertex> mVertices;
    std::vector<GLuint> mIndices;

    Mesh (GLenum const& mode=GL_TRIANGLES);
    Mesh (std::vector<Vertex> const&, GLenum const& mode=GL_TRIANGLES);
    Mesh (std::vector<Vertex> const&, std::vector<GLuint> const&, GLenum const& mode=GL_TRIANGLES);
    //TO-DO: Add move versions
//    Mesh (std::vector<Vertex>&&, std::vector<GLuint>&&);

    //Getter/setter functions 
    inline MeshQuery getQuery () const {return mQuery;}
    inline void toggleDraw () {mQuery.fDrawQuery = !mQuery.fDrawQuery;}
    void setMinMax () {setMinMax(mVertices.begin(), mVertices.end());}
    void setCenter () {setCenter(mVertices.begin(), mVertices.end());};
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
    GraphMesh (std::vector<Vertex> const&, Indexer const&);
    GraphMesh (std::vector<Vertex> const&, std::vector<GLuint> const&, Indexer const&, Indexer const&);

    //Getter/setter functions 
    inline Indexer getSigIndexer () const 
        {return mQuery.fPureVertexDraw?mVboIndexer:mEboIndexer;}
    inline void setSigIndexer (Indexer const& sigIndexer)
        {(mQuery.fPureVertexDraw?mVboIndexer:mEboIndexer) = sigIndexer;} 
    inline Indexer getVboIndexer () const {return mVboIndexer;}
    inline Indexer getEboIndexer () const {return mEboIndexer;}
    inline void setIndexers (Indexer const& vboIndexer, Indexer const& eboIndexer)
        {mVboIndexer = vboIndexer;
         if (mQuery.fPureVertexDraw)
             mEboIndexer = {(GLuint)-1, -1};
         else 
             mEboIndexer = eboIndexer;}
};

class Node
{
protected:
    enum eType 
    {
        OBJECT=0, TRANSFORM=1, DYNAMIC_TRANSFORM=2, PURE_BRANCH=3
    } mType;
    std::vector<Node*> mChildren;
    Node (eType const&);
public:
    Node ();
    virtual ~Node ();
    virtual void render (glm::mat4x4 const&, SceneGraph*);
    virtual void cut (std::vector<std::pair<Indexer,Indexer>>&) {}

    //Getter/setter functions 
    inline void addChild (Node* node) {mChildren.push_back(node);}
    inline void addChildren (std::vector<Node*> const& nodes)
        {mChildren.insert(mChildren.end(), nodes.begin(), nodes.end());}
    inline Node* getChild (unsigned const& index) const {return mChildren[index];}
    inline std::vector<Node*> getChildren () const {return mChildren;}
};

//class DynamicTransformNode : public Node 
//{
//protected:
//    glm::mat4x4 (*mTransFunc)(double const&);
//public:
//    DynamicTransformNode();
//    virtual void render (glm::mat4x4 const&, SceneGraph*) override;
//};
//
class TransformNode : public Node 
{
protected:
    glm::mat4x4 mModMat;
public:
    TransformNode ();
    TransformNode (glm::mat4x4 const& modMat);
    virtual void render (glm::mat4x4 const&, SceneGraph*) override;

    //Getter/setter functions 
    inline void transform (glm::mat4x4 const& transform) {mModMat = transform*mModMat;}
    inline void setTransform (glm::mat4x4 const& transform) {mModMat = transform;}
    inline glm::mat4x4 getTransform () const {return mModMat;}
};

class ObjectNode : public Node
{
protected:
    GraphMesh mGraphMesh;
public:
    ObjectNode ();
    ObjectNode (GraphMesh const&);
    //DEFINE ME
    virtual void render (glm::mat4x4 const&, SceneGraph*) override;
    virtual void cut (std::vector<std::pair<Indexer,Indexer>>& regions) final
        {regions.push_back({mGraphMesh.getVboIndexer(), mGraphMesh.getEboIndexer()});}

    //Getter/setter functions 
    inline GraphMesh getGraphMesh () const {return mGraphMesh;}
    inline void toggleDraw () {mGraphMesh.toggleDraw();}
};

struct GraphQuery
{
    bool fGlobalMinMax;
    glm::vec3 center;
    std::pair<GLint,glm::vec4> min, max; //Pairs of uniform location and value
    GLuint matLoc;
};

class SceneGraph
{
private:
    Node* mRoot;
    GLuint mVao, mVbo, mEbo;
    std::vector<Vertex> mVertices;
    std::vector<GLuint> mIndices;
    GraphQuery mQuery;

    void setGlobalMinMax (std::vector<Vertex>::const_iterator, std::vector<Vertex>::const_iterator);
    void setGlobalCenter (std::vector<Vertex>::const_iterator, std::vector<Vertex>::const_iterator);
public:
    SceneGraph () = delete;
    SceneGraph (GLchar const* names[3]);
    SceneGraph (Node*, GLuint const&, bool const& useGlobalMinMax, GLchar const* names[3]);
    ~SceneGraph() {/*delete(mRoot);*/}

    void render (glm::mat4x4);
    void mergeGraph (SceneGraph&, Node*); 
    void cut (Node*);

    GraphMesh bindMesh (Mesh&);
    Mesh unbindMesh (GraphMesh&);
    GraphMesh bindMesh (Mesh const&);
    Mesh unbindMesh (GraphMesh const&);

    void setGlobalMinMax () {setGlobalMinMax(mVertices.begin(), mVertices.end());}
    void setGlobalCenter () {setGlobalCenter(mVertices.begin(), mVertices.end());}

    //Getter/setter functions 
    inline void setRoot (Node* node) 
		{delete mRoot; mRoot = node;}
    inline Node* getRoot () const {return mRoot;}
    inline void toggleGlobalMinMax () 
		{mQuery.fGlobalMinMax = !mQuery.fGlobalMinMax;}
    inline GraphQuery getQuery () const {return mQuery;}
};

#endif //__SCENE_GRAPH_H__
