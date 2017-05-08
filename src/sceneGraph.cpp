#include "sceneGraph.h"
#include <glm/gtc/type_ptr.hpp>

Mesh::Mesh (GLenum const& mode)
    : Mesh({},mode) {}

Mesh::Mesh (std::vector<Vertex> const& verts, GLenum const& mode)
    : Mesh(verts, {}, mode) 
{
    mInfo.fPureVertexDraw = true;
}

Mesh::Mesh (std::vector<Vertex> const& verts, std::vector<unsigned> const& inds, GLenum const& mode)
    : mVertices{verts}, mIndices{inds}, mMode{mode} {}

void Mesh::setLocalMinMax (std::vector<Vertex>::const_iterator begin, std::vector<Vertex>::const_iterator end)
{
    for (; begin != end; begin++)
    {
        Vertex const& vert{*begin};
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

TransformNode::TransformNode (glm::mat4x4 const& modMat, eType const& type)
    : mModMat{modMat}, Node(type) {}

StandardNode::StandardNode (GraphMesh const& mesh, glm::mat4x4 const& modMat)
    : mGraphMesh{mesh}, TransformNode(modMat, eType::STANDARD) {}

void Node::render (glm::mat4x4& modMat, GraphQuery const& query) const
{
    intermediate(modMat, query);
    for (auto const& child: mChildren) {child->render(modMat, query);}
}

Node::~Node ()
{
    for (auto& child: mChildren) {delete(child);}
}

void TransformNode::intermediate (glm::mat4x4& modMat, GraphQuery const&) const
{
    modMat = modMat*mModMat;
}

void StandardNode::intermediate (glm::mat4x4& modMat, GraphQuery const& query) const
{
    modMat = modMat*mModMat;
    MeshQuery meshInfo{mGraphMesh.getQuery()};
    if (meshInfo.fDrawQuery)
    {
//        glUniformMatrix4fv(query.matLoc, 1, GL_FALSE, glm::value_ptr(modMat));
        glUniformMatrix4fv(4, 1, GL_FALSE, glm::value_ptr(glm::mat4x4(1.0f)));
        if (!query.fGlobalMinMax)
        {
//            glUniform4fv(query.min.first, 1, glm::value_ptr(meshInfo.min));
//            glUniform4fv(query.max.first, 1, glm::value_ptr(meshInfo.max));
        }
        Indexer indexer{mGraphMesh.getSigIndexer()};
        if (meshInfo.fPureVertexDraw)
            glDrawArrays(mGraphMesh.getMode(), indexer.first, indexer.count);
        else 
            glDrawElements(mGraphMesh.getMode(), indexer.count, 
                    GL_UNSIGNED_INT, (void*)(size_t)indexer.first);
    }
}

SceneGraph::SceneGraph (Node* node, GLuint const& shaderProgram, bool const& useGlobalMinMax, GLchar const* names[3])
    : mRoot{node} 
{
    glGenBuffers(1, &mVbo);
    glGenBuffers(1, &mEbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    mInfo.min.first = glGetUniformLocation(shaderProgram, names[0]);
    mInfo.max.first = glGetUniformLocation(shaderProgram, names[1]);
    mInfo.matLoc = glGetUniformLocation(shaderProgram, names[2]);
    mInfo.fGlobalMinMax = useGlobalMinMax;
    setGlobalMinMax();
    //This could be redundent
    if (mRoot != nullptr)
        setLocalMinMax();
}

void SceneGraph::setGlobalMinMax ()
{
    for (auto const& vert: mVertices)
    {
        for (int coord=0; coord<3; ++coord)
        {
            if (vert.pos[coord] < mInfo.min.second[coord]) 
                mInfo.min.second[coord] = vert.pos[coord];
            else if (vert.pos[coord] > mInfo.max.second[coord])
                mInfo.max.second[coord] = vert.pos[coord];
        }
        GLfloat len{length(vert.pos)};
        if (len < mInfo.min.second[4])
            mInfo.min.second[4] = len;
        if (len > mInfo.max.second[4])
            mInfo.max.second[4] = len;
    }
}

void SceneGraph::render (glm::mat4x4& modMat) const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
    glBindBuffer(GL_ARRAY_BUFFER, mVbo); 
    mRoot->render(modMat, mInfo);
}

void SceneGraph::mergeGraph (SceneGraph& graph, Node* node)
{
    node->addChild(graph.mRoot);
    mVertices.insert(mVertices.end(), 
                     std::make_move_iterator(graph.mVertices.begin()), 
                     std::make_move_iterator(graph.mVertices.end()));
    mIndices.insert(mIndices.end(), 
                    std::make_move_iterator(graph.mIndices.begin()), 
                    std::make_move_iterator(graph.mIndices.end()));

    glBindBuffer(GL_ARRAY_BUFFER, mVbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*mVertices.size(), mVertices.data(), GL_DYNAMIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned)*mIndices.size(), mIndices.data(), GL_DYNAMIC_DRAW);
}

GraphMesh SceneGraph::bindMesh(Mesh& mesh)
{
    GraphMesh graphMesh = std::move(static_cast<GraphMesh&>(mesh));
    graphMesh.setIndexers({{(GLuint)mVertices.size(), (GLsizei)graphMesh.mVertices.size()},
                           {(GLuint)mIndices.size(),  (GLsizei)graphMesh.mIndices.size()}});
    std::cout<<graphMesh.getVboIndexer().count<<std::endl;
    mVertices.insert(mVertices.end(), 
                     std::make_move_iterator(graphMesh.mVertices.begin()), 
                     std::make_move_iterator(graphMesh.mVertices.end()));
    mIndices.insert(mIndices.end(), 
                    std::make_move_iterator(graphMesh.mIndices.begin()), 
                    std::make_move_iterator(graphMesh.mIndices.end()));
    graphMesh.mVertices.clear();
    graphMesh.mIndices.clear();

    glBindBuffer(GL_ARRAY_BUFFER, mVbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*mVertices.size(), mVertices.data(), GL_DYNAMIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned)*mIndices.size(), mIndices.data(), GL_DYNAMIC_DRAW);
    return graphMesh;
}

