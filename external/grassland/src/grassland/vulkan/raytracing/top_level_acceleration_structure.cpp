#include "grassland/vulkan/raytracing/top_level_acceleration_structure.h"

#include "cstring"
#include "grassland/vulkan/helper/helper.h"

namespace grassland::vulkan::raytracing {

TopLevelAccelerationStructure::TopLevelAccelerationStructure(
    Device *device,
    CommandPool *command_pool,
    const std::vector<std::pair<BottomLevelAccelerationStructure *, glm::mat4>>
        &objects)
    : device_(device) {
  std::vector<VkAccelerationStructureInstanceKHR>
      acceleration_structure_instances;
  for (int i = 0; i < objects.size(); i++) {
    auto &object = objects[i];
    VkAccelerationStructureInstanceKHR acceleration_structure_instance{};
    acceleration_structure_instance.transform = {
        object.second[0][0], object.second[1][0], object.second[2][0],
        object.second[3][0], object.second[0][1], object.second[1][1],
        object.second[2][1], object.second[3][1], object.second[0][2],
        object.second[1][2], object.second[2][2], object.second[3][2]};
    acceleration_structure_instance.instanceCustomIndex = i;
    acceleration_structure_instance.mask = 0xFF;
    acceleration_structure_instance.instanceShaderBindingTableRecordOffset = 0;
    acceleration_structure_instance.flags =
        VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
    acceleration_structure_instance.accelerationStructureReference =
        object.first->GetDeviceAddress();
    acceleration_structure_instances.push_back(acceleration_structure_instance);
  }

  std::unique_ptr<Buffer> instances_buffer;
  instances_buffer = std::make_unique<Buffer>(
      device_,
      sizeof(VkAccelerationStructureInstanceKHR) *
          acceleration_structure_instances.size(),
      VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  std::memcpy(instances_buffer->Map(), acceleration_structure_instances.data(),
              acceleration_structure_instances.size() *
                  sizeof(VkAccelerationStructureInstanceKHR));
  instances_buffer->Unmap();

  VkDeviceOrHostAddressConstKHR instance_data_device_address{};
  instance_data_device_address.deviceAddress =
      instances_buffer->GetDeviceAddress();

  // The top level acceleration structure contains (bottom level) instance as
  // the input geometry
  VkAccelerationStructureGeometryKHR acceleration_structure_geometry{};
  acceleration_structure_geometry.sType =
      VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
  acceleration_structure_geometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
  acceleration_structure_geometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
  acceleration_structure_geometry.geometry.instances.sType =
      VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
  acceleration_structure_geometry.geometry.instances.arrayOfPointers = VK_FALSE;
  acceleration_structure_geometry.geometry.instances.data =
      instance_data_device_address;

  // Get the size requirements for buffers involved in the acceleration
  // structure build process
  VkAccelerationStructureBuildGeometryInfoKHR
      acceleration_structure_build_geometry_info{};
  acceleration_structure_build_geometry_info.sType =
      VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
  acceleration_structure_build_geometry_info.type =
      VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
  acceleration_structure_build_geometry_info.flags =
      VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
  acceleration_structure_build_geometry_info.geometryCount = 1;
  acceleration_structure_build_geometry_info.pGeometries =
      &acceleration_structure_geometry;

  const uint32_t primitive_count = acceleration_structure_instances.size();

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
      VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR);

  // Create the acceleration structure
  VkAccelerationStructureCreateInfoKHR acceleration_structure_create_info{};
  acceleration_structure_create_info.sType =
      VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
  acceleration_structure_create_info.buffer = buffer_->GetHandle();
  acceleration_structure_create_info.size =
      acceleration_structure_build_sizes_info.accelerationStructureSize;
  acceleration_structure_create_info.type =
      VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
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
      VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
  acceleration_build_geometry_info.flags =
      VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR |
      VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR;
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

  // Get the top acceleration structure's handle, which will be used to setup
  // it's descriptor
  VkAccelerationStructureDeviceAddressInfoKHR
      acceleration_device_address_info{};
  acceleration_device_address_info.sType =
      VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
  acceleration_device_address_info.accelerationStructure = handle_;
  device_address_ = vkGetAccelerationStructureDeviceAddressKHR(
      device_->GetHandle(), &acceleration_device_address_info);
}

TopLevelAccelerationStructure::~TopLevelAccelerationStructure() {
  vkDestroyAccelerationStructureKHR(device_->GetHandle(), handle_, nullptr);
}

void TopLevelAccelerationStructure::UpdateAccelerationStructure(
    CommandPool *command_pool,
    const std::vector<std::pair<BottomLevelAccelerationStructure *, glm::mat4>>
        &objects) {
  std::vector<VkAccelerationStructureInstanceKHR>
      acceleration_structure_instances;
  for (int i = 0; i < objects.size(); i++) {
    auto &object = objects[i];
    VkAccelerationStructureInstanceKHR acceleration_structure_instance{};
    acceleration_structure_instance.transform = {
        object.second[0][0], object.second[1][0], object.second[2][0],
        object.second[3][0], object.second[0][1], object.second[1][1],
        object.second[2][1], object.second[3][1], object.second[0][2],
        object.second[1][2], object.second[2][2], object.second[3][2]};
    acceleration_structure_instance.instanceCustomIndex = i;
    acceleration_structure_instance.mask = 0xFF;
    acceleration_structure_instance.instanceShaderBindingTableRecordOffset = 0;
    acceleration_structure_instance.flags =
        VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
    acceleration_structure_instance.accelerationStructureReference =
        object.first->GetDeviceAddress();
    acceleration_structure_instances.push_back(acceleration_structure_instance);
  }

  std::unique_ptr<Buffer> instances_buffer;
  instances_buffer = std::make_unique<Buffer>(
      device_,
      sizeof(VkAccelerationStructureInstanceKHR) *
          acceleration_structure_instances.size(),
      VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  std::memcpy(instances_buffer->Map(), acceleration_structure_instances.data(),
              acceleration_structure_instances.size() *
                  sizeof(VkAccelerationStructureInstanceKHR));
  instances_buffer->Unmap();

  VkDeviceOrHostAddressConstKHR instance_data_device_address{};
  instance_data_device_address.deviceAddress =
      instances_buffer->GetDeviceAddress();

  // The top level acceleration structure contains (bottom level) instance as
  // the input geometry
  VkAccelerationStructureGeometryKHR acceleration_structure_geometry{};
  acceleration_structure_geometry.sType =
      VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
  acceleration_structure_geometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
  acceleration_structure_geometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
  acceleration_structure_geometry.geometry.instances.sType =
      VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
  acceleration_structure_geometry.geometry.instances.arrayOfPointers = VK_FALSE;
  acceleration_structure_geometry.geometry.instances.data =
      instance_data_device_address;

  // Get the size requirements for buffers involved in the acceleration
  // structure build process
  VkAccelerationStructureBuildGeometryInfoKHR
      acceleration_structure_build_geometry_info{};
  acceleration_structure_build_geometry_info.sType =
      VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
  acceleration_structure_build_geometry_info.type =
      VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
  acceleration_structure_build_geometry_info.flags =
      VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
  acceleration_structure_build_geometry_info.geometryCount = 1;
  acceleration_structure_build_geometry_info.pGeometries =
      &acceleration_structure_geometry;

  const uint32_t primitive_count = acceleration_structure_instances.size();

  VkAccelerationStructureBuildSizesInfoKHR
      acceleration_structure_build_sizes_info{};
  acceleration_structure_build_sizes_info.sType =
      VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
  vkGetAccelerationStructureBuildSizesKHR(
      device_->GetHandle(), VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
      &acceleration_structure_build_geometry_info, &primitive_count,
      &acceleration_structure_build_sizes_info);

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
      VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
  acceleration_build_geometry_info.flags =
      VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR |
      VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR;
  acceleration_build_geometry_info.mode =
      VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR;
  acceleration_build_geometry_info.srcAccelerationStructure = handle_;
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

  // Get the top acceleration structure's handle, which will be used to setup
  // it's descriptor
  VkAccelerationStructureDeviceAddressInfoKHR
      acceleration_device_address_info{};
  acceleration_device_address_info.sType =
      VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
  acceleration_device_address_info.accelerationStructure = handle_;
  device_address_ = vkGetAccelerationStructureDeviceAddressKHR(
      device_->GetHandle(), &acceleration_device_address_info);
}
}  // namespace grassland::vulkan::raytracing
