#ifndef VK_PHYSICAL_DEVICE_WRAPPER_HPP
#define VK_PHYSICAL_DEVICE_WRAPPER_HPP

#include <vulkan/vulkan.h>
#include <map>
#include <vector>
#include <stdexcept>

class VkPhysicalDeviceWrapper
{
private:
    int rateDeviceSuitability(VkPhysicalDevice device);

    VkPhysicalDevice physicalDevice;

public:
    VkPhysicalDeviceWrapper(VkInstance instance);
    ~VkPhysicalDeviceWrapper();

    VkPhysicalDevice getPhysicalDevice();
};

#endif
