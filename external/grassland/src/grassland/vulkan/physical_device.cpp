#include <grassland/util/logging.h>
#include <grassland/vulkan/physical_device.h>
#include <grassland/vulkan/surface.h>

#include <set>
#include <utility>

namespace grassland::vulkan {

namespace {
SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device,
                                              VkSurfaceKHR surface) {
  SwapChainSupportDetails details;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
                                            &details.capabilities);
  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

  if (formatCount != 0) {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
                                         details.formats.data());
  }
  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount,
                                            nullptr);

  if (presentModeCount != 0) {
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, surface, &presentModeCount, details.presentModes.data());
  }
  return details;
}
}  // namespace

PhysicalDevice::PhysicalDevice() : PhysicalDevice(nullptr) {
}

PhysicalDevice::PhysicalDevice(VkPhysicalDevice handle) {
  handle_ = handle;
  if (handle_) {
    properties_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    properties_.pNext = &ray_tracing_properties_;
    ray_tracing_properties_.sType =
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
    vkGetPhysicalDeviceProperties2(handle_, &properties_);
    features_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features_.pNext = &ray_tracing_features_;
    ray_tracing_features_.sType =
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
    vkGetPhysicalDeviceFeatures2(handle_, &features_);
    vkGetPhysicalDeviceMemoryProperties(handle_, &memory_properties_);
  }
}

std::string PhysicalDevice::DeviceName() const {
  return properties_.properties.deviceName;
}

