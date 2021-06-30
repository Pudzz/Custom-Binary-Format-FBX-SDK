#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include "Format.h"

// Custom float2 format, could use XMFloat2 instead
struct float2
{
	float u, v;
};
// Custom float3 format, could use XMFloat3 instead
struct float3
{
	float x, y, z;
};

class MemoryHandler
{
private:

	/*
	 * Private vectors which holds all the information in the current scene in the memory
	 * The ImportLibTA namespace uses the MemoryHandlers public functions to access these vectors
	 * and store the current scenes data

	 * The user can also gain access to the vectors with the public functions, in case the user would like to store copies
	 * An example of those functions would be "GetSceneVector", where the memoryHandler returns the whole vector as reference
	*/
	std::vector<SceneHeader> scene;
	std::vector<Meshheader> meshes;
	std::vector<std::vector<Controlpoints>> meshControlPoints;
	std::vector<std::vector<Vertex>> vertices;

	std::vector<LightHeader> lights;
	std::vector<CameraHeader> cameras;

	std::vector<Materialheader> materials;
	std::vector<Texture> textures;

	std::vector<Animation> animations;
	std::vector<BlendShape> blendShapes;
	std::vector<std::vector<Controlpoints>> blendShapeControlPoints;
	std::vector<std::vector<Keyframe>> keyframes;

	/*
	 * Here ends all the vectors in the MemoryHandler
	*/

public:

	// Default constructor of memoryHandler
	MemoryHandler();

	/*
	 * Contructor of MemoryHandler
	 * \param otherHandler		Pointer to the MemoryHandler which you want to use to create a new MemoryHandler

	 * This constructor can be used to create a copy of the memoryHandler,
	 * that way you dont need to lose its memory when you load in a new scene in the same instance
	*/
	MemoryHandler(MemoryHandler* otherHandler);

	// Destructor of memoryHandler
	~MemoryHandler();

	/*
	* Returns the vector which contains the current scene in memory
	* Can be used for both reading and writing, in case the user want to fill it with copies
	*/
	std::vector<SceneHeader>& GetSceneVector();

	/*
	 * Get the amount of meshes in the scene
	 * \param sceneID		The index of the scene of interest

	 * \return	The amount of meshes in this scene
	*/
	int GetSceneNrOfMeshes(const int sceneID);

	/*
	 * Get the amount of cameras in the scene
	 * \param sceneID		The index of the scene of interest

	 * \return	The amount of cameras in the scene
	*/
	int GetSceneNrOfCameras(const int sceneID);

	/*
	 * Get the amount of lights in the scene
	 * \param sceneID		The index of the scene of interest

	 * \return	The amount of lights in the scene
	*/
	int GetSceneNrOfLights(const int sceneID);

	/*
	 * Get the amount of animation in the scene
	 * \param sceneID		The index of the scene of interest

	 * \return The amount of animations in the scene
	 */
	int GetSceneNrOfAnimation(const int sceneID);

	/*
	* Return the vector which contains all the meshes in memory
	* Will be the size of nrOfMeshes from the scene vector
	* Can be used for both reading and writing, in case the user want to fill it with copies
	*/
	std::vector<Meshheader>& GetMeshVector();

	/*
	* Get the nr of polygons from the mesh at the given index
	* \param meshID	The index of which mesh you want the info from

	* \return		The amount of polygons from the mesh at the given index
	*/
	int GetMeshNrOfPolygons(const int meshID);

	/*
	* Get the amount of materials from the mesh at the given index
	* \param meshID	The index of which mesh you want the info from

	* \return		The amount of materials from the mesh at the given index
	*/
	int GetMeshNrOfMaterials(const int meshID);

	/*
	* Get the name of the mesh at the given index
	* \param meshID	The index of which mesh you want the info from

	* return		The name of the mesh at the given index
	*/
	std::string GetMeshName(const int meshID);

	/*
	* Get the vertex vector which holds the same amount of vectors as there are meshes, 
	* which in turn holds all the vertices from each mesh
	* Is used internally to store all the vertices from each mesh
	*/
	std::vector<std::vector<Vertex>>& GetVertexVector();

