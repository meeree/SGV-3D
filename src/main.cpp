#include "sceneGraph.h"
#include "graphics.h"
#include "Common/meshStorage.cpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <time.h>

Graphics g{1920, 1080, "/home/jhazelden/Cpp/OpenGL/SGV3D/src/Shaders/vert.glsl", "/home/jhazelden/Cpp/OpenGL/SGV3D/src/Shaders/frag.glsl", "SG3D v0.0"};

std::vector<Vertex> cylinder (GLfloat const& dTheta, GLfloat const& rad=1.0f, GLfloat const& h=1.0f)
{
    std::vector<Vertex> vertices;
    std::vector<Vertex> newVerts(12);
    GLfloat x0{rad}, z0{0.0};
    for (GLfloat theta = 0.0; theta < 2*M_PI; theta += dTheta) 
    {
        GLfloat x1{rad*(GLfloat)cos(theta+dTheta)}, z1{rad*(GLfloat)sin(theta+dTheta)}; 
        glm::vec3 nm{glm::vec3((GLfloat)cos(theta+dTheta/2), 0.0, (GLfloat)sin(theta))};
        newVerts = {{{x0,-h/2,z0},nm},{{x1,-h/2,z1},nm},{{x0, h/2,z0},nm},
                    {{x1, h/2,z1},nm},{{x0, h/2,z0},nm},{{x1,-h/2,z1},nm},
                    {{0.0, h/2,0.0},nm},{{x0, h/2,z0},nm},{{x1, h/2,z1},nm},
                    {{0.0,-h/2,0.0},nm},{{x0,-h/2,z0},nm},{{x1,-h/2,z1},nm}};
        vertices.insert(vertices.end(), newVerts.begin(), newVerts.end());
        x0 = x1; z0 = z1;
    }
    return vertices;
}

std::vector<Vertex> flower(GLfloat dTheta)
{
    std::vector<Vertex> vertices;
    std::vector<Vertex> newVerts(3);
    GLfloat x0{0.0}, z0{0.0};
    glm::vec3 nm{glm::vec3(0.0,1.0,0.0)};
    for (GLfloat theta = 0.0; theta < 2*M_PI; theta += dTheta) 
    {
        GLfloat rad{(GLfloat)fabs(cos(theta+dTheta)*sin(theta+dTheta))};
        GLfloat x1{rad*(GLfloat)cos(theta+dTheta)}, z1{rad*(GLfloat)sin(theta+dTheta)}; 
        newVerts = {{{0.0,0.0,0.0},nm},{{x0,0.1f*fabs(sin(4*theta)),z0},nm},{{x1,fabs(0.1f*sin(4*(theta+dTheta))),z1},nm}};
        vertices.insert(vertices.end(), newVerts.begin(), newVerts.end());
        x0 = x1; z0 = z1;
    }
    return vertices;
}

extern std::ifstream audiofl;
std::ifstream audiofl;
extern glm::mat4x4 matPrev;
glm::mat4x4 matPrev = glm::mat4x4(1.0f);

extern glm::mat4x4 matPrev2;
glm::mat4x4 matPrev2 = glm::mat4x4(1.0f);

std::pair<GLfloat,GLfloat> sampleAudio (double const& t)
{
    char bytes[4];
    audiofl.seekg(44+4*long(44100*t));
    audiofl.read(bytes,4);
    return {((bytes[1]<<8)|bytes[0])/32768.0,((bytes[3]<<8)|bytes[2])/32768.0};
}

glm::mat4x4 rotTrans(double& t)
{
//    t = glfwGetTime();
    auto bytes = sampleAudio(t);
    glm::mat4x4 rotMat{glm::rotate(0.5f*glm::radians((GLfloat)bytes.second+(GLfloat)bytes.first), glm::vec3(0.1f,1.0f,0.0f))};
    glm::mat4x4 retMat{matPrev2 * rotMat};
    matPrev2 = retMat;
    return retMat;
}

glm::mat4x4 rotTrans2(double& t)
{
    auto bytes = sampleAudio(t);
    glm::mat4x4 retMat{matPrev
                      *glm::rotate(5.0f*glm::radians((GLfloat)bytes.first), glm::vec3(0.0f,0.0f,1.0f))
                      *glm::rotate(5.0f*glm::radians((GLfloat)bytes.second), glm::vec3(1.0f,0.0f,0.0f))};
    return retMat;
}

