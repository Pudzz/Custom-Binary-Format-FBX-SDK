#include "FBXConverter.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>
#include <math.h>
#include "Format.h"
#include <assert.h>
#include <iomanip>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

static bool gVerbose = true;
using namespace std;

FbxNode* meshNode;

// Checks the incoming fbx scene for how many different objects there are and stores the info for future use
void processFbxScene(FbxScene* scene, int& nrOfMeshes, int& nrOfLights, int& nrOfCameras, int& nrOfAnimations, vector<FbxMesh*>& meshes, vector<FbxNode*>& nodes, vector<FbxNodeAttribute::EType>& types, 
	vector<string>& nodeNames, vector<FbxLight*>& lights, vector<FbxCamera*>& cameras, vector<int>& nrOfPolygons, vector<int>& nrOfMeshMaterials, vector<int>& nrOfMeshTextures, vector<FbxNode*>& nodeToSave);

// Functions for filling our own format structures
void fillPolygonHeader(Polygon* polygonStructure, int nrOfPolygons, FbxVector4* vertices, FbxMesh* mesh, Fbxconverter fbxConverter);
void fillLightHeader(FbxLight* lights, LightHeader* lightHeaders, Fbxconverter fbxConverter);
void fillCameraHeader(FbxCamera* cameras, CameraHeader& cameraHeaders, Fbxconverter fbxConverter);
void fillMaterials(FbxNode* node, FbxGeometry* geometry, std::vector<Materialheader*>& matVector, int nrOfmaterials);
void fillTextureHeaders(FbxNode* node, int nrOfMaterials, std::vector<Texture*>& textureHeader, int& nrOfTextures);
void findConnectionsForTexture(FbxProperty pProperty, bool& pDisplayHeader, int pMaterialIndex, std::vector<Texture*>& textureHeader, int& nrOfTextures);
void SaveTextureInfo(FbxTexture* pTexture, int pBlendMode, std::vector<Texture*>& textureheader, Texture* textureInfo, FbxProperty pProperty);


void SaveControlpoints(FbxMesh* pMesh, std::vector<Controlpoints*>& ctrlPoints);
void fillAnimation(FbxScene* scene, FbxNode* currentNode, std::vector<Animation>& animation, std::vector<BlendShape*>& blendshapes,
	vector<vector<Controlpoints>>& blendCtrlpoints, vector<vector<Keyframe>>& keyframes);

// These functions are for testing prints of various objects when read from our own file format
void testScenes(SceneHeader* scene);
void testPositions(Polygon* polygonstruct, int nrOfPolygons);
void testLights(LightHeader* light, int nrOfLights);
void testCameras(CameraHeader camera);
void testMaterials(Materialheader mat);
void testTextures(Texture tex);

