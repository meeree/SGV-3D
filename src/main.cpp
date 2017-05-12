#include "sceneGraph.h"
#include "graphics.h"
#include "Common/meshStorage.cpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#include <fstream>
#include <iostream>

GLuint loadInShader(char const *fname, GLenum const &shaderType) { /* Called by shaders function */
    std::vector<char> buffer;
    std::ifstream in;
    in.open(fname, std::ios::binary);

    if(in.is_open()) 
    {
        in.seekg(0, std::ios::end);
        size_t const &length = in.tellg();

        in.seekg(0, std::ios::beg);

        buffer.resize(length + 1);
        in.read(&buffer[0], length);
        in.close();
        buffer[length] = '\0';
    } else 
    {
        std::cerr<<"Unable to open "<<fname<<std::endl;
        exit(-1);
    }

    GLchar const *src = &buffer[0];

    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    GLint test;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &test);

    if(!test) 
    {
        std::cerr<<"Shader compilation failed with this message:"<<std::endl;
        std::vector<char> compilationLog(512);
        glGetShaderInfoLog(shader, compilationLog.size(), NULL, &compilationLog[0]);
        std::cerr<<&compilationLog[0]<<std::endl;
        glfwTerminate();
        exit(-1);
    }

    return shader;
}

void mouseButtonCallback (GLFWwindow*, int, int, int)
{
}

Graphics g{1920, 1080, "/home/jhazelden/Cpp/OpenGL/SGV3D/src/Shaders/vert.glsl", "/home/jhazelden/Cpp/OpenGL/SGV3D/src/Shaders/frag.glsl", "SG3D v0.0"};

void keyCallback(GLFWwindow*, int key, int, int action, int)
{   
    if (key == GLFW_KEY_K && action == GLFW_PRESS)
    {
        g.incColorSceme();
    }
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
	    glfwTerminate(); 
        exit(EXIT_SUCCESS);
    }
    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
    {
        g.scale(11.0f/10);
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
    {
        g.scale(10.0f/11);
    }
//    if (key == GLFW_KEY_X && action == GLFW_PRESS)
//    {
//        g.toggleAxes();
//    }
}

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

    glEnable(GL_BLEND); glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);
    glfwSetKeyCallback(g.getWindow(), keyCallback);
    glfwSetMouseButtonCallback(g.getWindow(), mouseButtonCallback);
    glfwSetInputMode(g.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(g.getWindow(), GLFW_STICKY_KEYS, false);

    GLuint shaderProgram = g.getShaderProgram();
    GLchar const* names [3]{"mMat","mini","maxi"};
    SceneGraph sg {nullptr, shaderProgram, true, names}; 

    std::vector<Vertex> vertices;
    meshIO::readFile("../Tests/asteroid_dual_contouring.vmesh", vertices); 

    std::vector<glm::mat4x4> modMats{ 
        glm::scale(glm::mat4x4(1.0f), glm::vec3(1.5f)),
        glm::scale(glm::translate(glm::mat4x4(1.0f), glm::vec3(-13.0f,-13.0f,0.0f)), glm::vec3(0.6f)),
        glm::scale(glm::translate(glm::mat4x4(1.0f), glm::vec3(13.0f,13.0f,0.0f)), glm::vec3(0.8f)),

        glm::scale(glm::translate(glm::mat4x4(1.0f), glm::vec3(0,0,20)+glm::vec3(0.0f,0.0f,0.0f)), glm::vec3(0.5f)),
        glm::scale(glm::translate(glm::mat4x4(1.0f), glm::vec3(0,0,20)+glm::vec3(-13.0f,-13.0f,0.0f)), glm::vec3(0.4f)),
        glm::scale(glm::translate(glm::mat4x4(1.0f), glm::vec3(0,0,20)+glm::vec3(13.0f,13.0f,0.0f)), glm::vec3(0.8f))};

    std::vector<Mesh> meshes(6, {vertices, GL_TRIANGLES});

    std::vector<GraphMesh> gmeshes;
    for (auto &mesh: meshes)
        gmeshes.push_back(sg.bindMesh(mesh));

    std::vector<ObjectNode*> objectNodes (6);
    std::transform(gmeshes.begin(), gmeshes.end(), objectNodes.begin(), [](GraphMesh const& gmesh){return new ObjectNode(gmesh);});
    std::vector<DynamicInputTransformNode*> ditNodes{
        {new DynamicInputTransformNode(calcTrans1)}, {new DynamicInputTransformNode(calcTrans2)}, {new DynamicInputTransformNode(calcTrans2)}, {new DynamicInputTransformNode(calcTrans2)}, {new DynamicInputTransformNode(calcTrans2)}};
    std::vector<TransformNode*> tNodes (modMats.size());
    std::transform(modMats.begin(), modMats.end(), tNodes.begin(), [](glm::mat4x4 const& modMat){return new TransformNode(modMat);});
    for (auto& objectNode: objectNodes)
    {
        objectNode->toggleDraw();
    }

    ditNodes[0]->addChild(tNodes[0]);
    tNodes[0]->addChild(objectNodes[0]);
    objectNodes[0]->addChild(ditNodes[1]);
    ditNodes[1]->addChild(tNodes[1]);
    tNodes[1]->addChild(objectNodes[1]);
    objectNodes[1]->addChild(tNodes[2]);
    tNodes[2]->addChild(objectNodes[2]);

    ditNodes[0]->addChild(tNodes[3]);
    tNodes[3]->addChild(objectNodes[3]);
    objectNodes[3]->addChild(ditNodes[2]);
    ditNodes[2]->addChild(tNodes[4]);
    tNodes[4]->addChild(objectNodes[4]);
    objectNodes[4]->addChild(tNodes[5]);
    tNodes[5]->addChild(objectNodes[5]);

    sg.setRoot(ditNodes[0]);

    sg.setGlobalCenter();
    sg.setGlobalMinMax();
    g.addGraph(sg);
    g.loop();
}