void tree (SceneGraph& sg, unsigned const& order, GLfloat const& scalar, GLfloat const& rot, GLfloat const& initLen=1.0f, GLfloat const& dTheta=M_PI/6)
{
    glm::vec3 branchBrown{0.34f,0.17f,0.07f}, branchGreen{0.42f,0.56f,0.14f}, flowPink{1.0f,0.07f,0.58f};
    std::vector<Vertex> branchVerts{cylinder(dTheta, 0.3f, initLen*2.0f)};
    std::vector<Vertex> flowVerts{flower(M_PI/10)};
    Mesh branchMesh{branchVerts};
    GraphMesh branchGmesh{sg.bindMesh(branchMesh)};
    Mesh flowMesh{flowVerts};
    GraphMesh flowGmesh{sg.bindMesh(flowMesh)};

    ObjectNode* branchNode1{new ObjectNode(branchGmesh)};
    ObjectNode* branchNode2{new ObjectNode(branchGmesh)};
    ObjectNode* flowNode{new ObjectNode(flowGmesh)};

    ColorNode* flowColorNode{new ColorNode(flowPink,2)};
    TransformNode* flowOffsetNode{new TransformNode(glm::scale(2.0f*glm::vec3(1.0f))*glm::rotate((GLfloat)M_PI/3,glm::vec3(0.0,0.0,1.0))*glm::translate(glm::vec3(0.5f,initLen/2,0.0f)))};

    branchNode2->addChild(flowColorNode);
    flowColorNode->addChild(flowOffsetNode);
    flowOffsetNode->addChild(flowNode);

    ColorNode* branchColorNode{new ColorNode(branchBrown,2)};
    TransformNode* initTnode{new TransformNode(glm::mat4x4(1.0f))};
    DynamicInputTransformNode* initDitnode{new DynamicInputTransformNode(rotTrans)};
    
    branchColorNode->addChild(initTnode);
    initTnode->addChild(initDitnode);
    initDitnode->addChild(branchNode1);
    sg.setRoot(branchColorNode);
    if (order == 0)
        return;
    std::vector<TransformNode*> layer0 {initTnode};
    for (unsigned i = 1; i <= order; ++i) 
    {
        ObjectNode*& branchNode{i != order-1 ? branchNode1 : branchNode2};
        int rands[4];
        if (i <= 4)
            for (int i = 0; i < 4; ++i) {rands[i] = 1;}
        else 
            for (int i = 0; i < 4; ++i) {rands[i] = rand()%(i+1);}
        std::vector<TransformNode*> layer1; 
        for (auto& tNode0: layer0)
        {
            for (int j = 0; j < 4; ++j)
            {
                GLfloat rotRand{(GLfloat)M_PI/((GLfloat)fmod(rand(),4.0f)+3.0f)};
                if (rands[j]) //Note: we use ! here because we want a lower prob. of getting a branch
                {
                    glm::mat4x4 rotate1{glm::rotate((GLfloat)pow(-1.0,j)*rotRand,j < 2 ? glm::vec3(0,0,1) : glm::vec3(1,0,0))},
                                rotate2{glm::rotate((GLfloat)pow(-1.0,j+1)*rotRand,j < 2 ? glm::vec3(0,0,1) : glm::vec3(1,0,0))};                        
                    DynamicInputTransformNode* ditNode{i <= 2 ? new DynamicInputTransformNode(rotTrans) : new DynamicInputTransformNode(rotTrans2)};
                    ColorNode* cNode{new ColorNode(glm::mix(branchBrown,branchGreen,i/GLfloat(order)),2)};
                    TransformNode* tNode1{new TransformNode(glm::scale(scalar*glm::vec3(1.0f))
                                                           *rotate1
                                                           *glm::translate(glm::mat3x3(rotate2*glm::scale(1./scalar*glm::vec3(1.0f)))*glm::vec3(pow(-1.0f,j+1)*(sin(rotRand)+0.5f),cos(rotRand)+initLen,0.0f)))};
                    
                    tNode0->addChild(cNode);
                    cNode->addChild(ditNode);
                    ditNode->addChild(tNode1);
                    tNode1->addChild(branchNode);
                    layer1.push_back(tNode1);
                }
            }
        }
        layer0 = layer1;
    }
}

int main (int argc, char* argv[])
{
    if (argc != 2)
        exit(0);
    srand(time(NULL));
    audiofl.open(std::string(argv[1]), std::ifstream::in | std::ifstream::binary);
    g.setCam(glm::vec3(0,0,4.5), glm::vec3(0,0,-1));
    g.setCamTransforms(glm::perspective(glm::radians(45.0f),(GLfloat)1920/1080, 0.1f, 200.0f),
                       glm::mat4x4(1.0f));
    g.setCallbackAndFlags();

    GLuint shaderProgram = g.getShaderProgram();
    GLchar const* names [3]{"mMat","mini","maxi"};
    std::vector<SceneGraph> sgs(3,{nullptr, shaderProgram, true, names}); 
	tree(sgs[0], 6, 1./1.5, M_PI/5, 3.0f, M_PI/4);
    tree(sgs[1], 6, 1./1.5, M_PI/5, 2.5f, M_PI/4);
    tree(sgs[2], 6, 1./1.5, M_PI/5, 2.5f, M_PI/4);

//    sg.setGlobalCenter();
//    sg.setGlobalMinMax();

    TransformNode* tNode1{new TransformNode(glm::translate(glm::vec3(15.0f, 0.0f, 0.0f)))};
    TransformNode* tNode2{new TransformNode(glm::translate(glm::vec3(-15.0f, 0.0f, 0.0f)))};
    tNode1->addChild(sgs[1].getRoot());
    sgs[1].setRoot(tNode1);
    tNode2->addChild(sgs[2].getRoot());
    sgs[2].setRoot(tNode2);
    g.setCamSpeed(200.0f, 1.0f);
    g.addGraph(sgs[0]);
    g.addGraph(sgs[1]);
    g.addGraph(sgs[2]);
    g.loop();
}
