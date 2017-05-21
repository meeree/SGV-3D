#ifndef __NODE_H__
#define __NODE_H__

#include <stack>
#include "base.h"

//TO-DO URGENT: add destructors

struct RenderContext
{
    GLfloat t;
    std::stack<glm::mat4x4> matStack;
    std::stack<GLContext> glContextStack;
};

class SceneTreeNode;

class Node 
{
public:
    enum eNodeType 
    {
        GROUP=0,LEAF=1 //I use these two classes to avoid having to handle both cases in each deriving node.
                       //Note: group nodes can be explicitly created whereas 
                       //leaf nodes are only intended for inheritance reasons
    };
protected:
    Node (eNodeType const&);
    eNodeType const mType;
public:
    virtual void render (RenderContext*&) = 0;
    virtual SceneTreeNode* extractCache (RenderContext*&) = 0;

    inline bool isGroup () const {return mType == GROUP;}
    inline bool isLeaf () const {return mType == LEAF;}
};

class GroupNode : public Node 
{
public:
    enum eGroupType 
    {
        GROUP=0,TRANSFORM=1,CACHE=2
    };
protected:
    std::vector<Node*> mChildren;
    eGroupType const mGroupType;
    GroupNode (std::vector<Node*> const&, eGroupType const&);
public:
    GroupNode (std::vector<Node*> const& children={});
    virtual void render (RenderContext*&) override;
    virtual SceneTreeNode* extractCache (RenderContext*&) override;

    //Getter/setter
    inline void addChild (Node* const& child) {mChildren.push_back(child);} 
    inline void addChildren (std::vector<Node*> const& children) {mChildren.insert(mChildren.end(), children.begin(), children.end());} 
    inline void removeChild (unsigned const& index) {mChildren.erase(mChildren.begin()+index);}
    inline std::vector<Node*> const& getChildren () const {return mChildren;}
    inline Node* const& getChild (unsigned const& index) const {return mChildren.at(index);} 

    inline bool isGroupType (eGroupType const& groupType) const {return mGroupType == groupType;}
    inline eGroupType const& getType () const {return mGroupType;}
};

class LeafNode : public Node 
{
public:
    enum eLeafType 
    {
        GEOMETRY=0 
    };
protected:
    eLeafType const mLeafType;
    LeafNode (eLeafType const&);
public:
    virtual void render (RenderContext*&) = 0;
    virtual SceneTreeNode* extractCache (RenderContext*&) = 0;

    inline bool isLeafType (eLeafType const& leafType) const {return mLeafType == leafType;}
    inline eLeafType const& getType () const {return mLeafType;}
};

class TransformNode : public GroupNode
{
protected:
    glm::mat4x4 mMat;
public:
    TransformNode (std::vector<Node*> const& children={}, glm::mat4x4 const& mat=glm::mat4x4(1.0f));
    virtual void render (RenderContext*&) override;

    //Getter/setter
    inline void setTransform (glm::mat4x4 const& mat) {mMat = mat;}
    inline glm::mat4x4 const& getTransform () const {return mMat;}
};

class GeometryNode : public LeafNode
{
protected:
    GraphMesh mGraphMesh;
public:
    GeometryNode ();
    GeometryNode (GraphMesh graphMesh);
    virtual void render (RenderContext*&) override;
    virtual SceneTreeNode* extractCache (RenderContext*&) override;

    //Getter/setter
    inline void setGraphMesh (GraphMesh const& graphMesh) {mGraphMesh = graphMesh;}
    inline GraphMesh const& getGraphMesh () const {return mGraphMesh;}
};

