#include "sceneGraph.h"
#include "graphics.h"

#include <fstream>
#include <iostream>
GLuint loadInShader(char const *fname, GLenum const &shaderType) { /* Called by shaders function */
    std::vector<char> buffer;
    std::ifstream in;
    in.open(fname, std::ios::binary);

    if(in.is_open()) {
        in.seekg(0, std::ios::end);
        size_t const &length = in.tellg();

        in.seekg(0, std::ios::beg);

        buffer.resize(length + 1);
        in.read(&buffer[0], length);
        in.close();
        buffer[length] = '\0';
    } else {
        std::cerr<<"Unable to open "<<fname<<std::endl;
        exit(-1);
    }

    GLchar const *src = &buffer[0];

    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    GLint test;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &test);

    if(!test) {
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

Graphics g{1920, 1080, "/home/jhazelden/Cpp/OpenGL/SGV3D/src/Shaders/vert2.glsl", "/home/jhazelden/Cpp/OpenGL/SGV3D/src/Shaders/frag2.glsl", "Volume renderer v0.1"};

void keyCallback(GLFWwindow*, int key, int, int action, int)
{   
    if (key == GLFW_KEY_K && action == GLFW_PRESS)
    {
        g.incColorScheme();
    }
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
	    glfwTerminate(); 
        exit(EXIT_SUCCESS);
    }
    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
    {
        GLFWAttrs.scalar *= 11.0f/10;
        g.scale(GLFWAttrs.scalar);
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
    {
        GLFWAttrs.scalar *= 10.0f/11;
        g.scale(GLFWAttrs.scalar);
    }
    if (key == GLFW_KEY_X && action == GLFW_PRESS)
    {
        g.toggleAxes();
    }
}

int main ()
{
//    g.setCam(glm::vec3(0,5,10), glm::vec3(0,-0.5,-1.0));
    g.setCam(glm::vec3(0,0,3.5), glm::vec3(0,0,-1));
    auto pMat = glm::perspective(glm::radians(45.0f),(GLfloat)1920/1080, 0.1f, 200.0f); 
    glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(pMat));
    glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
    glUniformMatrix4fv(4, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

    glEnable(GL_BLEND); glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);
    glfwSetKeyCallback(g.getWindow(), keyCallback);
    glfwSetMouseButtonCallback(g.getWindow(), mouseButtonCallback);
    //DRAW A CUBE WITH TWO SMALLER CUBES ATTACHED TO IT 
    GLuint shaderProgram = g.getShaderProgram();
    GLchar const* names [3]{"mMat","mini","maxi"};
    SceneGraph sg {nullptr, shaderProgram, true, names}; 
	std::vector<Vertex> vertices{
        {{-1.0f,-1.0f,-1.0f},{-1.0f,0.0f,0.0f}},
		{{-1.0f,-1.0f, 1.0f},{-1.0f,0.0f,0.0f}},
		{{-1.0f, 1.0f, 1.0f},{-1.0f,0.0f,0.0f}},
		{{1.0f, 1.0f,-1.0f},{0.0f,0.0f,-1.0f}},
		{{-1.0f,-1.0f,-1.0f},{0.0f,0.0f,-1.0f}},
		{{-1.0f, 1.0f,-1.0f},{0.0f,0.0f,-1.0f}},
		{{1.0f,-1.0f, 1.0f},{0.0f,-1.0f,0.0f}},
		{{-1.0f,-1.0f,-1.0f},{0.0f,-1.0f,0.0f}},
		{{1.0f,-1.0f,-1.0f},{0.0f,-1.0f,0.0f}},
		{{1.0f, 1.0f,-1.0f},{0.0f,0.0f,-1.0f}},
		{{1.0f,-1.0f,-1.0f},{0.0f,0.0f,-1.0f}},
		{{-1.0f,-1.0f,-1.0f},{0.0f,0.0f,-1.0f}},
		{{-1.0f,-1.0f,-1.0f},{-1.0f,0.0f,0.0f}},
		{{-1.0f, 1.0f, 1.0f},{-1.0f,0.0f,0.0f}},
		{{-1.0f, 1.0f,-1.0f},{-1.0f,0.0f,0.0f}},
		{{1.0f,-1.0f, 1.0f},{0.0f,-1.0f,0.0f}},
		{{-1.0f,-1.0f, 1.0f},{0.0f,-1.0f,0.0f}},
		{{-1.0f,-1.0f,-1.0f},{0.0f,-1.0f,0.0f}},
		{{-1.0f, 1.0f, 1.0f},{0.0f,0.0f,1.0f}},
		{{-1.0f,-1.0f, 1.0f},{0.0f,0.0f,1.0f}},
		{{1.0f,-1.0f, 1.0f},{0.0f,0.0f,1.0f}},
		{{1.0f, 1.0f, 1.0f},{1.0f,0.0f,0.0f}},
		{{1.0f,-1.0f,-1.0f},{1.0f,0.0f,0.0f}},
		{{1.0f, 1.0f,-1.0f},{1.0f,0.0f,0.0f}},
		{{1.0f,-1.0f,-1.0f},{1.0f,0.0f,0.0f}},
		{{1.0f, 1.0f, 1.0f},{1.0f,0.0f,0.0f}},
		{{1.0f,-1.0f, 1.0f},{1.0f,0.0f,0.0f}},
		{{1.0f, 1.0f, 1.0f},{0.0f,1.0f,0.0f}},
		{{1.0f, 1.0f,-1.0f},{0.0f,1.0f,0.0f}},
		{{-1.0f, 1.0f,-1.0f},{0.0f,1.0f,0.0f}},
		{{1.0f, 1.0f, 1.0f},{0.0f,1.0f,0.0f}},
		{{-1.0f, 1.0f,-1.0f},{0.0f,1.0f,0.0f}},
		{{-1.0f, 1.0f, 1.0f},{0.0f,1.0f,0.0f}},
		{{1.0f, 1.0f, 1.0f},{0.0f,0.0f,1.0f}},
		{{-1.0f, 1.0f, 1.0f},{0.0f,0.0f,1.0f}},
		{{1.0f,-1.0f, 1.0f}, {0.0f,0.0f,1.0f}}};
    glm::mat4x4 modMats[1]; 
    modMats[0] = glm::mat4x4(1.0f);
    Mesh meshes[1]{
		 {vertices}};
//    g.update(); 
    GraphMesh gmesh = sg.bindMesh(meshes[0]);
    StandardNode node1{gmesh, modMats[0]}; 
    node1.toggleDraw();
    //FIX ME
    sg.setRoot(&node1);

    GLFWAttrs.scalar = 1;
    g.scale(GLFWAttrs.scalar);

    glUniform1f(6, 1);

    glfwSetInputMode(g.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(g.getWindow(), GLFW_STICKY_KEYS, false);

    g.addSurface(1, std::vector<Vertex>(vertices.begin(), vertices.begin()+4));
//    g.render(glfwGetTime());
    for (;;)
    {
        glm::mat4x4 modMat = glm::mat4x4(1.0f);
//        sg.render(modMat);
        g.render(glfwGetTime(), sg);
        glfwSwapBuffers(g.getWindow());
        glfwPollEvents();
    }
}
