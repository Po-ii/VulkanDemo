#pragma once
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "vulkan/vulkan_raii.hpp"

class Device;
class SwapChain;
class CommandManager;
class FrameSynchronizer;
class VertexBuffer;
class Texture;

class DepthBuffer
{
public:
    void createDepthResources(Device const& device, SwapChain const& swapChain, VertexBuffer const& vertBuffer, Texture const& texture);
    vk::Format findDepthFormat(Device const& device) const;

    vk::raii::Image const& GetDepthImage() const { return depthImage; }
    vk::raii::ImageView const& GetDepthImageView() const { return depthImageView; }

private:
    vk::raii::Image depthImage = nullptr;
    vk::raii::DeviceMemory depthImageMemory = nullptr;
    vk::raii::ImageView depthImageView = nullptr;

    vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features, Device const& device) const;
};