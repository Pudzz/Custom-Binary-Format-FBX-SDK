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
