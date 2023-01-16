#pragma once
#include <grassland/vulkan/vulkan.h>

namespace grassland::vulkan::framework {
struct CoreSettings {
  CoreSettings() = default;
  bool has_window{true};
  uint32_t window_width{1280};
  uint32_t window_height{720};
  const char *window_title{"Grassland Demo"};
  uint32_t frames_in_flight{3};
  bool raytracing_pipeline_required{false};
  bool validation_layer{kDefaultEnableValidationLayers};
};
}  // namespace grassland::vulkan::framework
