#include "runtimeOptions.h"
#include "graphics.h"
#include "sceneGraph.h"
#include "../../Common/meshStorage.cpp"

#include <cstdlib>
#include <algorithm>
#include <iostream>

#define GLM_FORCE_RADIANS
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define SLOW 1.0f

#ifdef USE_NORMALS
void SimpleVertexNormals (std::vector<Vertex>& vertices, bool const& normalize=true)
{
    for (unsigned i = 0; i < vertices.size(); i += 3)
    {
        Vertex& v1{vertices[i]};    
        Vertex& v2{vertices[i+1]};    
        Vertex& v3{vertices[i+2]};    

        glm::vec3 nm{glm::cross(v2.pos-v1.pos, v3.pos-v1.pos)};
        if (length(nm) < 0.00000001)
        {
            v1.normal.x = nanf("");
            v2.normal.x = nanf("");
            v3.normal.x = nanf("");
            continue;
        }
        if (normalize)
            nm = glm::normalize(nm);
        
        v1.normal = nm;
        v2.normal = nm;
        v3.normal = nm;
    }
    vertices.erase(std::remove_if(vertices.begin(), vertices.end(), [](Vertex const& v){return std::isnan(v.pos.x);}), vertices.end()); 
}
#endif

#define SCALE(t) glm::scale(glm::vec3((t)))

class CustomAnimationNode final : public AnimationNode
{
public:
    glm::vec2 m_sclrs;
    double m_startTime;
    CustomAnimationNode (glm::vec2 sclrs, double startTime) : m_sclrs{sclrs}, m_startTime{startTime} {}
    static std::ifstream ms_wavStream;
    virtual glm::mat4x4 animate (double const& t) override;
};

glm::mat4x4 CustomAnimationNode::animate (double const& t)
{
    char bytes[4];
    ms_wavStream.seekg(44+4*long(44100*(m_startTime+glfwGetTime())/SLOW));
    ms_wavStream.read(bytes,4);
    double channel = ((bytes[1]<<8)|bytes[0])/32768.0;

    glUniform1f(12, channel);

//    channel = sin(channel);
//    channel = 0.05 * channel * (100.0 - m_x);
    channel = (exp(channel) - 1) / 1.71828182846;
    channel *= 20.0f;

    glUniform1i(13, 0);

    return glm::translate(glm::vec3(channel*m_sclrs.x, 0.0f, channel*m_sclrs.y));
}

class CustomAnimationNode2 final : public AnimationNode
{
public:
    glm::vec2 m_sclrs;
    double m_startTime;
    CustomAnimationNode2 (glm::vec2 sclrs, double startTime) : m_sclrs{sclrs}, m_startTime{startTime} {}
    static std::ifstream ms_wavStream;
    virtual glm::mat4x4 animate (double const& t) override;
};

glm::mat4x4 CustomAnimationNode2::animate (double const& t)
{
    char bytes[4];
    ms_wavStream.seekg(44+4*long(44100*(m_startTime+glfwGetTime())/SLOW));
    ms_wavStream.read(bytes,4);
    double channel = ((bytes[3]<<8)|bytes[2])/32768.0;

    glUniform1f(12, channel);

//    channel = log(channel);
//    channel = 0.05 * channel * (100.0 - m_x);
    channel = (exp(channel) - 1) / 1.71828182846;
    channel *= 20.0f;

    glUniform1i(13, 1);

    return glm::translate(glm::vec3(channel*m_sclrs.x, 0.0f, channel*m_sclrs.y));
}

void GetBounds (std::vector<Vertex> const& vertices, glm::vec2& xBound, glm::vec2& yBound, glm::vec2& zBound)
{
    xBound = {FLT_MAX, FLT_MIN};
    yBound = {FLT_MAX, FLT_MIN};
    zBound = {FLT_MAX, FLT_MIN};

    for(auto& vert: vertices)
    {
        if(vert.pos.x < xBound.x)
            xBound.x = vert.pos.x;
        else if(vert.pos.x > xBound.y)
            xBound.y = vert.pos.x;

        if(vert.pos.y < yBound.x)
            yBound.x = vert.pos.y;
        else if(vert.pos.y > yBound.y)
            yBound.y = vert.pos.y;

        if(vert.pos.z < zBound.x)
            zBound.x = vert.pos.z;
        else if(vert.pos.z > zBound.y)
            yBound.y = vert.pos.z;
    }
}

