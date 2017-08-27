#include "../../src/sgv_graphics.h"
#include <iostream>

#define PRESS(key_code) (key == key_code && action == GLFW_PRESS)

void KeyCallback(GLFWwindow* window, int key, int, int action, int)
{   
    GLFWContext* windowPtr{reinterpret_cast<GLFWContext*>(glfwGetWindowUserPointer(window))};

    if(PRESS(GLFW_KEY_ESCAPE))
        windowPtr->Done();
}


class RotationNode final : public AnimationNode
{
private:
    GLfloat m_speed;
public:
    RotationNode (GLfloat const& speed) : m_speed{speed} {}
    virtual glm::mat4x4 animate (double const& t) override;
};

glm::mat4x4 RotationNode::animate (double const& t)
{
    return glm::rotate(m_speed * (GLfloat)t, glm::vec3(1.0f, 1.0f, 0.0f));
}

int main () 
{
    //Start the logger 
    const char* logFileName{"SGV3D_Log.txt"};
    if(!Logger::singleton().init(logFileName))
    {
        std::cerr<<"Failed to initialize logger"<<std::endl;
        exit(0);
    }
    DEBUG_MSG("Beginning main loop");

    SGVGraphics sgv;
    if(!sgv.Initailize(1920.0, 1080.0, true, KeyCallback))
    {
        std::cerr<<"Failed to Initialize GLFWContext!"<<std::endl;
        ERROR("Failed to Initialize GLFWContext!");
        exit(0);
    }

    //Create a new shader program
    GLProgram program;
    sgv.GetNewProgram(program, "../../Shaders/vert.glsl", "../../Shaders/frag.glsl", (SGV_POSITION | SGV_NORMAL), true);
    sgv.BindProgram(program);

    Mesh mesh;
    mesh.positions = {
        {-1.0f, -1.0f, -1.0f},
        {-1.0f, -1.0f,  1.0f},
        {-1.0f,  1.0f,  1.0f},

        {-1.0f,  1.0f,  1.0f},
        {-1.0f,  1.0f, -1.0f},
        {-1.0f, -1.0f, -1.0f},

        { 1.0f, -1.0f,  1.0f},
        { 1.0f, -1.0f, -1.0f},
        { 1.0f,  1.0f,  1.0f},

        { 1.0f,  1.0f, -1.0f},
        { 1.0f,  1.0f,  1.0f},
        { 1.0f, -1.0f, -1.0f},

        {-1.0f,  1.0f,  1.0f},
        { 1.0f,  1.0f,  1.0f},
        {-1.0f,  1.0f, -1.0f},

        { 1.0f,  1.0f, -1.0f},
        {-1.0f,  1.0f, -1.0f},
        { 1.0f,  1.0f,  1.0f},

        {-1.0f, -1.0f,  1.0f},
        { 1.0f, -1.0f,  1.0f},
        {-1.0f, -1.0f, -1.0f},

        { 1.0f, -1.0f, -1.0f},
        {-1.0f, -1.0f, -1.0f},
        { 1.0f, -1.0f,  1.0f},

        { 1.0f, -1.0f,  1.0f},
        { 1.0f,  1.0f,  1.0f},
        {-1.0f, -1.0f,  1.0f},

        {-1.0f,  1.0f,  1.0f},
        {-1.0f, -1.0f,  1.0f},
        { 1.0f,  1.0f,  1.0f},

        { 1.0f, -1.0f, -1.0f},
        { 1.0f,  1.0f, -1.0f},
        {-1.0f, -1.0f, -1.0f},

        {-1.0f,  1.0f, -1.0f},
        {-1.0f, -1.0f, -1.0f},
        { 1.0f,  1.0f, -1.0f},
    };

    mesh.normals = {
        {-1.0f,  0.0f,  0.0f},
        {-1.0f,  0.0f,  0.0f},
        {-1.0f,  0.0f,  0.0f},
                              
        {-1.0f,  0.0f,  0.0f},
        {-1.0f,  0.0f,  0.0f},
        {-1.0f,  0.0f,  0.0f},
                              
        { 1.0f,  0.0f,  0.0f},
        { 1.0f,  0.0f,  0.0f},
        { 1.0f,  0.0f,  0.0f},
                              
        { 1.0f,  0.0f,  0.0f},
        { 1.0f,  0.0f,  0.0f},
        { 1.0f,  0.0f,  0.0f},
                              
        { 0.0f,  1.0f,  0.0f},
        { 0.0f,  1.0f,  0.0f},
        { 0.0f,  1.0f,  0.0f},
                              
        { 0.0f,  1.0f,  0.0f},
        { 0.0f,  1.0f,  0.0f},
        { 0.0f,  1.0f,  0.0f},
                              
        { 0.0f, -1.0f,  0.0f},
        { 0.0f, -1.0f,  0.0f},
        { 0.0f, -1.0f,  0.0f},
                              
        { 0.0f, -1.0f,  0.0f},
        { 0.0f, -1.0f,  0.0f},
        { 0.0f, -1.0f,  0.0f},
                              
        { 0.0f,  0.0f,  1.0f},
        { 0.0f,  0.0f,  1.0f},
        { 0.0f,  0.0f,  1.0f},
                              
        { 0.0f,  0.0f,  1.0f},
        { 0.0f,  0.0f,  1.0f},
        { 0.0f,  0.0f,  1.0f},
                              
        { 0.0f,  0.0f, -1.0f},
        { 0.0f,  0.0f, -1.0f},
        { 0.0f,  0.0f, -1.0f},
                              
        { 0.0f,  0.0f, -1.0f},
        { 0.0f,  0.0f, -1.0f},
        { 0.0f,  0.0f, -1.0f},
    };

    GraphMesh gmesh{program.AddMesh(mesh, GL_TRIANGLES)};

    //Set root scene graph node
    RotationNode* root{new RotationNode(0.5f)};
    GeometryNode* gNode{new GeometryNode(gmesh)};
    sgv.SetRoot(root);
    root->addChild(gNode);

    sgv.DisableCursor();

    glm::mat4x4 projection = glm::perspective(glm::radians(45.0f), 1920.0f/1080.0f, 0.1f, 200.0f);
    glm::vec3 pos{0.0f, 0.0f, 10.0f};
    glm::mat4x4 view{glm::lookAt(pos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f))};

    glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(4, 1, GL_FALSE, glm::value_ptr(view));
//    glUniformMatrix4fv(5, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1f(6, 1.0f);
    glUniform3fv(7, 1, glm::value_ptr(pos));

    FreeRoamCamera camera(4.0f, 1.0f);
    camera.SetPosition(pos);
    camera.SetDirection(glm::vec3(0.0f, 0.0f, -1.0f));
    camera.SetProjection(45.0f, 1920.0f/1080.0f, 0.1f, 50.0f);
    sgv.SetCamera(camera);

    bool cont{true};
    while(cont)
    {
        cont = sgv.Render(program.Strip());
    }

    delete gNode;
    delete root;
}
