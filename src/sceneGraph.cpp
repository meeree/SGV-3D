#include "sceneGraph.h"
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Node::Node (eNodeType const& type)
    : mType{type} {}

LeafNode::LeafNode (eLeafType const& leafType)
    : mLeafType{leafType}, Node(eNodeType::LEAF) {}

GroupNode::GroupNode (std::vector<Node*> const& children, eGroupType const& type) 
    : mChildren{children}, mGroupType(type), Node(eNodeType::GROUP) {}

GroupNode::GroupNode (std::vector<Node*> const& children) 
    : mChildren{children}, mGroupType(eGroupType::GROUP), Node(eNodeType::GROUP) {}

TransformNode::TransformNode (std::vector<Node*> const& children, glm::mat4x4 const& mat)
    : mMat{mat}, GroupNode(children, eGroupType::TRANSFORM) {}

GeometryNode::GeometryNode ()
    : LeafNode(eLeafType::GEOMETRY) {}

GeometryNode::GeometryNode (GraphMesh graphMesh)
    : mGraphMesh{graphMesh}, LeafNode(eLeafType::GEOMETRY) {}

CacheNode::CacheNode ()
    : GroupNode({}, eGroupType::CACHE) {}

void GroupNode::render (RenderContext*& rc)
{
    for (auto& child: mChildren) {child->render(rc);}
}

void GeometryNode::render (RenderContext*&)
{
}   

SceneTreeNode* GroupNode::extractCache (RenderContext*& rc)
{
    assert(mChildren.size() > 0);
    std::vector<SceneTreeNode*> subTrees;
    for (auto& child: mChildren) 
    {
        subTrees.push_back(std::move(child->extractCache(rc)));
    }
    return new GroupSceneTreeNode(std::move(subTrees), this);
}

SceneTreeNode* GeometryNode::extractCache (RenderContext*& rc)
{
    return new LeafSceneTreeNode(*rc, this);
}

SceneTreeNode::SceneTreeNode (eType const& type, Node* const& dependent) 
    : mType{type}, mDependent{dependent} {}

GroupSceneTreeNode::GroupSceneTreeNode (std::vector<SceneTreeNode*>&& children, Node* const& dependent)
    : mChildren{children}, SceneTreeNode(eType::GROUP, dependent) {}

LeafSceneTreeNode::LeafSceneTreeNode (RenderContext const& rc, Node* const& dependent)
    : mContext{rc}, SceneTreeNode(eType::LEAF, dependent) {}