	/*
	* Get a vector with the vertices of a mesh at a given index
	* \param meshID	The index of which mesh you want the info from

	* \return The vector which holds the vertices of the given mesh index
	*/
	std::vector<Vertex> GetVerticesAtIndex(const int meshID);

	/*
	* Return the control point vector which holds the same amount of vectors as there are meshes,
	* which in turn hold all the control points for each mesh
	* Is used internally to store all control points from each mesh

	* The control points is not used to build the mesh, it can be used to make morph animations,
	* if there are any blendshapes that targets the mesh
	*/
	std::vector<std::vector<Controlpoints>>& GetMeshCtrlVector();

	/*
	* Get the vector which the control points of a mesh at a given index
	* \param meshID		The index of which mesh you want the info from

	* \return The vector who holds the control points of the given mesh index
	*/
	std::vector<Controlpoints> GetMeshControlPoints(const int meshID);

	/*
	* Return the vector which hold all the materials
	* Can be used both for reading and writing, in case the user would like to store copies
	*/
	std::vector<Materialheader>& GetMaterialVector();

	/*
	* Get the ambient of the material at the given index
	* \param meshID	The index of the mesh who holds the wanted info

	* \return		The ambient color of the material at the given index as a custom float3 structure
	*/
	float3 GetMaterialAmbient(const int meshID);

	/*
	* Get the diffuse of the material at the given index
	* \param meshID	The index of the mesh who holds the wanted info

	* \return		The diffuse color of the material at the given index as a custom float3 structure
	*/
	float3 GetMaterialDiffuse(const int meshID);

	/*
	* Get the emissive of the material at the given index
	* \param meshID	The index of the mesh who holds the wanted info
					

	* \return		The emissive color of the material at the given index as a custom float3 structure
	*/
	float3 GetMaterialEmissive(const int meshID);

	/*
	* Get the index of the material from a given mesh index
	* Since a mesh can have more than 1 material, the material index will correspond to which material in memory of the given mesh
	* If the mesh only has 1 material, its index will be 0

	* \param meshID	The index of the mesh whos material you want

	* \return The index of the material according to its mesh
	*/
	int GetMaterialIndex(const int meshID);

	/*
	* Get the amount of textures the material at the given index has
	* \param meshID	The mesh index which material you are looking at

	* \return		The amount of textures the material at the given index has
	*/
	int GetMaterialNrofTextures(const int meshID);

	/*
	* Get the opacity of the material at the given index
	* \param meshID	The mesh index of the material of interest

	* \return		The opacity of the material at the given index
	*/
	float GetMaterialLambertOpaque(const int meshID);

	/*
	* Get the name of the material at the given index
	* \param meshID	The mesh index of the material of interest

	* \return		The name of the material at the given index
	*/
	std::string GetMaterialName(const int meshID);

	/*
	* Get the shading model type of the material at the given index
	* \param meshID	The mesh index of the material of interest

	* \return		The shading model type of the material at the given index, as a string
	*/
	std::string GetMaterialShadingModel(const int meshID);

	/*
	* Return the vector which holds all the textures
	* Can be used for both writing and reading, in case the user would like to store copies
	*/

	/*
	 * Return the vector which holds all the textures in the scene
	 * Is used interally to store all the textures to memory
	*/
	std::vector<Texture>& GetTextureVector();

	/*
	 * Get the index of the material which the texture at the given index belongs to
	 * \param textureIndex		The index of the texture of interest

	 * \return	The index of the material which the texture at the given index belongs to
	*/
	int GetTextureIndex(const int textureIndex);

	/*
	 * Get the name of the texture at the given index
	 * \param textureIndex		The index of the texture of interest

	 * \return	The name of the texture at the given index, as a string
	*/
	std::string GetTextureName(const int textureIndex);

	/*
	 * Get the type of the texture at the given index
	 * The texture types supported as of know is "Diffuse Color" and "Normal map"
	 * \param textureIndex		The index of the texture of interest

	 * \return	The type of the texture at the given index, as a string
	*/
	std::string GetTextureType(const int textureIndex);

	/*
	 * Get the path of the texture at the given index
	 * The texture path can be used with f.e. "WicTextureLoader" to create a shaderResourceView
	 * \param textureIndex		The index of the texture of interest

	 * \return The path of the texture at the given index, as a string
	*/
	std::string GetTexturePath(const int textureIndex);

