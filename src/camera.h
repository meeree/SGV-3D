#ifndef  __CAMERA_H__
#define  __CAMERA_H__

#include "base.h"
#include <glm/gtc/matrix_transform.hpp>

//Perhaps have camera running on seperate thread??
class BasicCamera
{
protected:
    glm::vec3 m_pos, m_dir;
    glm::mat4x4 m_projection, m_view;
    float m_lookSpeed, m_moveSpeed;

public:
    BasicCamera (float const& lookSpeed=1.0f, float const& moveSpeed=1.0f) : m_pos(0.0f), m_dir(0.0f), m_projection(1.0f), m_view(1.0f), m_lookSpeed{lookSpeed}, m_moveSpeed{moveSpeed} {}

    void UpdateUniforms (GLint const& posIdx, GLint const& dirIdx, GLint const& projIdx, GLint const& viewIdx);

    inline void SetPosition  (glm::vec3 const& pos) {m_pos = pos;} 
    inline void SetDirection (glm::vec3 const& dir) {m_dir = dir;}

    inline void Look (glm::vec3 const& lookVec) {m_dir += m_lookSpeed*lookVec;} 
    inline void Move (glm::vec3 const& moveVec) {m_pos += m_moveSpeed*moveVec;}
    inline void SetProjection (float const& fov, float const& aspectRatio, float const& near, float const& far) {m_projection = glm::perspective(glm::radians(fov), aspectRatio, near, far);}
    inline glm::vec3 GetPosition  () const {return m_pos;} 
    inline glm::vec3 GetDirection () const {return m_dir;}
};

class FreeRoamCamera : public BasicCamera
{
private:
    glm::vec2 m_theta;

public:
    FreeRoamCamera (float const& lookSpeed=1.0f, float const& moveSpeed=1.0f) : m_theta{0.0f, M_PI}, BasicCamera(lookSpeed, moveSpeed) {}
    
    void Update (glm::vec2 const& mouse, uint8_t const& keyMask, float const& dt);

    void SetDirection (glm::vec3 const& dir); 
};

#endif //__CAMERA_H__
