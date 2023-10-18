#include "VkPhysicalDeviceWrapper.hpp"

VkPhysicalDeviceWrapper::VkPhysicalDeviceWrapper(VkInstance instance)
{
    physicalDevice = VK_NULL_HANDLE;
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0)
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    std::multimap<int, VkPhysicalDevice> candidatesDevices;
    for (const auto& device : devices)
    {
        int score = rateDeviceSuitability(device);
        candidatesDevices.insert(std::make_pair(score, device));
    }

    if (candidatesDevices.rbegin()->first > 0) {
        physicalDevice = candidatesDevices.rbegin()->second;
    } else {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

VkPhysicalDeviceWrapper::~VkPhysicalDeviceWrapper()
{
}

int VkPhysicalDeviceWrapper::rateDeviceSuitability(VkPhysicalDevice device)
{
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    if (!deviceFeatures.geometryShader)
        return (0);

    int score = 0;
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        score += 1000;
    
    score += deviceProperties.limits.maxImageDimension2D;
    return (score);
}

VkPhysicalDevice VkPhysicalDeviceWrapper::getPhysicalDevice()
{
    return (physicalDevice);
}