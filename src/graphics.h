#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include "sceneGraph.h"
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Graphics
{
private:
    std::vector<SceneGraph> mGraphs;
    GLfloat mWidth, mHeight;
    GLuint mShaderProgram;
    GLFWwindow* mWindow;
    bool fUpdate;

    struct Camera 
    {
        std::pair<GLint, glm::mat3> pMat, vMat;
        std::pair<GLint, glm::vec3> pos, dir;
        double horiAngle, vertAngle;
        GLfloat lookSpeed, moveSpeed;
    } mCamera; 

    //Keep custom uniforms in their own container with their locations 
//    struct UniformContainer
//    {
//        std::vector<std::pair<GLuint,glm::value_ptr>> mCustomUniforms;
//    } mUniforms;
    std::pair<GLint, GLuint> mColorScheme;
    std::pair<GLint, GLfloat> mScalar;


    static GLuint msColorSchemeCount;
    static void defaultMouseButtonCallback (GLFWwindow*, int, int, int);
    static void defaultKeyCallback(GLFWwindow*, int, int, int, int);
	static GLuint loadInShader (char const*, GLenum const&);

    void setShaders (char const*, char const*);
    void update ();
    void performTransforms ();
public:
    Graphics (GLfloat const&, GLfloat const&, char const*, char const*, const char* title="Untitled Window");
    void setCallbackAndFlags(); 

    void render ();
    void loop ();

    //Getter/setter functions 
    inline void scale (GLfloat const& scaleFactor)
        {mScalar.second *= scaleFactor; 
         glUniform1f(mScalar.first, mScalar.second);}
    inline void setScalar (GLfloat const& scalar) 
        {mScalar.second = scalar;
         glUniform1f(mScalar.first, mScalar.second);}
    inline void setCamTransforms (glm::mat4x4 const& pMat, glm::mat4x4 const& vMat)
        {mCamera.pMat.second = pMat; 
         mCamera.vMat.second = vMat;
         glUniformMatrix4fv(mCamera.pMat.first, 1, GL_FALSE, glm::value_ptr(mCamera.pMat.second));
         glUniformMatrix4fv(mCamera.vMat.first, 1, GL_FALSE, glm::value_ptr(mCamera.vMat.second));}
    inline void setCam (glm::vec3 const& camPos, glm::vec3 const& camDir)
        {mCamera.pos.second = camPos; 
         mCamera.dir.second = glm::normalize(camDir);
         mCamera.horiAngle = acos(glm::dot(glm::vec3(0.0f,0.0f,1.0f), mCamera.dir.second));
         mCamera.vertAngle = acos(glm::dot(glm::vec3(0.0f,1.0f,0.0f), mCamera.dir.second))-M_PI/2;
         glUniform3f(mCamera.pos.first, mCamera.pos.second.x, mCamera.pos.second.y, mCamera.pos.second.z);
         glUniform3f(mCamera.dir.first, mCamera.dir.second.x, mCamera.dir.second.y, mCamera.dir.second.z);}
    inline void setCamSpeed (GLfloat const& lookSpeed, GLfloat const& moveSpeed)
        {mCamera.lookSpeed = lookSpeed; 
         mCamera.moveSpeed = moveSpeed;};
    inline void incCamLookSpeed (GLfloat const& inc) {mCamera.lookSpeed += inc;}
    inline void incCamMoveSpeed (GLfloat const& inc) {mCamera.moveSpeed += inc;}
    inline void moveCam (glm::vec3 const& inc) {mCamera.pos.second += inc;}
    inline void mustUpdate () {fUpdate = true;}
    inline void incColorSceme () 
        {mColorScheme.second = (mColorScheme.second+1)%msColorSchemeCount;
         glUniform1ui(mColorScheme.first, mColorScheme.second);}
    inline void addGraph (SceneGraph sg) 
        {mGraphs.push_back(sg);}
    inline GLuint getShaderProgram () const {return mShaderProgram;}
    inline GLFWwindow* getWindow () const {return mWindow;};
    //------------------------------------
};

extern Graphics g;

#endif //__GRAPHICS_H__
