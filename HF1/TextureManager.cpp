#include "TextureManager.h"
#include <context.h>
#include <filesystem>
#include <string>


namespace fs = std::filesystem;

const char* TEXTURE_DIRECTORY = "./HF1/textures/";

void TextureManager::CreateDsetLayout()
{
    auto descSetLayoutBinding = VkDescriptorSetLayoutBinding{
        .binding            = 0,
        .descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount    = 1,
        .stageFlags         = VK_SHADER_STAGE_ALL,
        .pImmutableSamplers = nullptr,
    };

    m_descSetLayout = m_context->descriptorPool().CreateLayout({descSetLayoutBinding});
}
void TextureManager::LoadTextures()
{
    printf("Loading textures from : %s \n", TEXTURE_DIRECTORY);

    if (!fs::exists(TEXTURE_DIRECTORY) || !fs::is_directory(TEXTURE_DIRECTORY)) {
        printf("Directory not found: %s \n", TEXTURE_DIRECTORY);
        exit(-1);
    }

    for (const auto& entry : fs::directory_iterator(TEXTURE_DIRECTORY)) {
        if (entry.is_regular_file()) {
            std::string extension = entry.path().extension().string();

            if (extension == ".jpg" || extension == ".png") {
                std::string filePath = entry.path().string();
                std::string nameOnly = entry.path().stem().string();

                printf("Loading texture : %s \n", filePath.c_str());
                Texture* texture = LoadTexture(filePath);

                m_textures.insert({nameOnly, texture});
            }
        }
    }
}

TextureManager::TextureManager(Context& context)
{
    m_textures = std::unordered_map<std::string, Texture*>();
    m_context = &context;

    CreateDsetLayout();
    LoadTextures();
}

void TextureManager::Destroy()
{
    for (auto it = m_textures.begin(); it != m_textures.end(); it++) {
        it->second->Destroy(m_context->device());
    }
}

Texture* TextureManager::GetTexture(std::string name)
{
    auto it = m_textures.find(name);
    if (it == m_textures.end()) {
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