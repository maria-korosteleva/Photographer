#include "Photographer.h"
#include <GK-Undressing-People-Ceres/GeneralMesh.h>

#include <iostream>

int main()
{
    GeneralMesh object("D:/Data/SketchFab/Sexy Girl.obj");

    std::cout << "Mesh Loaded!" << std::endl;

    Photographer photographer(&object);

    // 2_2 front, 2 back
    //photographer.addCameraToPosition(0.0f, 1.0f, 3.0f, 4.0f);
    //photographer.addCameraToPosition(1.0f, -0.5f, 2.0f, 4.0f);
    //photographer.addCameraToPosition(0.0f, -1.0f, -2.0f, 5.0f);
    //photographer.addCameraToPosition(-1.0f, 1.0f, -2.0f, 5.0f);

    // 3_ 3 front
    photographer.addCameraToPosition(0.0f, 1.0f, 3.0f, 4.0f);
    photographer.addCameraToPosition(1.0f, -0.5f, 2.0f, 4.0f);
    photographer.addCameraToPosition(-1.0f, 0.0f, 1.0f, 4.0f);


    photographer.renderToImages("./images/");
    photographer.saveImageCamerasParamsCV("./images/");

    photographer.viewScene();
}
