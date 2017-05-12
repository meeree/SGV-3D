#include "sceneGraph.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <iostream>

Mesh::Mesh (GLenum const& mode)
    : mMode{mode} {}

Mesh::Mesh (std::vector<Vertex> const& verts, GLenum const& mode)
    : Mesh(verts, {}, mode) 
{
    mQuery.fPureVertexDraw = true;
}

Mesh::Mesh (std::vector<Vertex> const& verts, std::vector<GLuint> const& inds, GLenum const& mode)
    : mVertices{verts}, mIndices{inds}, mMode{mode} 
{
    setCenter();
    setMinMax();
}

void Mesh::setCenter (std::vector<Vertex>::const_iterator begin, std::vector<Vertex>::const_iterator end)
{
    long range{end-begin-1};
    for (; begin != end; begin++) {mQuery.center += (*begin).pos;}
    mQuery.center /= range;
}

void Mesh::setMinMax (std::vector<Vertex>::const_iterator begin, std::vector<Vertex>::const_iterator end)
{
    mQuery.min = {FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX};
    mQuery.max = {FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN};
    for (; begin != end; begin++)
    {
        Vertex const& vert{*begin};
        for (int coord=0; coord<3; ++coord)
        {
            if (vert.pos[coord] < mQuery.min[coord]) 
                mQuery.min[coord] = vert.pos[coord];
            else if (vert.pos[coord] > mQuery.min[coord])
                mQuery.max[coord] = vert.pos[coord];
        }
        GLfloat len{distance(vert.pos, mQuery.center)};
        if (len < mQuery.min.w)
        {
            mQuery.min.w = len;
        }
        if (len > mQuery.max.w)
            mQuery.max.w = len;
    }
}

GraphMesh::GraphMesh (std::vector<Vertex> const& verts, Indexer const& vboIndexer)
    : mVboIndexer{vboIndexer}, mEboIndexer{(GLuint)-1, -1}
{
    Mesh::setCenter(verts.begin()+mVboIndexer.first, 
                    verts.begin()+mVboIndexer.first+mVboIndexer.count);
    Mesh::setMinMax(verts.begin()+mVboIndexer.first, 
                    verts.begin()+mVboIndexer.first+mVboIndexer.count);
}

GraphMesh::GraphMesh (std::vector<Vertex> const& verts, std::vector<GLuint> const& /*inds*/, 
           Indexer const& vboIndexer, Indexer const& eboIndexer)
    : mVboIndexer{vboIndexer}, mEboIndexer{eboIndexer}
{
    Mesh::setCenter(verts.begin()+mVboIndexer.first, 
                    verts.begin()+mVboIndexer.first+mVboIndexer.count);
    Mesh::setMinMax(verts.begin()+mVboIndexer.first, 
                    verts.begin()+mVboIndexer.first+mVboIndexer.count);
}

Node::Node (eType const& type)
    : mType{type} {}

Node::Node ()
    : mType{PURE_BRANCH} {}

TransformNode::TransformNode ()
    : Node(TRANSFORM) {}

TransformNode::TransformNode (glm::mat4x4 const& modMat)
    : mModMat{modMat}, Node(TRANSFORM) {}

DynamicTransformNode::DynamicTransformNode ()
    : Node(DYNAMIC_TRANSFORM) {} 

DynamicInputTransformNode::DynamicInputTransformNode (glm::mat4x4 (*transCalc) (double const&))
    : mTransCalc{transCalc} {}

ObjectNode::ObjectNode ()
    : Node(OBJECT) {}

ObjectNode::ObjectNode (GraphMesh const& mesh)
    : mGraphMesh{mesh}, Node{OBJECT} {}

void Node::render (glm::mat4x4 const& modMat, SceneGraph* sg, double const& t)
{
    for (auto const& child: mChildren) {child->render(modMat, sg, t);}
}

Node::~Node ()
{
    for (auto& child: mChildren) {/*delete(child);*/}
}

void TransformNode::render(glm::mat4x4 const& modMat, SceneGraph* sg, double const& t) 
{
    Node::render(modMat*mModMat, sg, t);
}

void DynamicTransformNode::render (glm::mat4x4 const& modMat, SceneGraph* sg, double const& t)
{
    Node::render(modMat*calculateTransform(t), sg, t);
}

void ObjectNode::render (glm::mat4x4 const& modMat, SceneGraph* sg, double const& t) 
{
    MeshQuery meshQuery{mGraphMesh.getQuery()};
    if (meshQuery.fDrawQuery)
    {
        GraphQuery graphQuery{sg->getQuery()};
        glUniformMatrix4fv(graphQuery.matLoc, 1, GL_FALSE, glm::value_ptr(modMat));
        if (!graphQuery.fGlobalMinMax)
        {
            glUniform4fv(graphQuery.min.first, 1, glm::value_ptr(meshQuery.min));
            glUniform4fv(graphQuery.max.first, 1, glm::value_ptr(meshQuery.max));
        }
        Indexer indexer{mGraphMesh.getSigIndexer()};
        if (meshQuery.fPureVertexDraw)
            glDrawArrays(mGraphMesh.getMode(), indexer.first, indexer.count);
        else 
            glDrawElements(mGraphMesh.getMode(), indexer.count, 
                    GL_UNSIGNED_INT, (void*)(size_t)indexer.first);
    }
    Node::render(modMat, sg, t);
}

