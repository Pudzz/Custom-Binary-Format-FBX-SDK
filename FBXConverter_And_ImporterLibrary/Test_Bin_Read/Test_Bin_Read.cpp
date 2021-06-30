#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <iostream>
#include <string>
#include "../Import_Library_TA/ImportLibTA.h"

int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    int read = ImportLibTA::ReadFile("C:/Users/Pad/Desktop/testDemo.bin");  // Set correct path to bin-file

    if (read == -1)
    {
        std::cout << "Couldnt open the requested file" << std::endl;
    }
    else
    {
        MemoryHandler* newMem = ImportLibTA::memory;

        //Scene
        int meshes = newMem->GetSceneNrOfMeshes(0);
        int camera = newMem->GetSceneNrOfCameras(0);
        int lights = newMem->GetSceneNrOfLights(0);
        int animation = newMem->GetSceneNrOfAnimation(0);

        //Mesh
        std::string meshName = newMem->GetMeshName(0);
        int nrOfPolys = newMem->GetMeshNrOfPolygons(0);
        int nrofMaterials = newMem->GetMeshNrOfMaterials(0);

        //Material
        int materialIndex = newMem->GetMaterialIndex(0);
        int nrOfTextures = newMem->GetMaterialNrofTextures(0);
        std::string matname = newMem->GetMaterialName(0);
        std::string shadingMod = newMem->GetMaterialShadingModel(0);

        //Animation
        std::string animMeshRelated = newMem->GetAnimationMeshRelated(0);
        int animDuration = newMem->GetAnimationDuration(0);
        int animnrofBlendShapes = newMem->GetAnimationNrOfBlendShapes(0);
                
        //Scene
        std::cout << "Nr of Meshes " << meshes << std::endl;
        std::cout << "Nr of cameras " << camera << std::endl;
        std::cout << "Nr of Lights " << lights << std::endl;
        std::cout << "Nr of Animation " << animation << std::endl;
        std::cout << std::endl << std::endl;

        //Mesh
        std::cout << "Mesh name " << meshName << std::endl;
        std::cout << "Nr of Polygons " << nrOfPolys << std::endl;
        std::cout << "Nr of materials " << nrofMaterials << std::endl;
        std::cout << std::endl << std::endl;

        //Material
        std::cout << "Material name " << matname << std::endl;
        std::cout << "Material index " << materialIndex << std::endl;
        std::cout << "Nr of textures " << nrOfTextures << std::endl;
        std::cout << "Shading model  " << shadingMod << std::endl;
        std::cout << std::endl << std::endl;

        std::cout << "animMeshRelated " << animMeshRelated << std::endl;
        std::cout << "animDuration " << animDuration << std::endl;
        std::cout << "animnrofBlendShapes " << animnrofBlendShapes << std::endl;
        std::cout << std::endl << std::endl;

        std::vector<Vertex> vertices = newMem->GetVerticesAtIndex(0);
        for (int i = 0; i < vertices.size(); i++)
        {
            std::cout << vertices[i].position[0] << " " << vertices[i].position[1] << " " << vertices[i].position[2] << " " << std::endl;
        }

        float stop = 0;
        delete newMem;
    }
    
    
    
}

//Guides
/*
https://www.youtube.com/watch?v=r0QR3HH3xDs
https://www.youtube.com/watch?v=a4mS6aKgxys
*/