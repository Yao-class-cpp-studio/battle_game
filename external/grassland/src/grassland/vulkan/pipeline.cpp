#include <grassland/file/file.h>
#include <grassland/util/logging.h>
#include <grassland/vulkan/pipeline.h>

namespace grassland::vulkan {

Pipeline::~Pipeline() {
  vkDestroyPipeline(device_->GetHandle(), handle_, nullptr);
}
Pipeline::Pipeline(
    Device *device,
    RenderPass *render_pass,
    PipelineLayout *pipeline_layout,
    const helper::ShaderStages &shader_stages,
    const helper::VertexInputDescriptions &vertex_input_descriptions,
    bool depth_test,
    bool blend_enable)
    : Pipeline(
          device,
          render_pass,
          pipeline_layout,
          shader_stages,
          vertex_input_descriptions,
          std::vector<VkPipelineColorBlendAttachmentState>{
              {blend_enable ? VK_TRUE : VK_FALSE, VK_BLEND_FACTOR_SRC_ALPHA,
               VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_OP_ADD,
               VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
               VK_BLEND_OP_ADD,
               VK_COLOR_COMPONENT_A_BIT | VK_COLOR_COMPONENT_R_BIT |
                   VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT}},
          depth_test) {
}
Pipeline::Pipeline(
    Device *device,
    RenderPass *render_pass,
    PipelineLayout *pipeline_layout,
    const helper::ShaderStages &shader_stages,
    const helper::VertexInputDescriptions &vertex_input_descriptions,
    const std::vector<VkPipelineColorBlendAttachmentState>
        &pipeline_color_blend_attachment_states,
    bool depth_test) {
  device_ = device;
  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount =
      vertex_input_descriptions.GetBindingDescriptions().size();
  if (vertexInputInfo.vertexBindingDescriptionCount) {
    vertexInputInfo.pVertexBindingDescriptions =
        vertex_input_descriptions.GetBindingDescriptions().data();
  }
  vertexInputInfo.vertexAttributeDescriptionCount =
      vertex_input_descriptions.GetAttributeDescription().size();
  if (vertexInputInfo.vertexAttributeDescriptionCount) {
    vertexInputInfo.pVertexAttributeDescriptions =
        vertex_input_descriptions.GetAttributeDescription().data();
  }

  VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
  inputAssembly.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  VkPipelineViewportStateCreateInfo viewportState{};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.scissorCount = 1;

  VkPipelineRasterizationStateCreateInfo rasterizer{};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_NONE;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;

  VkPipelineMultisampleStateCreateInfo multisampling{};
  multisampling.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  VkPipelineColorBlendStateCreateInfo colorBlending{};
  colorBlending.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY;
  colorBlending.attachmentCount = pipeline_color_blend_attachment_states.size();
  colorBlending.pAttachments = pipeline_color_blend_attachment_states.data();
  colorBlending.blendConstants[0] = 0.0f;
  colorBlending.blendConstants[1] = 0.0f;
  colorBlending.blendConstants[2] = 0.0f;
  colorBlending.blendConstants[3] = 0.0f;

  std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT,
                                               VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamicState{};
  dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
  dynamicState.pDynamicStates = dynamicStates.data();

  VkPipelineDepthStencilStateCreateInfo depthStencil{};
  depthStencil.sType =
      VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencil.depthTestEnable = VK_TRUE;
  depthStencil.depthWriteEnable = VK_TRUE;
  depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
  depthStencil.depthBoundsTestEnable = VK_FALSE;
  depthStencil.stencilTestEnable = VK_FALSE;

  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = shader_stages.Size();
  pipelineInfo.pStages = shader_stages.Data();
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDynamicState = &dynamicState;
  pipelineInfo.layout = pipeline_layout->GetHandle();
  pipelineInfo.renderPass = render_pass->GetHandle();
  pipelineInfo.pDepthStencilState = depth_test ? &depthStencil : nullptr;
  pipelineInfo.subpass = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

  if (vkCreateGraphicsPipelines(device_->GetHandle(), VK_NULL_HANDLE, 1,
                                &pipelineInfo, nullptr,
                                &handle_) != VK_SUCCESS) {
    LAND_ERROR("Vulkan failed to create graphics pipeline!");
  }
}
}  // namespace grassland::vulkan
