#include "graphics.h"

void Graphics::draw ()
{
    //...

    for (auto const& mesh: mMeshes)
    {
        if (mesh->isStatic())
        {
            VboIndexer indexer{static_cast<StaticMesh*>(mesh)->getIndexer()};
            GLuint const& vbo{mVbos[indexer.vboIndex]};
                   
        }
        else 
        {
        }
    }
}
