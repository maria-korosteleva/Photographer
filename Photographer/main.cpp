#define MY
#ifndef MY

// ----------------------------------------------------------------------------------
#else
#include "Photographer.h"
#include <GK-Undressing-People-Ceres/GeneralMesh.h>

#include <iostream>

int main()
{
    GeneralMesh object("D:/Data/smpl_outs/pose_00048_270_dyna_thin.obj");

    std::cout << "Mesh Loaded!" << std::endl;

    //std::cout << object.getGLNormalizedVertices().size() << std::endl;

    Photographer photographer;
    photographer.setObject(&object);
    return photographer.run();
}

#endif // !1