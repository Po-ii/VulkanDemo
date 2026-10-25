#include "DepthBuffer.h"
#include "Device.h"
#include "SwapChain.h"
#include "CommandManager.h"
#include "FrameSynchronizer.h"
#include "VertexBuffer.h"
#include "Texture.h"

void DepthBuffer::createDepthResources(Device const& device, SwapChain const& swapChain,VertexBuffer const& vertBuffer, Texture const& texture)
{
    vk::Format depthFormat = findDepthFormat(device);
    std::tie(depthImage, depthImageMemory) = texture.createImage(swapChain.GetSwapChainExtent().width, swapChain.GetSwapChainExtent().height, depthFormat, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal, device, vertBuffer);
    depthImageView = texture.createImageView(depthImage, depthFormat, vk::ImageAspectFlagBits::eDepth, device);
}

vk::Format DepthBuffer::findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features, Device const& device) const
{
    for (const auto format : candidates) {
        vk::FormatProperties props = device.GetPhysicalDevice().getFormatProperties(format);

        if (((tiling == vk::ImageTiling::eLinear) && ((props.linearTilingFeatures & features) == features)) ||
            ((tiling == vk::ImageTiling::eOptimal) && ((props.optimalTilingFeatures & features) == features)))
        {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}

vk::Format DepthBuffer::findDepthFormat(Device const& device) const
{
    return findSupportedFormat({ vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment, device);
}

void DepthBuffer::cleanupDepthResources()
{
    depthImageView = nullptr;
    depthImage = nullptr;
    depthImageMemory = nullptr;
}