int main(){
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	Fbxconverter fbxConverter;
	FbxManager* manager = NULL;
	FbxScene* scene = NULL;
	bool result;
	
	
	int nrOfObjects = 0;
	int nrOfMeshes = 0;
	int nrOfLights = 0;
	int nrOfCameras = 0;
	int nrOfAnimations = 0;

	 //Vector variables to store information dynamically
	vector<FbxNode*> nodes;
	vector<FbxNode*> meshNodes;
	vector<string> nodeNames;
	vector<FbxNodeAttribute::EType> types;
	vector<FbxMesh*> meshes;
	vector<FbxLight*> lights;
	vector<FbxCamera*> cameras;
	vector<FbxLight::EType> lightTypes;
	vector<int> nrOfPolygons;
	vector<Polygon*> scenePolygons;
	vector<int> nrOfMeshMaterials;
	vector<int> nrOfMeshTextures;
	vector<vector<Keyframe>> keyframes;
	FbxVector4* vertices;

	// Own structures where we will store the info of our choosing
	Polygon* polygons;
	Meshheader* meshHeaders;
	LightHeader* lightHeaders;
	SceneHeader* sceneHeaders = new SceneHeader;
	CameraHeader* cameraHeaders;
	vector<Materialheader*> materialVector;
	vector<Texture*> textureVector;

	vector<Controlpoints*> controlpointsMeshvector;

	 //Animation stuff
	vector<Animation> animVector;
	vector<BlendShape*> blendshapeVector;
	vector<vector<Controlpoints>> controlpointsBlendshape;

	//string filePath = "../../Test_Bin_Read/";
	string filePath = "C:/Users/";
	string fileName = "";
	string user = "";
	string desktop = "/Desktop/";
	string bin = ".bin";

	cout << "PLEASE ENTER YOUR USER ON YOUR DRIVE :" << endl;
	cin >> user; cout << endl;

	cout << "PLEASE ENTER YOUR WANTED FILENAME :" << endl;
	cin >> fileName;

	string fullPath = filePath + user + desktop + fileName + bin;

	// Open the file where we will print our info
	ofstream outFile(fullPath, ofstream::binary);

	// Init manager and scene
	fbxConverter.InitializeSdkObjects(manager, scene);

	// Opens the fbx file to read
	FbxString lFilePath("");
	if (lFilePath.IsEmpty())
	{
		lFilePath = "demoScene.fbx";
		result = fbxConverter.loadScene(manager, scene, lFilePath.Buffer());
	}
	else
	{
		result = fbxConverter.loadScene(manager, scene, lFilePath.Buffer());
	}
	if (result == false)
	{
		cout << "Error while reading the fbx file" << endl;
	}

	// Checks the scene for info
	processFbxScene(scene, nrOfMeshes, nrOfLights, nrOfCameras, nrOfAnimations, meshes, nodes, types,
		nodeNames, lights, cameras, nrOfPolygons, nrOfMeshMaterials, nrOfMeshTextures, meshNodes);

	// Store and write the amount of objects in the scene
	sceneHeaders[0].nrOfMeshes = nrOfMeshes;
	sceneHeaders[0].nrOfLight = nrOfLights;
	sceneHeaders[0].nrOfCamera = nrOfCameras;
	sceneHeaders[0].nrOfAnimations = nrOfAnimations;

	outFile.write((const char*)sceneHeaders, sizeof(SceneHeader));

	 //For each mesh in the scene, store and write mesh info
	if (nrOfMeshes > 0)
	{
		meshHeaders = new Meshheader[nrOfMeshes];
		for (int i = 0; i < nrOfMeshes; i++)
		{
			// Set polygonamount for the meshHeader, for every mesh
			meshHeaders[i].nrOfPolygons = nrOfPolygons[i];
			meshHeaders[i].nrOfMaterials = nrOfMeshMaterials[i];
			meshHeaders[i].meshIndex = i;

			// Set the name for every mesh to the header
			string name = meshes[i]->GetNode()->GetName();
			strcpy_s(meshHeaders[i].meshName, name.c_str());

			// ctrl points
			int ctrlPoints = meshes[i]->GetControlPointsCount();
			meshHeaders[i].nrOfControlpoints = ctrlPoints;
		}

		outFile.write((const char*)meshHeaders, sizeof(Meshheader) * nrOfMeshes);
		delete[] meshHeaders;

		// For each mesh, store and write info such as polygons, materials and textures
		for (int i = 0; i < nrOfMeshes; i++)
		{
			// Get the vertices from the current mesh
			vertices = meshes[i]->GetControlPoints();

			// ctrl points
			SaveControlpoints(meshes[i], controlpointsMeshvector);
			if (controlpointsMeshvector.size() > 0) {
				for (int i = 0; i < controlpointsMeshvector.size(); i++) {
					outFile.write((const char*)controlpointsMeshvector[i], sizeof(Controlpoints));
					delete controlpointsMeshvector[i];
				}				
			}

			// Create a new dynamic polygonarray with matching amount of polygons
			polygons = new Polygon[nrOfPolygons[i]];
			// Fill the dynamic polygonarray with information based on the mesh, vertices e.t.c.
			fillPolygonHeader(polygons, nrOfPolygons[i], vertices, meshes[i], fbxConverter);
			// Write the polygonarray to a memory file
			outFile.write((const char*)polygons, sizeof(Polygon) * nrOfPolygons[i]);
			delete[] polygons;

			fillMaterials(meshNodes[i], meshes[i], materialVector, nrOfMeshMaterials[i]);

			int textureCount = 0;
			fillTextureHeaders(meshNodes[i], nrOfMeshMaterials[i], textureVector, textureCount);

			// Material
			materialVector[i]->nrOfTextures = textureCount;
			materialVector[i]->index = i;
			outFile.write((const char*)materialVector[i], sizeof(Materialheader));

			// Textures
			for (int j = 0; j < textureCount; j++)
			{
				if (textureVector.size() > 0)
				{
					textureVector[j]->materialIndex = i;

					int width, height, channels;
					unsigned char* image = stbi_load(textureVector[j]->texturePath, &width, &height, &channels, STBI_rgb);
					if (image == NULL)
					{
						cout << "Couldnt open the selected png" << endl;
					}

					string oldPath = textureVector[j]->texturePath;
					size_t pos = oldPath.find_last_of("/\\");

					string newPath = "";
					newPath = filePath + user + desktop + oldPath.substr(pos + 1);
					
					strcpy_s(textureVector[j]->texturePath, newPath.c_str());
					outFile.write((char*)textureVector[j], sizeof(Texture));
					
					stbi_write_png(newPath.c_str(), width, height, channels, image, width * channels);
					stbi_image_free(image);
				}
			}

			if (textureVector.size() > 0)
			{
				for (int j = 0; j < textureVector.size(); j++)
				{
					delete textureVector[j];
				}
				textureVector.clear();
			}
		}

		for (int i = 0; i < materialVector.size(); i++)
		{
			delete materialVector[i];
		}
	}
	
	// Store and write lights info
	if (nrOfLights > 0)
	{
		for (int i = 0; i < nrOfLights; i++)
		{
			lightHeaders = new LightHeader;
			fillLightHeader(lights[i], lightHeaders, fbxConverter);
			outFile.write((const char*)lightHeaders, sizeof(LightHeader));
			delete lightHeaders;
		}

	}
	
	// Store and write cameras info
	if (nrOfCameras > 0)
	{
		cameraHeaders = new CameraHeader[nrOfCameras];
		for (int i = 0; i < nrOfCameras; i++)
		{
			fillCameraHeader(cameras[i], cameraHeaders[i], fbxConverter);
		}
		outFile.write((const char*)cameraHeaders, sizeof(CameraHeader) * nrOfCameras);
		delete[] cameraHeaders;
	}

	if (nrOfAnimations > 0) {
		for (int ik = 0; ik < nrOfMeshes; ik++) {
			fillAnimation(scene, meshNodes[ik], animVector, blendshapeVector, controlpointsBlendshape, keyframes);

			if (animVector.size() > 0) {
				for (int aniIndex = 0; aniIndex < animVector.size(); aniIndex++)
				{
					outFile.write((const char*)&animVector[aniIndex], sizeof(Animation));

					if (blendshapeVector.size() > 0) {
						for (int blendIndex = 0; blendIndex < blendshapeVector.size(); blendIndex++)
						{
							outFile.write((const char*)blendshapeVector[blendIndex], sizeof(BlendShape));
							for (int keyIndex = 0; keyIndex < keyframes[blendIndex].size(); keyIndex++)
							{
								outFile.write((const char*)&keyframes[blendIndex][keyIndex], sizeof(Keyframe));
							}

							if (controlpointsBlendshape.size() > 0) {
								for (int ctrlIndex = 0; ctrlIndex < controlpointsBlendshape[blendIndex].size(); ctrlIndex++)
								{
									outFile.write((const char*)&controlpointsBlendshape[blendIndex][ctrlIndex], sizeof(Controlpoints));
								}
							}
						}
					}
				}
				animVector.clear();
			}	
		}

		for (int i = 0; i < blendshapeVector.size(); i++)
		{
			delete blendshapeVector[i];
		}
		blendshapeVector.clear();
		controlpointsBlendshape.clear();
	}

	delete sceneHeaders;

	SceneHeader* testing = new SceneHeader;
	testing->nrOfAnimations = 3;
	testing->nrOfCamera = 2;
	testing->nrOfLight = 1;
	testing->nrOfMeshes = 11;
	
	outFile.write((const char*)testing, sizeof(SceneHeader));
	delete testing;

	 //Close the file
	outFile.close();


	//THIS IS FOR TESTING THE WRITING AND READING //
	//ifstream inFile(fullPath, ifstream::binary);

	//SceneHeader* testScene = new SceneHeader;
	//Polygon* polygonstruct = nullptr;
	//Meshheader* testMesh = new Meshheader[5];
	//Materialheader* testMaterial = new Materialheader;
	//Texture* testTexture = new Texture;
	//Controlpoints* ctrlpointsMesh;


	//Animation* animations;
	//BlendShape* blendshapes;
	//Controlpoints* ctrlpointsBlendshapes;
	//Keyframe* keys;
	//inFile.read((char*)testScene, sizeof(SceneHeader));
	//testScenes(testScene);

	//if (nrOfMeshes > 0)
	//{
	//	inFile.read((char*)testMesh, sizeof(Meshheader)* nrOfMeshes);
	//	for (int i = 0; i < nrOfMeshes; i++)
	//	{
	//		cout << "mesh has index :" << testMesh[i].meshIndex << endl;
	//		cout << "mesh name is : " << testMesh[i].meshName << endl;
	//		cout << "Mesh has nr of polygons: " << testMesh[i].nrOfPolygons << endl;
	//		cout << "Mesh has nr of controlpoints: " << testMesh[i].nrOfControlpoints << endl;

	//		ctrlpointsMesh = new Controlpoints[testMesh[i].nrOfControlpoints];
	//		inFile.read((char*)ctrlpointsMesh, sizeof(Controlpoints)* testMesh[i].nrOfControlpoints);
	//		
	//		for (int y = 0; y < testMesh[i].nrOfControlpoints; y++) {
	//			cout << "Controlpoint " << y << endl;
	//			cout << "Position: " << ctrlpointsMesh[y].position[0] << " " << ctrlpointsMesh[y].position[1] << " " << ctrlpointsMesh[y].position[2] << endl;
	//			cout << "Normal: " << ctrlpointsMesh[y].normal[0] << " " << ctrlpointsMesh[y].normal[1] << " " << ctrlpointsMesh[y].normal[2] << endl;
	//			cout << endl;
	//		}

	//		polygonstruct = new Polygon[nrOfPolygons[i]];
	//		inFile.read((char*)polygonstruct, sizeof(Polygon) * nrOfPolygons[i]);

	//		testPositions(polygonstruct, nrOfPolygons[i]);

	//		inFile.read((char*)testMaterial, sizeof(Materialheader));
	//		testMaterials(*testMaterial);

	//		if (testMaterial->nrOfTextures > 0)
	//		{
	//			for (int j = 0; j < testMaterial->nrOfTextures; j++)
	//			{
	//				inFile.read((char*)testTexture, sizeof(Texture));
	//				testTextures(*testTexture);
	//			
	//			}
	//		}
	//	}
	//}
	//
	//if (nrOfLights > 0)
	//{
	//	for (int i = 0; i < nrOfLights; i++)
	//	{
	//		LightHeader* testLight = new LightHeader;
	//		inFile.read((char*)testLight, sizeof(LightHeader));
	//		testLights(testLight, 1);
	//		delete testLight;
	//	}
	//}

	//if (nrOfCameras > 0)
	//{
	//	CameraHeader* testCam = new CameraHeader[nrOfCameras];
	//	inFile.read((char*)testCam, sizeof(CameraHeader)* nrOfCameras);
	//	for (int i = 0; i < nrOfCameras; i++)
	//	{
	//		testCameras(testCam[i]);
	//	}
	//	delete[] testCam;
	//}


	//if (nrOfAnimations > 0) {
	//	
	//	animations = new Animation[nrOfAnimations];
	//	inFile.read((char*)animations, sizeof(Animation)* nrOfAnimations);

	//	for (int i = 0; i < nrOfAnimations; i++) {
	//		cout << " ANIMATION INDEX: " << animations[i].index << endl;
	//		cout << " ANIMATION DURATION: " << animations[i].duration << endl;
	//		cout << " ANIMATION MESHRELATED: " << animations[i].meshRelated << endl;
	//		cout << " ANIMATION, nr of blendshapes: " << animations[i].nrOfBlendshapes << endl;

	//		for (int b = 0; b < animations[i].nrOfBlendshapes; b++)
	//		{
	//			blendshapes = new BlendShape;
	//			inFile.read((char*)blendshapes, sizeof(BlendShape));

	//			cout << "Blendshape name: " << blendshapes->blendShapeName << endl;
	//			cout << "Blendshape channel name: " << blendshapes->blendShapeChannelname << endl;
	//			cout << "Blendshape target name: " << blendshapes->targetShapeName << endl;
	//			cout << "Blendshape mesh related: " << blendshapes->meshRelated << endl;

	//			cout << "Blendshape nr of keyframes: " << blendshapes->nrOfKeyframes << endl;

	//			for (int key = 0; key < blendshapes->nrOfKeyframes; key++)
	//			{
	//				keys = new Keyframe;
	//				inFile.read((char*)keys, sizeof(Keyframe));
	//				cout << "Keyframe: " << key << endl;
	//				cout << "Blendshape index: " << keys->BlendIndex << endl;
	//				cout << "Time: " << keys->time << endl;
	//				cout << "Value: " << keys->value << endl;
	//			}

	//			cout << "Blendshape, nr of controlpoints: " << blendshapes->nrOfControlpoints << endl;
	//			for (int ctrl = 0; ctrl < blendshapes->nrOfControlpoints; ctrl++)
	//			{
	//				ctrlpointsBlendshapes = new Controlpoints;
	//				inFile.read((char*)ctrlpointsBlendshapes, sizeof(Controlpoints));
	//				cout << "Controlpoint " << ctrl << endl;
	//				cout << "Position: " << ctrlpointsBlendshapes->position[0] << " " << ctrlpointsBlendshapes->position[1] << " " << ctrlpointsBlendshapes->position[2] << endl;
	//				cout << "Normal: " << ctrlpointsBlendshapes->normal[0] << " " << ctrlpointsBlendshapes->normal[1] << " " << ctrlpointsBlendshapes->normal[2] << endl;
	//				cout << endl;

	//			}
	//		}
	//	}
	//	delete[] animations;
	//}
	//delete testScene;

	/*Check so that everyting is correct */
	//SceneHeader *te = new SceneHeader;
	//inFile.read((char*)te, sizeof(SceneHeader));
	//cout << te->nrOfAnimations << endl;
	//cout << te->nrOfCamera << endl;
	//cout << te->nrOfLight << endl;
	//cout << te->nrOfMeshes << endl;

	//inFile.close();
	// 
	// HERE ENDS TESTING OF WRITE AND READ //

	// Destroy manager
	fbxConverter.DestroySdkObjects(manager);

	return 0;
}