	/*
	* Return the vector which holds all the lights in the scene
	* Can be used both for reading and writing in case the user wants to store copies
	*/
	std::vector<LightHeader>& GetLightVector();

	/*
	* Get the type of the light at a given index
	* \param lightID		The index of the light of interest

	* \return The type of the light at a given index, as a string
	*/
	std::string GetLightType(const int lightID);

	/*
	* Get the name of the light at a given index
	* \param lightID		The index of the light of interest

	* \return The name of the light at a given index, as a string
	*/
	std::string GetLightName(const int lightID);

	/*
	* Get the decay type of the light at a given index
	* \param lightID		The index of the light of interest

	* \return The decay type of the light at a given index, as a string
	*/
	std::string GetLightDecayType(const int lightID);

	/*
	* Get the gobo filename of the light at a given index
	* \param lightID		The index of the light of interest

	* \return The gobo filename of the light at a given index, as a string
	*/
	std::string GetLightGoboFileName(const int lightID);

	/*
	* Get the position of the light at a given index
	* \param lightID		The index of the light of interest

	* \return The position of the light at a given index, as a custom float3 structure
	*/
	float3 GetLightPosition(const int lightID);

	/*
	* Get the color of the light at a given index
	* \param lightID		The index of the light of interest

	* \return The color of the light at a given index, as a custom float3 structure
	*/
	float3 GetLightColor(const int lightID);

	/*
	* Get the direction of the light at a given index
	* \param lightID		The index of the light of interest

	* \return The direction of the light at a given index, as a custom float3 structure
	*/
	float3 GetLightDirection(const int lightID);

	/*
	* Get the shadow color of the light at a given index
	* \param lightID		The index of the light of interest

	* \return The shadow color of the light at a given index, as a custom float3 structure
	*/
	float3 GetLightShadowColor(const int lightID);

	/*
	* Get the intensity of the light at a given index
	* \param lightID		The index of the light of interest

	* \return The intensity of the light at a given index
	*/
	float GetLightIntensity(const int lightID);

	/*
	* Get the range of the light at a given index
	* \param lightID		The index of the light of interest

	* \return The range of the light at a given index
	*/
	float GetLightRange(const int lightID);

	/*
	* Get the inner angle of the light at a given index
	* \param lightID		The index of the light of interest

	* \return The inner angle of the light at a given index
	*/
	float GetLightInnerAngle(const int lightID);

	/*
	* Get the outer angle of the light at a given index
	* \param lightID		The index of the light of interest

	* \return The outer angle of the light at a given index
	*/
	float GetLightOuterAngle(const int lightID);

	/*
	* Get the fog value of the light at a given index
	* \param lightID		The index of the light of interest

	* \return The fog of the light at a given index
	*/
	float GetLightFog(const int lightID);

	/*
	* Get the range of when the decay start, of the light at a given index
	* \param lightID		The index of the light of interest

	* \return The range of decay of the light at a given index
	*/
	float GetLightDecayStart(const int lightID);

	/*
	* Get the range when near attenuation starts of the light at a given index
	* \param lightID		The index of the light of interest

	* \return The range when near attenuation starts of the light at a given index
	*/
	float GetLightnearAttenuationStart(const int lightID);

	/*
	* Get the range when far attenuation starts of the light at a given index
	* \param lightID		The index of the light of interest

	* \return The range when far attenuation starts of the light at a given index
	*/
	float GetLightFarAttenuationStart(const int lightID);

	/*
	* Get the range when near attenuation ends of the light at a given index
	* \param lightID		The index of the light of interest

	* \return The range when near attenuation ends of the light at a given index
	*/
	float GetLightnearAttenuationEnd(const int lightID);

	/*
	* Get the range when far attenuation ends of the light at a given index
	* \param lightID		The index of the light of interest

	* \return The range when far attenuation ends of the light at a given index
	*/
	float GetLightFarAttenuationEnd(const int lightID);

	/*
	* Get the left barn door value of the light at a given index
	* \param lightID		The index of the light of interest

	* \return The left barn door of the light at a given index
	*/
	float GetLightLeftBarnDoor(const int lightID);

