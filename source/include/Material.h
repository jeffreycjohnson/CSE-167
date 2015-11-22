#ifndef INCLUDE_MATERIAL_H
#define INCLUDE_MATERIAL_H

#include "ForwardDecs.h"
#include "Component.h"
#include "Texture.h"
#include "Shader.h"
#include <map>

class Material : public Component
{
private:
    class UniformType
    {
    public:
        virtual void set(Shader::Uniform) = 0;
        virtual ~UniformType() = default;
    };
    template<typename T>
    class UniformTypeInner : public UniformType
    {
    public:
        T value;
        explicit UniformTypeInner(T value) : value(value) {}

        void set(Shader::Uniform u) override
        {
            u = value;
        };
    };
    std::map<std::string, UniformType*> uniforms;
    std::map<std::string, const Texture*> textures;

    class UniformSetter
    {
    public:
        const std::string& name;
        Material* mat;

        UniformSetter(Material*, const std::string&);
        template<typename T>
        void operator=(T value) {
            mat->uniforms[name] = new UniformTypeInner<T>(value);
        }
    };

public:
    Shader * shader;

    explicit Material(Shader *);
    UniformSetter operator[](const std::string& name);
    void bind();
};

template<>
void Material::UniformSetter::operator=<Texture*>(Texture* value);
template<>
void Material::UniformSetter::operator=<const Texture*>(const Texture* value);

#endif