void processFbxScene(FbxScene* scene, int& nrOfMeshes, int& nrOfLights, int& nrOfCameras, int& nrOfAnimations, vector<FbxMesh*>& meshes, vector<FbxNode*>& nodes, vector<FbxNodeAttribute::EType>& types,
	vector<string>& nodeNames, vector<FbxLight*>& lights, vector<FbxCamera*>& cameras, vector<int>& nrOfPolygons, vector<int>& nrOfMeshMaterials, vector<int>& nrOfMeshTextures, vector<FbxNode*>& nodeToSave)
{
	// Get amount of nodes in the fbx scene
	int nodeCount = scene->GetNodeCount();

	// For each raw node
	for (int i = 0; i < nodeCount; i++)
	{
		// Store each node
		nodes.push_back(scene->GetNode(i));

		// If the current node isnt a valid object
		if ((nodes[i]->GetNodeAttribute()) == NULL)
		{
			cout << "There is no node attribute which belongs to: " << nodes[i]->GetName() << endl;
		}
		else
		{
			// Store the current objects type (type is for example a mesh or a camera)
			types.push_back(nodes[i]->GetNodeAttribute()->GetAttributeType());

			// Store the names
			nodeNames.push_back(nodes[i]->GetName());

			// If the current object type is a mesh
			if (types.back() == FbxNodeAttribute::eMesh)
			{
				// Global mesh node for use of skeleton, can be erased
				meshNode = nodes[i];

				// Save the node for future use
				nodeToSave.push_back(nodes[i]);
				// Add one more mesh
				nrOfMeshes+=1;
				// Store the current mesh object as a "mesh" in a dynamic vector
				meshes.push_back((FbxMesh*)nodes[i]->GetNodeAttribute());
				// Store the amount of polygons for that object
				nrOfPolygons.push_back(meshes.back()->GetPolygonCount());
				// Store the amount of materials on this mesh
				nrOfMeshMaterials.push_back(meshNode->GetMaterialCount());

				// Current node
				FbxGeometry* geonode = (FbxGeometry*)meshNode->GetNodeAttribute();

				// Total blendshapes for this node
				int blendshapescount = geonode->GetDeformerCount(FbxDeformer::eBlendShape);
				if(blendshapescount > 0)
					nrOfAnimations++;
			}

			// If the current object type is a camera
			if (types.back() == FbxNodeAttribute::eCamera)
			{
				nrOfCameras+=1;
				cameras.push_back((FbxCamera*)nodes[i]->GetNodeAttribute());
			}

			// If the current object type is a light
			if (types.back() == FbxNodeAttribute::eLight)
			{
				nrOfLights+=1;
				lights.push_back((FbxLight*)nodes[i]->GetNodeAttribute());
			}
		}
	}
}

