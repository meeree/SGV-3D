#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include "mesh.h"
#include <GLFW/glfw3.h>

class SceneGraphWrapper;

class Graphics 
{
private:
    std::vector<SceneGraphWrapper*> mDynamicGraph;
    std::vector<std::pair<StaticMesh,VboIndexer>> mMeshes;
    std::vector<GLuint> mVbos;
public:
    Graphics (GLfloat const&, GLfloat const&, char const*, char const*, const char* title="Untitled Window");

    void draw ();
};

#endif //__GRAPHICS_H__
