#include "MemoryHandler.h"

MemoryHandler::MemoryHandler()
{
}

MemoryHandler::MemoryHandler(MemoryHandler* otherHandler)
{
}

MemoryHandler::~MemoryHandler()
{

	if (vertices.size() > 0)
	{
		for (int i = 0; i < vertices.size(); i++)
		{
			vertices[i].clear();
		}
		vertices.clear();
	}
}

std::vector<SceneHeader>& MemoryHandler::GetSceneVector()
{
	return this->scene;
}

int MemoryHandler::GetSceneNrOfMeshes(const int sceneID)
{
	return this->scene[sceneID].nrOfMeshes;
}

int MemoryHandler::GetSceneNrOfCameras(const int sceneID)
{
	return this->scene[sceneID].nrOfCamera;
}

int MemoryHandler::GetSceneNrOfLights(const int sceneID)
{
	return this->scene[sceneID].nrOfLight;
}

int MemoryHandler::GetSceneNrOfAnimation(const int sceneID)
{
	return this->scene[sceneID].nrOfAnimations;
}

std::vector<Meshheader>& MemoryHandler::GetMeshVector()
{
	return this->meshes;
}

int MemoryHandler::GetMeshNrOfPolygons(const int meshID)
{
	return this->meshes[meshID].nrOfPolygons;
}

int MemoryHandler::GetMeshNrOfMaterials(const int meshID)
{
	return this->meshes[meshID].nrOfMaterials;
}

std::string MemoryHandler::GetMeshName(const int meshID)
{
	return this->meshes[meshID].meshName;
}

std::vector<std::vector<Vertex>>& MemoryHandler::GetVertexVector()
{
	return this->vertices;
}

std::vector<Vertex> MemoryHandler::GetVerticesAtIndex(const int meshID)
{
	return this->vertices[meshID];
}

std::vector<std::vector<Controlpoints>>& MemoryHandler::GetMeshCtrlVector()
{
	return this->meshControlPoints;
}

std::vector<Controlpoints> MemoryHandler::GetMeshControlPoints(const int meshID)
{
	return this->meshControlPoints[meshID];
}

std::vector<Materialheader>& MemoryHandler::GetMaterialVector()
{
	return this->materials;
}

float3 MemoryHandler::GetMaterialAmbient(const int meshID)
{
	float3 temp;
	temp.x = this->materials[meshID].lambertAmbient[0];
	temp.y = this->materials[meshID].lambertAmbient[1];
	temp.z = this->materials[meshID].lambertAmbient[2];

	return temp;
}

float3 MemoryHandler::GetMaterialDiffuse(const int meshID)
{
	float3 temp;
	temp.x = this->materials[meshID].lambertDiffuse[0];
	temp.y = this->materials[meshID].lambertDiffuse[1];
	temp.z = this->materials[meshID].lambertDiffuse[2];
	
	return temp;
}

float3 MemoryHandler::GetMaterialEmissive(const int meshID)
{
	float3 temp;
	temp.x = this->materials[meshID].lambertEmissive[0];
	temp.y = this->materials[meshID].lambertEmissive[1];
	temp.z = this->materials[meshID].lambertEmissive[2];

	return temp;
}

int MemoryHandler::GetMaterialIndex(const int meshID)
{
	return this->materials[meshID].index;
}

int MemoryHandler::GetMaterialNrofTextures(const int meshID)
{
	return this->materials[meshID].nrOfTextures;
}

float MemoryHandler::GetMaterialLambertOpaque(const int meshID)
{
	return this->materials[meshID].lambertOpacity;
}

std::string MemoryHandler::GetMaterialName(const int meshID)
{
	return this->materials[meshID].materialName;
}

std::string MemoryHandler::GetMaterialShadingModel(const int meshID)
{
	return this->materials[meshID].shadingModel;
}

std::vector<Texture>& MemoryHandler::GetTextureVector()
{
	return this->textures;
}

int MemoryHandler::GetTextureIndex(const int textureIndex)
{
	return this->textures[textureIndex].materialIndex;
}

std::string MemoryHandler::GetTextureName(const int textureIndex)
{
	return this->textures[textureIndex].textureName;
}

