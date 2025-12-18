#pragma once
#include <texture.h>
#include <unordered_map>

class Context;

class TextureManager {
public:
    TextureManager(Context& context);

    void Create(Context &context);
    void Destroy();
    Texture* GetTexture(std::string name);
    VkDescriptorSetLayout& DescriptorSetLayout(){return m_descSetLayout;};

private:
    void CreateDsetLayout();
    void LoadTextures();
    Texture *LoadTexture(const std::string &filePath);

    Context *m_context;
    VkDescriptorSetLayout m_descSetLayout;
    std::unordered_map<std::string, Texture*> m_textures;
};