#define PRESS(key_code) (key == key_code && action == GLFW_PRESS)

void KeyCallback(GLFWwindow* window, int key, int, int action, int)
{   
    GLFWContext* windowPtr{reinterpret_cast<GLFWContext*>(glfwGetWindowUserPointer(window))};

    if(PRESS(GLFW_KEY_UP))
    {
        windowPtr->Info().ScaleUp(11.0f/10.0f);
        windowPtr->Info().UpdateScalarUniform(6);
    }

    else if(PRESS(GLFW_KEY_DOWN))
    {
        windowPtr->Info().ScaleDown(11.0f/10.0f);
        windowPtr->Info().UpdateScalarUniform(6);
    }
}

std::ifstream CustomAnimationNode::ms_wavStream;
std::ifstream CustomAnimationNode2::ms_wavStream;

int main (int argc, char** argv) 
{
    const char* logFileName{"SGV3D_Log.txt"};
    if(!Logger::singleton().init(logFileName))
    {
        std::cerr<<"Failed to initialize logger"<<std::endl;
        exit(0);
    }
    DEBUG_MSG("Beginning main loop");

    CustomAnimationNode::ms_wavStream.open(argv[1]);
    CustomAnimationNode2::ms_wavStream.open(argv[1]);

    std::vector<Vertex> mesh{
//        {{-1.0f, -1.0f, -1.0f}, {-1.0f,  0.0f,  0.0f}},
//        {{-1.0f, -1.0f,  1.0f}, {-1.0f,  0.0f,  0.0f}},
//        {{-1.0f,  1.0f,  1.0f}, {-1.0f,  0.0f,  0.0f}},
//
//        {{-1.0f,  1.0f,  1.0f}, {-1.0f,  0.0f,  0.0f}},
//        {{-1.0f,  1.0f, -1.0f}, {-1.0f,  0.0f,  0.0f}},
//        {{-1.0f, -1.0f, -1.0f}, {-1.0f,  0.0f,  0.0f}},
//
//        {{ 1.0f, -1.0f,  1.0f}, { 1.0f,  0.0f,  0.0f}},
//        {{ 1.0f, -1.0f, -1.0f}, { 1.0f,  0.0f,  0.0f}},
//        {{ 1.0f,  1.0f,  1.0f}, { 1.0f,  0.0f,  0.0f}},
//
//        {{ 1.0f,  1.0f, -1.0f}, { 1.0f,  0.0f,  0.0f}},
//        {{ 1.0f,  1.0f,  1.0f}, { 1.0f,  0.0f,  0.0f}},
//        {{ 1.0f, -1.0f, -1.0f}, { 1.0f,  0.0f,  0.0f}},
//
//        {{-1.0f,  1.0f,  1.0f}, { 0.0f,  1.0f,  0.0f}},
//        {{ 1.0f,  1.0f,  1.0f}, { 0.0f,  1.0f,  0.0f}},
//        {{-1.0f,  1.0f, -1.0f}, { 0.0f,  1.0f,  0.0f}},
//
//        {{ 1.0f,  1.0f, -1.0f}, { 0.0f,  1.0f,  0.0f}},
//        {{-1.0f,  1.0f, -1.0f}, { 0.0f,  1.0f,  0.0f}},
//        {{ 1.0f,  1.0f,  1.0f}, { 0.0f,  1.0f,  0.0f}},
//
//        {{-1.0f, -1.0f,  1.0f}, { 0.0f, -1.0f,  0.0f}},
//        {{ 1.0f, -1.0f,  1.0f}, { 0.0f, -1.0f,  0.0f}},
//        {{-1.0f, -1.0f, -1.0f}, { 0.0f, -1.0f,  0.0f}},
//
//        {{ 1.0f, -1.0f, -1.0f}, { 0.0f, -1.0f,  0.0f}},
//        {{-1.0f, -1.0f, -1.0f}, { 0.0f, -1.0f,  0.0f}},
//        {{ 1.0f, -1.0f,  1.0f}, { 0.0f, -1.0f,  0.0f}},
//
        {glm::vec3(0.0f)},
//        {{ 0.15f, -0.15f,  0.15f}, { 0.0f,  0.0f,  1.0f}},
//        {{ 0.15f,  0.15f,  0.15f}, { 0.0f,  0.0f,  1.0f}},
//        {{-0.15f, -0.15f,  0.15f}, { 0.0f,  0.0f,  1.0f}},
//
//        {{-0.15f,  0.15f,  0.15f}, { 0.0f,  0.0f,  1.0f}},
//        {{-0.15f, -0.15f,  0.15f}, { 0.0f,  0.0f,  1.0f}},
//        {{ 0.15f,  0.15f,  0.15f}, { 0.0f,  0.0f,  1.0f}},
//
//        {{ 1.0f, -1.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}},
//        {{ 1.0f,  1.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}},
//        {{-1.0f, -1.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}},
//
//        {{-1.0f,  1.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}},
//        {{-1.0f, -1.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}},
//        {{ 1.0f,  1.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}},
    };

//    std::vector<Vertex> mesh;
//    if (!MeshIO::ReadRaw("../../Common/asteroid_dual_contouring.vmesh", mesh))
//    {
//       std::cerr<<"File read failed"<<std::endl;
//       exit(0);
//    } 
//    SimpleVertexNormals(mesh);
//    DEBUG_MSG("Loaded vertices");

    static unsigned const k_nPositions{10000};
//    std::vector<Vertex> mesh
//    {
//        {{-(100.0f/k_nPositions), 1.0f, 0.0f}},
//        {{ (100.0f/k_nPositions), 1.0f, 0.0f}},
//    };

    GLFWContext context;
    if(!context.Initailize({4, 5}, 1920.0, 1080.0, true, "Audio Animation with SGV3D!", KeyCallback))
    {
        std::cerr<<"Failed to Initialize GLFWContext!"<<std::endl;
        ERROR("Failed to Initialize GLFWContext!");
        exit(0);
    }

    GLProgram program;
    context.GetNewProgram(program, "../src/Shaders/vert.glsl", "../src/Shaders/frag.glsl");
    GraphMesh gmesh{program.AddMesh(mesh, GL_POINTS)};

    BasicCamera* camera{new BasicCamera()};
    context.SetCamera(camera);
    context.BindProgram(program);

    camera->SetPosition(glm::vec3(1.0f, 10.0f, 0.0f));
    camera->SetDirection(glm::vec3(0.0f, -10.0f, -1.0f));
    camera->SetProjection(45.0f, 1920.0f / 1080.0f, 0.1f, 200.0f);
    camera->UpdateUniforms(8, -1, 3, 4);

    unsigned i{0};
    Node* initTNode{new TransformNode(glm::scale(glm::vec3(0.3f, 1.0f, 0.3f)))};
    Node* initGNode{new GeometryNode(gmesh, program)};

    std::string startTimeStr{argc > 2 ? argv[2] : "0.0"};
    double startTime{std::stof(startTimeStr)};

//    Node* curNode{initTNode};
//    for(; i < 10; ++i)
//    {
//        Node* aNode{new CustomAnimationNode(argv[1])};
//        Node* tNode{new TransformNode(glm::translate(glm::vec3(0.0f, -6.0f, 0.0f))
//                                     *glm::scale(glm::vec3(0.5f))                 )};
//
//        Node* gNode{new GeometryNode(gmesh, program)};
//
//        static_cast<GroupNode*>(curNode)->addChild(gNode);
//        static_cast<GroupNode*>(curNode)->addChild(aNode);
//        static_cast<GroupNode*>(aNode)->addChild(tNode);
//
//        curNode = tNode;
//    }

//    glm::vec3 positions[]{
//        {-10.0f, 0.0f, 0.0f}, {10.0f, 0.0f, 0.0f},
//        {0.0f, -10.0f, 0.0f}, {0.0f, 10.0f, 0.0f},
//        {0.0f, 0.0f, -10.0f}, {0.0f, 0.0f, 10.0f},
//    };

    for(unsigned i = 0; i < k_nPositions; ++i)
    {
//        Node* tNode1{new TransformNode(glm::translate(glm::vec3(-100.0f + (200.0f/k_nPositions)*i, 0.0f, 0.0f)))};
//        Node* tNode2{new TransformNode(glm::translate(glm::vec3(-100.0f + (200.0f/k_nPositions)*i, 0.0f, 0.0f)))};
//        Node* aNode1{new CustomAnimationNode(-100.0f + (200.0f/k_nPositions)*i)};
//        Node* aNode2{new CustomAnimationNode2(-100.0f + (200.0f/k_nPositions)*i)};

        double t{5*2*M_PI*i/(float)k_nPositions};
//        double x_t{16 * sin(t) * sin(t) * sin(t)};
//        double y_t{13 * cos(t) - 5 * cos(2*t) - 2 * cos(3*t) - cos(4*t)};
//        double x_t{cos(t)*(exp(cos(t)) - 2*cos(4*t) - sin(t/12)*5.0)};
//        double y_t{sin(t)*(exp(cos(t)) - 2*cos(4*t) - sin(t/12)*5.0)};
        double a{0.6}, b{1.0};
        double x_t{(a + b) * cos(t) - b * cos((a/b+1)*t)};
        double y_t{(a + b) * sin(t) - b * sin((a/b+1)*t)};
//        double x_t{cos(t)};
//        double y_t{sin(t)};
        x_t /= 3.0f;
        y_t /= 3.0f;

        glm::vec3 pos1{15.0f*x_t, 0.0f, 15.0f*y_t};
        glm::vec3 pos2{30.0f*x_t, 0.0f, 30.0f*y_t};
        Node* tNode1{new TransformNode(glm::translate(pos1))};
        Node* tNode2{new TransformNode(glm::translate(pos2))};
        Node* aNode1{new CustomAnimationNode({0.5f*x_t, 0.5f*y_t}, startTime)};
        Node* aNode2{new CustomAnimationNode2({x_t, y_t}, startTime)};
        Node* gNode1{new GeometryNode(gmesh, program)};
        Node* gNode2{new GeometryNode(gmesh, program)};

        static_cast<GroupNode*>(initTNode)->addChild(tNode1);
        static_cast<GroupNode*>(tNode1)->addChild(aNode1);
        static_cast<GroupNode*>(aNode1)->addChild(gNode1);

        static_cast<GroupNode*>(initTNode)->addChild(tNode2);
        static_cast<GroupNode*>(tNode2)->addChild(aNode2);
        static_cast<GroupNode*>(aNode2)->addChild(gNode2);
    }

    glUniformMatrix4fv(5, 1, GL_FALSE, glm::value_ptr(glm::mat4x4(1.0f)));
//    glUniformMatrix4fv(4, 1, GL_FALSE, glm::value_ptr(glm::mat4x4(1.0f)));
//    glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(glm::mat4x4(1.0f)));
//    glUniform1ui(7, 0);
    glUniform1f(6, 1.0f);
    auto pMat = glm::perspective(glm::radians(45.0f),(GLfloat)1920/1080, 0.1f, 50.0f);
    glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(pMat));

    context.SetRoot(initTNode);
    i = 0;

    //Play wav file
    std::cout<<"Playing wav \""<<argv[1]<<"\" at time "<<startTimeStr<<std::endl;
    std::string sysCmd{"play -r " + std::to_string(long(44100/SLOW)) + " " + std::string(argv[1]) + " trim " + startTimeStr  + "&"};
    system(sysCmd.c_str());

//    glEnable(GL_CULL_FACE); glCullFace(GL_BACK);
    glm::mat4x4 viewMat{glm::lookAt(glm::vec3(0.0f, 30.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f))};
    float sclr{1.0f};
    
    glUniformMatrix4fv(4, 1, GL_FALSE, glm::value_ptr(viewMat));
    glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(pMat));

    for(;;)
    {
//        GLfloat color[4]{0.4f*(GLfloat)fabs(sin(10*glfwGetTime())), 0.4f*(GLfloat)fabs(cos(10*glfwGetTime())), 0.0f, 1.0f};

        context.Render(program.Strip());
        glUniform1f(6, 10.0f);
    }
}
