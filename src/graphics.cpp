#include "graphics.h"
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#include <iostream>

GLuint Graphics::loadInShader(char const *fname, GLenum const &shaderType) 
{
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
    } 
	else 
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

GLuint Graphics::msColorSchemeCount = 1;

void Graphics::setShaders (char const* vertLoc, char const* fragLoc)
{
	mShaderProgram = glCreateProgram();

    auto vertShader = loadInShader(vertLoc, GL_VERTEX_SHADER);
    auto fragShader = loadInShader(fragLoc, GL_FRAGMENT_SHADER);

    glAttachShader(mShaderProgram, vertShader);
    glAttachShader(mShaderProgram, fragShader);

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    glLinkProgram(mShaderProgram);
}

Graphics::Graphics (GLfloat const& width, GLfloat const& height, char const* vertLoc, char const* fragLoc, const char* title)
    : mWidth{width}, mHeight{height}, fUpdate{true}
{
    mColorScheme.second = 0;
    mCamera.horiAngle = M_PI; 
    mCamera.vertAngle = 0.0;
    mCamera.lookSpeed = 50.0f;
    mCamera.moveSpeed = 1.0f;
    if(!glfwInit()) 
    {
        std::cerr<<"failed to initialize glfw"<<std::endl;
        exit(EXIT_SUCCESS);
    }
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    mWindow = glfwCreateWindow(width, height, title, NULL, NULL);
    if(!mWindow) 
    {
        std::cerr<<"failed to initialize window"<<std::endl;
        exit(EXIT_SUCCESS);
    }
    glfwMakeContextCurrent(mWindow);

    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK) 
    {
        std::cerr<<"failed to initialize glew"<<std::endl;
        exit(EXIT_SUCCESS);
    }
    setShaders(vertLoc, fragLoc);
    glUseProgram(mShaderProgram);

    //IMPROVE THIS
    mCamera.pos.first = glGetUniformLocation(mShaderProgram, "camPos");
    mCamera.dir.first = glGetUniformLocation(mShaderProgram, "camDir");
    mCamera.pMat.first = glGetUniformLocation(mShaderProgram, "pMat");
    if (mCamera.pMat.first == -1)
        std::cerr<<"Warning: projection matrix not active or present in shader. It should be named 'pMat'"<<std::endl;
    mCamera.vMat.first = glGetUniformLocation(mShaderProgram, "vMat");
    if (mCamera.vMat.first == -1)
        std::cerr<<"Warning: view matrix not active or present in shader. It should be named 'vMat'"<<std::endl;
    mColorScheme.first = glGetUniformLocation(mShaderProgram, "colorScheme");
    if (mColorScheme.first == -1)
        std::cerr<<"Warning: color scheme not active or present in shader. It should be named 'colorScheme'"<<std::endl;
    mScalar.first = glGetUniformLocation(mShaderProgram, "scalar");
    if (mScalar.first == -1)
        std::cerr<<"Warning: scalar not active or present in shader. It should be named 'scalar'"<<std::endl;


    glUniform1ui(mColorScheme.first, mColorScheme.second);
//    glUniform3fv(mCamera.pos.first, 1, glm::value_ptr(mCamera.pos.second));
//    glUniform3fv(mCamera.dir.first, 1, glm::value_ptr(mCamera.dir.second));
//    glUniformMatrix4fv(mCamera.vMat.first, 1, GL_FALSE, glm::value_ptr(mCamera.vMat.second));
    glUniformMatrix4fv(mCamera.pMat.first, 1, GL_FALSE, glm::value_ptr(mCamera.pMat.second));
    setScalar(1.0f);
}

void Graphics::update () {}

