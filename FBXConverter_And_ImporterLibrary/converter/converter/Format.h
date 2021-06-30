#pragma once

/*
 * Maximum values for different char arrays
 
 * MESH_NAME_LENGTH:	Max size of mesh name
 * BLENDSHAPE_NAME_LENGTH:	Max size of blendshape name
 * BLENDSHAPE_CHANNEL_NAME_LENGTH:	Max size of channel name
 * SHADING_MODEL_NAME_LENGTH:	Max size of shading models name
 * MATERIAL_NAME_LENGTH:	Max size of materials name
 * TEXTURE_TYPE_NAME:	Max size of texture type
 * TEXTURE_NAME_LENGTH:		Max size of textures name
 * TEXTURE_PATH_NAME_LENGTH:	Max size of textures path 
 * CAMERA_NAME_LENGTH:	Max size of cameras name
 * LIGHT_TYPE_NAME_LENGTH:	Max size of lights type
 * LIGHT_NAME_LENGTH:	Max size of lights name
*/
const int MESH_NAME_LENGTH = 100;
const int BLENDSHAPE_NAME_LENGTH = 100;
const int BLENDSHAPE_CHANNEL_NAME_LENGTH = 100;
const int SHADING_MODEL_NAME_LENGTH = 100;
const int MATERIAL_NAME_LENGTH = 100;
const int TEXTURE_TYPE_NAME = 100;
const int TEXTURE_NAME_LENGTH = 100;
const int TEXTURE_PATH_NAME_LENGTH = 100;
const int CAMERA_NAME_LENGTH = 50;
const int LIGHT_TYPE_NAME_LENGTH = 12;
const int LIGHT_NAME_LENGTH = 50;

/*
 * Structure for use of a mesh controlpoints
 * Controlpoints can be seen as every unique vertex in a mesh

 * \position[]	The vertex positions, represents x, y and z
 * \normal[]	The vertex normals, represents x, y, z
*/
struct Controlpoints {
	float position[3];
	float normal[3];
};

/*
 * Structure for use of a mesh vertices
 
 * \position[]	The vertex positions, represents x, y and z
 * \textcoord[] The vertex uvs, represents u and v
 * \normal[]	The vertex normals, represents x, y and z
 * \tangent[]	The vertex tangents, represents x, y and z
 * \bitangent[] The vertex bitangent/binormals, represents x, y and z
*/
struct Vertex {
	float position[3];	// ok
	float texCoord[2];	// ok
	float normal[3];	// ok
	float tangent[3];	// ok
	float bitangent[3];	// ok
};

/*
 * Structure for use a mesh polygons
 * A mesh will have x amount of polygons, and every polygon will have 3 vertices

 * \vertices	The vertices of this polygon, represents vertex 1, 2 and 3 in the polygon
*/
struct Polygon {
	Vertex vertices[3];
};

/*
 * Structure for use of textures in a material

 * \materialIndex	The material index which this texture belongs to
 * \textureType[]	The type of this texture, current supports Diffuse and Normal map
 * \textureName[]	The name of this texture, when exported from the 3D modelling program (OBS: Not the same as image name)
 * \texturePath[]	The filepath to where this texture is, this path will change depending on where you export your file
*/
struct Texture {
	int materialIndex;
	char textureType[TEXTURE_TYPE_NAME];
	char textureName[TEXTURE_NAME_LENGTH];
	char texturePath[TEXTURE_PATH_NAME_LENGTH];
};

/*
 * Structure for use of materials in a mesh
 
 * \index	The index of the mesh which this material belongs to 
 * \nrOfTexture		The amount of textures this materials has
 * \materialName[]	The name of this material
 * \lambertAmbient[]	The ambient color of this material, represents r, g and b
 * \lambertDiffuse[]	The diffuse color of this material, represents r, g and b
 * \lambertEmissive[]	The emissive color of this material, represents r, g and b
 * \lambertOpacity	The opacity of this material
 * \shadingModel[]	The shading model type of this material, currently supports lambert
*/
struct Materialheader {
	int index;
	int nrOfTextures;
	char materialName[MATERIAL_NAME_LENGTH]; 
	float lambertAmbient[3];
	float lambertDiffuse[3];
	float lambertEmissive[3];
	float lambertOpacity;
	char shadingModel[SHADING_MODEL_NAME_LENGTH];
};

/*
 * Structure for use of a mesh

 * \meshIndex	The index of this mesh, the first mesh of the scene will have 0, the second 1 e.t.c.
 * \meshName[]	The name of the mesh
 * \nrOfPolygons	The amount of polygons in this mesh
 * \nrOfMaterials	The amount of materials in this mesh, will most certainly be 1, unless the mesh is within a group
 * \nrOfControlpoints	The amount of control points in this mesh
*/
struct Meshheader {
	int meshIndex;
	char meshName[MESH_NAME_LENGTH];
	int nrOfPolygons;	
	int nrOfMaterials;
	int nrOfControlpoints;
};

/*
 * Structure for use of cameras

 * \cameraName[]	The name of the camera
 * \camPosition[]	The position of the camera, represents x, y and z
 * \direction[]		The direction of the camera, represents x, y, z
 * \camUp[]		The vector direction of where the cameras up is facing, represents x, y and z
 * \nearPlane	The value of the near plane
 * \farPlane	The value of the far plane
 * \fieldOfView		The value of the cameras field of view
*/
struct CameraHeader {
	char cameraName[CAMERA_NAME_LENGTH];
	float camPosition[3];
	float direction[3];
	float camUp[3];
	float nearPlane;
	float farPlane;
	float fieldOfView;
};

