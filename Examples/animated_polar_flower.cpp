

//\\\\\\\\\\\\\\\\\\\\!!!USAGE!!!\\\\\\\\\\\\\\\\\\\\\\\\//
//Run by typing "./flower [step] [petals]" where step is //
//the increment for the parameter 't' below and petals   //
//is the number of petals of the flower.                 //
//By default, step is 0.01 and petals is 100             //
//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//


//Includes
#include "../src/runtimeOptions.h"
#include "../src/graphics.h"
#include "../src/sceneGraph.h"

#include <cstdlib>
#include <algorithm>
#include <iostream>

#define GLM_FORCE_RADIANS
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Construct flower polar graph to be displayed using GL_LINES
Mesh FlowerLines (double const& dt, glm::vec4 const& color, unsigned const& numPetals)
{
    Mesh mesh;
    for(double t1 = 0.0, t2 = dt; t1 <= 1.0f + 1e-9f; t1 += dt, t2 += dt)
    {
        double theta1{2*M_PI*t1}, theta2{2*M_PI*t2};
        double r1{sin(theta1)*cos(theta1)}, r2{sin(theta2)*cos(theta2)};

        mesh.positions.push_back(glm::vec3(r1 * cos(theta1), r1 * sin(theta1), 0.0f));
        mesh.positions.push_back(glm::vec3(r2 * cos(theta2), r2 * sin(theta2), 0.0f));
    }
    mesh.colors.insert(mesh.colors.end(), 2*numPetals, color);

    return mesh;
}

//Construct flower polar graph to be displayed using GL_POINTS
Mesh FlowerPoints (double const& dt, glm::vec4 const& color, unsigned const& numPetals)
{
    Mesh mesh;
    for(double t1 = 0.0; t1 <= 1.0f + 1e-9f; t1 += dt)
    {
        double theta1{2*M_PI*t1};
        double r1{sin(theta1)*cos(theta1)};

        mesh.positions.push_back(glm::vec3(r1 * cos(theta1), r1 * sin(theta1), 0.0f));
    }
    mesh.colors.insert(mesh.colors.end(), numPetals, color);

    return mesh;
}

class CustomAnimationNode final : public AnimationNode
{
public:
    double percent;
    virtual glm::mat4x4 animate (double const& t) override;
};

glm::mat4x4 CustomAnimationNode::animate (double const& t) 
{
    //Scale and rotate based on percentage of this petal to total petals and time
    GLfloat k1 = 1.0f * (sin(6.0f*percent*t));
    GLfloat k2 = 0.5f * (sin(6.0f*(percent*t)));
    return glm::rotate(k1, glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale((0.7+k2)* glm::vec3(1.0f, 1.0f, 0.0f));
}

int main (int argc, char** argv) 
{
    //Start the logger 
    const char* logFileName{"SGV3D_Log.txt"};
    if(!Logger::singleton().init(logFileName))
    {
        std::cerr<<"Failed to initialize logger"<<std::endl;
        exit(0);
    }
    DEBUG_MSG("Beginning main loop");

    //Initalize our GLFWcontext with version, window width and height, title, etc.
    GLFWContext context;
    if(!context.Initailize({4, 5}, 1080.0, 1080.0, true, "Parmetric Plotter with SGV3D!"))
    {
        std::cerr<<"Failed to Initialize GLFWContext!"<<std::endl;
        ERROR("Failed to Initialize GLFWContext!");
        exit(0);
    }

    //Create a new shader program
    GLProgram program;
    context.GetNewProgram(program, "../Shaders/basic2d_vert.glsl", "../Shaders/basic2d_frag.glsl", (SGV_POSITION | SGV_COLOR));
    context.BindProgram(program);

    //Set root scene graph node
    GroupNode* root{new GroupNode};
    context.SetRoot(root);

    //Set polar plot step and number of flower petals
    double step{0.01};
    if(argc > 1)
        step = std::stof(argv[1]);
    unsigned numPetals{100};
    if(argc > 2)
        numPetals = std::stoi(argv[2]);

    for(unsigned i = 0; i < numPetals; ++i)
    {
        //Create flower mesh and add it to shader program
        GLfloat p{i / (GLfloat)numPetals};
        Mesh mesh{FlowerLines(step, {p, 0.0f, 1.0f, 1.0f}, numPetals)};
        GraphMesh gmesh{program.AddMesh(mesh, GL_LINES)};

        //Create nodes
        TransformNode* tNode{new TransformNode(glm::rotate(p * (GLfloat)M_PI/4.0f, glm::vec3(0.0f, 0.0f, 1.0f)))};
        GeometryNode* gNode{new GeometryNode(gmesh)};
        CustomAnimationNode* aNode{new CustomAnimationNode};
        aNode->percent = p;

        //Hook up nodes
        root->addChild(tNode);
        tNode->addChild(aNode);
        aNode->addChild(gNode);
    }

    //Set openGL point size
    glPointSize(3.0f);

    //Continuously render opengl 
    for(;;)
    {
        context.Render(program.Strip(), {0.0f, 0.3f, 0.0f, 1.0f});
    }
}
