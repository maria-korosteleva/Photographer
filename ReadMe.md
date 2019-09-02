# Photographer 

A module to take pictures of the 3D object from multiple cameras =) Accepts GeneralMesh object as an input. 

## Functionality
* Save the rendered images from the cameras as files
* Save the camera parameters in OpenCV-friendly formats (works for OpenPos: https://github.com/CMU-Perceptual-Computing-Lab/openpose/))
* View the scene with the object and all the cameras

### Cameras: 
You can setup as many cameras as you want thtough addCameraToPosition(). 

To add the camera you need to specify it's position, assuming the object is at the world origin. Two options here:
* Absolute camera position
* Vector to the intended camera position and the distance to the camera. 

The resulting camera will be put at the specified position looking at the object

## Dependencies
* GeneralMesh (https://github.com/maria-korosteleva/GeneralMesh) 
* OpenGL 3.3 or higher.
* glad (https://glad.dav1d.de/): please use the version that suits your settings
* glfw (https://github.com/glfw/glfw)
* glm (http://glm.g-truc.net/0.9.5/index.html)
* stb_image_write.h (https://github.com/nothings/stb/blob/master/stb_image_write.h)
(on Win you could borrow them here: https://motionlab.kaist.ac.kr/git/mariako/Installed_libs)

Don't forget to add opengl32.lib; glfw3.lib; glad.lib; in the Linker options. If you don't have glad.lib, just add glad.c to your project. 


## How to link (VisualStudio):
* Add the project directory (or parent of it) to the include directories 
         (Configuration Properties -> C/C++ -> General -> Additional Include Directories)
* Add the Photographer.lib to the Linker options (Configuration Properties -> Linker -> Input -> Additional Dependencies

## For MCLab members
Please, contact Maria before making any modifications

## Further development: 
You can extend the class with the fuctionality you need
     * For backward compatibility, do not change existing interfaces
     * Kindly ask to follow the existing code style and to keep the new finctions small and readible. 
	 
### Ideas
* Other camera parameters formats
* Allow to remove cameras
* Allow to add arbitrary cameras
* Backward projection
* Support quad meshes (minor)
