#include "sceneGraph.h"
#include "graphics.h"
#include "Common/meshStorage.cpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#include <fstream>
#include <iostream>

Graphics g{1920, 1080, "/home/jhazelden/Cpp/OpenGL/SGV3D/src/Shaders/vert.glsl", "/home/jhazelden/Cpp/OpenGL/SGV3D/src/Shaders/frag.glsl", "SG3D v0.0"};

glm::mat4x4 calcTrans1 (double const& t) 
{
    float f = (GLfloat)t * (GLfloat)M_PI * 0.1f;
    return glm::translate(glm::vec3(sinf(2.1f * f) * 0.5f,
               cosf(1.7f * f) * 0.5f,
               sinf(1.3f * f) * cosf(1.5f * f) * 2.0f)) *
           glm::rotate(glm::radians((GLfloat)t * 45.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
           glm::rotate(glm::radians((GLfloat)t * 81.0f), glm::vec3(1.0f, 0.0f, 0.0f));
}

glm::mat4x4 calcTrans2 (double const& t) 
{
    return glm::rotate((GLfloat)t, glm::vec3(0.0f,1.0f,0.0f));
}

int main ()
{
    g.setCam(glm::vec3(0,0,4.5), glm::vec3(0,0,-1));
    g.setCamTransforms(glm::perspective(glm::radians(45.0f),(GLfloat)1920/1080, 0.1f, 200.0f),
                       glm::mat4x4(1.0f));
    g.setCallbackAndFlags();

    std::vector<Vertex> vertices;
    if (!meshIO::readFile("../Tests/asteroid_dual_contouring.vmesh", vertices))
    {
       std::cerr<<"File read failed"<<std::endl;
       exit(0);
    } 

    std::vector<glm::mat4x4> modMats{ 
        glm::scale(glm::mat4x4(1.0f), glm::vec3(1.5f)),
        glm::scale(glm::translate(glm::mat4x4(1.0f), glm::vec3(-13.0f,-13.0f,0.0f)), glm::vec3(0.6f)),
        glm::scale(glm::translate(glm::mat4x4(1.0f), glm::vec3(13.0f,13.0f,0.0f)), glm::vec3(0.8f)),

        glm::scale(glm::translate(glm::mat4x4(1.0f), glm::vec3(0,0,20)+glm::vec3(0.0f,0.0f,0.0f)), glm::vec3(0.7f)),
        glm::scale(glm::translate(glm::mat4x4(1.0f), glm::vec3(0,0,20)+glm::vec3(-13.0f,-13.0f,0.0f)), glm::vec3(0.7f)),
        glm::scale(glm::translate(glm::mat4x4(1.0f), glm::vec3(0,0,20)+glm::vec3(13.0f,13.0f,0.0f)), glm::vec3(0.8f))};

    GLuint shaderProgram = g.getShaderProgram();
    GLchar const* names [3]{"mMat","mini","maxi"};
    SceneGraph sg {nullptr, shaderProgram, true, names}; 

    Mesh mesh{vertices, GL_TRIANGLES};
    ObjectNode* objNode{new ObjectNode(sg.bindMesh(mesh))};
    objNode->toggleDraw();
    std::vector<DynamicInputTransformNode*> ditNodes{
        new DynamicInputTransformNode(calcTrans1),
        new DynamicInputTransformNode(calcTrans2),
        new DynamicInputTransformNode(calcTrans2)};
    std::vector<TransformNode*> tNodes (modMats.size());
    std::transform(modMats.begin(), modMats.end(), tNodes.begin(), [](glm::mat4x4 const& modMat){return new TransformNode(modMat);});

    //LAYER 1 
    ditNodes[0]->addChildren({tNodes[0],tNodes[3]});
    //LAYER 2 
    tNodes[0]->addChildren({objNode,ditNodes[1]});
    tNodes[3]->addChildren({objNode,ditNodes[2]});
    //LAYER 3 
    ditNodes[1]->addChild(tNodes[1]);
    ditNodes[2]->addChild(tNodes[4]);
    //LAYER 4 
    tNodes[1]->addChildren({objNode,tNodes[2]});
    tNodes[4]->addChildren({objNode,tNodes[5]});
    //LAYER 5 
    tNodes[2]->addChild(objNode);
    tNodes[5]->addChild(objNode);

    sg.setRoot(ditNodes[0]);
    sg.setGlobalCenter();
    sg.setGlobalMinMax();

    g.addGraph(sg);
    g.loop();

    delete objNode;
    for (auto& ditNode: ditNodes) {delete ditNode;}
    for (auto& tNode: tNodes) {delete tNode;}
}
