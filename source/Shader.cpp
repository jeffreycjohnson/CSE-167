#include "Shader.h"
#include <fstream>
#include "gtc/type_ptr.hpp"

#include "Renderer.h"

Shader::Uniform::Uniform(GLint program, GLint location)
    : program(program), location(location)
{
    if(program == -1 || location == -1)
    {
		CHECK_ERROR();
        LOG("Invalid Uniform Location");
    //    throw;
    }
}

void Shader::Uniform::operator=(bool val)
{
    glProgramUniform1i(program, location, val);
}

void Shader::Uniform::operator=(float val)
{
    glProgramUniform1f(program, location, val);
}

void Shader::Uniform::operator=(int val)
{
    glProgramUniform1i(program, location, val);
}

void Shader::Uniform::operator=(unsigned int val)
{
    glProgramUniform1ui(program, location, val);
}

void Shader::Uniform::operator=(const glm::vec2& val)
{
    glProgramUniform2fv(program, location, 1, glm::value_ptr(val));
}

void Shader::Uniform::operator=(const glm::vec3& val)
{
    glProgramUniform3fv(program, location, 1, glm::value_ptr(val));
}

void Shader::Uniform::operator=(const glm::vec4& val)
{
    glProgramUniform4fv(program, location, 1, glm::value_ptr(val));
}

void Shader::Uniform::operator=(const glm::mat2& val)
{
    glProgramUniformMatrix2fv(program, location, 1, GL_FALSE, glm::value_ptr(val));
}

void Shader::Uniform::operator=(const glm::mat3& val)
{
    glProgramUniformMatrix3fv(program, location, 1, GL_FALSE, glm::value_ptr(val));
}

void Shader::Uniform::operator=(const glm::mat4& val)
{
    glProgramUniformMatrix4fv(program, location, 1, GL_FALSE, glm::value_ptr(val));
}

static bool compile(const std::string& file, GLuint shader)
{
    std::ifstream data(file, std::ios::binary);
    data.seekg(0, data.end);
    int length = static_cast<int>(data.tellg());
    data.seekg(0, data.beg);
    char * buffer = new char[length+1];
    data.read(buffer, length);
    data.close();
	buffer[length] = '\0';

    glShaderSource(shader, 1, &buffer, nullptr);
    delete[] buffer;
    glCompileShader(shader);
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        char errbuf[512];
        glGetShaderInfoLog(shader, 512, nullptr, errbuf);
        LOG((static_cast<const char *>(errbuf)));
        return true;;
    }
    return false;
}

Shader::Shader(const std::string& vertex, const std::string& fragment, bool autoReload)
    : vertexFile(vertex), fragFile(fragment), autoReload(autoReload)
{
    reload();
}

Shader::~Shader()
{
    glDeleteProgram(id);
}

//TODO should we pre-extract these into a map?
Shader::Uniform Shader::operator[](const std::string& name)
{
    return Uniform(id, glGetUniformLocation(id, name.c_str()));
}

void Shader::use()
{
    reloadTimer++;
    if(reloadTimer % 30) reload();
    glUseProgram(id);
	Renderer::setCurrentShader(this);
}

void Shader::reload()
{
    if (id != -1 && !autoReload) return;
    else if(id == -1 || autoReload)
    {
        bool reload = false;
        auto t = last_write_time(path(vertexFile));
        if (vertFileTime != t)
        {
            reload = true;
            vertFileTime = t;
        }
        t = last_write_time(path(fragFile));
        if (fragFileTime != t)
        {
            reload = true;
            fragFileTime = t;
        }
        if (!reload) return;
    }

    if(id != -1) glDeleteProgram(id);

    id = glCreateProgram();
    auto vertexShader = glCreateShader(GL_VERTEX_SHADER);
    auto fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    CHECK_ERROR();

    if (compile(vertexFile, vertexShader)) throw vertexFile;
    glAttachShader(id, vertexShader);
    if (compile(fragFile, fragmentShader)) throw fragFile;
    glAttachShader(id, fragmentShader);

    glLinkProgram(id);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    CHECK_ERROR();

    GLint status;
    glGetProgramiv(id, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        char errbuf[512];
        glGetProgramInfoLog(id, 512, nullptr, errbuf);
        LOG(static_cast<const char *>(errbuf));
        throw;
    }
    glValidateProgram(id);
    glGetProgramiv(id, GL_VALIDATE_STATUS, &status);
    if (status == GL_FALSE)
    {
        char errbuf[512];
        glGetProgramInfoLog(id, 512, nullptr, errbuf);
        LOG(static_cast<const char *>(errbuf));
        throw;
    }
    CHECK_ERROR();
}