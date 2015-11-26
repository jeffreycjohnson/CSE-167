#include "Material.h"

Material::UniformSetter::UniformSetter(Material* mat, const std::string& name) : name(name), mat(mat)
{
}

template<>
void Material::UniformSetter::operator=<Texture*>(Texture* value) {
    mat->textures[name] = value;
}

template<>
void Material::UniformSetter::operator=<const Texture*>(const Texture* value) {
    mat->textures[name] = value;
}

Material::Material(Shader* shader) : shader(shader)
{
}

Material::UniformSetter Material::operator[](const std::string& name)
{
    return UniformSetter(this, name);
}

void Material::bind()
{
    shader->use();
    for (auto uniform : uniforms)
    {
        uniform.second->set((*shader)[uniform.first]);
    }
    unsigned int i = 0;
    for (auto texture : textures)
    {
        texture.second->bindTexture(i);
        (*shader)[texture.first] = i;
        i++;
    }
}