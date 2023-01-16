#include <GLFW/glfw3.h>
#include <grassland/util/logging.h>
#include <grassland/vulkan/instance.h>
#include <grassland/vulkan/instance_procedures.h>

#include <cstring>
#include <iostream>
#include <vector>

namespace grassland::vulkan {

namespace {

std::vector<const char *> GetRequiredExtensions(bool require_surface,
                                                bool enable_validation_layers) {
  uint32_t glfw_extension_count = 0;
  const char **glfw_extensions = nullptr;
  if (require_surface) {
    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
  }

  std::vector<const char *> extensions;

  for (uint32_t i = 0; i < glfw_extension_count; i++) {
    extensions.push_back(glfw_extensions[i]);
  }

  if (enable_validation_layers) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

  return extensions;
}

bool CheckValidationLayerSupport() {
  uint32_t layer_count;
  vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

  std::vector<VkLayerProperties> available_layers(layer_count);
  vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

  for (const char *layerName : validationLayers) {
    bool layer_found = false;

    for (const auto &layerProperties : available_layers) {
      if (std::strcmp(layerName, layerProperties.layerName) == 0) {
        layer_found = true;
        break;
      }
    }

    if (!layer_found) {
      return false;
    }
  }

  return true;
}

VKAPI_ATTR VkBool32 VKAPI_CALL
DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
              VkDebugUtilsMessageTypeFlagsEXT message_type,
              const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
              void *user_data) {
  if (message_severity <= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
    return VK_FALSE;
  }
  std::string message_tag;
  auto add_tag = [&message_tag](const char *tag) {
    if (!message_tag.empty()) {
      message_tag += ", ";
    }
    message_tag += tag;
  };
  if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
    add_tag("ERROR");
  }
  if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    add_tag("WARNING");
  }
  if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
    add_tag("INFO");
  }
  if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
    add_tag("VERBOSE");
  }
  std::cerr << fmt::format("validation layer ({}): {}", message_tag,
                           callback_data->pMessage)
            << std::endl;
  return VK_FALSE;
}

void PopulateDebugMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT &create_info) {
  create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  create_info.messageSeverity =
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  create_info.pfnUserCallback = DebugCallback;
}

}  // namespace

Instance::Instance(bool require_surface, bool enable_validation_layers) {
  require_surface_ = require_surface;
  enable_validation_layers_ = enable_validation_layers;
  CreateInstance();
  InstanceProcedures::GetStaticInstance()->SetInstance(handle_);
  CreateDebugMessenger();
}

Instance::~Instance() {
  if (enable_validation_layers_) {
    vkDestroyDebugUtilsMessengerEXT(handle_, debug_messenger_, nullptr);
  }
  vkDestroyInstance(handle_, nullptr);
}

void Instance::CreateInstance() {
  if (enable_validation_layers_ && !CheckValidationLayerSupport()) {
    LAND_ERROR("[Vulkan] validation layer is required, but not supported.");
  }

  VkApplicationInfo app_info{};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "Grassland";
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.pEngineName = "No Engine";
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.apiVersion = VK_API_VERSION_1_2;

  VkInstanceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &app_info;

  auto extensions =
      GetRequiredExtensions(require_surface_, enable_validation_layers_);
#ifdef __APPLE__
  extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif

  spdlog::info("Instance extensions:");
  for (auto extension : extensions) {
    spdlog::info("- {}", extension);
  }
  spdlog::info("");

  create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  create_info.ppEnabledExtensionNames = extensions.data();

  VkDebugUtilsMessengerCreateInfoEXT debug_create_info{};
  if (enable_validation_layers_) {
    create_info.enabledLayerCount =
        static_cast<uint32_t>(validationLayers.size());
    create_info.ppEnabledLayerNames = validationLayers.data();

    PopulateDebugMessengerCreateInfo(debug_create_info);
    create_info.pNext =
        (VkDebugUtilsMessengerCreateInfoEXT *)&debug_create_info;
  } else {
    create_info.enabledLayerCount = 0;

    create_info.pNext = nullptr;
  }

#ifdef __APPLE__
  create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

  if (vkCreateInstance(&create_info, nullptr, &handle_) != VK_SUCCESS) {
    LAND_ERROR("[Vulkan] failed to create instance!");
  }
}

void Instance::CreateDebugMessenger() {
  if (enable_validation_layers_) {
    VkDebugUtilsMessengerCreateInfoEXT create_info;
    PopulateDebugMessengerCreateInfo(create_info);

    if (vkCreateDebugUtilsMessengerEXT(handle_, &create_info, nullptr,
                                       &debug_messenger_) != VK_SUCCESS) {
      LAND_ERROR("[Vulkan] failed to set up debug messenger!");
    }
  }
}

}  // namespace grassland::vulkan