	/*
	* Get the right barn door value of the light at a given index
	* \param lightID		The index of the light of interest

	* \return The right barn door of the light at a given index
	*/
	float GetLightRightBarnDoor(const int lightID);

	/*
	* Get the top barn door value of the light at a given index
	* \param lightID		The index of the light of interest

	* \return The top barn door of the light at a given index
	*/
	float GetLightTopBarnDoor(const int lightID);

	/*
	* Get the bottom barn door value of the light at a given index
	* \param lightID		The index of the light of interest

	* \return The botoom barn door of the light at a given index
	*/
	float GetLightBottomBarnDoor(const int lightID);

	/*
	* Get true or false if the light at the given index casts light
	* \param lightID	The index of the light of interest

	* \return	True if cast light, false if not
	*/
	int GetLightCastLight(const int lightID);

	/*
	* Get true or false if the light at the given index casts shadows
	* \param lightID	The index of the light of interest

	* \return	True if cast shadows, false if not
	*/
	int GetLightCastShadows(const int lightID);

	/*
	* Get true or false if the light at the given index draws volumetric light
	* \param lightID	The index of the light of interest

	* \return	True if draws volumetric light, false if not
	*/
	int GetLightDrawVolumetricLight(const int lightID);

	/*
	* Get true or false if the light at the given index draws ground projection
	* \param lightID	The index of the light of interest

	* \return	True if draws ground projection, false if not
	*/
	int GetLightDrawGroundProjection(const int lightID);

	/*
	* Get true or false if the light at the given index draws front facing volumetric light
	* \param lightID	The index of the light of interest

	* \return	True if draws front facing volumetric light, false if not
	*/
	int GetLightDrawFrontfacingVolumetricLight(const int lightID);

	/*
	* Get true or false if the light at the given index has near attenuation enabled
	* \param lightID	The index of the light of interest

	* \return	True if the light has near attenuation enabled, false if not
	*/
	int GetLightEnableNearAttenuation(const int lightID);

	/*
	* Get true or false if the light at the given index has far attenuation enabled
	* \param lightID	The index of the light of interest

	* \return	True if the light has far attenuation enabled, false if not
	*/
	int GetLightEnableFarAttenuation(const int lightID);

	/*
	* Get true or false if the light at the given index has barn door enabled
	* \param lightID	The index of the light of interest

	* \return	True if the light has barn door enabled, false if not
	*/
	int GetLightEnableBarnDoor(const int lightID);

	/*
	* Get true or false if the light at the given index has an area light shape
	* \param lightID	The index of the light of interest

	* \return	True if the light has an area light shape, false if not
	*/
	int GetLightAreaLightShape(const int lightID);

	/*
	* Get the cameras name at a given index
	* \param cameraID	The index of the wanted camera

	* \return The name of a camera at the given index, as a string
	*/
	std::string GetCameraName(const int cameraID);

	/*
	* Get the cameras near plane value at a given index
	* \param cameraID	The index of the wanted camera

	* \return The near plane of a camera at the given index
	*/
	float GetCameraNearPlane(const int cameraID);

	/*
	* Get the cameras far plane value at a given index
	* \param cameraID	The index of the wanted camera

	* \return The far plane of a camera at the given index
	*/
	float GetCameraFarPlane(const int cameraID);

	/*
	* Get the cameras field of view at a given index
	* \param cameraID	The index of the wanted camera

	* \return the field of view of a camera at the given index
	*/
	float GetCameraFieldOfView(const int cameraID);

	/*
	* Return the vector which holds all the cameras in the scene
	* Can be used both for reading and writing, in case the user want to store copies
	*/
	std::vector<CameraHeader>& GetCameraVector();

	/*
	* Get the cameras position at a given index
	* \param cameraID	The index of the wanted camera

	* \return The position of a camera at the given index, as a custom float3 structure
	*/
	float3 GetCameraPosition(const int cameraID);

	/*
	* Get the cameras direction at a given index
	* \param cameraID	The index of the wanted camera

	* \return the direction of a camera at the given index, as a custom float3 structure
	*/
	float3 GetCameraDirection(const int cameraID);

