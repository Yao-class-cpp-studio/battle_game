#include <grassland/vulkan/framework/data_buffer.h>

namespace grassland::vulkan::framework {

DataBuffer::DataBuffer(Core *core) {
  core_ = core;
}

Core *DataBuffer::GetCore() const {
  return core_;
}
}  // namespace grassland::vulkan::framework