/*
 * Structure for use of lights

 * \lightType[]		The light type of this light
 * \lightName[]		The name of the light
 * \decayType[]		The type of decay of this light
 * \lightPos[]		The position of the light, represents x, y and z
 * \diffuseLight[]	The color of this light, represents x, y and z
 * \direction[]		The direction of this light, represents x, y and z
 * \intensity		The intensity of the light
 * \range		The range of the light
 * \innerAngle		The inner angle value of the light
 * \outerAngle		The outer angle value
 * \fog		The fog value 
 * \decayStart		The value of when the decay starts
 * \goboFileName[]		The filename of the "gobofile". This is used if you use an external image for your light
 * \nearAttenuationstart	The value of when near attenuation starts, if enabled
 * \farAttenuationStart		The value of when far attenuation starts, if enabled
 * \nearAttenuationEnd		The value of when near attenuation ends, if enabled
 * \farAttenuationEnd		The value of when far attenuation ends, if enabled
 * \shadowColor[]		The color of the lights shadow, represents r, g and b
 * \leftBarnDoor		The value of the left barn door
 * \rightBarnDoor		The value of the right barn door
 * \areaLightShape		Bool if the light has a areaLightShape, represents false or true
 * \castLight		Bool if the light should cast light, represents false or true
 * \drawVolumetricLight		If the light should draw volumetric light
 * \drawGroundProjection	If the light should draw projections on the ground
 * \drawFrontFacingVolumetricLight		If the light should draw volumetric light, facing front
 * \enableNearAttenuation		If the light should enable near attenuation
 * \enableFarAttenuation		If the light should enable far attenuation
 * \castShadows			If the light should cast shadows
 * \enableBarnDoor		If the light should enable the barn door
*/
struct LightHeader {
	char lightType[LIGHT_TYPE_NAME_LENGTH];
	char lightName[LIGHT_NAME_LENGTH];
	char decayType[LIGHT_TYPE_NAME_LENGTH];
	float lightPos[3];
	float diffuseLight[3];
	float direction[3];
	float intensity;
	float range;
	float innerAngle;
	float outerAngle;

	float fog;
	float decayStart;
	char goboFileName[50];
	float nearAttenuationStart;
	float farAttenuationStart;
	float nearAttenuationEnd;
	float farAttenuationEnd;
	float shadowColor[3];
	float leftBarnDoor;
	float rightBarnDoor;
	float topBarnDoor;
	float bottomBarnDoor;
	bool areaLightShape;
	bool castLight;
	bool drawVolumetricLight;
	bool drawGroundProjection;
	bool drawFrontFacingVolumetricLight;
	bool enableNearAttenuation;
	bool enableFarAttenuation;
	bool castShadows;
	bool enableBarnDoor;
};

/*
 * Structure for use of blendshapes
 * \blendShapeName[]	The name of the blendshape
 * \blendShapeChannelName[]		The name of the channel between this blendshape and its mesh, no real use except for validation
 * \targetShapeName[]	The name of the mesh which this blendshape is created from, not the same as the mesh whom is affected by the blendshape
 * \meshRelated[]		The name of the mesh which is affected by this blendshape
 * \nrOfControlPoints	The amount of controlpoints of this blendshape, will be the same amount as its related mesh amount
 * \nrOfKeyframes		The amount of keyframes that this blendshape has affecting an animation
*/
struct BlendShape {
	char blendShapeName[BLENDSHAPE_NAME_LENGTH];					// ok
	char blendShapeChannelname[BLENDSHAPE_CHANNEL_NAME_LENGTH];		// ok
	char targetShapeName[MESH_NAME_LENGTH];							// ok
	char meshRelated[MESH_NAME_LENGTH];								// ok
	int nrOfControlpoints;											// ok
	int nrOfKeyframes;												// ok
};

/*
 * Structure for use of keyframes

 * \blendIndex	The index of which blendshape this keyframe belongs to
 * \time	The time where this keyframe is in an animation
 * \value	The value in percent of how much the affected mesh should be affected, will be a value between 0-100, 
*/
struct Keyframe
{
	int BlendIndex;
	int time;
	float value;
};

/* 
 * Structure for use of animation

 * \index	The index of this animation
 * \meshRelated		The name of the mesh who is affected by this animation
 * \duration	The time in the animation where the last keyframe is placed
 * \nrOfBlendShapes		The amount of blendshapes affecting this animation
*/
struct Animation {
	int index;								
	char meshRelated[MESH_NAME_LENGTH];		
	int duration;								
	int nrOfBlendshapes;					
};

/*
 * Structure for use of a scene

 * \nrOfMeshes		The amount of meshes in the scene
 * \nrOfLights		The amount of lights in the scene
 * \nrOfCameras		The amount of cameras in the scene
 * \nrOfAnimations	The amount of animations in the scene
*/
struct SceneHeader {
	int nrOfMeshes;	
	int nrOfLight;
	int nrOfCamera;
	int nrOfAnimations;
};
