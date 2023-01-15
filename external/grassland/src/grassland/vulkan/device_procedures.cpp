#include "device_procedures.h"

namespace grassland::vulkan {

namespace {
template <class FuncTy>
FuncTy GetProcedure(VkDevice device, const char *function_name) {
  auto func =
      reinterpret_cast<FuncTy>(vkGetDeviceProcAddr(device, function_name));
  return func;
};
}  // namespace

#define GET_PROCEDURE(device, function_name)                               \
  function_name##_ = grassland::vulkan::GetProcedure<PFN_##function_name>( \
      device->GetHandle(), #function_name)

DeviceProcedures::DeviceProcedures() : device_(nullptr) {
}

DeviceProcedures *DeviceProcedures::GetStaticInstance() {
  static DeviceProcedures device_procedures;
  return &device_procedures;
}

VkDeviceAddress DeviceProcedures::vkGetBufferDeviceAddressKHR(
    VkDevice device,
    const VkBufferDeviceAddressInfo *pInfo) {
  return vkGetBufferDeviceAddressKHR_(device, pInfo);
}

void DeviceProcedures::vkGetAccelerationStructureBuildSizesKHR(
    VkDevice device,
    VkAccelerationStructureBuildTypeKHR buildType,
    const VkAccelerationStructureBuildGeometryInfoKHR *pBuildInfo,
    const uint32_t *pMaxPrimitiveCounts,
    VkAccelerationStructureBuildSizesInfoKHR *pSizeInfo) {
  return vkGetAccelerationStructureBuildSizesKHR_(
      device, buildType, pBuildInfo, pMaxPrimitiveCounts, pSizeInfo);
}

VkResult DeviceProcedures::vkCreateAccelerationStructureKHR(
    VkDevice device,
    const VkAccelerationStructureCreateInfoKHR *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkAccelerationStructureKHR *pAccelerationStructure) {
  return vkCreateAccelerationStructureKHR_(device, pCreateInfo, pAllocator,
                                           pAccelerationStructure);
}

void DeviceProcedures::vkCmdBuildAccelerationStructuresKHR(
    VkCommandBuffer commandBuffer,
    uint32_t infoCount,
    const VkAccelerationStructureBuildGeometryInfoKHR *pInfos,
    const VkAccelerationStructureBuildRangeInfoKHR *const *ppBuildRangeInfos) {
  return vkCmdBuildAccelerationStructuresKHR_(commandBuffer, infoCount, pInfos,
                                              ppBuildRangeInfos);
}

void DeviceProcedures::vkDestroyAccelerationStructureKHR(
    VkDevice device,
    VkAccelerationStructureKHR accelerationStructure,
    const VkAllocationCallbacks *pAllocator) {
  return vkDestroyAccelerationStructureKHR_(device, accelerationStructure,
                                            pAllocator);
}

VkDeviceAddress DeviceProcedures::vkGetAccelerationStructureDeviceAddressKHR(
    VkDevice device,
    const VkAccelerationStructureDeviceAddressInfoKHR *pInfo) {
  return vkGetAccelerationStructureDeviceAddressKHR_(device, pInfo);
}

VkResult DeviceProcedures::vkCreateRayTracingPipelinesKHR(
    VkDevice device,
    VkDeferredOperationKHR deferredOperation,
    VkPipelineCache pipelineCache,
    uint32_t createInfoCount,
    const VkRayTracingPipelineCreateInfoKHR *pCreateInfos,
    const VkAllocationCallbacks *pAllocator,
    VkPipeline *pPipelines) {
  return vkCreateRayTracingPipelinesKHR_(device, deferredOperation,
                                         pipelineCache, createInfoCount,
                                         pCreateInfos, pAllocator, pPipelines);
}

void DeviceProcedures::SetDevice(Device *device) {
  device_ = device;
  GET_PROCEDURE(device_, vkGetBufferDeviceAddressKHR);
  GET_PROCEDURE(device_, vkGetAccelerationStructureBuildSizesKHR);
  GET_PROCEDURE(device_, vkCreateAccelerationStructureKHR);
  GET_PROCEDURE(device_, vkCmdBuildAccelerationStructuresKHR);
  GET_PROCEDURE(device_, vkDestroyAccelerationStructureKHR);
  GET_PROCEDURE(device_, vkGetAccelerationStructureDeviceAddressKHR);
  GET_PROCEDURE(device_, vkCreateRayTracingPipelinesKHR);
  GET_PROCEDURE(device_, vkGetRayTracingShaderGroupHandlesKHR);
  GET_PROCEDURE(device_, vkCmdTraceRaysKHR);
}
VkResult DeviceProcedures::vkGetRayTracingShaderGroupHandlesKHR(
    VkDevice device,
    VkPipeline pipeline,
    uint32_t firstGroup,
    uint32_t groupCount,
    size_t dataSize,
    void *pData) {
  return vkGetRayTracingShaderGroupHandlesKHR_(device, pipeline, firstGroup,
                                               groupCount, dataSize, pData);
}

void DeviceProcedures::vkCmdTraceRaysKHR(
    VkCommandBuffer commandBuffer,
    const VkStridedDeviceAddressRegionKHR *pRaygenShaderBindingTable,
    const VkStridedDeviceAddressRegionKHR *pMissShaderBindingTable,
    const VkStridedDeviceAddressRegionKHR *pHitShaderBindingTable,
    const VkStridedDeviceAddressRegionKHR *pCallableShaderBindingTable,
    uint32_t width,
    uint32_t height,
    uint32_t depth) {
  vkCmdTraceRaysKHR_(commandBuffer, pRaygenShaderBindingTable,
                     pMissShaderBindingTable, pHitShaderBindingTable,
                     pCallableShaderBindingTable, width, height, depth);
}

}  // namespace grassland::vulkan

