#ifndef INCLUDE_SHADER_H
#define INCLUDE_SHADER_H

#include "ForwardDecs.h"
#include <experimental/filesystem>
#include <map>

class Shader
{
private:
    class Uniform
    {
    private:
        const GLint program, location;

    public:
        Uniform(GLint program, GLint location);
        void operator=(bool val);
        void operator=(float val);
        void operator=(int val);
        void operator=(unsigned int val);
        void operator=(const glm::vec2& val);
        void operator=(const glm::vec3& val);
        void operator=(const glm::vec4& val);
        void operator=(const glm::mat2& val);
        void operator=(const glm::mat3& val);
        void operator=(const glm::mat4& val);
    };

public:
    friend class Material;

    Shader(const std::string& vertex, const std::string& fragment, bool autoReload = true);
    ~Shader();

    Uniform operator[](const std::string& name);
    void use();
    void reload();

    GLint id = -1;
    bool autoReload;

private:
    int reloadTimer = 0;
    std::string vertexFile, fragFile;
    typedef std::experimental::filesystem::file_time_type file_time_type;
    typedef std::experimental::filesystem::path path;
    file_time_type vertFileTime, fragFileTime;
};

#endif