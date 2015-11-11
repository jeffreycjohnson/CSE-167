#ifndef INCLUDE_SHADER_H
#define INCLUDE_SHADER_H

#include "ForwardDecs.h"

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
    Shader(const std::string& vertex, const std::string& fragment);
    ~Shader();

    Uniform operator[](const std::string& name);
    void use() const;

    const GLint id;
};

#endif