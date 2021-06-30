# Custom-Binary-Format-FBX-SDK
Custom format created using the FBX SDK. 

## Format 
FBX files are opened with the FBS SDK, and the information is converted to a custom bin-file.  

The custom format includes the following:

    Mesh data [Vertices]
    Lambert materials
    Textures [Diffuse and Normal/Bump]
    Cameras
    Lights
    Morph target animation

## Included in repository
Included in this repository is the converter that converts the FBX file to a bin-file. An import library that reads the bin-files is also included.
