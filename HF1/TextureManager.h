#pragma once
#include <texture.h>
#include <unordered_map>

class Context;

class TextureManager {
public:
    TextureManager(){m_textures = new std::unordered_map<std::string, Texture*>();};

    void Create(Context &context);
    void Destroy();
    Texture* GetTexture(std::string name);

private:
    Texture *LoadTexture(const std::string &filePath);

    const char* TEXTURE_DIRECTORY = "./HF1/textures/";
    Context *m_context;
    std::unordered_map<std::string, Texture*> *m_textures;
};
