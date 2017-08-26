#include "../src2/runtimeOptions.h"
#include "../src2/graphics.h"
#include "../src2/sceneGraph.h"

#include <cstdlib>
#include <algorithm>
#include <iostream>

#define GLM_FORCE_RADIANS
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define PRESS(key_code) (key == key_code && action == GLFW_PRESS)

void KeyCallback(GLFWwindow* window, int key, int, int action, int)
{   
}

int main (int argc, char** argv) 
{
    const char* logFileName{"SGV3D_Log.txt"};
    if(!Logger::singleton().init(logFileName))
    {
        std::cerr<<"Failed to initialize logger"<<std::endl;
        exit(0);
    }
    DEBUG_MSG("Beginning main loop");

    #define STEP   0.01
    #define X_MIN -1.0
    #define X_MAX  1.0
    #define Y_MIN -1.0
    #define Y_MAX  1.0

    double step{argc > 1 ? std::stof(argv[1]) : STEP },
           xMin{argc > 2 ? std::stof(argv[2]) : X_MIN},
           xMax{argc > 3 ? std::stof(argv[3]) : X_MAX},
           yMin{argc > 4 ? std::stof(argv[4]) : Y_MIN},
           yMax{argc > 5 ? std::stof(argv[5]) : Y_MAX};
    
    unsigned numPetals{200};
    auto theta_t{[&](double const& t){return numPetals*2*M_PI*t;}};

#define F(t) 5*sin((t))*cos((t))
    auto r_t{[&](double const& theta)
    {
        int petalCount = theta / (2*M_PI);
        if(petalCount < 0)
            petalCount = 0;
        if((unsigned)petalCount >= numPetals)
            petalCount = numPetals-1;
        double petalTheta{theta + petalCount * M_PI / (2 * numPetals)};
        return F(petalCount/(double)numPetals)*sin(petalTheta)*cos(petalTheta) 
    ;} };

    Mesh mesh;
    for(double t1 = 0.0, t2 = step; t1 <= 1.0f + 1e-9f; t1 += step, t2 += step)
    {
        double theta1{theta_t(t1)};
        double theta2{theta_t(t2)};
        double r1{r_t(theta1)}, r2{r_t(theta2)};

        mesh.positions.push_back(glm::vec3(r1 * cos(theta1), r1 * sin(theta1), 0.0f));
        mesh.colors.push_back(glm::vec4(t1, 0.0, 1.0 - t1, 1.0));

        mesh.positions.push_back(glm::vec3(r2 * cos(theta2), r2 * sin(theta2), 0.0f));
        mesh.colors.push_back(glm::vec4(t2, 0.0, 1.0 - t2, 1.0));
    }

    GLFWContext context;
    if(!context.Initailize({4, 5}, 1080.0, 1080.0, true, "Parmetric Plotter with SGV3D!", KeyCallback))
    {
        std::cerr<<"Failed to Initialize GLFWContext!"<<std::endl;
        ERROR("Failed to Initialize GLFWContext!");
        exit(0);
    }

    GLProgram program;
    context.GetNewProgram(program, "../Shaders/basic2d_vert.glsl", "../Shaders/basic2d_frag.glsl", mesh.GetMeshMask(), true);

    GraphMesh gmesh{program.AddMesh(mesh, GL_LINES)};
    GeometryNode* root{new GeometryNode(gmesh)};
    context.SetRoot(root);

    glPointSize(1.0);

    context.BindProgram(program);

    for(;;)
    {
        context.Render(program.Strip(), {0.0f, 0.3f, 0.0f, 1.0f});
    }
}
