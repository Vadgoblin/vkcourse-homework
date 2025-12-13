#include "TextureManager.h"
#include <context.h>
#include <filesystem>
#include <string>


namespace fs = std::filesystem;

void TextureManager::Create(Context& context)
{
    this->m_context = &context;

    printf("Loading textures from : %s \n", this->TEXTURE_DIRECTORY);

    if (!fs::exists(this->TEXTURE_DIRECTORY) || !fs::is_directory(this->TEXTURE_DIRECTORY)) {
        printf("Directory not found: %s \n", this->TEXTURE_DIRECTORY);
        exit(-1);
    }

    for (const auto& entry : fs::directory_iterator(this->TEXTURE_DIRECTORY)) {
        if (entry.is_regular_file()) {
            std::string extension = entry.path().extension().string();

            if (extension == ".jpg" || extension == ".png") {
                std::string filePath = entry.path().string();
                std::string nameOnly = entry.path().stem().string();

                printf("Loading texture : %s \n", filePath.c_str());
                Texture* texture = LoadTexture(filePath);

                m_textures->insert({nameOnly, texture});
            }
        }
    }
}

void TextureManager::Destroy()
{
    for (auto it = m_textures->begin(); it != m_textures->end(); it++) {
        it->second->Destroy(m_context->device());
    }
}

Texture* TextureManager::GetTexture(std::string name)
{
    auto it = m_textures->find(name);
    if (it == m_textures->end()) {
        printf("Texture not found: %s \n", name.c_str());
        exit(-1);
    }
    return it->second;
}

Texture* TextureManager::LoadTexture(const std::string& filePath)
{
    Texture *texture = Texture::LoadFromFile(m_context->physicalDevice(), m_context->device(), m_context->queue(), m_context->commandPool(), filePath, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
    if (texture == nullptr) {
        printf("[ERROR] Was unable to create texture %s\n", filePath.c_str());
        exit(-1);
    }

    return texture;
}