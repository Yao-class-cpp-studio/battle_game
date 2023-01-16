#include <grassland/file/file.h>
#include <grassland/util/logging.h>
#include <grassland/vulkan/shader_module.h>

namespace grassland::vulkan {

ShaderModule::ShaderModule(Device *device, const char *spv_file_path)
    : ShaderModule(device, file::ReadFileBinary(spv_file_path)) {
}

ShaderModule::ShaderModule(Device *device, const std::vector<uint8_t> &spv_data)
    : handle_{} {
  device_ = device;
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = spv_data.size();
  createInfo.pCode = reinterpret_cast<const uint32_t *>(spv_data.data());
  if (vkCreateShaderModule(device_->GetHandle(), &createInfo, nullptr,
                           &handle_) != VK_SUCCESS) {
    LAND_ERROR("Vulkan failed to create shader module!");
  }
}

ShaderModule::~ShaderModule() {
  vkDestroyShaderModule(device_->GetHandle(), GetHandle(), nullptr);
}

}  // namespace grassland::vulkan
