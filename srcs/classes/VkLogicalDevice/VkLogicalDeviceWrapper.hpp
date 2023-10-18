#ifndef VK_LOGICAL_DEVICE_WRAPPER_HPP
#define VK_LOGICAL_DEVICE_WRAPPER_HPP

#include <vulkan/vulkan.h>
#include <optional>
#include <vector>
#include <stdexcept>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;

    bool isComplete() {
        return graphicsFamily.has_value();
    }
};

class VkLogicalDeviceWrapper
{
    private:
        VkDevice logicalDevice;

    public:
        VkLogicalDeviceWrapper(VkPhysicalDevice physicalDevice, VkQueue graphicsQueue);
        ~VkLogicalDeviceWrapper();

        VkDevice getLogicalDevice();
};

#endif