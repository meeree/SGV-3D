#include "sceneGraph.h"

void sceneGraphToGraphviz (Node*, char const*, bool const&, bool const&);
void sceneTreeToGraphviz (SceneTreeNode*, char const*, bool const&, bool const&);

int main (int argc, char* argv[])
{
    assert(argc == 2);
    Node* root{new CacheNode()};
    std::vector<Node*> children1{new GroupNode()};
    std::vector<Node*> children2{new TransformNode(), new TransformNode()};
    std::vector<Node*> children3{new GeometryNode()};

    static_cast<GroupNode*>(root)->addChildren(children1);
    static_cast<GroupNode*>(children1[0])->addChildren(children2);
    static_cast<GroupNode*>(children2[0])->addChildren(children3);
    static_cast<GroupNode*>(children2[1])->addChildren(children3);
    sceneGraphToGraphviz (root, argv[1], true, false);
    
    RenderContext* rc {new RenderContext()};
    root->render(rc);
    SceneTreeNode* sceneTree{static_cast<CacheNode*>(root)->getSceneTree()};
    sceneTreeToGraphviz(sceneTree, argv[1], false, true);
}
