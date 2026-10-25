#include "UniformBuffer.h"
#include "Device.h"
#include "SwapChain.h"
#include "GraphicPipeline.h"
#include "VertexBuffer.h"
#include "FrameSynchronizer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

void UniformBuffer::createUniformBuffer(Device const& device, GraphicPipeline const& pipeline,FrameSynchronizer const& sync, VertexBuffer const& vertBuffer)
{
    for (size_t i = 0; i < sync.GetMaxFramesInFlight(); i++)
    {
        vk::DeviceSize bufferSize = sizeof(UniformBufferObject);
        auto [buffer, bufferMem] = vertBuffer.createBuffer(
            bufferSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, device, pipeline);
        uniformBuffers.emplace_back(std::move(buffer));
        uniformBuffersMemory.emplace_back(std::move(bufferMem));
        uniformBuffersMapped.emplace_back(uniformBuffersMemory.back().mapMemory(0, bufferSize));
    }
}

void UniformBuffer::updateUniformBuffer(uint32_t currentImage, SwapChain const& swapChain)
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.model = rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = 
        glm::perspective(glm::radians(45.0f), static_cast<float>(swapChain.GetSwapChainExtent().width) / static_cast<float>(swapChain.GetSwapChainExtent().height), 0.1f, 10.0f);
    memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}