void fillPolygonHeader(Polygon* polygonStructure, int nrOfPolygons, FbxVector4* vertices, FbxMesh* mesh, Fbxconverter fbxConverter)
{
	int vertexId = 0;
	// For each polygon
	for (int i = 0; i < nrOfPolygons; i++)
	{
		int polygonSize = mesh->GetPolygonSize(i);

		// For each unique vertex in the polygon/triangle
		for (int j = 0; j < polygonSize; j++)
		{
			// Get the index to a vertex, i represents which polygon we are currently in and j represents which vertex in that polygon
			int controlPointIndex = mesh->GetPolygonVertex(i, j);

			// If its less than 0 there are no index
			if (controlPointIndex < 0)
			{
				cout << "Invalid index found for coordinates in polygon " << i << " at vertex nr: " << j << endl;
				continue;
			}
			else
			{
				// Stores the x, y and z positions at the current vertex in a custom float3 structure
				float3 vertex = fbxConverter.get3DVector(vertices[controlPointIndex]);

				// Pass the x, y and z positions to our polygonHeader
				// The Z value is multiplied with -1, since it needs to be converted from a right handed system to a left handed
				// Such as DirectX
				polygonStructure[i].vertices[j].position[0] = vertex.x;
				polygonStructure[i].vertices[j].position[1] = vertex.y;
				polygonStructure[i].vertices[j].position[2] = vertex.z * -1.0f;
			}

			// For each UV element
			for (int l = 0; l < mesh->GetElementUVCount(); l++)
			{
				// Get the uv element, OBS: not the actual UV coordinates
				FbxGeometryElementUV* UV = mesh->GetElementUV(l);
				float2 uv = {0.0f, 0.0f };

				// Switch statement depending on what mode is used for UV
				switch (UV->GetMappingMode())
				{
				default:
					break;
				case FbxGeometryElement::eByControlPoint:
					switch (UV->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
						// Stores the uv coordinates in a custom float2 structure
						uv = fbxConverter.get2DVector(UV->GetDirectArray().GetAt(controlPointIndex));
						break;
					case FbxGeometryElement::eIndexToDirect:
					{
						// Stores the uv coordinates in a custom float2 structure
						int id = UV->GetIndexArray().GetAt(controlPointIndex);
						uv = fbxConverter.get2DVector(UV->GetDirectArray().GetAt(id));
					}
					break;
					default:
						break; // other reference modes not shown here!
					}
					break;

				case FbxGeometryElement::eByPolygonVertex:
				{
					int lTextureUVIndex = mesh->GetTextureUVIndex(i, j);
					switch (UV->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					case FbxGeometryElement::eIndexToDirect:
					{
						// Stores the uv coordinates in a custom float2 structure
						uv = fbxConverter.get2DVector(UV->GetDirectArray().GetAt(lTextureUVIndex));
					}
					break;
					default:
						break; // other reference modes not shown here!
					}
				}
				break;

				case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
				case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
				case FbxGeometryElement::eNone:      // doesn't make much sense for UVs
					break;
				}

				// Pass the u and v values to the polygonStructure
				// The V value is changed by 1, since it needs to be converted from right handed to left handed space
				polygonStructure[i].vertices[j].texCoord[0] = floor(10000 * uv.u) / 10000;
				polygonStructure[i].vertices[j].texCoord[1] = 1.0f - (floor(10000 * uv.v) / 10000);
			}
			
			// For each normal
			for (int l = 0; l < mesh->GetElementNormalCount(); l++)
			{
				// Get the normal element
				FbxGeometryElementNormal* normal = mesh->GetElementNormal(l);
				float3 normals;

				// Different mapping methods, may not be needed
				if (normal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					switch (normal->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
						// Stores the x, y, and z values from the normal in a custom float3 structure
						normals = fbxConverter.get3DVector(normal->GetDirectArray().GetAt(vertexId));
						break;
					case FbxGeometryElement::eIndexToDirect :
						{
							// Stores the x, y, and z values from the normal in a custom float3 structure
							int id = normal->GetIndexArray().GetAt(vertexId);
							normals = fbxConverter.get3DVector(normal->GetDirectArray().GetAt(id));
						}
						break;
					default:
						break;
					}
				}
				// If the mapping mode for normals is "by control point"! This will be the case if the mesh is a smoothed object, like a sphere
				if (normal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
				{
					switch (normal->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
						normals = fbxConverter.get3DVector(normal->GetDirectArray().GetAt(l));
					}
				}

				// Pass the normal to our polygonStructure 
				// The Z value is changed to negative to convert from right handed to left handed space
				polygonStructure[i].vertices[j].normal[0] = floor(10000 * normals.x)/ 10000;
				polygonStructure[i].vertices[j].normal[1] = floor(10000 * normals.y) / 10000;
				polygonStructure[i].vertices[j].normal[2] = (floor(10000 * normals.z) / 10000) * -1.0f;
			}

			// For each tangent
			for (int l = 0; l < mesh->GetElementTangentCount(); l++)
			{
				// Get tangent
				FbxGeometryElementTangent* tangent = mesh->GetElementTangent(l);
				float3 tangents;

				// Mapping mode for tangent
				if (tangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					switch (tangent->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
						// Store the tangent in a custom float3 structure
						tangents = fbxConverter.get3DVector(tangent->GetDirectArray().GetAt(vertexId));
						break;
					case FbxGeometryElement::eIndexToDirect:
					{
						// Store the tangent in a custom float3 structure
						int id = tangent->GetIndexArray().GetAt(vertexId);
						tangents = fbxConverter.get3DVector(tangent->GetDirectArray().GetAt(id));
					}
					break;
					default:
						break; // other reference modes not shown here!
					}
				}
				// Pass the tangent to our polygonstructure
				// The Z value is changed to negative to convert from right handed to left handed space
				polygonStructure[i].vertices[j].tangent[0] = floor(10000 * tangents.x) / 10000;
				polygonStructure[i].vertices[j].tangent[1] = floor(10000 * tangents.y) / 10000;
				polygonStructure[i].vertices[j].tangent[2] = (floor(10000 * tangents.z) / 10000) * -1.0f;
			}

			// For each binormal
			for(int l = 0; l < mesh->GetElementBinormalCount(); l++)
			{
				FbxGeometryElementBinormal* binormal = mesh->GetElementBinormal(l);
				float3 binormals;
				switch (binormal->GetReferenceMode())
				{
				case FbxGeometryElement::eDirect:
					binormals = fbxConverter.get3DVector(binormal->GetDirectArray().GetAt(vertexId));
					break;
				case FbxGeometryElement::eIndexToDirect:
				{
					int id = binormal->GetIndexArray().GetAt(vertexId);
					binormals = fbxConverter.get3DVector(binormal->GetDirectArray().GetAt(id));
				}
				break;
				default:
					break; // other reference modes not shown here!
				}
				// Pass the bitangent to our structure
				// The Z value is changed to negative to convert from right handed to left handed space
				polygonStructure[i].vertices[j].bitangent[0] = (floor(10000 * binormals.x) / 10000);
				polygonStructure[i].vertices[j].bitangent[1] = (floor(10000 * binormals.y) / 10000);
				polygonStructure[i].vertices[j].bitangent[2] = (floor(10000 * binormals.z) / 10000) * -1.0f;
			}

			// Next vertexIndex
			vertexId++;
		}
	}
}

void fillLightHeader(FbxLight* lights, LightHeader* lightHeaders, Fbxconverter fbxConverter)
{
	// Reference to the "node" of the light
	FbxNode* lightNode = lights->GetNode();

	// Get the name of the light
	string name = lightNode->GetName();

	// Get the lighttype
	const char* lightTypes[] = { "Point", "Directional", "Spot", "Area", "Volume" };
	string type = lightTypes[lights->LightType.Get()];

	// Get the lights decaytype, may be needed for pointLight?
	const char* decayTypes[] = { "eNone", "eLinear", "eQuadratic", "eCubic" };
	FbxLight::EDecayType decayType = lights->DecayType.Get();
	string decay = decayTypes[decayType];

	// Get the lights color
	FbxDouble3 light = lights->Color.Get();
	FbxColor color(light[0], light[1], light[2]);

	// Get the local position of the light
	FbxDouble3 translation = lightNode->LclTranslation.Get();
	float posx = (float)translation[0]; float posy = (float)translation[1]; float posz = (float)translation[2];
	
	// Get the range of the light
	float range = (float)lights->DecayStart.Get();

	// Get the rotation/direction of the light
	FbxDouble3 direction = lightNode->LclRotation.Get();
	float rotX = (float)direction[0]; float rotY = (float)direction[1]; float rotZ = (float)direction[2];

	// Get the lights intensity
	float intensity = (float)lights->Intensity.Get();

	// Get inner and outer angles
	float innerAngle = (float)lights->InnerAngle.Get();
	float outerAngle = (float)lights->OuterAngle.Get();

	// Set the usefull values to our lightHeader struct
	strcpy_s(lightHeaders->lightName, name.c_str());
	strcpy_s(lightHeaders->lightType, type.c_str());
	strcpy_s(lightHeaders->decayType, decay.c_str());
	lightHeaders->diffuseLight[0] = (float)color.mRed; lightHeaders->diffuseLight[1] = (float)color.mGreen; lightHeaders->diffuseLight[2] = (float)color.mBlue;
	lightHeaders->lightPos[0] = posx; lightHeaders->lightPos[1] = posy; lightHeaders->lightPos[2] = posz;
	lightHeaders->range = range;
	lightHeaders->direction[0] = rotX; lightHeaders->direction[1] = rotY; lightHeaders->direction[2] = rotZ;
	lightHeaders->intensity = intensity;
	lightHeaders->innerAngle = innerAngle;
	lightHeaders->outerAngle = outerAngle;

	lightHeaders->castLight = lights->CastLight.Get();
	lightHeaders->drawVolumetricLight = lights->DrawVolumetricLight.Get();
	lightHeaders->drawGroundProjection = lights->DrawGroundProjection.Get();
	lightHeaders->drawFrontFacingVolumetricLight = lights->DrawFrontFacingVolumetricLight.Get();
	lightHeaders->fog = lights->Fog.Get();
	lightHeaders->decayStart = lights->DecayStart.Get();
	lightHeaders->enableFarAttenuation = lights->EnableFarAttenuation.Get();
	lightHeaders->enableNearAttenuation = lights->EnableNearAttenuation.Get();
	lightHeaders->nearAttenuationStart = lights->NearAttenuationStart.Get();
	lightHeaders->farAttenuationStart = lights->FarAttenuationStart.Get();
	lightHeaders->nearAttenuationEnd = lights->NearAttenuationEnd.Get();
	lightHeaders->farAttenuationEnd = lights->FarAttenuationEnd.Get();
	lightHeaders->castShadows = lights->CastShadows.Get();
	lightHeaders->shadowColor[0] = lights->ShadowColor.Get()[0]; lightHeaders->shadowColor[1] = lights->ShadowColor.Get()[1]; lightHeaders->shadowColor[2] = lights->ShadowColor.Get()[2];
	lightHeaders->leftBarnDoor = lights->LeftBarnDoor.Get();
	lightHeaders->rightBarnDoor = lights->RightBarnDoor.Get();
	lightHeaders->topBarnDoor = lights->TopBarnDoor.Get();
	lightHeaders->bottomBarnDoor = lights->BottomBarnDoor.Get();
	lightHeaders->enableBarnDoor = lights->EnableBarnDoor.Get();
	lightHeaders->areaLightShape = lights->AreaLightShape.Get();

	string file = lights->FileName.Get().Buffer();
	strcpy_s(lightHeaders->goboFileName, file.c_str());
}

void fillCameraHeader(FbxCamera* cameras, CameraHeader& cameraHeaders, Fbxconverter fbxConverter)
{
	FbxNode* cameraNode = cameras->GetNode();

	// Get the name of the camera
	string name = cameraNode->GetName();

	FbxDouble3 parentPosition = cameraNode->GetParent()->LclTranslation.Get();
	FbxDouble3 parentRotation = cameraNode->GetParent()->LclRotation.Get();
	FbxDouble3 camPosition = cameraNode->LclTranslation.Get();
	float posX = (float)camPosition[0] + (float)parentPosition[0]; float posY = (float)camPosition[1] + (float)parentPosition[1]; float posZ = (float)camPosition[2] + (float)parentPosition[2];

	FbxDouble3 direction = cameras->InterestPosition.Get();
	float length = sqrt((direction[0] * direction[0]) + (direction[1] * direction[1]) + (direction[2] * direction[2]));
	direction[0] = direction[0] / length; direction[1] = direction[1] / length; direction[2] = direction[2] / length;

	FbxNode* up = cameraNode->GetTargetUp();
	FbxDouble3 upVector;
	if (up)
	{
		upVector = up->LclTranslation.Get();
	}
	else
	{
		upVector = cameras->UpVector.Get();
	}

	length = sqrt((upVector[0] * upVector[0]) + (upVector[1] * upVector[1]) + (upVector[2] * upVector[2]));
	upVector[0] = upVector[0] / length; upVector[1] = upVector[1] / length; upVector[2] = upVector[2] / length;

	float fieldOfView = 0;
	FbxCamera::EApertureMode mode = cameras->ApertureMode.Get();

	// Check which mode is used to calculate field of view
	if (mode == FbxCamera::EApertureMode::eVertical || mode == FbxCamera::EApertureMode::eHorizontal)
	{
		fieldOfView = (float)cameras->FieldOfView.Get();
	}
	else
	{
		fieldOfView = (float)cameras->ComputeFieldOfView(cameras->FocalLength.Get());
	}

	strcpy_s(cameraHeaders.cameraName, name.c_str());
	cameraHeaders.camPosition[0] = posX; cameraHeaders.camPosition[1] = posY; cameraHeaders.camPosition[2] = posZ;
	cameraHeaders.direction[0] = direction[0]; cameraHeaders.direction[1] = direction[1]; cameraHeaders.direction[2] = direction[2];
	cameraHeaders.camUp[0] = upVector[0]; cameraHeaders.camUp[1] = upVector[1]; cameraHeaders.camUp[2] = upVector[2];
	cameraHeaders.fieldOfView = fieldOfView;
	cameraHeaders.nearPlane = (float)cameras->NearPlane.Get();
	cameraHeaders.farPlane = (float)cameras->FarPlane.Get();
}

void fillMaterials(FbxNode* node, FbxGeometry* geometry, std::vector<Materialheader*>& matVector, int nrOfmaterials)
{
	if (geometry) {
		if (node) {
			//std::cout << std::endl << "    MATERIALHEADER" << std::endl;
		}
	}

	Materialheader* material;
	material = new Materialheader[nrOfmaterials];

	// If mesh has a material    
	if (nrOfmaterials > 0) {
		FbxDouble fbxDouble3;
		FbxDouble fbxDouble1;
		FbxColor rgbaColor;

		FbxPropertyT<FbxDouble3> lKFbxDouble3;
		FbxPropertyT<FbxDouble> lKFbxDouble1;
		FbxColor theColor;

		// New material
		// Iterate through each material
		for (int count = 0; count < nrOfmaterials; count++) {

			// Set material index to struct
			material[count].index = count;

			FbxSurfaceMaterial* lMaterial = node->GetMaterial(count);

			// Material name (phong, pbr osv osv)
			// Store matname
			std::string matName = lMaterial->GetName();
			strcpy_s(material[count].materialName, matName.c_str());

			// Get the implementation to see if its a hardware shader (HLSL)
			const FbxImplementation* lImplementation = GetImplementation(lMaterial, FBXSDK_IMPLEMENTATION_HLSL);

			FbxString ImplementationType = "HLSL";
			if (!lImplementation) {
				lImplementation = GetImplementation(lMaterial, FBXSDK_IMPLEMENTATION_CGFX);
				ImplementationType = "CGFX";
			}

			if (lImplementation) {
				//std::cout << "*** THIS IS A HLSL IMPLEMENTATION ***" << std::endl;
				/*
				////Now we have a hardware shader, let's read it
				//std::cout << "            Hardware Shader Type: " << ImplementationType.Buffer() << std::endl;
	//
	//            const FbxBindingTable* lRootTable = lImplementation->GetRootTable();
	//            FbxString lFileName = lRootTable->DescAbsoluteURL.Get();
	//            FbxString lTechniqueName = lRootTable->DescTAG.Get();


	//            const FbxBindingTable* lTable = lImplementation->GetRootTable();
	//            size_t lEntryNum = lTable->GetEntryCount();

	//            for (int i = 0; i < (int)lEntryNum; ++i)
	//            {
	//                const FbxBindingTableEntry& lEntry = lTable->GetEntry(i);
	//                const char* lEntrySrcType = lEntry.GetEntryType(true);
	//                FbxProperty lFbxProp;


	//                FbxString lTest = lEntry.GetSource();
	//                FBXSDK_printf("            Entry: %s\n", lTest.Buffer());


	//                if (strcmp(FbxPropertyEntryView::sEntryType, lEntrySrcType) == 0)
	//                {
	//                    lFbxProp = lMaterial->FindPropertyHierarchical(lEntry.GetSource());
	//                    if (!lFbxProp.IsValid())
	//                    {
	//                        lFbxProp = lMaterial->RootProperty.FindHierarchical(lEntry.GetSource());
	//                    }


	//                }
	//                else if (strcmp(FbxConstantEntryView::sEntryType, lEntrySrcType) == 0)
	//                {
	//                    lFbxProp = lImplementation->GetConstants().FindHierarchical(lEntry.GetSource());
	//                }
	//                if (lFbxProp.IsValid())
	//                {
	//                    if (lFbxProp.GetSrcObjectCount<FbxTexture>() > 0)
	//                    {
	//                        //do what you want with the textures
	//                        for (int j = 0; j < lFbxProp.GetSrcObjectCount<FbxFileTexture>(); ++j)
	//                        {
	//                            FbxFileTexture* lTex = lFbxProp.GetSrcObject<FbxFileTexture>(j);
	//                            FBXSDK_printf("           File Texture: %s\n", lTex->GetFileName());
	//                        }
	//                        for (int j = 0; j < lFbxProp.GetSrcObjectCount<FbxLayeredTexture>(); ++j)
	//                        {
	//                            FbxLayeredTexture* lTex = lFbxProp.GetSrcObject<FbxLayeredTexture>(j);
	//                            FBXSDK_printf("        Layered Texture: %s\n", lTex->GetName());
	//                        }
	//                        for (int j = 0; j < lFbxProp.GetSrcObjectCount<FbxProceduralTexture>(); ++j)
	//                        {
	//                            FbxProceduralTexture* lTex = lFbxProp.GetSrcObject<FbxProceduralTexture>(j);
	//                            FBXSDK_printf("     Procedural Texture: %s\n", lTex->GetName());
	//                        }
	//                    }
	//                    else
	//                    {
	//                        FbxDataType lFbxType = lFbxProp.GetPropertyDataType();
	//                        FbxString blah = lFbxType.GetName();
	//                        if (FbxBoolDT == lFbxType)
	//                        {
	//                            std::cout << "                Bool: " << lFbxProp.Get<FbxBool>() << std::endl;
	//                            //DisplayBool("                Bool: ", lFbxProp.Get<FbxBool>());
	//                        }
	//                        else if (FbxIntDT == lFbxType || FbxEnumDT == lFbxType)
	//                        {
	//                            std::cout << "                Int: " << lFbxProp.Get<FbxInt>() << std::endl;
	//                           // DisplayInt("                Int: ", lFbxProp.Get<FbxInt>());
	//                        }
	//                        else if (FbxFloatDT == lFbxType)
	//                        {
	//                            std::cout << "                Float: " << lFbxProp.Get<FbxInt>() << std::endl;
	//                            //DisplayDouble("                Float: ", lFbxProp.Get<FbxFloat>());

	//                        }
	//                        else if (FbxDoubleDT == lFbxType)
	//                        {
	//                            std::cout << "                Double: " << lFbxProp.Get<FbxDouble>() << std::endl;
	//                           // DisplayDouble("                Double: ", lFbxProp.Get<FbxDouble>());
	//                        }
	//                        else if (FbxStringDT == lFbxType
	//                            || FbxUrlDT == lFbxType
	//                            || FbxXRefUrlDT == lFbxType)
	//                        {
	//                            std::cout << "                String: " << lFbxProp.Get<FbxString>().Buffer() << std::endl;
	//                            //DisplayString("                String: ", lFbxProp.Get<FbxString>().Buffer());
	//                        }
	//                        else if (FbxDouble2DT == lFbxType)
	//                        {
	//                            FbxDouble2 lDouble2 = lFbxProp.Get<FbxDouble2>();
	//                            FbxVector2 lVect;
	//                            lVect[0] = lDouble2[0];
	//                            lVect[1] = lDouble2[1];

	//                            std::cout << "                2D vector: " << lVect[0] << " " << lVect[1] << std::endl;

	//                           // Display2DVector("                2D vector: ", lVect);
	//                        }
	//                        else if (FbxDouble3DT == lFbxType || FbxColor3DT == lFbxType)
	//                        {
	//                            FbxDouble3 lDouble3 = lFbxProp.Get<FbxDouble3>();


	//                            FbxVector4 lVect;
	//                            lVect[0] = lDouble3[0];
	//                            lVect[1] = lDouble3[1];
	//                            lVect[2] = lDouble3[2];
	//                            std::cout << "                3D vector: " << lVect[0] << " " << lVect[1] << " " << lVect[2] << std::endl;

	//                           // Display3DVector("                3D vector: ", lVect);
	//                        }

	//                        else if (FbxDouble4DT == lFbxType || FbxColor4DT == lFbxType)
	//                        {
	//                            FbxDouble4 lDouble4 = lFbxProp.Get<FbxDouble4>();
	//                            FbxVector4 lVect;
	//                            lVect[0] = lDouble4[0];
	//                            lVect[1] = lDouble4[1];
	//                            lVect[2] = lDouble4[2];
	//                            lVect[3] = lDouble4[3];
	//                            std::cout << "                3D vector: " << lVect[0] << " " << lVect[1] << " " << lVect[2] << " " << lVect[3] << std::endl;

	//                          //  Display4DVector("                4D vector: ", lVect);
	//                        }
	//                        else if (FbxDouble4x4DT == lFbxType)
	//                        {
	//                            FbxDouble4x4 lDouble44 = lFbxProp.Get<FbxDouble4x4>();
	//                            for (int j = 0; j < 4; ++j)
	//                            {

	//                                FbxVector4 lVect;
	//                                lVect[0] = lDouble44[j][0];
	//                                lVect[1] = lDouble44[j][1];
	//                                lVect[2] = lDouble44[j][2];
	//                                lVect[3] = lDouble44[j][3];

	//                                std::cout << "                3D vector: " << lVect[0] << " " << lVect[1] << " " << lVect[2] << " " << lVect[3] << std::endl;

	//                               // Display4DVector("                4x4D vector: ", lVect);
	//                            }

	//                        }
	//                    }

	//                }
	//            }
*/
			}
			else if (lMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))  // <-------------------------------- PHONG MATERIAL
			{
				//std::cout << "            * This is a Phong shading from Autodesk Maya *           " << std::endl;
				/*		TA BORT KOMMENTAREN OM PHONG SKA ANVÄNDAS

								//Display PHONG MATERIAL properties.
								lKFbxDouble3 = ((FbxSurfacePhong*)lMaterial)->Ambient;
								material->ambient[0] = lKFbxDouble3.Get()[0];
								material->ambient[1] = lKFbxDouble3.Get()[1];
								material->ambient[2] = lKFbxDouble3.Get()[2];

								// Ambient Color
								std::cout << "            Ambient: ";
								for (int i = 0; i < 3; i++)
									std::cout << lKFbxDouble3.Get()[i] << " ";
								std::cout << std::endl;

								// Diffuse Color
								lKFbxDouble3 = ((FbxSurfacePhong*)lMaterial)->Diffuse;
								material->diffuse[0] = lKFbxDouble3.Get()[0];
								material->diffuse[1] = lKFbxDouble3.Get()[1];
								material->diffuse[2] = lKFbxDouble3.Get()[2];

								std::cout << "            Diffuse: ";
								for (int i = 0; i < 3; i++)
									std::cout << lKFbxDouble3.Get()[i] << " ";
								std::cout << std::endl;

								// Specular Color (unique to Phong materials)
								lKFbxDouble3 = ((FbxSurfacePhong*)lMaterial)->Specular;
								material->specular[0] = lKFbxDouble3.Get()[0];
								material->specular[1] = lKFbxDouble3.Get()[1];
								material->specular[2] = lKFbxDouble3.Get()[2];

								std::cout << "            Specular: ";
								for (int i = 0; i < 3; i++)
									std::cout << lKFbxDouble3.Get()[i] << " ";
								std::cout << std::endl;

								// Emissive Color
								lKFbxDouble3 = ((FbxSurfacePhong*)lMaterial)->Emissive;
								material->emissive[0] = lKFbxDouble3.Get()[0];
								material->emissive[1] = lKFbxDouble3.Get()[1];
								material->emissive[2] = lKFbxDouble3.Get()[2];
								std::cout << "            Emissive: ";
								for (int i = 0; i < 3; i++)
									std::cout << lKFbxDouble3.Get()[i] << " ";
								std::cout << std::endl;

								// Opacity (is Transparency factor)
								lKFbxDouble1 = ((FbxSurfacePhong*)lMaterial)->TransparencyFactor;
								material->opacity = 1.0f - lKFbxDouble1.Get();

								std::cout << "            Opacity: ";
								std::cout << 1.0f - lKFbxDouble1.Get() << std::endl;

								// Shininess
								lKFbxDouble1 = ((FbxSurfacePhong*)lMaterial)->Shininess;
								material->shininess = lKFbxDouble1.Get();

								std::cout << "            Shininess: ";
								std::cout << lKFbxDouble1.Get() << std::endl;

								// Reflectivity
								lKFbxDouble1 = ((FbxSurfacePhong*)lMaterial)->ReflectionFactor;
								material->reflectivity = lKFbxDouble1.Get();

								std::cout << "            Reflectivity: ";
								std::cout << lKFbxDouble1.Get() << std::endl;



								// Push phong material to vector
								matVector.push_back(material);
				*/
			}
			else if (lMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId))    // <-------- LAMBERT MATERIAL
			{

				lKFbxDouble3 = ((FbxSurfaceLambert*)lMaterial)->Ambient;
				material[count].lambertAmbient[0] = lKFbxDouble3.Get()[0];
				material[count].lambertAmbient[1] = lKFbxDouble3.Get()[1];
				material[count].lambertAmbient[2] = lKFbxDouble3.Get()[2];

				lKFbxDouble3 = ((FbxSurfaceLambert*)lMaterial)->Diffuse;
				material[count].lambertDiffuse[0] = lKFbxDouble3.Get()[0];
				material[count].lambertDiffuse[1] = lKFbxDouble3.Get()[1];
				material[count].lambertDiffuse[2] = lKFbxDouble3.Get()[2];

				lKFbxDouble3 = ((FbxSurfaceLambert*)lMaterial)->Emissive;
				material[count].lambertEmissive[0] = (float)lKFbxDouble3.Get()[0];
				material[count].lambertEmissive[1] = (float)lKFbxDouble3.Get()[1];
				material[count].lambertEmissive[2] = (float)lKFbxDouble3.Get()[2];
				
				double opacity = ((FbxSurfaceLambert*)lMaterial)->TransparencyFactor;				
				material[count].lambertOpacity = (float)opacity;

				
				std::cout << std::endl;

			}
			else {
				//std::cout << "Unknown type of Material" << std::endl;
			}

			FbxPropertyT<FbxString> lString;
			lString = lMaterial->ShadingModel;

			// GET SHADING MODEL            
			std::string shadingMod = lString.Get().Buffer();
			strcpy_s(material[count].shadingModel, shadingMod.c_str());

			// Push lambert to vector
			matVector.push_back(&material[count]);
		}
	}
}

