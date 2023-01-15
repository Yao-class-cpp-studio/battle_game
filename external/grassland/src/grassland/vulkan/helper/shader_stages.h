#pragma once
#include <grassland/vulkan/shader_module.h>
#include <grassland/vulkan/util.h>

namespace grassland::vulkan::helper {
class ShaderStages {
 public:
  void AddShaderModule(const ShaderModule *shader_module,
                       VkShaderStageFlagBits shader_stage_flag_bits);
  [[nodiscard]] uint32_t Size() const;
  [[nodiscard]] const VkPipelineShaderStageCreateInfo *Data() const;

 private:
  std::vector<VkPipelineShaderStageCreateInfo> shader_stage_create_infos_{};
};
}  // namespace grassland::vulkan::helper
