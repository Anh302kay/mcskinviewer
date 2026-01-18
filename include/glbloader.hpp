#pragma once
#include <iostream>
#include <ranges>
#include <string>
#include <array>
#include "external/tiny_gltf.h"

#include "utils.hpp"
#include "mesh.hpp"

inline std::array<Mesh, 6> loadSkinGLB(const std::string& path) {
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string error, warning;
    std::array<Mesh, 6> skin;
    if(!loader.LoadBinaryFromFile(&model, &error, &warning, path, false)) {
        std::cout << "failed to load gltf model:" << path << ", Error:" << error << "\n";
        return skin;
    }

    if(!warning.empty()) {
        std::cout << "Warning: " << warning << "\n";
    }

    for(auto [index, mesh] : std::views::enumerate(model.meshes))  {
        std::vector<vertex> vertices;
        std::vector<u16> indicesVec;
        std::cout << mesh.name;
        for(auto& primitive : mesh.primitives) {
            if(primitive.indices >= 0) {
                tinygltf::Accessor& accessor = model.accessors.at(primitive.attributes["POSITION"]);
                tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                const float* positions = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

                // accessor = model.accessors.at(primitive.attributes["NORMAL"]);
                // bufferView = model.bufferViews[accessor.bufferView];
                // buffer = model.buffers[bufferView.buffer];
                
                // const float* normals = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

                accessor = model.accessors.at(primitive.attributes["TEXCOORD_0"]);
                bufferView = model.bufferViews[accessor.bufferView];
                buffer = model.buffers[bufferView.buffer];

                const float* texcoords = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

                for(size_t i = 0; i < accessor.count; ++i) {
                    float posX = positions[i * 3 + 0];
                    float posY = positions[i * 3 + 1];
                    float posZ = positions[i * 3 + 2];

                    // float normalX = normals[i * 3 + 0];
                    // float normalY = normals[i * 3 + 1];
                    // float normalZ = normals[i * 3 + 2];

                    float U = texcoords[i * 2 + 0];
                    float V = 1.f - texcoords[i * 2 + 1];
                    vertices.push_back({v3f(posX, posY, posZ), v2f(U, V)});
                }

                accessor = model.accessors.at(primitive.indices);
                bufferView = model.bufferViews[accessor.bufferView];
                buffer = model.buffers[bufferView.buffer];
                const unsigned short* indices = reinterpret_cast<const unsigned short*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);
                for(size_t i = 0; i < accessor.count; i++) {
                    indicesVec.push_back(indices[i]);
                }
            }
        }
        Mesh& m = skin.at(index);
        m.meshData = linearAlloc(vertices.size() * sizeof(vertex));
        m.meshSize = vertices.size();
        std::copy(vertices.begin(), vertices.end(), (vertex*)m.meshData);
        m.indexCount = indicesVec.size();
        m.indices = (u16*)linearAlloc(indicesVec.size() * sizeof(u16));
        std::copy(indicesVec.begin(), indicesVec.end(), (u16*)m.indices);
        BufInfo_Init(&m.bufInfo);
        BufInfo_Add(&m.bufInfo, m.meshData, sizeof(vertex), 2, 0x10);
        // skin.at(index).meshData;

    }
    std::cout << "\n";
    return skin;
}

inline void loadSkinGLB(std::array<Mesh, 6>& skin, const std::string& path) {
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string error, warning;
    
    if(!loader.LoadBinaryFromFile(&model, &error, &warning, path, false)) {
        std::cout << "failed to load gltf model:" << path << ", Error:" << error << "\n";
    }

    if(!warning.empty()) {
        std::cout << "Warning: " << warning << "\n";
    }

    for(auto [index, mesh] : std::views::enumerate(model.meshes))  {
        std::vector<vertex> vertices;
        std::vector<u16> indicesVec;
        for(auto& primitive : mesh.primitives) {
            if(primitive.indices >= 0) {
                tinygltf::Accessor& accessor = model.accessors.at(primitive.attributes["POSITION"]);
                tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                const float* positions = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

                // accessor = model.accessors.at(primitive.attributes["NORMAL"]);
                // bufferView = model.bufferViews[accessor.bufferView];
                // buffer = model.buffers[bufferView.buffer];
                
                // const float* normals = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

                accessor = model.accessors.at(primitive.attributes["TEXCOORD_0"]);
                bufferView = model.bufferViews[accessor.bufferView];
                buffer = model.buffers[bufferView.buffer];

                const float* texcoords = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

                for(size_t i = 0; i < accessor.count; ++i) {
                    float posX = positions[i * 3 + 0];
                    float posY = positions[i * 3 + 1];
                    float posZ = positions[i * 3 + 2];

                    // float normalX = normals[i * 3 + 0];
                    // float normalY = normals[i * 3 + 1];
                    // float normalZ = normals[i * 3 + 2];

                    float U = texcoords[i * 2 + 0];
                    float V = 1.f - texcoords[i * 2 + 1];
                    vertices.push_back({v3f(posX, posY, posZ), v2f(U, V)});
                }

                accessor = model.accessors.at(primitive.indices);
                bufferView = model.bufferViews[accessor.bufferView];
                buffer = model.buffers[bufferView.buffer];
                const unsigned short* indices = reinterpret_cast<const unsigned short*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);
                for(size_t i = 0; i < accessor.count; i++) {
                    indicesVec.push_back(indices[i]);
                }
            }
        }
        Mesh& m = skin.at(index);
        m.meshData = linearAlloc(vertices.size() * sizeof(vertex));
        m.meshSize = vertices.size();
        std::copy(vertices.begin(), vertices.end(), (vertex*)m.meshData);
        m.indexCount = indicesVec.size();
        m.indices = (u16*)linearAlloc(indicesVec.size() * sizeof(u16));
        std::copy(indicesVec.begin(), indicesVec.end(), (u16*)m.indices);
        BufInfo_Init(&m.bufInfo);
        BufInfo_Add(&m.bufInfo, m.meshData, sizeof(vertex), 2, 0x10);
        // skin.at(index).meshData;

    }
}