std::string MemoryHandler::GetTextureType(const int textureIndex)
{
	return this->textures[textureIndex].textureType;
}

std::string MemoryHandler::GetTexturePath(const int textureIndex)
{
	return this->textures[textureIndex].texturePath;
}

std::vector<LightHeader>& MemoryHandler::GetLightVector()
{
	return this->lights;
}

std::string MemoryHandler::GetLightType(const int lightID)
{
	return this->lights[lightID].lightType;
}

std::string MemoryHandler::GetLightName(const int lightID)
{
	return this->lights[lightID].lightName;
}

std::string MemoryHandler::GetLightDecayType(const int lightID)
{
	return this->lights[lightID].decayType;
}

std::string MemoryHandler::GetLightGoboFileName(const int lightID)
{
	return this->lights[lightID].goboFileName;
}

float3 MemoryHandler::GetLightPosition(const int lightID)
{
	float3 temp;
	temp.x = this->lights[lightID].lightPos[0];
	temp.y = this->lights[lightID].lightPos[1];
	temp.z = this->lights[lightID].lightPos[2];

	return temp;
}

float3 MemoryHandler::GetLightColor(const int lightID)
{
	float3 temp;
	temp.x = this->lights[lightID].diffuseLight[0];
	temp.y = this->lights[lightID].diffuseLight[1];
	temp.z = this->lights[lightID].diffuseLight[2];

	return temp;
}

float3 MemoryHandler::GetLightDirection(const int lightID)
{
	float3 temp;
	temp.x = this->lights[lightID].direction[0];
	temp.y = this->lights[lightID].direction[1];
	temp.z = this->lights[lightID].direction[2];

	return temp;
}

float3 MemoryHandler::GetLightShadowColor(const int lightID)
{
	float3 temp;
	temp.x = this->lights[lightID].shadowColor[0];
	temp.y = this->lights[lightID].shadowColor[1];
	temp.z = this->lights[lightID].shadowColor[2];

	return temp;
}

float MemoryHandler::GetLightIntensity(const int lightID)
{
	return this->lights[lightID].intensity;
}

float MemoryHandler::GetLightRange(const int lightID)
{
	return this->lights[lightID].range;
}

float MemoryHandler::GetLightInnerAngle(const int lightID)
{
	return this->lights[lightID].innerAngle;
}

float MemoryHandler::GetLightOuterAngle(const int lightID)
{
	return this->lights[lightID].outerAngle;
}

float MemoryHandler::GetLightFog(const int lightID)
{
	return this->lights[lightID].fog;
}

float MemoryHandler::GetLightDecayStart(const int lightID)
{
	return this->lights[lightID].decayStart;
}

float MemoryHandler::GetLightnearAttenuationStart(const int lightID)
{
	return this->lights[lightID].nearAttenuationStart;
}

float MemoryHandler::GetLightFarAttenuationStart(const int lightID)
{
	return this->lights[lightID].farAttenuationStart;
}

float MemoryHandler::GetLightnearAttenuationEnd(const int lightID)
{
	return this->lights[lightID].nearAttenuationEnd;
}

float MemoryHandler::GetLightFarAttenuationEnd(const int lightID)
{
	return this->lights[lightID].farAttenuationEnd;
}

float MemoryHandler::GetLightLeftBarnDoor(const int lightID)
{
	return this->lights[lightID].leftBarnDoor;
}

float MemoryHandler::GetLightRightBarnDoor(const int lightID)
{
	return this->lights[lightID].rightBarnDoor;
}

float MemoryHandler::GetLightTopBarnDoor(const int lightID)
{
	return this->lights[lightID].topBarnDoor;
}

float MemoryHandler::GetLightBottomBarnDoor(const int lightID)
{
	return this->lights[lightID].bottomBarnDoor;
}

int MemoryHandler::GetLightCastLight(const int lightID)
{
	return this->lights[lightID].castLight;
}

int MemoryHandler::GetLightCastShadows(const int lightID)
{
	return this->lights[lightID].castShadows;
}

int MemoryHandler::GetLightDrawVolumetricLight(const int lightID)
{
	return this->lights[lightID].drawVolumetricLight;
}

int MemoryHandler::GetLightDrawGroundProjection(const int lightID)
{
	return this->lights[lightID].drawGroundProjection;
}

