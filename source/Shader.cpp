#include "Shader.h"
#include <fstream>
#include <gtc/type_ptr.hpp>
#include "FileWatcher.h"
#include "Renderer.h"

Shader::Uniform::Uniform(GLint program, GLint location)
    : program(program), location(location)
{
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

    glShaderSource(shader, 1, const_cast<const char**>(&buffer), nullptr);
    delete[] buffer;
    glCompileShader(shader);
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        char errbuf[512];
        glGetShaderInfoLog(shader, 512, nullptr, errbuf);
        LOG(file);
        LOG((static_cast<const char *>(errbuf)));
        return true;
    }
    return false;
}

Shader::Shader(const std::string& vertex, const std::string& fragment, bool autoReload)
    : autoReload(autoReload)
{
    vertWatcher = new FileWatcher(vertex, 30);
    fragWatcher = new FileWatcher(fragment, 30);
    reload();
}

Shader::~Shader()
{
    glDeleteProgram(id);
}

//TODO should we pre-extract these into a map?
Shader::Uniform Shader::operator[](const std::string& name)
{
    auto loc = glGetUniformLocation(id, name.c_str());
    if(id == -1 || loc == -1)
    {
        CHECK_ERROR();
        //LOG(name + "not defined correctly.");
    }
    return Uniform(id, loc);
}

void Shader::use()
{
    if(autoReload && (vertWatcher->changed() || fragWatcher->changed())) reload();
    glUseProgram(id);
	Renderer::setCurrentShader(this);
}

void Shader::reload()
{
    if(id != -1) glDeleteProgram(id);

    id = glCreateProgram();
    auto vertexShader = glCreateShader(GL_VERTEX_SHADER);
    auto fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    CHECK_ERROR();

    if (compile(vertWatcher->file, vertexShader)) throw vertWatcher->file;
    glAttachShader(id, vertexShader);
    if (compile(fragWatcher->file, fragmentShader)) throw fragWatcher->file;
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
    CHECK_ERROR();
}