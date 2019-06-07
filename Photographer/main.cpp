#include "Photographer.h"
#include <GK-Undressing-People-Ceres/GeneralMesh.h>

#include <iostream>

int main()
{
    // deal with the camera object

    GeneralMesh camera_obj("C:/Users/Maria/3D Objects/Camera_simplified.obj");

    std::cout << "vertices" << std::endl;
    for (int i = 0; i < camera_obj.getNormalizedVertices().rows(); ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            std::cout << camera_obj.getNormalizedVertices()(i, j) << ", ";
        }
        std::cout << std::endl;
    }

    std::cout << "Faces" << std::endl;
    for (int i = 0; i < camera_obj.getFaces().rows(); ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            std::cout << camera_obj.getFaces()(i, j) << ", ";
        }
        std::cout << std::endl;
    }


    GeneralMesh object("D:/Data/SketchFab/Sexy Girl.obj");

    std::cout << "Mesh Loaded!" << std::endl;

    Photographer photographer(&object);

    photographer.addCameraToPosition(0.0f, 1.0f, 3.0f, 4.0f);
    photographer.addCameraToPosition(0.0f, -1.0f, -2.0f, 4.0f);
    photographer.addCameraToPosition(-1.0f, 1.0f, 3.0f, 4.0f);

    photographer.renderToImages("./images/");
    photographer.saveImageCamerasParamsCV("./images/");

    photographer.viewScene();
}
