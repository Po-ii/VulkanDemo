#pragma once
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "vulkan/vulkan_raii.hpp"

class Device;
class GraphicPipeline;
class VertexBuffer;
class CommandManager;

class Texture
{
public:
	void createTextureImage(Device const& device, GraphicPipeline const& pipeline, VertexBuffer const& vertBuffer, CommandManager const& commands);
	void createTextureImageViews(Device const& device);
	vk::raii::ImageView createImageView(vk::Image const& image, vk::Format format, vk::ImageAspectFlags aspectFlags, Device const& device) const;
	void createTexturSampler(Device const& device);
	std::pair<vk::raii::Image, vk::raii::DeviceMemory> createImage(
		uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, Device const& device, VertexBuffer const& vertBuffer) const;

	vk::raii::ImageView const& GetTextureImageView() const { return textureImageView; }
	vk::raii::Sampler const& GetTextureSampler() const { return textureSampler; }

private:
	vk::raii::Image textureImage = nullptr;
	vk::raii::DeviceMemory textureImageMemory = nullptr;
	vk::raii::ImageView textureImageView = nullptr;
	vk::raii::Sampler textureSampler = nullptr;

	void transitionImageLayout(vk::raii::CommandBuffer& commandBuffer, const vk::raii::Image& image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
	void copyBufferToImage(vk::raii::CommandBuffer& commandBuffer, const vk::raii::Buffer& buffer, vk::raii::Image& image, uint32_t width, uint32_t height);
};

