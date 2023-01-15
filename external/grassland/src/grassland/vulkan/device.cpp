#include <grassland/util/logging.h>
#include <grassland/vulkan/device.h>
#include <grassland/vulkan/device_procedures.h>
#include <grassland/vulkan/queue.h>

#include <set>

namespace grassland::vulkan {

Device::Device(PhysicalDevice *physical_device,
               const std::vector<const char *> &extra_device_extensions,
               bool enable_validation_layers)
    : Device(physical_device,
             nullptr,
             extra_device_extensions,
             enable_validation_layers,
             nullptr) {
}

Device::Device(PhysicalDevice *physical_device,
               Surface *surface,
               const std::vector<const char *> &extra_device_extensions,
               bool enable_validation_layers,
               void *extraDeviceFeatures)
    : handle_{} {
  physical_device_ = physical_device;
  surface_ = surface;
  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = {
      physical_device->GraphicsFamilyIndex()};

  std::vector<const char *> device_extensions;
  if (surface) {
    uniqueQueueFamilies.insert(physical_device->PresentFamilyIndex(surface));
    device_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
  }
#ifdef __APPLE__
  device_extensions.push_back("VK_KHR_portability_subset");
#endif
  device_extensions.push_back(VK_KHR_MAINTENANCE3_EXTENSION_NAME);
  device_extensions.push_back(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);
  if (!extra_device_extensions.empty()) {
    device_extensions.insert(device_extensions.end(),
                             extra_device_extensions.begin(),
                             extra_device_extensions.end());
  }

  spdlog::info("Device extensions:");
  for (auto extension : device_extensions) {
    spdlog::info("- {}", extension);
  }
  spdlog::info("");

  float queuePriority = 1.0f;
  for (uint32_t queueFamily : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(queueCreateInfo);
  }
  VkPhysicalDeviceFeatures deviceFeatures = physical_device_->GetFeatures();
  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.queueCreateInfoCount =
      static_cast<uint32_t>(queueCreateInfos.size());
  createInfo.pQueueCreateInfos = queueCreateInfos.data();
  createInfo.pEnabledFeatures = &deviceFeatures;
  createInfo.enabledExtensionCount =
      static_cast<uint32_t>(device_extensions.size());
  if (createInfo.enabledExtensionCount) {
    createInfo.ppEnabledExtensionNames = device_extensions.data();
  }

  if (enable_validation_layers) {
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
  } else {
    createInfo.enabledLayerCount = 0;
  }

  VkPhysicalDeviceDescriptorIndexingFeaturesEXT
      physicalDeviceDescriptorIndexingFeatures{};
  physicalDeviceDescriptorIndexingFeatures.sType =
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
  physicalDeviceDescriptorIndexingFeatures
      .shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
  physicalDeviceDescriptorIndexingFeatures.runtimeDescriptorArray = VK_TRUE;
  physicalDeviceDescriptorIndexingFeatures
      .descriptorBindingVariableDescriptorCount = VK_TRUE;

  physicalDeviceDescriptorIndexingFeatures.pNext = extraDeviceFeatures;
  createInfo.pNext = &physicalDeviceDescriptorIndexingFeatures;

  if (vkCreateDevice(physical_device->GetHandle(), &createInfo, nullptr,
                     &handle_) != VK_SUCCESS) {
    LAND_ERROR("[Vulkan] failed to create logical device!");
  }

  graphics_queue_ =
      std::make_unique<Queue>(this, physical_device_->GraphicsFamilyIndex());

  if (surface_) {
    present_queue_ = std::make_unique<Queue>(
        this, physical_device_->PresentFamilyIndex(surface_));
  }

  DeviceProcedures::GetStaticInstance()->SetDevice(this);
}

Device::~Device() {
  graphics_queue_.reset();
  vkDestroyDevice(handle_, nullptr);
}

PhysicalDevice *Device::GetPhysicalDevice() const {
  return physical_device_;
}

Surface *Device::GetSurface() const {
  return surface_;
}

Queue *Device::GetGraphicsQueue() const {
  return graphics_queue_.get();
}

void Device::WaitIdle() {
  vkDeviceWaitIdle(handle_);
}
Queue *Device::GetPresentQueue() const {
  return present_queue_.get();
}

}  // namespace grassland::vulkan
