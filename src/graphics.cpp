#include "graphics.h"
#include "sceneGraph.h"

#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

#include <iostream>
#include <iomanip>

void GLUniformCache::CacheUniforms (GLuint const& shader)
{
    DEBUG_MSG("Beginning to cache uniforms for current shader program");

    GLint numActiveUniforms = 0;
    glGetProgramInterfaceiv(shader, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numActiveUniforms);

    std::vector<GLchar> nameBuff(256);
    std::vector<GLenum> properties;
    properties.push_back(GL_NAME_LENGTH);
    properties.push_back(GL_TYPE);
    properties.push_back(GL_ARRAY_SIZE);
    std::vector<GLint> values(properties.size());
    for(int i = 0; i < numActiveUniforms; ++i)
    {
      glGetProgramResourceiv(shader, GL_UNIFORM, i, properties.size(),
        &properties[0], values.size(), NULL, &values[0]);
    
      nameBuff.resize(values[0]); //The length of the name.
      glGetProgramResourceName(shader, GL_UNIFORM, i, nameBuff.size(), NULL, nameBuff.data());
      std::string name(nameBuff.data(), nameBuff.size() - 1);

      GLint loc{glGetUniformLocation(shader, name.c_str())};
      DEBUG_MSG("Cached uniform \"%s\" to location %i", name.c_str(), loc);
    }
}

void BasicCamera::UpdateView (glm::vec3 const& upVec)
{
    m_view = glm::lookAt(m_pos, m_pos+m_dir, upVec);
}

void BasicCamera::UpdateUniforms (GLint const& posIdx, GLint const& dirIdx, GLint const& projIdx, GLint const& viewIdx)
{
    glUniform3fv(posIdx, 1, glm::value_ptr(m_pos));
    glUniform3fv(dirIdx, 1, glm::value_ptr(m_dir));
    glUniformMatrix4fv(projIdx, 1, GL_FALSE, glm::value_ptr(m_projection));
    glUniformMatrix4fv(viewIdx, 1, GL_FALSE, glm::value_ptr(m_view));
}

void FreeRoamCamera::Update (glm::vec2 const& scaledMouse, glm::vec3 const& moveVec, float const& dt) 
{
    m_theta += dt * scaledMouse;
    glm::mat3x3 rotMat = glm::mat3x3({cos(m_theta[0]), 0.0f, -sin(m_theta[0])}, {0.0f, 1.0f, 0.0f}, {sin(m_theta[0]), 0.0f, cos(m_theta[0])})
                        *glm::mat3x3({1.0f, 0.0f, 0.0f}, {0.0f, cos(m_theta[1]), sin(m_theta[1])}, {0.0f, -sin(m_theta[1]), cos(m_theta[1])});

    Look(rotMat * glm::vec3(0.0f, 0.0f, 1.0f));
    glm::vec3 up = rotMat * glm::vec3(0.0f, 1.0f, 0.0f);
    UpdateView(up);

    Move(moveVec);
}

void GLInfo::CacheProgram (StrippedGLProgram const& program) 
{
    GLUniformCache uc(program.Shader()); //Cache uniforms in current context
    m_programs.push_back({program, std::move(uc)});
}

bool GLInfo::SetProgram (StrippedGLProgram const& program)
{
    auto progIdx = std::find(m_programs.begin(), m_programs.end(), program);
    if(progIdx == m_programs.end())
        return false;

    m_curProgram = &(*progIdx);
    return true;
}

bool GLContext::GetNewProgram (GLProgram& program, const char* const& vertShader, const char* const& fragShader, uint8_t const& meshMask, bool const& isStatic)
{
    program = GLProgram(meshMask, vertShader, fragShader, isStatic);
    m_info.CacheProgram(program.Strip());
    m_info.SetProgram(program.Strip());

    return true;
}

bool GLContext::BindProgram (GLProgram const& program)
{
    return BindProgram(program.Strip());
}

bool GLContext::BindProgram (StrippedGLProgram const& program)
{
    glUseProgram(program.Shader());
    
    if(m_info.SetProgram(program))
    {
        glBindVertexArray(program.Vao()); 
        return true;
    }

    return false;
}

GLFWContext::GLFWContext () 
    : m_keyCallback{nullptr}, m_mouseButtonCallback{nullptr}, m_window{nullptr} 
{
    DEBUG_MSG("Constructed GLFWContext");
}

