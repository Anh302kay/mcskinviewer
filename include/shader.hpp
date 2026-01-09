#pragma once
#include <iostream>
#include <string>
#include <3ds.h>
#include <citro3d.h>

class Shader {
public:
    Shader(u32* shbinData, u32 shbinSize, int gStride = 0);
    ~Shader();

    void use();

    s8 getUniformLocation(const GPU_SHADER_TYPE type, const std::string uniform);
    void setUniform(const GPU_SHADER_TYPE type, const std::string& name, int x, int y, int z, int w);
    void setUniform(const GPU_SHADER_TYPE type, const std::string& name, float x, float y, float z, float w);
    void setUniform(const GPU_SHADER_TYPE type, const std::string& name, bool p_bool);
    void setUniform2x4(const GPU_SHADER_TYPE type, const std::string& name, const C3D_Mtx* mtx);
    void setUniform3x4(const GPU_SHADER_TYPE type, const std::string& name, const C3D_Mtx* mtx);
    void setUniform4x4(const GPU_SHADER_TYPE type, const std::string& name, const C3D_Mtx* mtx);
    
private:
    shaderProgram_s shader;
    DVLB_s* shader_DVLB = nullptr;

};