Mesh SceneGraph::unbindMesh(GraphMesh& graphMesh)
{
    Indexer vboIndexer{graphMesh.getVboIndexer()}, eboIndexer{graphMesh.getEboIndexer()};
    Mesh mesh = std::move(static_cast<Mesh&>(graphMesh));
    //IMPROVE THIS
    assert(mesh.mVertices.size() == 0 && mesh.mIndices.size() == 0);
    mesh.mVertices.insert(mesh.mVertices.end(),
                 std::make_move_iterator(mVertices.begin()+vboIndexer.first),
                 std::make_move_iterator(mVertices.begin()+vboIndexer.first+vboIndexer.count));
    mesh.mIndices.insert(mesh.mIndices.end(),
                std::make_move_iterator(mIndices.begin()+eboIndexer.first),
                std::make_move_iterator(mIndices.begin()+eboIndexer.first+eboIndexer.count));
    mVertices.erase(mVertices.begin()+vboIndexer.first,
                    mVertices.begin()+vboIndexer.first+vboIndexer.count);
    mIndices.erase(mIndices.begin()+eboIndexer.first,
                   mIndices.begin()+eboIndexer.first+eboIndexer.count);

    glBindBuffer(GL_ARRAY_BUFFER, mVbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*mVertices.size(), mVertices.data(), GL_DYNAMIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned)*mIndices.size(), mIndices.data(), GL_DYNAMIC_DRAW);
    return mesh;
}

void SceneGraph::cut (Node* node)
{
    std::vector<std::pair<Indexer, Indexer>> regions;
    node->cut(regions);
    for (auto const& region: regions)
    {
        auto const& vertStart = mVertices.begin()+region.first.first;
        auto const& indStart = mIndices.begin()+region.second.first;
        mVertices.erase(vertStart, vertStart+region.first.count);
        mIndices.erase(indStart, indStart+region.second.count);
    }
}
