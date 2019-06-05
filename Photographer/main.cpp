#include "Photographer.h"
#include <GK-Undressing-People-Ceres/GeneralMesh.h>

#include <iostream>

int main()
{
    GeneralMesh object("D:/Data/smpl_outs/pose_00048_270_dyna_thin.obj");

    std::cout << "Mesh Loaded!" << std::endl;

    Photographer photographer(&object);
    return photographer.run();
}