bool GLFWContext::Initailize (GLint const (&version)[2], 
                 GLfloat const& width, GLfloat const& height, 
                 bool const& initGlew,
                 std::string const& title,
                 GLFWkeyfun const& keyCallback, GLFWmousebuttonfun const& mouseButtonCallback) 
{
    DEBUG_MSG("Initializing GLFWContext \"%s\"; window width %.1f, height %.1f; for OpenGL v.%i.%i", 
              title.c_str(), width, height, version[0], version[1]);

    m_info.SetDimension(width, height);
    m_info.SetTitle(title);

    if(!glfwInit())
    {
        ERROR("Failed to initialize GLFW");
        return false;
    }    
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, version[0]);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, version[1]);
    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if(!m_window)
    {
        ERROR("Failed to initialize GLFW window");
        return false;
    }
    glfwMakeContextCurrent(m_window);

    if(initGlew)
    {
        glewExperimental = GL_TRUE;
        if(glewInit() != GLEW_OK) 
        {
            ERROR("Failed to initialize glew");
            return false;
        }
    }

    if(keyCallback)
        glfwSetKeyCallback(m_window, keyCallback);

    if(mouseButtonCallback)
        glfwSetMouseButtonCallback(m_window, mouseButtonCallback);

    //Mouse and key callbacks can retrieve information from and update GLFWContext
    //through glfw's "user pointer":
    //see http://www.glfw.org/docs/latest/window_guide.html#window_userptr
    glfwSetWindowUserPointer(m_window, this);

    glEnable(GL_BLEND); 
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST); 
    glDepthFunc(GL_LESS);

    DEBUG_MSG("Successfully initialized GLFWContext");

    return true;
}

void GLFWContext::performTransforms ()
{
    double t = glfwGetTime();
    double xpos, ypos;
    glfwGetCursorPos(m_window, &xpos, &ypos);
    glfwSetCursorPos(m_window, m_info.Width()/2, m_info.Height()/2);
    glm::vec2 scaledMouse{m_info.Width()/2-xpos, m_info.Height()/ypos};

    glm::vec3 moveVec(0.0f);
	static_cast<FreeRoamCamera*>(m_camera)->Update(scaledMouse, moveVec, glfwGetTime() - t);
}

bool GLFWContext::Render (StrippedGLProgram const& program, GLfloat const (&color)[4]) 
{
    glClearBufferfv(GL_COLOR, 0.0f, color); 
    glClear(GL_DEPTH_BUFFER_BIT);

    if(m_root)
    {
		if(m_camera)
			performTransforms();

        RenderContext* rc {new RenderContext{{{},-1.0f}, std::stack<glm::mat4x4>(), program}}; //FIX ME 
        rc->matStack.push(glm::mat4x4(1.0f));
        rc->globals.t = glfwGetTime();
        m_root->render(rc);
    }
    else
    {
        WARNING("No root node in GLFWContext for rendering");
    }

    glfwSwapBuffers(m_window);
    glfwPollEvents();

    return true;
}

void GLFWContext::DisableCursor () const
{
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(m_window, GLFW_STICKY_KEYS, false);
}

//void DefaultMouseButtonCallback (GLFWwindow*, int, int, int)
//{
//}
//
//void DefaultKeyCallback(GLFWwindow*, int key, int, int action, int)
//{   
//    if(action != GLFW_PRESS)
//        return;
//
//    GLGraphicsManager const& ggm{GLGraphicsManager::Get()};
//    switch(key)
//    {
//        case GLFW_KEY_K:
//            ggm.incColorSceme();
//            break;
//        case GLFW_KEY_ESCAPE:
//           glfwTerminate(); 
//           exit(EXIT_SUCCESS);
//           break;
//        case GLFW_KEY_UP:
//            ggm.scale(11.0f/10);
//            break;
//GL_ELEMENT_ARRAY_BUFFER_BINDING
//        case GLFW_KEY_DOWN:
//            ggm.scale(10.0f/11);
//            break;
//        case GLFW_KEY_LEFT:
//           glfwSetTime(glfwGetTime()-10);
//           break;
//        case GLFW_KEY_RIGHT:
//           glfwSetTime(glfwGetTime()+10);
//           break;
//        default:
//           break;
//    }
////    if (key == GLFW_KEY_X && action == GLFW_PRESS)
////    {
////        g.toggleAxes();
////    }
//}