	/*
	* Get the cameras up vector at a given index
	* \param cameraID	The index of the wanted camera

	* \return The up vector of a camera at the given index, as a custom float3 structure
	*/
	float3 GetCameraUpVector(const int cameraID);

	/*
	* Return the vector which holds the animations in memory
	* Can be used both for reading and writing, in case the user would like to make copies
	*/
	std::vector<Animation>& GetAnimationVector() { return this->animations; }

	/*
	 * Get the name of the mesh which is related to the animation at the given index
	 * \param animIndex		The index of which animation you want

	 *\return	The name of the related mesh, as a string
	*/
	std::string GetAnimationMeshRelated(const int animIndex);

	/*
	 * Get the duration of the animation at the given index
	 * The duration of the animation is in amount of ticks
	 * Every tick do not need to represent an actual keyframe within the blendshape
	 * \param animIndex		The index of which animation you want

	 * \return The duration or length of the animation at the given index
	*/
	int GetAnimationDuration(const int animIndex);

	/*
	 * Get the amount of blendShapes which is affected by the animation at the given index
	 * \param animIndex		The index of which animation is wanted

	 * \return	The amount of blendshapes affected by the animation at the given index
	*/
	int GetAnimationNrOfBlendShapes(const int animIndex);


	/*
	 * Return the vector which holds all the blendshapes in the scene
	 * Can be used both for reading and writing, in case the user would like to make copies
	*/
	std::vector<BlendShape>& GetBlendShapeVector();

	/*
	 * Get the vector which holds the same amount of vectors as there are blendshapes,
	 * which in turn holds all the control points for the blendshapes
	 * Is used internally to store all the control points for every blendshape
	*/
	std::vector<std::vector<Controlpoints>>& GetBlendShapeCtrlVector();

	/*
	 * Get the name of the blendshape at the given index
	 * \param blendID	The index of the needed blendshape

	 *\return	The name of the blendshape at the given index, as a string
	*/
	std::string GetBlendShapeName(const int blendID);

	/*
	 * Get the name of the channel connected between this blendshape and its mesh
	 * \param blendID	The index of the needed blendshape

	 * \return	The name of the channel at the given index, as a string
	*/
	std::string GetBlendShapeChannelname(const int blendID);

	/*
	 * Get the name of the mesh which the blendshape is built from
	 * This mesh is not the same as the mesh which is affected by the blendshape
	 * This name can be the same as the actual blendshapes name, but its not a certainty
	 * \param blendID	The index of the needed blendshape

	 * \return The name of the mesh which the blendshape is built from, as a string
	*/
	std::string GetBlendShapeTargetShapeName(const int blendID);

	/*
	 * Get the name of the mesh which is affected by the blendshape at the given index
	 * \param blendID	The index of the needed blendshape

	 * \return	The name of the mesh which is affected by the blendshape, as a string
	*/
	std::string GetBlendShapeMeshRelated(const int blendID);
	
	/*
	 * Get the number of control points of the blendshape at the given index
	 * This should be the same amount as the number of control points of the mesh which is affected by the blendshape
	 * \param blendID	The index of the needed blendshape

	 * \return The number of control points of the blendshape at the given index
	*/
	int GetBlendShapeNrOfControlPoints(const int blendID);

	/*
	 * Get the number of keyframes that this blendshape is affecting
	 * If you want to know which animation this blendshapes keyframes is used in,
	 * you can check which animations related mesh is by its name, and compare it with this blendshapes own related mesh
	 * \param blendID	The index of the needed blendshape

	 * \return The amount of keyframes this blendshape is affecting
	*/
	int GetBlendShapeNrOfKeyFrames(const int blendID);

	/*
	 * Return the vector which holds as many vectors as there are blendshapes, 
	 * which in turn holds all the keyframes on each blendshape
	 * Is used internally to store all the keyframes from the .bin file. 
	*/
	std::vector<std::vector<Keyframe>>& GetKeyframeVector();

	/*
	 * Get the index of which blendshape the vector with keyframes belongs to
	 * This function can be used to check which keyframe belongs to what blendshape
	 * \param keyframeVectorIndex		The index of which vector you want to check

	 * \return The index of the blendshape who owns the selected keyframes 
	*/
	int GetKeyFrameBlendIndex(const int keyFrameVectorIndex);
};