//
//Instruction::Instruction (eType const& type)
//    : mType{type} {}
//
//BindShaderInstruction::BindShaderInstruction (GLuint const& shader)
//    : mShader{shader}, Instruction{BIND_SHADER} {}
//
//void BindShaderInstruction::operator() () const
//{
//    glLinkProgram(mShader);
//}
//
//BindVaoInstruction::BindVaoInstruction (GLuint const& vao)
//    : mVao{vao}, Instruction{BIND_VAO} {}
//
//void BindVaoInstruction::operator() () const
//{
//    glBindVertexArray(mVao);
//}
//
//SetMatrixInstruction::SetMatrixInstruction (glm::mat4x4 const& mat, GLint const& matLoc)
//    : mMat{mat}, mMatLoc{matLoc}, Instruction{SET_UNIFORM} {}
//
//void SetMatrixInstruction::operator() () const
//{
//    glUniformMatrix4fv(mMatLoc, 1, GL_FALSE, glm::value_ptr(mMat));
//}
//
//ArrayDrawInstruction::ArrayDrawInstruction (Indexer const& vboIndexer, GLenum const& primType)
//    : mVboIndexer{vboIndexer}, mPrimType{primType}, Instruction{DRAW_ARRAYS} {}
//
//void ArrayDrawInstruction::operator() () const
//{
//    glDrawArrays(mPrimType, mVboIndexer.first, mVboIndexer.count);
//}
//
//ElementDrawInstruction::ElementDrawInstruction (Indexer const& eboIndexer, GLenum const& primType)
//    : mEboIndexer{eboIndexer}, mPrimType{primType}, Instruction{DRAW_ELEMENTS} {}
//
//void ElementDrawInstruction::operator() () const
//{
//    //VERIFY THE FINAL PARAMENTER OF THIS CALL
//    glDrawElements(mPrimType, mEboIndexer.count, GL_UNSIGNED_INT, (GLvoid*)(size_t)mEboIndexer.first);
//}
//
//InstructionStream::InstructionStream (std::vector<Instruction*> const& stream)
//    : mStream{stream} {}
//
//void RenderCache::sort ()
//{
//    //We sort the render cache so that shader programs are in acsending order and 
//    //meshes using the same shader program are nearby and in ascending order. 
//    std::sort(mJobs.begin(), mJobs.end(), [](RenderJob& rj1, RenderJob& rj2) 
//        {return rj1.context.shader < rj2.context.shader
//                || (rj1.context.shader == rj2.context.shader
//                   && rj1.gmesh.getSigIndexer().first < rj2.gmesh.getSigIndexer().second);});
//}
//
//InstructionStream RenderCache::expand () const
//{
//    InstructionStream instStream;
//    for (std::vector<RenderJob>::reverse_iterator it = mJobs.rbegin(); it != mJobs.rend(); ++it)
//    {
//        RenderJob& job{*it};
//        instStream.addInstructions({
//            new BindShaderInstruction(job.context.shader),
//            new BindVaoInstruction(job.context.vao),
//                                              //I SHOULDN'T PUSH THIS NAMING CONVENTION ON USER!
//            new SetMatrixInstruction(job.mat, glGetUniformLocation(job.context.shader, "mMat")),
//            job.gmesh.fPureVertex ? new ArrayDrawInstruction(job.gmesh.getSigIndexer())
//                                  : new ElementDrawInstruction(job.gmesh.getSigIndexer())});
//    }
//}
//
//void GroupSceneTreeNode::constructRenderCache (RenderCache& jobs) const
//{
//    for (auto const& child: mChildren) {child->constructRenderCache(jobs);}
//}
//
//void LeafSceneTreeNode::constructRenderCache (RenderCache& jobs) const
//{
//    jobs.addJob({
//        {mContext.glContextStack.top().vao,mContext.glContextStack.top().shader},
//        mContext.matStack.top(), 
//        dynamic_cast<GeometryNode*>(mDependent)->getGraphMesh()});
//}
//
////TO-DO: add more optimizations
//void InstructionStream::optimize ()
//{
//    //Important note: the instruction stream is assumed to
//    //be state-sorted before optimization
//    //
//    //We do optimiation in multiple passes. First, equal contexts are 
//    //merged. Next, context-specific instructions like unfiform updates
//    //and draw calls are easily merged after the first pass.
//    
//    //First pass
////    for (auto it1 = mStream.begin(); it1 != mStream.end(); ++it1)
////    {
////        loop_label:
////        for (auto it2 = it1+1; it1 != mStream.end(); ++it2)
////        {
////            Instruction*& inst1{*it1};
////            Instruction*& inst2{*it2};            
////            if (inst1->getType() != inst2->getType())
////                continue;
////            switch (inst1->getType())
////            {
////                case Instruction::eType::BIND_VAO: 
////                    if (*static_cast<BindVaoInstruction*>(inst1) == *static_cast<BindVaoInstruction*>(inst2))
////                        delete inst2; inst2 = nullptr; break;
////                    goto loop_label;
////                case Instruction::eType::BIND_SHADER:
////                    if (*static_cast<BindShaderInstruction*>(inst1) == *static_cast<BindShaderInstruction*>(inst2))
////                        delete inst2; inst2 = nullptr; break;
////                    goto loop_label;
////                default: continue; 
////            }
////        }
////    }
////
////    //Second pass
////    for (auto it1 = mStream.begin(); it1 != mStream.end(); ++it1)
////    {
////        for (auto it2 = it1+1; it1 != mStream.end(); ++it2)
////        {
////            Instruction*& inst1{*it1};
////            Instruction*& inst2{*it2};            
////            if (inst1->getType() != inst2->getType()
////                ||(   !inst1->isType(Instruction::eType::SET_UNIFORM)
////                   && !inst1->isType(Instruction::eType::DRAW_ARRAYS)
////                   && !inst1->isType(Instruction::eType::DRAW_ELEMENTS)))
////                continue;
////            if (*inst1 != *inst2)
////                break;
////            delete inst2; inst2 = nullptr;
////        }
////    }
////
////    mStream.erase(std::remove_if(mStream.begin(), mStream.end(),
////                  [](Instruction*& inst){return (bool)inst;}), mStream.end()); //Delete all nullptrs from vector 
//}

SceneTreeNode* CacheNode::extractCache (RenderContext*&)
{
    std::cerr<<"Error in call to CacheNode::extractCache: cache nodes cannot be nested"<<std::endl;
    exit(0);
}

void CacheNode::render (RenderContext*& rc)
{
    if (!fCached)
    {
        assert(mChildren.size() <= 1);
        if (mChildren.size() == 0)
            return;
        mSceneTree = std::move(mChildren[0]->extractCache(rc));
 //       RenderCache cache;
 //       mSceneTree->constructRenderCache(cache);
 //       cache.sort();
 //       mInstructionStream = cache.expand();
 //       fCached = true;
        //WORK ON OPTIMIZATION 
//        mInstructionStream->optimize();
    }
//    mInstructionStream.evaluate();
}

void TransformNode::render (RenderContext*& rc)
{
    rc->matStack.push(rc->matStack.top()*mMat);
    GroupNode::render(rc);
    rc->matStack.pop();
}