int MemoryHandler::GetLightDrawFrontfacingVolumetricLight(const int lightID)
{
	return this->lights[lightID].drawFrontFacingVolumetricLight;
}

int MemoryHandler::GetLightEnableNearAttenuation(const int lightID)
{
	return this->lights[lightID].enableNearAttenuation;
}

int MemoryHandler::GetLightEnableFarAttenuation(const int lightID)
{
	return this->lights[lightID].enableFarAttenuation;
}

int MemoryHandler::GetLightEnableBarnDoor(const int lightID)
{
	return this->lights[lightID].enableBarnDoor;
}

int MemoryHandler::GetLightAreaLightShape(const int lightID)
{
	return this->lights[lightID].areaLightShape;
}

std::vector<CameraHeader>& MemoryHandler::GetCameraVector()
{
	return this->cameras;
}

float3 MemoryHandler::GetCameraPosition(const int cameraID)
{
	float3 temp;
	temp.x = this->cameras[cameraID].camPosition[0];
	temp.y = this->cameras[cameraID].camPosition[1];
	temp.z = this->cameras[cameraID].camPosition[2];

	return temp;
}

float3 MemoryHandler::GetCameraDirection(const int cameraID)
{
	float3 temp;
	temp.x = this->cameras[cameraID].direction[0];
	temp.y = this->cameras[cameraID].direction[1];
	temp.z = this->cameras[cameraID].direction[2];

	return temp;
}

float3 MemoryHandler::GetCameraUpVector(const int cameraID)
{
	float3 temp;
	temp.x = this->cameras[cameraID].camUp[0];
	temp.y = this->cameras[cameraID].camUp[1];
	temp.z = this->cameras[cameraID].camUp[2];

	return temp;
}

std::string MemoryHandler::GetCameraName(const int cameraID)
{
	return this->cameras[cameraID].cameraName;
}

float MemoryHandler::GetCameraNearPlane(const int cameraID)
{
	return this->cameras[cameraID].nearPlane;
}

float MemoryHandler::GetCameraFarPlane(const int cameraID)
{
	return this->cameras[cameraID].farPlane;
}

float MemoryHandler::GetCameraFieldOfView(const int cameraID)
{
	return this->cameras[cameraID].fieldOfView;
}

std::string MemoryHandler::GetAnimationMeshRelated(const int animIndex)
{
	return this->animations[animIndex].meshRelated;
}

int MemoryHandler::GetAnimationDuration(const int animIndex)
{
	return this->animations[animIndex].duration;
}

int MemoryHandler::GetAnimationNrOfBlendShapes(const int animIndex)
{
	return this->animations[animIndex].nrOfBlendshapes;
}

std::vector<BlendShape>& MemoryHandler::GetBlendShapeVector()
{
	return this->blendShapes;
}

std::vector<std::vector<Controlpoints>>& MemoryHandler::GetBlendShapeCtrlVector()
{
	return this->blendShapeControlPoints;
}

std::string MemoryHandler::GetBlendShapeName(const int blendID)
{
	return this->blendShapes[blendID].blendShapeName;
}

std::string MemoryHandler::GetBlendShapeChannelname(const int blendID)
{
	return this->blendShapes[blendID].blendShapeChannelname;
}

std::string MemoryHandler::GetBlendShapeTargetShapeName(const int blendID)
{
	return this->blendShapes[blendID].targetShapeName;
}

std::string MemoryHandler::GetBlendShapeMeshRelated(const int blendID)
{
	return this->blendShapes[blendID].meshRelated;
}

int MemoryHandler::GetBlendShapeNrOfControlPoints(const int blendID)
{
	return this->blendShapes[blendID].nrOfControlpoints;
}

int MemoryHandler::GetBlendShapeNrOfKeyFrames(const int blendID)
{
	return this->blendShapes[blendID].nrOfKeyframes;
}

std::vector<std::vector<Keyframe>>& MemoryHandler::GetKeyframeVector()
{
	return this->keyframes;
}

int MemoryHandler::GetKeyFrameBlendIndex(const int keyFrameVectorIndex)
{
	return this->keyframes[keyFrameVectorIndex][0].BlendIndex;
}