// DELETE ME
//		{{-1.0f,-1.0f,-1.0f,}, {-1.0f,0.0f,0.0f}},
//		{{-1.0f,-1.0f, 1.0f,}, {-1.0f,0.0f,0.0f}},
//		{{-1.0f, 1.0f, 1.0f,}, {-1.0f,0.0f,0.0f}},
//		{{1.0f, 1.0f,-1.0f, }, {0.0f,0.0f,-1.0f}},
//		{{-1.0f,-1.0f,-1.0f,}, {0.0f,0.0f,-1.0f}},
//		{{-1.0f, 1.0f,-1.0f,}, {0.0f,0.0f,-1.0f}},
//		{{1.0f,-1.0f, 1.0f, }, {0.0f,-1.0f,0.0f}},
//		{{-1.0f,-1.0f,-1.0f,}, {0.0f,-1.0f,0.0f}},
//		{{1.0f,-1.0f,-1.0f, }, {0.0f,-1.0f,0.0f}},
//		{{1.0f, 1.0f,-1.0f, }, {0.0f,0.0f,-1.0f}},
//		{{1.0f,-1.0f,-1.0f, }, {0.0f,0.0f,-1.0f}},
//		{{-1.0f,-1.0f,-1.0f,}, {0.0f,0.0f,-1.0f}},
//		{{-1.0f,-1.0f,-1.0f,}, {-1.0f,0.0f,0.0f}},
//		{{-1.0f, 1.0f, 1.0f,}, {-1.0f,0.0f,0.0f}},
//		{{-1.0f, 1.0f,-1.0f,}, {-1.0f,0.0f,0.0f}},
//		{{1.0f,-1.0f, 1.0f, }, {0.0f,-1.0f,0.0f}},
//		{{-1.0f,-1.0f, 1.0f,}, {0.0f,-1.0f,0.0f}},
//		{{-1.0f,-1.0f,-1.0f,}, {0.0f,-1.0f,0.0f}},
//		{{-1.0f, 1.0f, 1.0f,}, {0.0f,0.0f,1.0f}},
//		{{-1.0f,-1.0f, 1.0f,}, {0.0f,0.0f,1.0f}},
//		{{1.0f,-1.0f, 1.0f, }, {0.0f,0.0f,1.0f}},
//		{{1.0f, 1.0f, 1.0f, }, {1.0f,0.0f,0.0f}},
//		{{1.0f,-1.0f,-1.0f, }, {1.0f,0.0f,0.0f}},
//		{{1.0f, 1.0f,-1.0f, }, {1.0f,0.0f,0.0f}},
//		{{1.0f,-1.0f,-1.0f, }, {1.0f,0.0f,0.0f}},
//		{{1.0f, 1.0f, 1.0f, }, {1.0f,0.0f,0.0f}},
//		{{1.0f,-1.0f, 1.0f, }, {1.0f,0.0f,0.0f}},
//		{{1.0f, 1.0f, 1.0f, }, {0.0f,1.0f,0.0f}},
//		{{1.0f, 1.0f,-1.0f, }, {0.0f,1.0f,0.0f}},
//		{{-1.0f, 1.0f,-1.0f,}, {0.0f,1.0f,0.0f}},
//		{{1.0f, 1.0f, 1.0f, }, {0.0f,1.0f,0.0f}},
//		{{-1.0f, 1.0f,-1.0f,}, {0.0f,1.0f,0.0f}},
//		{{-1.0f, 1.0f, 1.0f,}, {0.0f,1.0f,0.0f}},
//		{{1.0f, 1.0f, 1.0f, }, {0.0f,0.0f,1.0f}},
//		{{-1.0f, 1.0f, 1.0f,}, {0.0f,0.0f,1.0f}},
//		{{1.0f,-1.0f, 1.0f  }, {0.0f,0.0f,1.0f}}};
