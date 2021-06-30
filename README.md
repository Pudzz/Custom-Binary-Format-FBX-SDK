# Custom-Binary-Format-FBX-SDK
Custom format created using the FBX SDK. 

## Format 
FBX files are opened with the FBS SDK, and the information is converted to a custom bin-file.  

The custom format includes the following:

    Mesh data [Vertices]
    Lambert materials [Support for Phong and HLSL, just uncomment the code]
    Textures [Diffuse and Normal/Bump]
    Cameras
    Lights
    Morph target animation

## Included in repository
- Included in this repository is the converter that converts the FBX file to a bin-file [Desktop in this demo]. 
- An import library that reads the bin-files. [Write the right path to the bin-file]

# To make this repository work
Because of files being to large, the FBX SDK is not included in this repository. If you want this code to work, you have to download FBX SDK from https://www.autodesk.com/developer-network/platform-technologies/fbx-sdk-2020-0 and place this map in the path:

    ...\Custom-Binary-Format-FBX-SDK\FBXConverter_And_Importer\Library
    
Or you can just write to my mail and I will help you: padsvensson@live.se