void Graphics::performTransforms ()
{
    double t = glfwGetTime();
    double xpos, ypos;
    glfwGetCursorPos(mWindow, &xpos, &ypos);
    glfwSetCursorPos(mWindow, mWidth/2, mHeight/2);
    mCamera.horiAngle += mCamera.lookSpeed*(glfwGetTime()-t)*(mWidth/2-xpos);
    mCamera.vertAngle += mCamera.lookSpeed*(glfwGetTime()-t)*(mHeight/2-ypos);

    mCamera.dir.second = glm::vec3(
        cos(mCamera.vertAngle) * sin(mCamera.horiAngle),
        sin(mCamera.vertAngle),
        cos(mCamera.vertAngle) * cos(mCamera.horiAngle)
    );
    glm::vec3 right{
        sin(mCamera.horiAngle - 3.14f/2.0f),
        0,
        cos(mCamera.horiAngle - 3.14f/2.0f)
    };
    glm::vec3 up = glm::cross(right, mCamera.dir.second);
    glm::vec3 moveVec{0.0f};
    GLfloat  alteredMoveSpeed = glfwGetKey(mWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? 
                                mCamera.moveSpeed/2 : mCamera.moveSpeed;
    if (glfwGetKey(mWindow, GLFW_KEY_S) == GLFW_PRESS)
    {
        moveVec -= alteredMoveSpeed*mCamera.dir.second;
    }
    if (glfwGetKey(mWindow, GLFW_KEY_W) == GLFW_PRESS)
    {
        moveVec += alteredMoveSpeed*mCamera.dir.second;
    }
    if (glfwGetKey(mWindow, GLFW_KEY_Q) == GLFW_PRESS)
    {
        moveVec -= alteredMoveSpeed*up;
    }
    if (glfwGetKey(mWindow, GLFW_KEY_E) == GLFW_PRESS)
    {
        moveVec += alteredMoveSpeed*up;
    }
    if (glfwGetKey(mWindow, GLFW_KEY_A) == GLFW_PRESS)
    {
        moveVec -= alteredMoveSpeed*right;
    }
    if (glfwGetKey(mWindow, GLFW_KEY_D) == GLFW_PRESS)
    {
        moveVec += alteredMoveSpeed*right;
    }
    if (length(moveVec) > FLT_EPSILON)
    {
        moveCam(moveVec);
    }
    mCamera.vMat.second = glm::lookAt(mCamera.pos.second, mCamera.pos.second+mCamera.dir.second, up); 
    glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(mCamera.vMat.second));
    glUniform3fv(mCamera.pos.first, 1, glm::value_ptr(mCamera.pos.second));
////    mCamera.vMat.second = glm::lookAt(mCamera.pos.second, mCamera.pos.second+mCamera.dir.second, up); 
//
//    //IMPROVE THIS METHOD
//    glUniform3fv(mCamera.pos.first, 1, glm::value_ptr(mCamera.pos.second));
//    mCamera.vMat.second = glm::lookAt(glm::vec3(0,0,3), glm::vec3(0,0,0), glm::vec3(0,1,0));
//    glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(mCamera.vMat.second));
////    glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(glm::lookAt(glm::vec3(0,0,3), glm::vec3(0,0,0), glm::vec3(0,1,0))));
    glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(glm::lookAt(mCamera.pos.second, mCamera.pos.second+mCamera.dir.second, up)));
}

void Graphics::render ()
{
    GLfloat const color [4] {0.0f, 0.0f, 0.0f, 1.0f};
    glClearBufferfv(GL_COLOR, 0.0f, color);
    glClear(GL_DEPTH_BUFFER_BIT);
    for (auto& graph: mGraphs)
    {
        graph.render(glm::mat4x4(1.0f));
    }
    glfwSwapBuffers(mWindow);
}

void Graphics::loop ()
{
    for (;;)
    {
        performTransforms();
        auto pMat = glm::perspective(glm::radians(45.0f),(GLfloat)1920/1080, 0.1f, 200.0f);
        glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(pMat));
        glUniformMatrix4fv(4, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
        render();
        glfwPollEvents();
    }
}
