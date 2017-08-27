#include "camera.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

void BasicCamera::UpdateUniforms (GLint const& posIdx, GLint const& dirIdx, GLint const& projIdx, GLint const& viewIdx)
{
    if(posIdx > 0)
        glUniform3fv(posIdx, 1, glm::value_ptr(m_pos));
    if(dirIdx > 0)
        glUniform3fv(dirIdx, 1, glm::value_ptr(m_dir));
    if(projIdx > 0)
        glUniformMatrix4fv(projIdx, 1, GL_FALSE, glm::value_ptr(m_projection));
    if(viewIdx > 0)
        glUniformMatrix4fv(viewIdx, 1, GL_FALSE, glm::value_ptr(m_view));
}

void FreeRoamCamera::SetDirection (glm::vec3 const& dir)
{
    m_dir = dir;
	m_theta = {acos(dot(glm::vec3(0.0f,0.0f,1.0f), m_dir)),
               acos(dot(glm::vec3(0.0f,1.0f,0.0f), m_dir))};
}

void FreeRoamCamera::Update (glm::vec2 const& mouse, uint8_t const& keyMask, float const& dt) 
{
    m_theta += m_lookSpeed * dt * mouse;
    m_dir = glm::vec3(
        cos(m_theta.y) * sin(m_theta.x),
        sin(m_theta.y),
        cos(m_theta.y) * cos(m_theta.x)
    );
    glm::vec3 right{
        sin(m_theta.x - M_PI_2),
        0,
        cos(m_theta.x - M_PI_2)
    };
    glm::vec3 up = glm::cross(right, m_dir);

    glm::vec3 moveVec(0.0f);
    if(keyMask & 1)              //'q'/pg_up   key 
        moveVec -= up;
    if(keyMask & 2)              //'w'/up      key 
        moveVec += m_dir;
    if(keyMask & 4)              //'e'/pg_down key 
        moveVec += up;       
    if(keyMask & 8)              //'a'/left    key 
        moveVec -= right;       
    if(keyMask & 16)             //'s'/down    key 
        moveVec -= m_dir;       
    if(keyMask & 32)             //'d'/right   key 
        moveVec += right;       

    Move(moveVec);
    m_view = glm::lookAt(m_pos, m_pos + m_dir, up);
}
