#include "sceneGraph.h"
#include <glm/gtc/type_ptr.hpp>

Mesh::Mesh () 
    : fDrawQuery{false} {setMinMax();}

void Mesh::setMinMax ()
{
    for (auto const& vert: mVertices)
    {
        for (int coord=0; coord<3; ++coord)
        {
            if (vert.pos[coord] < mInfo.min[coord]) 
                mInfo.min[coord] = vert.pos[coord];
            else if (vert.pos[coord] > mInfo.max[coord])
                mInfo.max[coord] = vert.pos[coord];
        }
        GLfloat len{length(vert.pos)};
        if (len < mInfo.min[4])
            mInfo.min[4] = len;
        if (len > mInfo.max[4])
            mInfo.max[4] = len;
    }
}

Node::Node (eType const& type)
    : mType{type} {}

TransformNode::TransformNode (glm::mat4x4 const& modMat, eTransformType const& type)
    : mModMat{modMat}, mTransType{type}, Node(eType::TRANSFORM) {}

StandardNode::StandardNode (Mesh const& mesh, glm::mat4x4 const& modMat)
    : mMesh{mesh}, TransformNode(modMat, eTransformType::MESH) {}

void Node::render (glm::mat4x4& modMat, GLuint const& matLoc) const
{
    intermediate(modMat, matLoc);
    for (auto const& child: mChildren)
    {
        child->render(modMat, matLoc);
    }
}

void TransformNode::intermediate (glm::mat4x4& modMat, GLuint const&) const
{
    modMat = modMat*mModMat;
}

void StandardNode::intermediate (glm::mat4x4& modMat, GLuint const& matLoc) const
{
    modMat = modMat*mModMat;
    if (mMesh.queryDraw())
    {
        glUniformMatrix4fv(matLoc, 1, GL_FALSE, glm::value_ptr(modMat));
        VboIndexer indexer{mMesh.getIndexer()};
        if (indexer.fPureVertexDraw)
            glDrawArrays(indexer.mode, indexer.first, indexer.count);
        else 
            glDrawElements(indexer.mode, indexer.count, 
                    GL_UNSIGNED_INT, (void*)(size_t)indexer.first);
    }
}