////RENDER CACHING CLASSES BELOW
//class Instruction
//{
//public:
//    enum eType 
//    {
//        BIND_SHADER=0,BIND_VAO=1,SET_UNIFORM=2,DRAW_ARRAYS=3,DRAW_ELEMENTS=4
//    };
//protected:
//    eType const mType;
//    Instruction (eType const&);
//public:
//    virtual ~Instruction () {}
//    virtual void operator() () const = 0;
//    bool operator== (Instruction const& inst) const
//        {return mType == inst.mType;}
//
//    inline bool isType (eType const& type) const {return mType == type;}
//    inline eType const& getType () const {return mType;}
//};
//
//class BindShaderInstruction : public Instruction
//{
//private:
//    GLuint mShader;
//public:
//    BindShaderInstruction (GLuint const&);
//    virtual void operator() () const override final;
//    bool operator== (BindShaderInstruction const& inst) const 
//        {return mShader == inst.mShader;}
//};
//
//class BindVaoInstruction : public Instruction
//{
//private:
//    GLuint mVao;
//public:
//    BindVaoInstruction (GLuint const&);
//    virtual void operator() () const override final;
//    bool operator== (BindVaoInstruction const& inst) const 
//        {return mVao == inst.mVao;}
//};
//
//class SetMatrixInstruction : public Instruction
//{
//private:
//    glm::mat4x4 mMat;
//    GLint mMatLoc;
//public:
//    SetMatrixInstruction (glm::mat4x4 const&, GLint const&);
//    virtual void operator() () const override final;
//    bool operator== (SetMatrixInstruction const& inst) const 
//        {return mMatLoc == inst.mMatLoc && mMat == inst.mMat;}
//};
//
//class ArrayDrawInstruction : public Instruction
//{
//private:
//    GLenum mPrimType;
//    Indexer mVboIndexer;
//public:
//    ArrayDrawInstruction (Indexer const&, GLenum const&);
//    virtual void operator() () const override final;
//    bool operator== (ArrayDrawInstruction const& inst) const 
//        {return mPrimType == inst.mPrimType && mVboIndexer.first == inst.mVboIndexer.first && mVboIndexer.count == inst.mVboIndexer.count;}
//};
//
//class ElementDrawInstruction : public Instruction
//{
//private:
//    GLenum mPrimType;
//    Indexer mEboIndexer;
//public:
//    ElementDrawInstruction (Indexer const&, GLenum const&);
//    virtual void operator() () const override final;
//    bool operator== (ElementDrawInstruction const& inst) const 
//        {return mPrimType == inst.mPrimType && mEboIndexer.first == inst.mEboIndexer.first && mEboIndexer.count == inst.mEboIndexer.count;}
//};
//
//class InstructionStream 
//{
//private:
//	std::vector<Instruction*> mStream;
//public:
//    InstructionStream () = default;
//    InstructionStream (std::vector<Instruction*> const&);
//    ~InstructionStream () {for (auto& inst: mStream) {delete inst;}};
//
//    void optimize ();
//    inline void evaluate () const {for (auto const& inst: mStream) {(*inst)();}}
//
//    //Getter/setter
//    inline void addInstruction (Instruction* inst) {mStream.push_back(inst);}
//    inline void addInstructions (std::vector<Instruction*> const& stream) {mStream.insert(mStream.end(), stream.begin(), stream.end());}
//};
//
//struct RenderJob
//{
//    LightGLContext context; //Notice that we only need vao because the vbo and ebo are not going to be edited 
//                            //or need to be bound when rendering 
//    glm::mat4x4 mat;
//    GraphMesh gmesh;
//};
//
//class RenderCache
//{
//private:
//    std::vector<RenderJob> mJobs;
//public:
//    RenderCache () = default;
//    RenderCache (std::vector<RenderJob> const&);
//    void sort ();
//    InstructionStream expand () const;
//    
//    //Getter/setter
//    void addJob (RenderJob const& job) {mJobs.push_back(job);}
//};

class SceneTreeNode 
{
public:
    enum eType
    {
        GROUP=0, LEAF=1
    };
protected:
    Node* const mDependent;
    SceneTreeNode(eType const&, Node* const&);
    eType const mType;
public:
    virtual ~SceneTreeNode () {}
//    virtual void constructRenderCache (RenderCache&) const = 0;
    
    //Getter/setter
    inline Node* const& getDependent () const {return mDependent;}

    inline bool isLeaf () const {return mType == LEAF;}
    inline bool isGroup () const {return mType == GROUP;}
};

class GroupSceneTreeNode final : public SceneTreeNode 
{
private:
    std::vector<SceneTreeNode*> mChildren;
public:
//    virtual ~GroupSceneTreeNode () override {for (auto& child: mChildren) {delete child;}}
    GroupSceneTreeNode (std::vector<SceneTreeNode*>&&, Node* const&);
//    virtual void constructRenderCache (RenderCache&) const override;
    
    //Getter/setter
    inline std::vector<SceneTreeNode*> const& getChildren () const {return mChildren;}
};

class LeafSceneTreeNode final : public SceneTreeNode
{
private:
   RenderContext mContext;
public:
    LeafSceneTreeNode (RenderContext const&, Node* const&);
//    virtual void constructRenderCache (RenderCache&) const override;
};

class CacheNode : public GroupNode 
{
private:
    SceneTreeNode* mSceneTree;
//    InstructionStream mInstructionStream;
    bool fCached;
public:
    CacheNode ();
    virtual SceneTreeNode* extractCache (RenderContext*&) override final;
    virtual void render (RenderContext*&) override;

    //Getter/setter
    inline SceneTreeNode* const& getSceneTree () const {return mSceneTree;} //This function is intended for use in testing:
                                                                            //caching should be hidden from the user.
};

#endif //NODE_H
