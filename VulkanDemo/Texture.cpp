#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "Texture.h"
#include "VertexBuffer.h"
#include "Device.h"
#include "GraphicPipeline.h"
#include "CommandManager.h"

void Texture::createTextureImage(Device const& device, GraphicPipeline const& pipeline, VertexBuffer const& vertBuffer, CommandManager const& commands)
{
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load("../textures/texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    vk::DeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels)
    {
        throw std::runtime_error("failed to load texture image!");
    }

    auto [stagingBuffer, stagingBufferMemory] = vertBuffer.createBuffer(imageSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, device, pipeline);
    void* data = stagingBufferMemory.mapMemory(0, imageSize);
    memcpy(data, pixels, imageSize);
    stagingBufferMemory.unmapMemory();
    stbi_image_free(pixels);

    std::tie(textureImage, textureImageMemory) = createImage(texWidth,
        texHeight,
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        device, vertBuffer);

    vk::raii::CommandBuffer commandBuffer = commands.beginSingleTimeCommands(device);
    transitionImageLayout(commandBuffer, textureImage, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
    copyBufferToImage(commandBuffer, stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
    transitionImageLayout(commandBuffer, textureImage, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
    commands.endSingleTimeCommands(std::move(commandBuffer), device);
}

void Texture::createTextureImageViews(Device const& device)
{
    textureImageView = createImageView(*textureImage, vk::Format::eR8G8B8A8Srgb, device);
}

std::pair<vk::raii::Image, vk::raii::DeviceMemory> Texture::createImage(
    uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, Device const& device, VertexBuffer const& vertBuffer)
{
    vk::ImageCreateInfo imageInfo
    { 
        .imageType = vk::ImageType::e2D,
        .format = format,
        .extent = {width, height, 1},
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = vk::SampleCountFlagBits::e1,
        .tiling = tiling,
        .usage = usage,
        .sharingMode = vk::SharingMode::eExclusive 
    };

    vk::raii::Image image = vk::raii::Image(device.GetDevice(), imageInfo);

    vk::MemoryRequirements memRequirements = image.getMemoryRequirements();
    vk::MemoryAllocateInfo allocInfo{ .allocationSize = memRequirements.size,
                                     .memoryTypeIndex = vertBuffer.findMemoryType(memRequirements.memoryTypeBits, properties, device) };
    vk::raii::DeviceMemory imageMemory = vk::raii::DeviceMemory(device.GetDevice(), allocInfo);
    image.bindMemory(imageMemory, 0);

    return { std::move(image), std::move(imageMemory) };
}

void Texture::transitionImageLayout(vk::raii::CommandBuffer& commandBuffer, const vk::raii::Image& image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
    vk::ImageMemoryBarrier barrier
    {
        .oldLayout = oldLayout,
        .newLayout = newLayout,
        .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
        .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
        .image = image,
        .subresourceRange = {.aspectMask = vk::ImageAspectFlagBits::eColor, .levelCount = 1, .layerCount = 1} 
    };

    vk::PipelineStageFlags sourceStage;
    vk::PipelineStageFlags destinationStage;

    if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
    {
        barrier.srcAccessMask = {};
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

        sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        destinationStage = vk::PipelineStageFlagBits::eTransfer;
    }
    else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
    {
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        sourceStage = vk::PipelineStageFlagBits::eTransfer;
        destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
    }
    else
    {
        throw std::invalid_argument("unsupported layout transition!");
    }
    commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, {}, nullptr, barrier);
}

void Texture::copyBufferToImage(vk::raii::CommandBuffer& commandBuffer, const vk::raii::Buffer& buffer, vk::raii::Image& image, uint32_t width, uint32_t height)
{
    vk::BufferImageCopy region
    { 
        .bufferOffset = 0,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = {.aspectMask = vk::ImageAspectFlagBits::eColor, .mipLevel = 0, .baseArrayLayer = 0, .layerCount = 1},
        .imageOffset = {0, 0, 0},
        .imageExtent = {width, height, 1} 
    };
    commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, region);
}

vk::raii::ImageView Texture::createImageView(vk::Image const& image, vk::Format format, Device const& device) const
{
    vk::ImageViewCreateInfo viewInfo
    {
      .image = image,
      .viewType = vk::ImageViewType::e2D,
      .format = format,
      .subresourceRange = {.aspectMask = vk::ImageAspectFlagBits::eColor, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1} 
    };
    return vk::raii::ImageView(device.GetDevice(), viewInfo);
}

void Texture::createTexturSampler(Device const& device)
{
    vk::PhysicalDeviceProperties properties = device.GetPhysicalDevice().getProperties();
    vk::SamplerCreateInfo        samplerInfo{ .magFilter = vk::Filter::eLinear,
                                             .minFilter = vk::Filter::eLinear,
                                             .mipmapMode = vk::SamplerMipmapMode::eLinear,
                                             .addressModeU = vk::SamplerAddressMode::eRepeat,
                                             .addressModeV = vk::SamplerAddressMode::eRepeat,
                                             .addressModeW = vk::SamplerAddressMode::eRepeat,
                                             .mipLodBias = 0.0f,
                                             .anisotropyEnable = vk::True,
                                             .maxAnisotropy = properties.limits.maxSamplerAnisotropy,
                                             .compareEnable = vk::False,
                                             .compareOp = vk::CompareOp::eAlways };
    textureSampler = vk::raii::Sampler(device.GetDevice(), samplerInfo);
}