SceneGraph::SceneGraph (Node* node, GLuint const& shaderProgram, bool const& useGlobalMinMax, GLchar const* names[3])
    : mRoot{node} 
{
    glGenVertexArrays(1, &mVao);
    glBindVertexArray(mVao);

    glGenBuffers(1, &mVbo);
    glGenBuffers(1, &mEbo);
    glBindBuffer(GL_ARRAY_BUFFER, mVbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    mQuery.matLoc = glGetUniformLocation(shaderProgram, names[0]);
    mQuery.min.first = glGetUniformLocation(shaderProgram, names[1]);
    mQuery.max.first = glGetUniformLocation(shaderProgram, names[2]);
    mQuery.fGlobalMinMax = useGlobalMinMax;
    if (node != nullptr)
    {
        setGlobalCenter();
        setGlobalMinMax(); //This is called even if fGlobalMinMax is false because then we don't need to 
                           //call it each time we toggle fGlobalMinMax to true
    }
}

void SceneGraph::setGlobalMinMax (std::vector<Vertex>::const_iterator begin, std::vector<Vertex>::const_iterator end)
{
    mQuery.min.second = {FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX};
    mQuery.max.second = {FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN};
    for (; begin != end; begin++)
    {
        Vertex const& vert{*begin};
        for (int coord=0; coord<3; ++coord)
        {
            if (vert.pos[coord] < mQuery.min.second[coord]) 
                mQuery.min.second[coord] = vert.pos[coord];
            else if (vert.pos[coord] > mQuery.min.second[coord])
                mQuery.max.second[coord] = vert.pos[coord];
        }
        GLfloat len{distance(vert.pos, mQuery.center)};
        if (len < mQuery.min.second.w)
        {
            mQuery.min.second.w = len;
        }
        if (len > mQuery.max.second.w)
            mQuery.max.second.w = len;
    }
}

void SceneGraph::setGlobalCenter (std::vector<Vertex>::const_iterator begin, std::vector<Vertex>::const_iterator end)
{
    long range{end-begin-1};
    for (; begin != end; begin++) {mQuery.center += (*begin).pos;}
    mQuery.center /= range;
}

void SceneGraph::render (glm::mat4x4 modMat, double const& t)
{
    glBindVertexArray(mVao);
//    glBindBuffer(GL_ARRAY_BUFFER, mVbo);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
    if (mQuery.fGlobalMinMax)
    {
        glUniform4fv(mQuery.min.first, 1, glm::value_ptr(mQuery.min.second));
        glUniform4fv(mQuery.max.first, 1, glm::value_ptr(mQuery.max.second));
    }
    mRoot->render(modMat, this, t);
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*mVertices.size(), mVertices.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*mIndices.size(), mIndices.data(), GL_DYNAMIC_DRAW);
}

GraphMesh SceneGraph::bindMesh(Mesh& mesh)
{
    GraphMesh graphMesh = std::move(static_cast<GraphMesh&>(mesh));
    graphMesh.setIndexers({(GLuint)mVertices.size(), (GLsizei)graphMesh.mVertices.size()},
                          {(GLuint)mIndices.size(),  (GLsizei)graphMesh.mIndices.size()});
    mVertices.insert(mVertices.end(), 
                     std::make_move_iterator(graphMesh.mVertices.begin()), 
                     std::make_move_iterator(graphMesh.mVertices.end()));
    graphMesh.mVertices.clear();
    glBindBuffer(GL_ARRAY_BUFFER, mVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*mVertices.size(), mVertices.data(), GL_DYNAMIC_DRAW);

    if (!graphMesh.getQuery().fPureVertexDraw)
    {
        mIndices.insert(mIndices.end(), 
                        std::make_move_iterator(graphMesh.mIndices.begin()), 
                        std::make_move_iterator(graphMesh.mIndices.end()));
        graphMesh.mIndices.clear();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*mIndices.size(), mIndices.data(), GL_DYNAMIC_DRAW);
    }
    return graphMesh;
}

Mesh SceneGraph::unbindMesh(GraphMesh& graphMesh)
{
    if (graphMesh.mVertices.size() > 0)
        std::cerr<<"Warning in call to unbindMesh: graphMesh arg. contains vertices that will be deleted"<<std::endl;

    Indexer vboIndexer{graphMesh.getVboIndexer()}, eboIndexer{graphMesh.getEboIndexer()};
    Mesh mesh = std::move(static_cast<Mesh&>(graphMesh));
    mesh.mVertices.insert(mesh.mVertices.end(),
                 std::make_move_iterator(mVertices.begin()+vboIndexer.first),
                 std::make_move_iterator(mVertices.begin()+vboIndexer.first+vboIndexer.count));
    mVertices.erase(mVertices.begin()+vboIndexer.first,
                    mVertices.begin()+vboIndexer.first+vboIndexer.count);
    glBindBuffer(GL_ARRAY_BUFFER, mVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*mVertices.size(), mVertices.data(), GL_DYNAMIC_DRAW);

    if (!graphMesh.getQuery().fPureVertexDraw)
    {
        mesh.mIndices.insert(mesh.mIndices.end(),
                    std::make_move_iterator(mIndices.begin()+eboIndexer.first),
                    std::make_move_iterator(mIndices.begin()+eboIndexer.first+eboIndexer.count));
        mIndices.erase(mIndices.begin()+eboIndexer.first,
                       mIndices.begin()+eboIndexer.first+eboIndexer.count);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*mIndices.size(), mIndices.data(), GL_DYNAMIC_DRAW);
    }
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
