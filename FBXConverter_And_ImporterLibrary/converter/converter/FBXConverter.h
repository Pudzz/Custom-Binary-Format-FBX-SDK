#pragma once
#include <fbxsdk.h>
#include <fbxsdk/fileio/fbxiosettings.h>

#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(pManager->GetIOSettings()))
#endif

struct float2
{
	float u, v;
};
struct float3
{
	float x, y, z;
};
struct float4
{
	float x, y, z, w;
};

class Fbxconverter {

private:

public:
	Fbxconverter();
	~Fbxconverter();
	void InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene);

	void DestroySdkObjects(FbxManager* pManager);

	bool loadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename);
	float2 get2DVector(FbxVector2 uvs);
	float3 get3DVector(FbxVector4 value);

};