#ifndef __RENDER_CACHE_H__
#define __RENDER_CACHE_H__

class Instruction
{
public:
    virtual void operator() () const = 0;
};

class BindShaderInstruction : public Instruction
{
private:
    GLuint mShader;
public:
    BindShaderInstruction (GLuint const&);
    virtual void operator() () const override final;
};

class BindVaoInstruction : public Instruction
{
private:
    GLuint mVao;
public:
    BindVaoInstruction (GLuint const&);
    virtual void operator() () const override final;
};

class SetMatrixInstruction : public Instruction
{
private:
    glm::mat4x4 mMat;
    GLint mMatLoc;
public:
    SetMatrixInstruction (glm::mat4x4 const&, GLint const&);
    virtual void operator() () const override final;
};

class ArrayDrawInstruction : public Instruction
{
private:
    GLenum mPrimType;
    Indexer mVboIndexer;
public:
    ArrayDrawInstruction (Indexer const&, GLenum const&);
    virtual void operator() () const override final;
};

class ElementDrawInstruction : public Instruction
{
private:
    GLenum mPrimType;
    Indexer mEboIndexer;
public:
    ElementDrawInstruction (Indexer const&, GLenum const&);
    virtual void operator() () const override final;
};

class InstructionStream 
{
private:
	std::vector<Instruction*> mStream;
};

struct LightGLContext 
{
    GLuint vao, shader;
};

struct RenderJob
{
    LightGLContext context; //Notice that we only need vao because the vbo and ebo are not going to be edited 
                            //or need to be bound when rendering 
    glm::mat4x4 mat;
    GraphMesh gmesh;
};

class RenderCache
{
private:
    std::vector<RenderJob> mJobs;
public:
    RenderCache () = default;
    RenderCache (std::vector<RenderJob> const&);
    void sort ();
    
    //Getter/setter
    void addJob (RenderJob const& job) {mJobs.push_back(job);}
};

class SceneTreeNode 
{
protected:
    enum eType
    {
        GROUP=0, LEAF=1
    } const mType;
    Node* const mDependent;
    SceneTreeNode(eType const&, Node* const&);
public:
    virtual void constructRenderCache (RenderCache&) const = 0;
};

class GroupSceneTreeNode final : public SceneTreeNode 
{
private:
    std::vector<SceneTreeNode*> mChildren;
public:
    GroupSceneTreeNode (std::vector<SceneTreeNode*>&&, Node* const&);
    virtual void constructRenderCache (RenderCache&) const override;
};

class LeafSceneTreeNode final : public SceneTreeNode
{
private:
    RenderContext mContext;
public:
    LeafSceneTreeNode (RenderContext const&, Node* const&);
    virtual void constructRenderCache (RenderCache&) const override;
};


#endif //RENDER_CACHE_H
