#include "Photographer.h"
#include <GK-Undressing-People-Ceres/GeneralMesh.h>

#include <iostream>

int main()
{
    GeneralMesh object("D:/Data/SketchFab/Sexy Girl.obj");

    std::cout << "Mesh Loaded!" << std::endl;

    Photographer photographer(&object);

    photographer.addCameraToPosition(0.0f, 1.0f, 3.0f);
    photographer.addCameraToPosition(0.0f, -1.0f, -2.0f);
    photographer.addCameraToPosition(-1.0f, 1.0f, 3.0f);

    photographer.renderToImages("./images/");
    photographer.saveImageCamerasParamsCV("./images/");

    photographer.viewScene();
}