VKAPI_ATTR VkDeviceAddress VKAPI_CALL
vkGetBufferDeviceAddressKHR(VkDevice device,
                            const VkBufferDeviceAddressInfo *pInfo) {
  return grassland::vulkan::DeviceProcedures::GetStaticInstance()
      ->vkGetBufferDeviceAddressKHR(device, pInfo);
}

VKAPI_ATTR void VKAPI_CALL vkGetAccelerationStructureBuildSizesKHR(
    VkDevice device,
    VkAccelerationStructureBuildTypeKHR buildType,
    const VkAccelerationStructureBuildGeometryInfoKHR *pBuildInfo,
    const uint32_t *pMaxPrimitiveCounts,
    VkAccelerationStructureBuildSizesInfoKHR *pSizeInfo) {
  return grassland::vulkan::DeviceProcedures::GetStaticInstance()
      ->vkGetAccelerationStructureBuildSizesKHR(device, buildType, pBuildInfo,
                                                pMaxPrimitiveCounts, pSizeInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateAccelerationStructureKHR(
    VkDevice device,
    const VkAccelerationStructureCreateInfoKHR *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkAccelerationStructureKHR *pAccelerationStructure) {
  return grassland::vulkan::DeviceProcedures::GetStaticInstance()
      ->vkCreateAccelerationStructureKHR(device, pCreateInfo, pAllocator,
                                         pAccelerationStructure);
}

VKAPI_ATTR void VKAPI_CALL vkCmdBuildAccelerationStructuresKHR(
    VkCommandBuffer commandBuffer,
    uint32_t infoCount,
    const VkAccelerationStructureBuildGeometryInfoKHR *pInfos,
    const VkAccelerationStructureBuildRangeInfoKHR *const *ppBuildRangeInfos) {
  return grassland::vulkan::DeviceProcedures::GetStaticInstance()
      ->vkCmdBuildAccelerationStructuresKHR(commandBuffer, infoCount, pInfos,
                                            ppBuildRangeInfos);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyAccelerationStructureKHR(
    VkDevice device,
    VkAccelerationStructureKHR accelerationStructure,
    const VkAllocationCallbacks *pAllocator) {
  return grassland::vulkan::DeviceProcedures::GetStaticInstance()
      ->vkDestroyAccelerationStructureKHR(device, accelerationStructure,
                                          pAllocator);
}

VKAPI_ATTR VkDeviceAddress VKAPI_CALL
vkGetAccelerationStructureDeviceAddressKHR(
    VkDevice device,
    const VkAccelerationStructureDeviceAddressInfoKHR *pInfo) {
  return grassland::vulkan::DeviceProcedures::GetStaticInstance()
      ->vkGetAccelerationStructureDeviceAddressKHR(device, pInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateRayTracingPipelinesKHR(
    VkDevice device,
    VkDeferredOperationKHR deferredOperation,
    VkPipelineCache pipelineCache,
    uint32_t createInfoCount,
    const VkRayTracingPipelineCreateInfoKHR *pCreateInfos,
    const VkAllocationCallbacks *pAllocator,
    VkPipeline *pPipelines) {
  return grassland::vulkan::DeviceProcedures::GetStaticInstance()
      ->vkCreateRayTracingPipelinesKHR(device, deferredOperation, pipelineCache,
                                       createInfoCount, pCreateInfos,
                                       pAllocator, pPipelines);
}

VKAPI_ATTR VkResult VKAPI_CALL
vkGetRayTracingShaderGroupHandlesKHR(VkDevice device,
                                     VkPipeline pipeline,
                                     uint32_t firstGroup,
                                     uint32_t groupCount,
                                     size_t dataSize,
                                     void *pData) {
  return grassland::vulkan::DeviceProcedures::GetStaticInstance()
      ->vkGetRayTracingShaderGroupHandlesKHR(device, pipeline, firstGroup,
                                             groupCount, dataSize, pData);
}

VKAPI_ATTR void VKAPI_CALL vkCmdTraceRaysKHR(
    VkCommandBuffer commandBuffer,
    const VkStridedDeviceAddressRegionKHR *pRaygenShaderBindingTable,
    const VkStridedDeviceAddressRegionKHR *pMissShaderBindingTable,
    const VkStridedDeviceAddressRegionKHR *pHitShaderBindingTable,
    const VkStridedDeviceAddressRegionKHR *pCallableShaderBindingTable,
    uint32_t width,
    uint32_t height,
    uint32_t depth) {
  grassland::vulkan::DeviceProcedures::GetStaticInstance()->vkCmdTraceRaysKHR(
      commandBuffer, pRaygenShaderBindingTable, pMissShaderBindingTable,
      pHitShaderBindingTable, pCallableShaderBindingTable, width, height,
      depth);
}
