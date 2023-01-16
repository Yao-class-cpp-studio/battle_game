#include <grassland/vulkan/helper/shader_stages.h>

namespace grassland::vulkan::helper {

uint32_t ShaderStages::Size() const {
  return static_cast<uint32_t>(shader_stage_create_infos_.size());
}

const VkPipelineShaderStageCreateInfo *ShaderStages::Data() const {
  return shader_stage_create_infos_.data();
}

void ShaderStages::AddShaderModule(
    const ShaderModule *shader_module,
    VkShaderStageFlagBits shader_stage_flag_bits) {
  VkPipelineShaderStageCreateInfo stage_info{};
  stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  stage_info.stage = shader_stage_flag_bits;
  stage_info.module = shader_module->GetHandle();
  stage_info.pName = "main";
  shader_stage_create_infos_.push_back(stage_info);
}

}  // namespace grassland::vulkan::helper