void fillTextureHeaders(FbxNode* node, int nrOfMaterials, std::vector<Texture*>& textureHeader, int& nrOfTextures)
{
	FbxProperty lProperty;
	if (node == NULL)
		return;

	for (int lMaterialIndex = 0; lMaterialIndex < nrOfMaterials; lMaterialIndex++)
	{
		FbxSurfaceMaterial* lMaterial = node->GetSrcObject<FbxSurfaceMaterial>(lMaterialIndex);
		bool lDisplayHeader = true;

		// If there is a material
		if (lMaterial)
		{
			// Check every texture layer
			int lTextureIndex;
			FBXSDK_FOR_EACH_TEXTURE(lTextureIndex)
			{
				lProperty = lMaterial->FindProperty(FbxLayerElement::sTextureChannelNames[lTextureIndex]);

				std::string name = lProperty.GetNameAsCStr();

				if (name == "DiffuseColor")
					findConnectionsForTexture(lProperty, lDisplayHeader, lMaterialIndex, textureHeader, nrOfTextures);
				if (name == "Bump")
					findConnectionsForTexture(lProperty, lDisplayHeader, lMaterialIndex, textureHeader, nrOfTextures);
				if (name == "NormalMap")
					findConnectionsForTexture(lProperty, lDisplayHeader, lMaterialIndex, textureHeader, nrOfTextures);
			}
		}
	}
}

