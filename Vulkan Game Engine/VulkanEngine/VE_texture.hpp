#pragma once
#include <string>
#include <memory>
#include <stb_image.h>


#include "VE_buffer.hpp"

namespace VE {
    class VETexture {
    public:
        struct Builder {
            VkImage textureImage;
            stbi_uc* pixels;
            VkDeviceSize imageSize;
            int texWidth, texHeight, texChannels;
            void loadTexture(const std::string& fpath);
        };

        void createTextureImage(const std::string& fpath, Builder builder);
        void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
        ~VETexture();


        VETexture(VEDevice& device, Builder& builder);
        VkImage textureImage;
        VkDeviceMemory textureImageMemory;
        VkImageView textureImageView;
        VkSampler textureSampler;
        static std::unique_ptr<VETexture> createTextureFromFile(VEDevice& device, const std::string& fpath);
    private:

        VEDevice& veDevice;
        std::unique_ptr<VEBuffer> textureBuffer;
        

        VETexture(const VETexture& texture);
        VETexture& operator=(const VETexture&);

        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer commandBuffer);
        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
        void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
        void createTextureImageView();
        VkImageView createImageView(VkImage image, VkFormat format);
        void createTextureSampler();

    };
}