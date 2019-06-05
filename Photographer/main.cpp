#include "Photographer.h"
#include <GK-Undressing-People-Ceres/GeneralMesh.h>

#include <iostream>

int main()
{
    GeneralMesh object("D:/Data/SketchFab/Sexy Girl.obj");

    std::cout << "Mesh Loaded!" << std::endl;

    Photographer photographer(&object);
    return photographer.run();
}
