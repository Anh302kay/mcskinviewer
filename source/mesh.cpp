#include <iostream>
#include <3ds.h>
#include <citro3d.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "mesh.hpp"
#include "utils.hpp"

Mesh::Mesh() {
    AttrInfo_Init(&attrInfo);
    AttrInfo_AddLoader(&attrInfo, 0, GPU_FLOAT, 3); // pos
    // AttrInfo_AddLoader(&attrInfo, 1, GPU_FLOAT, 3); // normal
    AttrInfo_AddLoader(&attrInfo, 1, GPU_FLOAT, 2); // uv
}

Mesh::Mesh(const std::string& obj) {
    AttrInfo_Init(&attrInfo);
    AttrInfo_AddLoader(&attrInfo, 0, GPU_FLOAT, 3); // pos
    // AttrInfo_AddLoader(&attrInfo, 1, GPU_FLOAT, 3); // normal
    AttrInfo_AddLoader(&attrInfo, 1, GPU_FLOAT, 2); // uv
    loadOBJ(obj);
}

Mesh::~Mesh() {
    linearFree(meshData);
    meshData = nullptr;
    linearFree(indices);
    indices = nullptr;
}

void Mesh::loadOBJ(const std::string& obj) {
    std::vector<v3f> vPos;
    std::vector<v2f> vTexcoords;
    std::vector<v3f> vNormals;

    std::vector<u16> vertexIndices, texIndices, normalIndices;

    std::stringstream ss;
    std::ifstream objFile(obj);
    std::string line;
    std::string prefix;

    v3f tempv3f;
    v2f tempv2f;

    if(!objFile.is_open())
        return;

    while(std::getline(objFile, line))
    {
        ss.clear();
        ss.str(line);
        ss >> prefix;
        if(prefix == "#") {
            continue;
        }
        else if (prefix == "o") {
            continue;
        }
        else if (prefix == "s")  {
            continue;
        }
        else if (prefix == "usemtl") {
            continue;
        }
        else if (prefix == "v") {
            ss >> tempv3f.x >> tempv3f.y >> tempv3f.z;
            vPos.push_back(tempv3f);
            continue;
        }
        else if (prefix == "vt") {
            ss >> tempv2f.x >> tempv2f.y;
            vTexcoords.push_back(tempv2f);
            continue;
        }
        else if (prefix == "vn") {
            ss >> tempv3f.x >> tempv3f.y >> tempv3f.z;
            vNormals.push_back(tempv3f);
            continue;
        }
        else if (prefix == "f") {
            int counter = 0;
            int tempInt = 0;
            while(ss >> tempInt)
            {
                switch(counter)
                {
                    case 0:
                        vertexIndices.push_back(tempInt);
                        break;
                    case 1:
                        texIndices.push_back(tempInt);
                        break;
                    case 2:
                        normalIndices.push_back(tempInt);
                        break;
                    default:
                        break;
                }
                if(ss.peek() == '/')
                {
                    counter++;
                    ss.ignore(1, '/');
                }
                else if(ss.peek() == ' ')
                {
                    counter++;
                    ss.ignore(1, ' ');
                }

                if(counter > 2)
                    counter = 0;
            }
        }
    }

    meshData = linearAlloc(vertexIndices.size()*sizeof(vertex));
    meshSize = vertexIndices.size();
    

    // model.resize(vertexIndices.size(), vertex());

    for(size_t i = 0; i < vertexIndices.size(); i++)
    {
        ((vertex*)meshData)[i].position = vPos.at(vertexIndices[i] - 1);
        ((vertex*)meshData)[i].texCoords = vTexcoords.at(texIndices[i] - 1);
        
    }
    BufInfo_Init(&bufInfo);
    BufInfo_Add(&bufInfo, meshData, sizeof(vertex), 2, 0x10);
}

void Mesh::render() {
    C3D_SetBufInfo(&bufInfo);
    C3D_SetAttrInfo(&attrInfo);
    // C3D_DrawArrays(GPU_TRIANGLES, 0, meshSize);
    C3D_DrawElements(GPU_TRIANGLES, indexCount, C3D_UNSIGNED_SHORT, indices);
}