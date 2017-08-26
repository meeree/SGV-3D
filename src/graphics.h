#include "base.h"
#include <GLFW/glfw3.h>

#include <unordered_map>
#include <string>

#include <glm/gtc/matrix_transform.hpp>

//URGENT: HOW TO SWITCH SHADERS?

//TODO: Add multi-context support

//Perhaps have camera running on seperate thread??
class BasicCamera
{
protected:
    glm::vec3 m_pos, m_dir;
    glm::mat4x4 m_projection, m_view;

    float m_lookSpeed, m_moveSpeed;

    ///\brief Update view matrix using upVec 
    ///\param [in] upVec Vector pointing upwards from camera
    void UpdateView (glm::vec3 const& upVec={0.0f,1.0f,0.0f});

public:
    BasicCamera (float const& lookSpeed=1.0f, float const& moveSpeed=1.0f) : m_pos(0.0f), m_dir(0.0f), m_projection(1.0f), m_view(1.0f), m_lookSpeed{lookSpeed}, m_moveSpeed{moveSpeed} {}

    void UpdateUniforms (GLint const& posIdx, GLint const& dirIdx, GLint const& projIdx, GLint const& viewIdx);

    inline void SetPosition  (glm::vec3 const& pos) {m_pos = pos;} 
    inline void SetDirection (glm::vec3 const& dir) {m_dir = dir;}
    inline void Look (glm::vec3 const& lookVec) {m_dir += m_lookSpeed*lookVec;} 
    inline void Move (glm::vec3 const& moveVec) {m_pos += m_moveSpeed*moveVec;}
    inline void SetProjection (float const& fov, float const& aspectRatio, float const& near, float const& far) {m_projection = glm::perspective(fov, aspectRatio, near, far);}
    inline glm::vec3 GetPosition  () const {return m_pos;} 
    inline glm::vec3 GetDirection () const {return m_dir;}

    inline void DisableCursor () const {
    };
};

class FreeRoamCamera : public BasicCamera 
{
private:
    glm::vec2 m_theta;

public:
    FreeRoamCamera (float const& lookSpeed=1.0f, float const& moveSpeed=1.0f) : m_theta{0.0f}, BasicCamera(lookSpeed, moveSpeed) {}
    
    void Update (glm::vec2 const& scaledMouse, glm::vec3 const& moveVec, float const& dt);
};

class GLUniformCache 
{
private:
    std::unordered_map<std::string, GLint> m_layout;

public:
    ///\brief Initialize cache using LightGLContext
    GLUniformCache (GLuint const& shader) {CacheUniforms(shader);}

    ///\brief Cache all uniforms in provided shader program 
    void CacheUniforms (GLuint const& shader);

    ///\brief Lookup uniform index 
    inline GLint Lookup (std::string const& uniform) {return m_layout[uniform];}
};

#include <iostream>

class GLInfo 
{
private:
    struct ProgramUniPair
    {
        StrippedGLProgram program;
        GLUniformCache uniCache; 

        inline bool operator< (StrippedGLProgram const& rhs) {return program < rhs;}
        inline bool operator== (StrippedGLProgram const& rhs) {return program == rhs;}
    };
    std::vector<ProgramUniPair> m_programs;
    ProgramUniPair* m_curProgram;

    GLfloat m_width, m_height;
    std::string m_title;
    GLuint m_colorscheme;
    GLfloat m_scalar;

public:
    GLInfo () : m_curProgram{nullptr}, m_colorscheme{0}, m_scalar{1.0f} {}

    void CacheProgram (StrippedGLProgram const& program);
    bool SetProgram (StrippedGLProgram const& program);
    inline void SetDimension (GLfloat const& width, GLfloat const& height) {m_width = width; m_height = height;}
    inline void SetTitle (std::string const& title) {m_title = title;}                                          

    inline GLfloat Width () const {return m_width;}
    inline GLfloat Height () const {return m_height;}
    inline std::string Title () const {return m_title;}
    inline GLuint LookupUniform (std::string const& uniform) const {return m_curProgram->uniCache.Lookup(uniform);}

    inline void ScaleUp (GLfloat const& scaleFactor) {m_scalar *= scaleFactor;}
    inline void ScaleDown (GLfloat const& scaleFactor) {m_scalar /= scaleFactor;}
    inline void SetScalar (GLfloat const& scalar) {m_scalar = scalar;}
    inline void UpdateScalarUniform (GLint const& scalarLoc) const {glUniform1f(scalarLoc, m_scalar);}
};

class GLContext
{
protected:
    GLContext () : m_camera{nullptr} {g_GLContextCreated = true;}
    virtual ~GLContext () {}

    GLInfo m_info;
    BasicCamera* m_camera;

public:
    virtual bool Render (StrippedGLProgram const& program, GLfloat const (&color)[4]) = 0;

    bool GetNewProgram (GLProgram& program, const char* const& vertShader, const char* const& fragShader, uint8_t const& meshMask, bool const& isStatic=false);
    bool BindProgram (GLProgram const& program);
    bool BindProgram (StrippedGLProgram const& program);

    inline BasicCamera* Camera () {return m_camera;}
    inline GLInfo& Info () {return m_info;}
    inline GLuint LookupUniform (std::string const& uniform) const {return m_info.LookupUniform(uniform);}
};

class Node;

class GLFWContext : public GLContext
{
private:
    GLFWkeyfun m_keyCallback;
    GLFWmousebuttonfun m_mouseButtonCallback;
    GLFWwindow* m_window;
    Node* m_root;

public:
    GLFWContext ();
    virtual ~GLFWContext () override {glfwDestroyWindow(m_window);}

    bool Initailize (GLint const (&version)[2], 
                     GLfloat const& width=640, GLfloat const& height=480,
                     bool const& initGlew=true, 
                     std::string const& title="Untitled Window",
                     GLFWkeyfun const& keyCallback=nullptr, GLFWmousebuttonfun const& mouseButtonCallback=nullptr);

    void performTransforms ();

    virtual bool Render (StrippedGLProgram const& program, GLfloat const (&color)[4]={0.0f,0.0f,0.0f,1.0f}) override;

    inline void SetCamera (BasicCamera* camera) {m_camera = camera;}
    inline void SetRoot (Node* root) {m_root = root;}
    void DisableCursor () const;
};

class GLGraphicsManager 
{
private:
    std::vector<GLContext*> m_contexts;

public:
    GLGraphicsManager () {DEBUG_MSG("Initialized GLGraphicsManager");}

    inline void AttachContext (GLContext* context) {m_contexts.push_back(context);}
};
