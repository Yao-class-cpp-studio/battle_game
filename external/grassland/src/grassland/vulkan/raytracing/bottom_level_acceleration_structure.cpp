#include "grassland/vulkan/raytracing/bottom_level_acceleration_structure.h"

#include "cstring"
#include "glm/detail/type_vec3.hpp"
#include "grassland/vulkan/helper/helper.h"

namespace grassland::vulkan::raytracing {

BottomLevelAccelerationStructure::BottomLevelAccelerationStructure(
    Device *device,
    CommandPool *command_pool,
    const void *vertices_data,
    uint32_t vertex_buffer_size,
    const uint32_t *indices,
    uint32_t num_index,
    uint32_t stride)
    : device_(device) {
  const VkBufferUsageFlags buffer_usage_flags =
      VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
      VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

  std::unique_ptr<Buffer> vertex_buffer, index_buffer;
  vertex_buffer =
      std::make_unique<Buffer>(device_, vertex_buffer_size, buffer_usage_flags,
                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
                                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
  std::memcpy(vertex_buffer->Map(), vertices_data, vertex_buffer_size);
  vertex_buffer->Unmap();

  index_buffer = std::make_unique<Buffer>(
      device_, num_index * sizeof(uint32_t), buffer_usage_flags,
      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
  std::memcpy(index_buffer->Map(), indices, num_index * sizeof(uint32_t));
  index_buffer->Unmap();

  // Setup a single transformation matrix that can be used to transform the
  // whole geometry for a single bottom level acceleration structure
  VkTransformMatrixKHR transform_matrix = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                                           0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f};
  std::unique_ptr<Buffer> transform_matrix_buffer;
  transform_matrix_buffer = std::make_unique<Buffer>(
      device_, sizeof(transform_matrix), buffer_usage_flags,
      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
  std::memcpy(transform_matrix_buffer->Map(), &transform_matrix,
              sizeof(transform_matrix));
  transform_matrix_buffer->Unmap();

  VkDeviceOrHostAddressConstKHR vertex_data_device_address{};
  VkDeviceOrHostAddressConstKHR index_data_device_address{};
  VkDeviceOrHostAddressConstKHR transform_matrix_device_address{};

  vertex_data_device_address.deviceAddress = vertex_buffer->GetDeviceAddress();
  index_data_device_address.deviceAddress = index_buffer->GetDeviceAddress();
  transform_matrix_device_address.deviceAddress =
      transform_matrix_buffer->GetDeviceAddress();

  // The bottom level acceleration structure contains one set of triangles as
  // the input geometry
  VkAccelerationStructureGeometryKHR acceleration_structure_geometry{};
  acceleration_structure_geometry.sType =
      VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
  acceleration_structure_geometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
  acceleration_structure_geometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
  acceleration_structure_geometry.geometry.triangles.sType =
      VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
  acceleration_structure_geometry.geometry.triangles.vertexFormat =
      VK_FORMAT_R32G32B32_SFLOAT;
  acceleration_structure_geometry.geometry.triangles.vertexData =
      vertex_data_device_address;
  acceleration_structure_geometry.geometry.triangles.maxVertex =
      vertex_buffer_size / stride;
  acceleration_structure_geometry.geometry.triangles.vertexStride = stride;
  acceleration_structure_geometry.geometry.triangles.indexType =
      VK_INDEX_TYPE_UINT32;
  acceleration_structure_geometry.geometry.triangles.indexData =
      index_data_device_address;
  acceleration_structure_geometry.geometry.triangles.transformData =
      transform_matrix_device_address;

  // Get the size requirements for buffers involved in the acceleration
  // structure build process
  VkAccelerationStructureBuildGeometryInfoKHR
      acceleration_structure_build_geometry_info{};
  acceleration_structure_build_geometry_info.sType =
      VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
  acceleration_structure_build_geometry_info.type =
      VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
  acceleration_structure_build_geometry_info.flags =
      VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
  acceleration_structure_build_geometry_info.geometryCount = 1;
  acceleration_structure_build_geometry_info.pGeometries =
      &acceleration_structure_geometry;

  const uint32_t primitive_count = num_index / 3;

  VkAccelerationStructureBuildSizesInfoKHR
      acceleration_structure_build_sizes_info{};
  acceleration_structure_build_sizes_info.sType =
      VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
  vkGetAccelerationStructureBuildSizesKHR(
      device_->GetHandle(), VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
      &acceleration_structure_build_geometry_info, &primitive_count,
      &acceleration_structure_build_sizes_info);

  // Create a buffer to hold the acceleration structure
  buffer_ = std::make_unique<Buffer>(
      device_,
      acceleration_structure_build_sizes_info.accelerationStructureSize,
      VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);

  // Create the acceleration structure
  VkAccelerationStructureCreateInfoKHR acceleration_structure_create_info{};
  acceleration_structure_create_info.sType =
      VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
  acceleration_structure_create_info.buffer = buffer_->GetHandle();
  acceleration_structure_create_info.size =
      acceleration_structure_build_sizes_info.accelerationStructureSize;
  acceleration_structure_create_info.type =
      VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
  vkCreateAccelerationStructureKHR(device_->GetHandle(),
                                   &acceleration_structure_create_info, nullptr,
                                   &handle_);

  // The actual build process starts here

  // Create a scratch buffer as a temporary storage for the acceleration
  // structure build
  std::unique_ptr<Buffer> scratch_buffer;
  scratch_buffer = std::make_unique<Buffer>(
      device_, acceleration_structure_build_sizes_info.buildScratchSize,
      VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);

  VkAccelerationStructureBuildGeometryInfoKHR
      acceleration_build_geometry_info{};
  acceleration_build_geometry_info.sType =
      VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
  acceleration_build_geometry_info.type =
      VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
  acceleration_build_geometry_info.flags =
      VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
  acceleration_build_geometry_info.mode =
      VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
  acceleration_build_geometry_info.dstAccelerationStructure = handle_;
  acceleration_build_geometry_info.geometryCount = 1;
  acceleration_build_geometry_info.pGeometries =
      &acceleration_structure_geometry;
  acceleration_build_geometry_info.scratchData.deviceAddress =
      scratch_buffer->GetDeviceAddress();

  VkAccelerationStructureBuildRangeInfoKHR
      acceleration_structure_build_range_info;
  acceleration_structure_build_range_info.primitiveCount = primitive_count;
  acceleration_structure_build_range_info.primitiveOffset = 0;
  acceleration_structure_build_range_info.firstVertex = 0;
  acceleration_structure_build_range_info.transformOffset = 0;
  std::vector<VkAccelerationStructureBuildRangeInfoKHR *>
      acceleration_build_structure_range_infos = {
          &acceleration_structure_build_range_info};

  // Build the acceleration structure on the device via a one-time command
  // buffer submission Some implementations may support acceleration structure
  // building on the host
  // (VkPhysicalDeviceAccelerationStructureFeaturesKHR->accelerationStructureHostCommands),
  // but we prefer device builds

  helper::SingleTimeCommands(command_pool, [&](VkCommandBuffer command_buffer) {
    vkCmdBuildAccelerationStructuresKHR(
        command_buffer, 1, &acceleration_build_geometry_info,
        acceleration_build_structure_range_infos.data());
  });

  VkAccelerationStructureDeviceAddressInfoKHR
      acceleration_device_address_info{};
  acceleration_device_address_info.sType =
      VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
  acceleration_device_address_info.accelerationStructure = handle_;
  device_address_ = vkGetAccelerationStructureDeviceAddressKHR(
      device_->GetHandle(), &acceleration_device_address_info);
}

Buffer *BottomLevelAccelerationStructure::GetBuffer() const {
  return buffer_.get();
}

VkDeviceAddress BottomLevelAccelerationStructure::GetDeviceAddress() const {
  return device_address_;
}

BottomLevelAccelerationStructure::~BottomLevelAccelerationStructure() {
  vkDestroyAccelerationStructureKHR(device_->GetHandle(), handle_, nullptr);
}
}  // namespace grassland::vulkan::raytracing
