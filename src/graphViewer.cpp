#ifndef __GRAPH_VIEWER_H__
#define __GRAPH_VIEWER_H__

#include "sceneGraph.h"

#include <fstream>
#include <algorithm>

std::string addressToLetter (void* const& ptr, std::vector<std::pair<size_t,unsigned>>& addresses)
{
    std::vector<std::pair<size_t,unsigned>>::iterator lookup = std::find_if(addresses.begin(), addresses.end(), 
                                                                           [ptr](std::pair<size_t,unsigned> const& ptrPair)
                                                                               {return ptrPair.first == (size_t)ptr;});    
    if (lookup != addresses.end())
        return std::string(1,'A'+(lookup-addresses.begin())%26)
              +std::string(++(*lookup).second, '\''); //We use the second field to determine the number of primes
                                                      //on the letter. Note that it is incremented here. 
    addresses.push_back({(size_t)ptr, 0});
    return std::string(1,'A'+addresses.size()%26);
}

void sceneGraphToGraphviz (Node* root, char const* flName,
                           bool const& start, bool const& end) //Flags used to determine if we should write 
                                                               //"digraph G {" at beginning and "}" at end 
{
    std::ofstream fl;
    if (start)
    {
        fl.open(flName, std::ios::out | std::ios::trunc);
        fl<<"digraph G {\n";
    }
    else
        fl.open(flName, std::ios::out | std::ios::app);

    std::vector<Node*> layer0{root}, layer1; //Switch between two layers
    std::vector<std::pair<size_t, unsigned>> addresses;
    while (layer0.size() > 0)
    {
        for (auto const& node: layer0)
        {
            if (node->isLeaf())
            {
                switch (static_cast<LeafNode*>(node)->getType())
                {
                    case LeafNode::eLeafType::GEOMETRY: 
                        fl<<"\tn"<<node<<" [label = \"Geom.\"];\n";
                        break;
                }
                continue;
            }
            switch (static_cast<GroupNode*>(node)->getType())
            {
                case GroupNode::eGroupType::GROUP: 
                    fl<<"\tn"<<node<<" [label = \"Group\"];\n";
                    break;
                case GroupNode::eGroupType::TRANSFORM: 
                    fl<<"\tn"<<node<<" [label = \"Trans.\"];\n";
                    break;
                case GroupNode::eGroupType::CACHE: 
                    fl<<"\tn"<<node<<" [label = \"Cache\"];\n";
                    break;
            }
            std::vector<Node*> children{static_cast<GroupNode*>(node)->getChildren()};
            for (auto const& child: children)
            {
                fl<<"\tn"<<node<<" -> n"<<child<<";\n";
            }
            layer1.insert(layer1.end(), std::make_move_iterator(children.begin()),
                                        std::make_move_iterator(children.end()));
        } 
        layer0 = std::move(layer1);
        layer1.clear();
    }
    if (end)
        fl<<"}";

    fl.close();
}

void sceneTreeToGraphviz (SceneTreeNode* root, char const* flName,
                          bool const& start, bool const& end)
{
    std::ofstream fl;
    if (start)
    {
        fl.open(flName, std::ios::out | std::ios::trunc);
        fl<<"digraph G {\n";
    }
    else
        fl.open(flName, std::ios::out | std::ios::app);

    std::vector<SceneTreeNode*> layer0{root}, layer1; //Switch between two layers
    std::vector<std::pair<size_t, unsigned>> addresses;
    while (layer0.size() > 0)
    {
        for (auto const& node: layer0)
        {
            if (node->isLeaf())
            {
                GeometryNode* const& gnode{static_cast<GeometryNode*>(static_cast<LeafSceneTreeNode*>(node)->getDependent())};
                Indexer const& sigIndexer{gnode->getGraphMesh().getSigIndexer()};
                fl<<"\tsgn"<<node<<" [label = \"Leaf\n"
                                              <<sigIndexer.first<<","<<sigIndexer.count<<"\"];\n";
                continue;
            }
            fl<<"\tsgn"<<node<<" [label = \"Group\"];\n";

            std::vector<SceneTreeNode*> children{static_cast<GroupSceneTreeNode*>(node)->getChildren()};
            for (auto const& child: children)
            {
                fl<<"\tsgn"<<node<<" -> sgn"<<child<<";\n";
            }
            layer1.insert(layer1.end(), std::make_move_iterator(children.begin()),
                                        std::make_move_iterator(children.end()));
        } 
        layer0 = std::move(layer1);
        layer1.clear();
    }
    if (end)
        fl<<"}";

    fl.close();
}

#endif //__GRAPH_VIEWER_H__
