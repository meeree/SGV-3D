#include "sgv_graphics.h"
#include "sceneGraph.h"

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


bool SGVGraphics::Render (StrippedGLProgram const& program, GLfloat const (&color)[4])
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