uint32_t PhysicalDevice::GraphicsFamilyIndex() const {
  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(handle_, &queue_family_count,
                                           nullptr);

  std::vector<VkQueueFamilyProperties> queue_family_properties(
      queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(handle_, &queue_family_count,
                                           queue_family_properties.data());

  int i = 0;
  for (const auto &queue_family_property : queue_family_properties) {
    if (queue_family_property.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      return i;
    }
    i++;
  }
  return -1;
}

uint32_t PhysicalDevice::PresentFamilyIndex(Surface *surface) const {
  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(handle_, &queue_family_count,
                                           nullptr);

  std::vector<VkQueueFamilyProperties> queue_family_properties(
      queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(handle_, &queue_family_count,
                                           queue_family_properties.data());
  int i = 0;
  for (const auto &queue_family_property : queue_family_properties) {
    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(handle_, i, surface->GetHandle(),
                                         &presentSupport);
    if (presentSupport) {
      return i;
    }
    i++;
  }
  return -1;
}

bool PhysicalDevice::HasPresentationSupport() const {
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties(handle_, nullptr, &extensionCount,
                                       nullptr);

  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(handle_, nullptr, &extensionCount,
                                       availableExtensions.data());

  std::set<std::string> requiredExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  for (const auto &extension : availableExtensions) {
    requiredExtensions.erase(extension.extensionName);
  }

  return requiredExtensions.empty();
}

void PhysicalDevice::PrintDeviceProperties() const {
  spdlog::info("{}", properties_.properties.deviceName);
  spdlog::info("- Vendor ID  : {:#X}", properties_.properties.vendorID);
  spdlog::info("- Device Type: {}", [](VkPhysicalDeviceType device_type) {
    if (device_type == VK_PHYSICAL_DEVICE_TYPE_CPU)
      return "CPU";
    else if (device_type == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
      return "Discrete";
    else if (device_type == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
      return "Integrated";
    else if (device_type == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU)
      return "Virtual";
    else if (device_type == VK_PHYSICAL_DEVICE_TYPE_OTHER)
      return "Other";
    return "Unknown";
  }(properties_.properties.deviceType));
  uint64_t memory_size = 0;
  for (int i = 0; i < memory_properties_.memoryHeapCount; i++) {
    memory_size += (memory_properties_.memoryHeaps[i].flags &
                    VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
                       ? uint64_t(memory_properties_.memoryHeaps[i].size)
                       : 0ull;
  }
  spdlog::info("- Memory Size: {:#.03}GB",
               double(memory_size) / 1024.0 / 1024.0 / 1024.0);
  spdlog::info("- Ray Tracing Support: {}",
               ray_tracing_features_.rayTracingPipeline ? "YES" : "NO");
  spdlog::info("");
}

void PhysicalDevice::PrintDeviceFeatures() const {
  spdlog::info("Geometry shader: {}",
               features_.features.geometryShader ? "True" : "False");
}

bool PhysicalDevice::IsDiscreteGPU() const {
  return properties_.properties.deviceType ==
         VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
}

bool PhysicalDevice::HasGeometryShader() const {
  return features_.features.geometryShader;
}

VkPhysicalDeviceFeatures PhysicalDevice::GetFeatures() const {
  return features_.features;
}

VkPhysicalDeviceProperties PhysicalDevice::GetProperties() const {
  return properties_.properties;
}

bool PhysicalDevice::SwapChainAdequate(Surface *surface) const {
  auto swap_chain_support = GetSwapChainSupportDetails(surface);
  return !swap_chain_support.formats.empty() &&
         !swap_chain_support.presentModes.empty();
}

SwapChainSupportDetails PhysicalDevice::GetSwapChainSupportDetails(
    Surface *surface) const {
  return QuerySwapChainSupport(GetHandle(), surface->GetHandle());
}

std::vector<VkExtensionProperties> PhysicalDevice::GetExtensions() const {
  uint32_t count = 0;
  vkEnumerateDeviceExtensionProperties(handle_, nullptr, &count, nullptr);
  std::vector<VkExtensionProperties> extensions(count);
  vkEnumerateDeviceExtensionProperties(handle_, nullptr, &count,
                                       extensions.data());
  for (auto &extension : extensions) {
    spdlog::info("- {} ({}.{}.{})", extension.extensionName,
                 VK_VERSION_MAJOR(extension.specVersion),
                 VK_VERSION_MINOR(extension.specVersion),
                 VK_VERSION_PATCH(extension.specVersion));
  }
  return extensions;
}

bool PhysicalDevice::HasRayTracingPipeline() const {
  return ray_tracing_features_.rayTracingPipeline;
}

uint64_t PhysicalDevice::DeviceMemorySize() const {
  uint64_t memory_size = 0;
  for (int i = 0; i < memory_properties_.memoryHeapCount; i++) {
    memory_size += (memory_properties_.memoryHeaps[i].flags &
                    VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
                       ? uint64_t(memory_properties_.memoryHeaps[i].size)
                       : 0ull;
  }
  return memory_size;
}

VkPhysicalDeviceRayTracingPipelinePropertiesKHR
PhysicalDevice::GetRayTracingProperties() const {
  return ray_tracing_properties_;
}

std::vector<PhysicalDevice> GetPhysicalDevices(Instance *instance) {
  uint32_t device_count = 0;
  vkEnumeratePhysicalDevices(instance->GetHandle(), &device_count, nullptr);
  std::vector<VkPhysicalDevice> vk_physical_devices(device_count);
  vkEnumeratePhysicalDevices(instance->GetHandle(), &device_count,
                             vk_physical_devices.data());
  std::vector<PhysicalDevice> physical_devices;

  for (auto &vk_physical_device : vk_physical_devices) {
    physical_devices.emplace_back(vk_physical_device);
  }

  return physical_devices;
}

PhysicalDevice PickPhysicalDevice(
    const std::vector<PhysicalDevice> &device_list,
    const std::function<int(PhysicalDevice)> &rate_function) {
  PhysicalDevice result;
  int res_score = -1;
  if (!device_list.empty()) {
    for (const auto &physical_device : device_list) {
      int score = rate_function(physical_device);
      if (score > res_score) {
        res_score = score;
        result = physical_device;
      }
    }
  }
  return result;
}

PhysicalDevice PickPhysicalDevice(
    Instance *instance,
    const std::function<int(PhysicalDevice)> &rate_function) {
  return PickPhysicalDevice(GetPhysicalDevices(instance), rate_function);
}

}  // namespace grassland::vulkan