void findConnectionsForTexture(FbxProperty pProperty, bool& pDisplayHeader, int pMaterialIndex, std::vector<Texture*>& textureHeader, int& nrOfTextures)
{
	if (pProperty.IsValid())
	{
		// iterate through every texture
		int lTextureCount = pProperty.GetSrcObjectCount<FbxTexture>();
		
		if (lTextureCount == 1)
		{
			nrOfTextures++;
		}
			
		Texture* texInfo;

		// Iterate if there is a texture connected to material
		for (int j = 0; j < lTextureCount; ++j)
		{
			// Here we have to check if it's layeredtextures, or just textures:
			FbxLayeredTexture* lLayeredTexture = pProperty.GetSrcObject<FbxLayeredTexture>(j);

			// Create texture struct			
			texInfo = new Texture;

			if (lLayeredTexture)
			{
				//std::cout << "    Layered Texture: " << j << std::endl;
				int lNbTextures = lLayeredTexture->GetSrcObjectCount<FbxTexture>();
				for (int k = 0; k < lNbTextures; ++k)
				{
					FbxTexture* lTexture = lLayeredTexture->GetSrcObject<FbxTexture>(k);
					if (lTexture)
					{
						if (pDisplayHeader) {
							//std::cout << "    Textures connected to Material " << pMaterialIndex << std::endl;
							pDisplayHeader = false;
						}

						// NOTE the blend mode is ALWAYS on the LayeredTexture and NOT the one on the texture.
						// Why is that?  because one texture can be shared on different layered textures and might
						// have different blend modes.

						FbxLayeredTexture::EBlendMode lBlendMode;
						lLayeredTexture->GetTextureBlendMode(k, lBlendMode);

						SaveTextureInfo(lTexture, (int)lBlendMode, textureHeader, texInfo, pProperty);
					}
				}
			}
			else
			{
				// No layered texture simply get on the property
				FbxTexture* lTexture = pProperty.GetSrcObject<FbxTexture>(j);

				if (lTexture)
				{					
					// display connected Material header only at the first time
					if (pDisplayHeader) {

						pDisplayHeader = false;
					}

					// Texture index (Connected to x material)
					texInfo->materialIndex = lTextureCount - 1;
					SaveTextureInfo(lTexture, -1, textureHeader, texInfo, pProperty);
				}
			}
		}
	}
}

void SaveTextureInfo(FbxTexture* pTexture, int pBlendMode, std::vector<Texture*>& textureheader, Texture* textureInfo, FbxProperty pProperty)
{
	FbxFileTexture* lFileTexture = FbxCast<FbxFileTexture>(pTexture);
	FbxProceduralTexture* lProceduralTexture = FbxCast<FbxProceduralTexture>(pTexture);

	// Texture name
	std::string texname = pTexture->GetName();

	// ---------------------- JUST LOOKING FOR DIFFUSE AND NORMAL ------------------------//

	// SET CORRECT TYPE AND NAME TO CORRECT MAP
	if (pProperty.GetName() == "DiffuseColor") {
		std::string textureType = pProperty.GetNameAsCStr();
		strcpy_s(textureInfo->textureType, textureType.c_str());

		// tex name
		strcpy_s(textureInfo->textureName, texname.c_str());
	}

	if (pProperty.GetName() == "NormalMap") {
		std::string textureType = pProperty.GetNameAsCStr();
		strcpy_s(textureInfo->textureType, textureType.c_str());

		// tex name
		strcpy_s(textureInfo->textureName, texname.c_str());
	}

	// -------------------------- ---------------------------- ----------------------------//

	if (lFileTexture) {

		// ---------------------- JUST LOOKING FOR DIFFUSE AND NORMAL ------------------------//

		// SET TEXTURE PATH 
		std::string pathName = (char*)lFileTexture->GetFileName();
		if (pProperty.GetName() == "DiffuseColor")
			strcpy_s(textureInfo->texturePath, pathName.c_str());
		if (pProperty.GetName() == "NormalMap")
			strcpy_s(textureInfo->texturePath, pathName.c_str());

		// -------------------------- ---------------------------- ----------------------------//

	}
	else if (lProceduralTexture) {
		//std::cout << "            Type: Procedural Texture" << std::endl;
	}

	double scaleU = pTexture->GetScaleU();
	double scaleV = pTexture->GetScaleV();
	double translationU = pTexture->GetTranslationU();
	double translationV = pTexture->GetTranslationV();
	bool swapUV = pTexture->GetSwapUV();
	double rotationU = pTexture->GetRotationU();
	double rotationV = pTexture->GetRotationV();
	double rotationW = pTexture->GetRotationW();

	const char* lAlphaSource[] = { "None", "RGB Intensity", "Black" };

	double leftCropping = pTexture->GetCroppingLeft();
	double topCropping = pTexture->GetCroppingTop();
	double rightCropping = pTexture->GetCroppingRight();
	double bottomCropping = pTexture->GetCroppingBottom();

	const char* lMappingTypes[] = { "Null", "Planar", "Spherical", "Cylindrical", "Box", "Face", "UV", "Environment" };

	if (pTexture->GetMappingType() == FbxTexture::ePlanar)
	{
		const char* lPlanarMappingNormals[] = { "X", "Y", "Z" };

		//std::cout << "            Planar Mapping Normal: " << lPlanarMappingNormals[pTexture->GetPlanarMappingNormal()] << std::endl;
	}

	const char* lBlendModes[] = { "Translucent", "Additive", "Modulate", "Modulate2", "Over", "Normal", "Dissolve", "Darken", "ColorBurn", "LinearBurn",
									"DarkerColor", "Lighten", "Screen", "ColorDodge", "LinearDodge", "LighterColor", "SoftLight", "HardLight", "VividLight",
									"LinearLight", "PinLight", "HardMix", "Difference", "Exclusion", "Substract", "Divide", "Hue", "Saturation", "Color",
									"Luminosity", "Overlay" };

	if (pBlendMode >= 0) {
		double alpha = pTexture->GetDefaultAlpha();
	}

	if (lFileTexture)
	{
		const char* lMaterialUses[] = { "Model Material", "Default Material" };
	}

	const char* pTextureUses[] = { "Standard", "Shadow Map", "Light Map",
		"Spherical Reflexion Map", "Sphere Reflexion Map", "Bump Normal Map" };


	// Push to texture header
	textureheader.push_back(textureInfo);
}

