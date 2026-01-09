#include <iostream>
#include <string>
#include <3ds.h>
#include <citro3d.h>
#include "shader.hpp"

Shader::Shader(u32* shbinData, u32 shbinSize, int gStride) {
	shader_DVLB = DVLB_ParseFile(shbinData, shbinSize);
	shaderProgramInit(&shader);
	shaderProgramSetVsh(&shader, &shader_DVLB->DVLE[0]);
    shaderProgramSetGsh(&shader, &shader_DVLB->DVLE[1], gStride);
}

Shader::~Shader() {
    shaderProgramFree(&shader);
    shader.vertexShader = nullptr;
    shader.geometryShader = nullptr;
    DVLB_Free(shader_DVLB);
    shader_DVLB = nullptr;
}

void Shader::use() {
    C3D_BindProgram(&shader);
}

s8 Shader::getUniformLocation(const GPU_SHADER_TYPE type, const std::string uniform) {
    return shaderInstanceGetUniformLocation(type == GPU_VERTEX_SHADER ? shader.vertexShader : shader.geometryShader, uniform.c_str());
}

void Shader::setUniform(const GPU_SHADER_TYPE type, const std::string& name, int x, int y, int z, int w) {
    C3D_IVUnifSet(type, getUniformLocation(type, name), x, y, z, w);
}

void Shader::setUniform(const GPU_SHADER_TYPE type, const std::string& name, float x, float y, float z, float w) {
    C3D_FVUnifSet(type, getUniformLocation(type, name), x, y, z, w);
}

void Shader::setUniform(const GPU_SHADER_TYPE type, const std::string& name, bool p_bool) {
    C3D_BoolUnifSet(type, getUniformLocation(type, name), p_bool);
}

void Shader::setUniform2x4(const GPU_SHADER_TYPE type, const std::string& name, const C3D_Mtx* mtx) {
    C3D_FVUnifMtx2x4(type, getUniformLocation(type, name), mtx);
}

void Shader::setUniform3x4(const GPU_SHADER_TYPE type, const std::string& name, const C3D_Mtx* mtx) {
    C3D_FVUnifMtx3x4(type, getUniformLocation(type, name), mtx);
}

void Shader::setUniform4x4(const GPU_SHADER_TYPE type, const std::string& name, const C3D_Mtx* mtx) {
    C3D_FVUnifMtx4x4(type, getUniformLocation(type, name), mtx);
}