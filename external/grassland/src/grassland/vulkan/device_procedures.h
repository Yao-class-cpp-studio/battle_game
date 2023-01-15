#pragma once
#include "grassland/vulkan/device.h"

namespace grassland::vulkan {
class DeviceProcedures {
 public:
  explicit DeviceProcedures();
  static DeviceProcedures *GetStaticInstance();
  void SetDevice(Device *device);
  VkDeviceAddress vkGetBufferDeviceAddressKHR(
      VkDevice device,
      const VkBufferDeviceAddressInfo *pInfo);
  void vkGetAccelerationStructureBuildSizesKHR(
      VkDevice device,
      VkAccelerationStructureBuildTypeKHR buildType,
      const VkAccelerationStructureBuildGeometryInfoKHR *pBuildInfo,
      const uint32_t *pMaxPrimitiveCounts,
      VkAccelerationStructureBuildSizesInfoKHR *pSizeInfo);
  VkResult vkCreateAccelerationStructureKHR(
      VkDevice device,
      const VkAccelerationStructureCreateInfoKHR *pCreateInfo,
      const VkAllocationCallbacks *pAllocator,
      VkAccelerationStructureKHR *pAccelerationStructure);
  void vkCmdBuildAccelerationStructuresKHR(
      VkCommandBuffer commandBuffer,
      uint32_t infoCount,
      const VkAccelerationStructureBuildGeometryInfoKHR *pInfos,
      const VkAccelerationStructureBuildRangeInfoKHR *const *ppBuildRangeInfos);
  void vkDestroyAccelerationStructureKHR(
      VkDevice device,
      VkAccelerationStructureKHR accelerationStructure,
      const VkAllocationCallbacks *pAllocator);
  VkDeviceAddress vkGetAccelerationStructureDeviceAddressKHR(
      VkDevice device,
      const VkAccelerationStructureDeviceAddressInfoKHR *pInfo);
  VkResult vkCreateRayTracingPipelinesKHR(
      VkDevice device,
      VkDeferredOperationKHR deferredOperation,
      VkPipelineCache pipelineCache,
      uint32_t createInfoCount,
      const VkRayTracingPipelineCreateInfoKHR *pCreateInfos,
      const VkAllocationCallbacks *pAllocator,
      VkPipeline *pPipelines);
  VkResult vkGetRayTracingShaderGroupHandlesKHR(VkDevice device,
                                                VkPipeline pipeline,
                                                uint32_t firstGroup,
                                                uint32_t groupCount,
                                                size_t dataSize,
                                                void *pData);
  void vkCmdTraceRaysKHR(
      VkCommandBuffer commandBuffer,
      const VkStridedDeviceAddressRegionKHR *pRaygenShaderBindingTable,
      const VkStridedDeviceAddressRegionKHR *pMissShaderBindingTable,
      const VkStridedDeviceAddressRegionKHR *pHitShaderBindingTable,
      const VkStridedDeviceAddressRegionKHR *pCallableShaderBindingTable,
      uint32_t width,
      uint32_t height,
      uint32_t depth);

 private:
  GRASSLAND_VULKAN_DEVICE_PTR;
  GRASSLAND_VULKAN_PROCEDURE_VAR(vkCmdBuildAccelerationStructuresKHR);
  GRASSLAND_VULKAN_PROCEDURE_VAR(vkCreateAccelerationStructureKHR);
  GRASSLAND_VULKAN_PROCEDURE_VAR(vkGetAccelerationStructureBuildSizesKHR);
  GRASSLAND_VULKAN_PROCEDURE_VAR(vkGetBufferDeviceAddressKHR);
  GRASSLAND_VULKAN_PROCEDURE_VAR(vkDestroyAccelerationStructureKHR);
  GRASSLAND_VULKAN_PROCEDURE_VAR(vkGetAccelerationStructureDeviceAddressKHR);
  GRASSLAND_VULKAN_PROCEDURE_VAR(vkCreateRayTracingPipelinesKHR);
  GRASSLAND_VULKAN_PROCEDURE_VAR(vkGetRayTracingShaderGroupHandlesKHR);
  GRASSLAND_VULKAN_PROCEDURE_VAR(vkCmdTraceRaysKHR);
};
}  // namespace grassland::vulkan