void SaveControlpoints(FbxMesh* pMesh, std::vector<Controlpoints*>& ctrlPoints)
{
	int i, lControlPointsCount = pMesh->GetControlPointsCount();
	FbxVector4* lControlPoints = pMesh->GetControlPoints();

	ctrlPoints.clear();
	Controlpoints* ctrlPnt;

	for (i = 0; i < lControlPointsCount; i++)
	{
		ctrlPnt = new Controlpoints;

		float x = lControlPoints[i][0];
		float y = lControlPoints[i][1];
		float z = lControlPoints[i][2];
		z = z * -1.0f;

		// POSITION
		ctrlPnt->position[0] = x;
		ctrlPnt->position[1] = y;
		ctrlPnt->position[2] = z;

		for (int j = 0; j < pMesh->GetElementNormalCount(); j++)
		{
			FbxGeometryElementNormal* leNormals = pMesh->GetElementNormal(j);

			if (leNormals->GetReferenceMode() == FbxGeometryElement::eDirect) {
				float nx = leNormals->GetDirectArray().GetAt(i)[0];
				float ny = leNormals->GetDirectArray().GetAt(i)[1];
				float nz = leNormals->GetDirectArray().GetAt(i)[2];
				nz = nz * -1.0f;

				// Normal
				ctrlPnt->normal[0] = nx;
				ctrlPnt->normal[1] = ny;
				ctrlPnt->normal[2] = nz;

			}
		}

		ctrlPoints.push_back(ctrlPnt);
	}
}

void fillAnimation(FbxScene* scene, FbxNode* currentNode, std::vector<Animation>& animation, std::vector<BlendShape*>& blendshapes,
	vector<vector<Controlpoints>>& blendCtrlpoints, vector<vector<Keyframe>>& keyframes)
{	
	Animation newAnimation; newAnimation.index = -1;
	BlendShape* blends =  nullptr;
	Controlpoints* blendCtrl = nullptr;
	
	char lTimeString[256];
	FbxGlobalSettings* timelineSettings = &scene->GetGlobalSettings();
	std::string fps = FbxGetTimeModeName(timelineSettings->GetTimeMode());

	FbxTimeSpan timeSpan;
	timelineSettings->GetTimelineDefaultTimeSpan(timeSpan);
	FbxTime start = timeSpan.GetStart();
	FbxTime end = timeSpan.GetStop();

	std::string startstring = start.GetTimeString(lTimeString, 256);
	std::string endstring = end.GetTimeString(lTimeString, 256);

	std::size_t startPos = startstring.find_last_of("*");
	std::size_t endPos = endstring.find_last_of("*");

	std::string startFrameString = startstring.substr(0, startPos);
	std::string endFrameString = endstring.substr(0, endPos);

	int startkeyframe = std::stoi(startFrameString);
	int endkeyframe = std::stoi(endFrameString);
	int animationlength = endkeyframe - startkeyframe;

	// Current node
	FbxGeometry* nodeGeo = (FbxGeometry*)currentNode->GetNodeAttribute();

	// Total blendshapes for this node
	int blendshapescount = nodeGeo->GetDeformerCount(FbxDeformer::eBlendShape);

	FbxAnimCurve* animCurve = nullptr;

	for (int i = 0; i < scene->GetSrcObjectCount<FbxAnimStack>(); i++) {
		FbxAnimStack* animStack = scene->GetSrcObject<FbxAnimStack>(i);
		int nrOfLayers = animStack->GetMemberCount<FbxAnimLayer>();

		for (int k = 0; k < nrOfLayers; k++) {
					
			for (int y = 0; y < blendshapescount; y++) {

				blends = new BlendShape;
				FbxBlendShape* blendShape = (FbxBlendShape*)nodeGeo->GetDeformer(y, FbxDeformer::eBlendShape);
				vector<Controlpoints> ctrlPointVector;

				// Mesh related to this blendshape and blendshape name
				strcpy_s(blends->meshRelated, (char*)nodeGeo->GetNode()->GetName());				
				strcpy_s(blends->blendShapeName, (char*)blendShape->GetName());

				int channelCount = blendShape->GetBlendShapeChannelCount();

				for (int h = 0; h < channelCount; h++) {

					FbxBlendShapeChannel* channel = blendShape->GetBlendShapeChannel(h);

					// Blendshape channelname
					strcpy_s(blends->blendShapeChannelname, (char*)channel->GetName());
										
					// KEYS KEYFRAMES AND STUFF
					animCurve = nodeGeo->GetShapeChannel(y, h, animStack->GetMember<FbxAnimLayer>(k), true);
					if (animCurve) {

						vector<Keyframe> newKeys;

						// Create new animation
						newAnimation.index = k;	
						newAnimation.nrOfBlendshapes = blendshapescount;	// blendshapes in this animation

						// Meshrelated layer
						strcpy_s(newAnimation.meshRelated, (char*)nodeGeo->GetNode()->GetName());

						int length = 0;						
						int keyCount = animCurve->KeyGetCount();

						// save nr of keyframes
						blends->nrOfKeyframes = keyCount;

						for (int key = 0; key < keyCount; key++) {
							Keyframe newKey;

							FbxAnimCurveKey keys = animCurve->KeyGet(key);
							FbxTime keyTime = animCurve->KeyGetTime(key);
							std::string test = keyTime.GetTimeString().Buffer();
							std::size_t pos = test.find_last_of("*");
							std::string keyframestring = test.substr(0, pos);
							int keyframeTime = std::stoi(keyframestring);
							float keyValue = animCurve->KeyGetValue(key);

							newKey.BlendIndex = y;
							newKey.time = keyframeTime;
							newKey.value = keyValue;

							newKeys.push_back(newKey);

							if (keyframeTime > length)
							{
								length = keyframeTime;
							}
						}
						keyframes.push_back(newKeys);
						newAnimation.duration = length;
					}


					// GET CONTROLPOINTS FOR SHAPE
					int targetShapeCount = channel->GetTargetShapeCount();
					for (int lTargetShapeIndex = 0; lTargetShapeIndex < targetShapeCount; ++lTargetShapeIndex)
					{
						FbxShape* meshShape = channel->GetTargetShape(lTargetShapeIndex);
						// Save target shape
						strcpy_s(blends->targetShapeName, (char*)meshShape->GetName());

						int lControlPointsCount = meshShape->GetControlPointsCount();
						FbxVector4* lControlPoints = meshShape->GetControlPoints();

						// Save nr of ctrlpoints for shape
						blends->nrOfControlpoints = lControlPointsCount;

						FbxLayerElementArrayTemplate<FbxVector4>* lNormals = NULL;
						bool lStatus = meshShape->GetNormals(&lNormals);

						for (int j = 0; j < lControlPointsCount; j++)
						{
							Controlpoints controlPoint;

							// Positions
							double x = lControlPoints[j][0];
							double y = lControlPoints[j][1];
							double z = lControlPoints[j][2];
							z = z * -1.0f;

							controlPoint.position[0] = x;
							controlPoint.position[1] = y;
							controlPoint.position[2] = z;

							// Normals
							double normX = lNormals->GetAt(j)[0];
							double normY = lNormals->GetAt(j)[1];
							double normZ = lNormals->GetAt(j)[2];
							normZ = normZ * -1.0f;

							controlPoint.normal[0] = normX;
							controlPoint.normal[1] = normY;
							controlPoint.normal[2] = normZ;

							// Pushback
							ctrlPointVector.push_back(controlPoint);
						}
					}

					blendCtrlpoints.push_back(ctrlPointVector);
					blendshapes.push_back(blends);
				}
			}
			if(newAnimation.index != -1)
				animation.push_back(newAnimation);
		}
	}
}

void testScenes(SceneHeader* scene)
{
	cout << "Scene has: " << scene->nrOfMeshes << " meshes" <<endl;
	cout << "Scene has: " << scene->nrOfAnimations << " animations" << endl;
	cout << "Scene has: " << scene->nrOfCamera << " cameras" << endl;
	cout << "Scene has: " << scene->nrOfLight << " lights" << endl;
	cout << endl;
}

