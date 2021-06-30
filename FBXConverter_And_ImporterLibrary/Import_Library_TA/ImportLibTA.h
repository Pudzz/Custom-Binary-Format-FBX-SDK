#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include "MemoryHandler.h"

namespace ImportLibTA
{
	// Initialize the memory
	/*
	 * To see the full dokumentation of the api, please see the MemoryHandler.h file
	 * The namespace ImportLibTA is only used to load the file, its the memoryHandler who hold all the memory afterwards
	*/
	MemoryHandler* memory = nullptr;

	// Read file function
	inline int ReadFile(const char* fileName)
	{
		// If the memory is not null, then delete it
		// This may occur if this function is used more than once in the same instance
		// This way, the importer will only control one scene at a time
		// It is the users obligation to make sure he saves the needed information to his own classes before changing scene
		if (memory)
		{
			delete memory;
		}

		// Create the new memory
		memory = new MemoryHandler();

		// Read the chosen file
		std::ifstream reader(fileName, std::ios::binary);

		// If the file couldnt be read, for whatever reason
		if (!reader) {
			std::cout << "Cant read file" << std::endl;
			return -1;
		}

		// Continue the reading
		else
		{
			int meshCounter = 1, polygonCounter = 1, lightCounter = 1, cameraCounter = 1, materialCounter = 1, textureCounter = 1;

			// The importer first reads the scene, so it can know how many objects there are to read of each class
			// And then saves it to the memory class
			SceneHeader* ScenePtr = new SceneHeader;
			reader.read((char*)ScenePtr, sizeof(SceneHeader));
			memory->GetSceneVector().push_back(*ScenePtr);

			// If there are any meshes in the file
			if (ScenePtr->nrOfMeshes > 0)
			{
				// Reserve the memory in the vectors, to eliminate future reallocations
				memory->GetMeshVector().reserve(ScenePtr->nrOfMeshes);
				memory->GetVertexVector().reserve(ScenePtr->nrOfMeshes);
				memory->GetMaterialVector().reserve(ScenePtr->nrOfMeshes);

				// Reads all the meshes info and saves it to the memory
				Meshheader* MeshPtr = new Meshheader[ScenePtr->nrOfMeshes];
				reader.read((char*)MeshPtr, sizeof(Meshheader) * ScenePtr->nrOfMeshes);
				for (int meshID = 0; meshID < ScenePtr->nrOfMeshes; meshID++)
				{
					memory->GetMeshVector().push_back(MeshPtr[meshID]);
				}

				// For every mesh
				for (int i = 0; i < ScenePtr->nrOfMeshes; i++)
				{
					meshCounter++;

					// Read the current mesh control points and save to memory
					// The control points can be used for morph animations if there are any blendshapes,
					// who targets this mesh
					Controlpoints* ctrlPoints = new Controlpoints[MeshPtr[i].nrOfControlpoints];
					reader.read((char*)ctrlPoints, sizeof(Controlpoints) * MeshPtr[i].nrOfControlpoints);
					std::vector<Controlpoints> tempCtrl;
					for (int ctrlID = 0; ctrlID < MeshPtr[i].nrOfControlpoints; ctrlID++)
					{
						tempCtrl.push_back(ctrlPoints[ctrlID]);
					}
					memory->GetMeshCtrlVector().push_back(tempCtrl);

					// Deallocate memory
					delete[] ctrlPoints;

					// Reads the amount of polygons from the file
					Polygon* PolygonPtr = new Polygon[MeshPtr[i].nrOfPolygons];
					reader.read((char*)PolygonPtr, sizeof(Polygon) * MeshPtr[i].nrOfPolygons);

					// Reserve memory for every vertex
					std::vector<Vertex> temp;
					temp.reserve(MeshPtr[i].nrOfPolygons * static_cast<long long>(3));

					// For every polygon, saves the vertices to memory
					for (int k = 0; k < MeshPtr[i].nrOfPolygons; k++)
					{
						for (int j = 0; j < 3; j++)
						{
							temp.push_back(PolygonPtr[k].vertices[j]);
						}
					}
					memory->GetVertexVector().push_back(temp);

					// Deallocate the polygonstruct, we only need the vertices
					delete[] PolygonPtr;

					// Read the materials for the current mesh and save it to memory
					Materialheader* MaterialPtr = new Materialheader;
					reader.read((char*)MaterialPtr, sizeof(Materialheader));
					memory->GetMaterialVector().push_back(*MaterialPtr);

					Texture* RTPtr = nullptr;

					// If the current material has any textures
					if (MaterialPtr->nrOfTextures > 0) {

						// Save every texture in the current material to memory
						for (int q = 0; q < MaterialPtr->nrOfTextures; q++) {
							RTPtr = new Texture;
							reader.read((char*)RTPtr, sizeof(Texture));
							memory->GetTextureVector().push_back(*RTPtr);

							// Deallocate the texture
							delete RTPtr;
						}
					}
					// Deallocate the materials
					delete MaterialPtr;
				}

				// Deallocate the meshes
				delete[] MeshPtr;
			}

			// Read and save the lights to memory
			if (ScenePtr->nrOfLight > 0)
			{
				// Reserve memory
				memory->GetLightVector().reserve(ScenePtr->nrOfLight);

				LightHeader* LightPtr = new LightHeader[ScenePtr->nrOfLight];
				reader.read((char*)LightPtr, sizeof(LightHeader) * ScenePtr->nrOfLight);
				for (int lightID = 0; lightID < ScenePtr->nrOfLight; lightID++)
				{
					memory->GetLightVector().push_back(LightPtr[lightID]);
				}

				// Deallocate the light
				delete[] LightPtr;
			}
			
			// Read and save cameras to memory
			if (ScenePtr->nrOfCamera > 0)
			{
				// Reserve memory
				memory->GetCameraVector().reserve(ScenePtr->nrOfCamera);

				CameraHeader* CameraPtr = new CameraHeader[ScenePtr->nrOfCamera];
				reader.read((char*)CameraPtr, sizeof(CameraHeader) * ScenePtr->nrOfCamera);
				for (int cameraID = 0; cameraID < ScenePtr->nrOfCamera; cameraID++)
				{
					memory->GetCameraVector().push_back(CameraPtr[cameraID]);
				}

				// Deallocate the cameras
				delete[] CameraPtr;
			}

			// If the scene contains any morph animations
			if (ScenePtr->nrOfAnimations > 0)
			{
				// For every morph animation
				for (int i = 0; i < ScenePtr->nrOfAnimations; i++)
				{
					// Read and save the current animation info to memory
					Animation* newAnim = new Animation[ScenePtr->nrOfAnimations];
					reader.read((char*)newAnim, sizeof(Animation)* ScenePtr->nrOfAnimations);
					for (int animID = 0; animID < ScenePtr->nrOfAnimations; animID++)
					{
						memory->GetAnimationVector().push_back(newAnim[animID]);
					}

					for (int blendID = 0; blendID < newAnim[i].nrOfBlendshapes; blendID++)
					{
						BlendShape* blendPtr = new BlendShape;
						reader.read((char*)blendPtr, sizeof(BlendShape));
						//memory->GetBlendShapeVector().push_back(BlendPtr[blendID]);
						memory->GetBlendShapeVector().push_back(*blendPtr);

						std::vector<Keyframe> keyFrames;

						for (int key = 0; key < blendPtr->nrOfKeyframes; key++)
						{
							Keyframe* keys = new Keyframe;
							reader.read((char*)keys, sizeof(Keyframe));
							keyFrames.push_back(*keys);
							delete keys;
						}
						memory->GetKeyframeVector().push_back(keyFrames);

						std::vector<Controlpoints> blendCtrlVector;

						for (int ctrl = 0; ctrl < blendPtr->nrOfControlpoints; ctrl++)
						{
							Controlpoints* blendCtrl = new Controlpoints;
							reader.read((char*)blendCtrl, sizeof(Controlpoints));
							blendCtrlVector.push_back(*blendCtrl);
							delete blendCtrl;
						}
						memory->GetBlendShapeCtrlVector().push_back(blendCtrlVector);
						delete blendPtr;
					}
					delete[] newAnim;
				}
			}

			// Deallocate the scene
			delete ScenePtr;

			/*SceneHeader* newScene = new SceneHeader;
			reader.read((char*)newScene, sizeof(SceneHeader));*/
			// Close the file
			reader.close();
		}
		
		// End
		return 0;
	}
}
