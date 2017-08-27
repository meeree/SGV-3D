#include "sgv_graphics.h"
#include "sceneGraph.h"

#define WAS_PRESSED(key) (glfwGetKey(m_window, key) == GLFW_PRESS)

bool SGVGraphics::Initailize (GLfloat const& width, GLfloat const& height,
                              bool const& initGlew, 
                              GLFWkeyfun const& keyCallback, GLFWmousebuttonfun const& mouseButtonCallback)
{
    std::string title{"SGV3D Version " + std::to_string(SGV_MAJOR) + "." + std::to_string(SGV_MINOR)};

    return GLFWContext::Initailize(
            {4, 5},                              //OpenGL version
            width, height,                       //Window dimension 
            initGlew,                            //Initialize GLEW? 
            title,                               //Window title
            keyCallback, mouseButtonCallback     //Callbacks 
    );
}

void SGVGraphics::SaveImportantUniforms ()
{
    m_modelLoc = m_info.LookupUniform("model");
    DEBUG_MSG("Stored model matrix shader location as %i", m_modelLoc);
}

bool SGVGraphics::Render (StrippedGLProgram const& program, GLfloat const (&color)[4])
{
    double t{glfwGetTime()};
    glfwPollEvents();
    if(m_done)
        return false;

    glClearBufferfv(GL_COLOR, 0.0f, color); 
    glClear(GL_DEPTH_BUFFER_BIT);

    if(!m_root)
    {
        WARNING("No root node in GLFWContext for rendering");
        return false;
    }

    RenderContext* rc {new RenderContext{{{},-1.0f}, std::stack<glm::mat4x4>(), program}}; //FIX ME 
    rc->matStack.push(glm::mat4x4(1.0f));
    rc->globals.t = glfwGetTime();
    rc->globals.modelLoc = m_modelLoc;

    if(m_useCamera)
    {
        //Get camera position
        double xpos, ypos;
        GLfloat width_2{m_info.Width()/2}, height_2{m_info.Height()/2};
        glfwGetCursorPos(m_window, &xpos, &ypos);
        glfwSetCursorPos(m_window, width_2, height_2);

        uint8_t keyMask{0};
        keyMask |= 1  * WAS_PRESSED(GLFW_KEY_Q);
        keyMask |= 2  * WAS_PRESSED(GLFW_KEY_W);
        keyMask |= 4  * WAS_PRESSED(GLFW_KEY_E);
        keyMask |= 8  * WAS_PRESSED(GLFW_KEY_A);
        keyMask |= 16 * WAS_PRESSED(GLFW_KEY_S);
        keyMask |= 32 * WAS_PRESSED(GLFW_KEY_D);

        glm::vec2 mouse{
            xpos - width_2,
            height_2 - ypos        
        };
        m_camera.Update(mouse, keyMask, glfwGetTime() - t);
        m_camera.UpdateUniforms(7, -1, 3, 4);
    }

    m_root->render(rc);

    glfwSwapBuffers(m_window);

    return true;
}