void testPositions(Polygon* polygonstruct, int nrOfPolygons)
{
	for (int i = 0; i < nrOfPolygons; i++)
	{
		cout << "Polygon " << i << endl;
		cout << "Vertex 1: " << "Positions: " << polygonstruct[i].vertices[0].position[0] << " " << polygonstruct[i].vertices[0].position[1] << " " << polygonstruct[i].vertices[0].position[2] << endl;
		cout << "Vertex 1: " << "Normals: " << polygonstruct[i].vertices[0].normal[0] << " " << polygonstruct[i].vertices[0].normal[1] << " " << polygonstruct[i].vertices[0].normal[2] << endl;
		cout << "Vertex 1: " << "UVs: " << polygonstruct[i].vertices[0].texCoord[0] << " " << polygonstruct[i].vertices[0].texCoord[1] << endl;
		cout << "Vertex 1: " << "Tangents: " << polygonstruct[i].vertices[0].tangent[0] << " " << polygonstruct[i].vertices[0].tangent[1] << " " << polygonstruct[i].vertices[0].tangent[2] << endl;
		cout << "Vertex 1: " << "Binormals: " << polygonstruct[i].vertices[0].bitangent[0] << " " << polygonstruct[i].vertices[0].bitangent[1] << " " << polygonstruct[i].vertices[0].bitangent[2] << endl;
		cout << endl;
		cout << "Vertex 2: " << "Positions: " << polygonstruct[i].vertices[1].position[0] << " " << polygonstruct[i].vertices[1].position[1] << " " << polygonstruct[i].vertices[1].position[2] << endl;
		cout << "Vertex 2: " << "Normals: " << polygonstruct[i].vertices[1].normal[0] << " " << polygonstruct[i].vertices[1].normal[1] << " " << polygonstruct[i].vertices[1].normal[2] << endl;
		cout << "Vertex 2: " << "UVs: " << polygonstruct[i].vertices[1].texCoord[0] << " " << polygonstruct[i].vertices[1].texCoord[1] << endl;
		cout << "Vertex 2: " << "Tangents: " << polygonstruct[i].vertices[1].tangent[0] << " " << polygonstruct[i].vertices[1].tangent[1] << " " << polygonstruct[i].vertices[1].tangent[2] << endl;
		cout << "Vertex 2: " << "Binormals: " << polygonstruct[i].vertices[1].bitangent[0] << " " << polygonstruct[i].vertices[1].bitangent[1] << " " << polygonstruct[i].vertices[1].bitangent[2] << endl;
		cout << endl;
		cout << "Vertex 3: " << "Positions: " << polygonstruct[i].vertices[2].position[0] << " " << polygonstruct[i].vertices[2].position[1] << " " << polygonstruct[i].vertices[2].position[2] << endl;
		cout << "Vertex 3: " << "Normals: " << polygonstruct[i].vertices[2].normal[0] << " " << polygonstruct[i].vertices[2].normal[1] << " " << polygonstruct[i].vertices[2].normal[2] << endl;
		cout << "Vertex 3: " << "UVs: " << polygonstruct[i].vertices[2].texCoord[0] << " " << polygonstruct[i].vertices[2].texCoord[1] << endl;
		cout << "Vertex 3: " << "Tangents: " << polygonstruct[i].vertices[2].tangent[0] << " " << polygonstruct[i].vertices[2].tangent[1] << " " << polygonstruct[i].vertices[2].tangent[2] << endl;
		cout << "Vertex 3: " << "Binormals: " << polygonstruct[i].vertices[2].bitangent[0] << " " << polygonstruct[i].vertices[2].bitangent[1] << " " << polygonstruct[i].vertices[2].bitangent[2] << endl;

		cout << endl;
	}
}

void testLights(LightHeader* light, int nrOfLights)
{
	for (int i = 0; i < nrOfLights; i++)
	{
		cout << "Name of light nr: " << i << " :" << light->lightName << endl;
	}
	cout << endl;

	for (int i = 0; i < nrOfLights; i++)
	{
		cout << "Type of light nr: " << i << " :" << light->lightType << endl;
	}
	cout << endl;

	for (int i = 0; i < nrOfLights; i++)
	{
		cout << "DecayType of light nr: " << i << " :" << light->decayType << endl;
	}
	cout << endl;

	for (int i = 0; i < nrOfLights; i++)
	{
		cout << "Colors of lights nr: " << i << " :" << light->diffuseLight[0] << " " << light->diffuseLight[1] << " " << light->diffuseLight[2] << " " << endl;
	}
	cout << endl;

	for (int i = 0; i < nrOfLights; i++)
	{
		cout << "Range of light nr: " << i << " :" << light->range << endl;
	}
	cout << endl;

	for (int i = 0; i < nrOfLights; i++)
	{
		cout << "Positions of light nr: " << i << " :" << light->lightPos[0] << " " << light->lightPos[1] << " " << light->lightPos[2] << endl;
	}
	cout << endl;

	for (int i = 0; i < nrOfLights; i++)
	{
		cout << "Intensity of light nr: " << i << " :" << light->intensity << endl;
	}
	cout << endl;

	for (int i = 0; i < nrOfLights; i++)
	{
		cout << "Rotation/Direction of light nr: " << i << " :" << light->direction[0] << " " << light->direction[1] << " " << light->direction[2] << endl;
	}
	cout << endl;

	for (int i = 0; i < nrOfLights; i++)
	{
		cout << "InneAngle of light nr: " << i << " :" << light->innerAngle << endl;
	}
	cout << endl;

	for (int i = 0; i < nrOfLights; i++)
	{
		cout << "OuterAngle of light nr: " << i << " :" << light->outerAngle << endl;
	}
	cout << endl;

	for (int i = 0; i < nrOfLights; i++)
	{
		cout << "Cast Light: " << i << " :" << light->castLight << endl;
	}
	cout << endl;

	for (int i = 0; i < nrOfLights; i++)
	{
		cout << "draw volume: " << i << " :" << light->drawVolumetricLight << endl;
	}
	cout << endl;

	for (int i = 0; i < nrOfLights; i++)
	{
		cout << "draw ground: " << i << " :" << light->drawGroundProjection << endl;
	}
	cout << endl;

	for (int i = 0; i < nrOfLights; i++)
	{
		cout << "draw front: " << i << " :" << light->drawFrontFacingVolumetricLight << endl;
	}
	cout << endl;

	for (int i = 0; i < nrOfLights; i++)
	{
		cout << "fog: " << i << " :" << light->fog << endl;
	}
	cout << endl;

	for (int i = 0; i < nrOfLights; i++)
	{
		cout << "decay start: " << i << " :" << light->decayStart << endl;
	}
	cout << endl;

	for (int i = 0; i < nrOfLights; i++)
	{
		cout << "gobofilename: " << i << " :" << light->goboFileName << endl;
	}
	cout << endl;

	for (int i = 0; i < nrOfLights; i++)
	{
		cout << "enable near att: " << i << " :" << light->enableNearAttenuation << endl;
	}
	cout << endl;

	for (int i = 0; i < nrOfLights; i++)
	{
		cout << "enable far att: " << i << " :" << light->enableFarAttenuation << endl;
	}
	cout << endl;

	for (int i = 0; i < nrOfLights; i++)
	{
		cout << "near att start: " << i << " :" << light->nearAttenuationStart << endl;
	}
	cout << endl;

	for (int i = 0; i < nrOfLights; i++)
	{
		cout << "far att start: " << i << " :" << light->farAttenuationStart << endl;
	}
	cout << endl;

	for (int i = 0; i < nrOfLights; i++)
	{
		cout << "near att end: " << i << " :" << light->nearAttenuationEnd << endl;
	}
	cout << endl;

	for (int i = 0; i < nrOfLights; i++)
	{
		cout << "far att end: " << i << " :" << light->farAttenuationEnd << endl;
	}
	cout << endl;

	for (int i = 0; i < nrOfLights; i++)
	{
		cout << "cast shadows: " << i << " :" << light->castShadows << endl;
	}
	cout << endl;

	for (int i = 0; i < nrOfLights; i++)
	{
		cout << "shadow color: " << i << " :" << light->shadowColor[0] << " " << light->shadowColor[1] << " " << light->shadowColor[2] << endl;
	}
	cout << endl;

	for (int i = 0; i < nrOfLights; i++)
	{
		cout << "left barn door: " << i << " :" << light->leftBarnDoor << endl;
	}
	cout << endl;

	for (int i = 0; i < nrOfLights; i++)
	{
		cout << "right barn door: " << i << " :" << light->rightBarnDoor << endl;
	}
	cout << endl;

	for (int i = 0; i < nrOfLights; i++)
	{
		cout << "top barn door: " << i << " :" << light->topBarnDoor << endl;
	}
	cout << endl;

	for (int i = 0; i < nrOfLights; i++)
	{
		cout << "bottom barn door: " << i << " :" << light->bottomBarnDoor << endl;
	}
	cout << endl;

	for (int i = 0; i < nrOfLights; i++)
	{
		cout << "enable barn door: " << i << " :" << light->enableBarnDoor << endl;
	}
	cout << endl;

	for (int i = 0; i < nrOfLights; i++)
	{
		cout << "area light shape: " << i << " :" << light->areaLightShape << endl;
	}
	cout << endl;
}

void testCameras(CameraHeader camera)
{
	cout << "Camera has name: " << camera.cameraName << endl;
	cout << "Camera has position: " << camera.camPosition[0] << " " << camera.camPosition[1] << " " << camera.camPosition[2] << endl << endl;
	cout << "Camera has direction/rotation " << camera.direction[0] << " " << camera.direction[1] << " " << camera.direction[2] << endl << endl;
	cout << "Camera has up vector " << camera.camUp[0] << " " << camera.camUp[1] << " " << camera.camUp[2] << endl << endl;
	cout << "Camera has field of view " << camera.fieldOfView << endl << endl;
	cout << "Camera has near plane " << camera.nearPlane << endl << endl;
	cout << "Camera has far plane " << camera.farPlane << endl << endl;
}

void testMaterials(Materialheader mat)
{
	cout << "Index: " << mat.index << " " << endl;
	cout << "nrOfTextures: " << mat.nrOfTextures << " " << endl;
	cout << "materialName: " << mat.materialName << " " << endl;
	cout << "ambient: " << mat.lambertAmbient[0] << " " << mat.lambertAmbient[1] << " " << mat.lambertAmbient[2] << " " << endl;
	cout << "diffuse: " << mat.lambertDiffuse[0] << " " << mat.lambertDiffuse[1] << " " << mat.lambertDiffuse[2] << " " << endl;
	cout << "emissive: " << mat.lambertEmissive[0] << " " << mat.lambertEmissive[1] << " " << mat.lambertEmissive[2] << " " << endl;
	cout << "opacity: " << mat.lambertOpacity << " " << endl;
	cout << "shadingModel: " << mat.shadingModel << " " << endl;
	cout << endl;
}

void testTextures(Texture tex)
{
	cout << "Material index: " << tex.materialIndex << endl;
	cout << "texture type: " << tex.textureType << endl;
	cout << "texture name: " << tex.textureName << endl;
	cout << "texture path: " << tex.texturePath << endl;
	cout << endl;
}

