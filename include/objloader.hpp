#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <3ds.h>
#include "utils.hpp"
inline bool loadOBJ(const std::string& file, std::vector<vertex>& model) {
    std::vector<v3f> vPos;
    std::vector<v2f> vTexcoords;
    std::vector<v3f> vNormals;

    std::vector<u32> vertexIndices, texIndices, normalIndices;

    std::stringstream ss;
    std::ifstream objFile(file);
    std::string line;
    std::string prefix;

    v3f tempv3f;
    v2f tempv2f;

    if(!objFile.is_open())
        return false;

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

    model.resize(vertexIndices.size(), vertex());

    for(size_t i = 0; i < model.size(); i++)
    {
        model[i].position = vPos.at(vertexIndices[i] - 1);
        model[i].texCoords = vTexcoords.at(texIndices[i] - 1);
        
    }
    return true;
}