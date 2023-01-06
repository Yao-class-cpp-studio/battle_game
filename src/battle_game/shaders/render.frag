#version 460
#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_nonuniform_qualifier : enable

// clang-format off
#include "object_structs.glsl"
// clang-format on

layout(location = 0) in flat int instance_index;
layout(location = 1) in vec2 frag_tex_coord;
layout(location = 2) in vec4 frag_color;
layout(location = 0) out vec4 color_output;

layout(binding = 0) buffer texture_info_array {
  TextureInfo texture_infos[];
};
layout(binding = 1) uniform sampler2D[] texture_samplers;
layout(binding = 2) uniform global_settings_object {
  GlobalSettings global_settings;
};
layout(binding = 3) buffer object_settings_array {
  ObjectSettings object_settings[];
};

void main() {
  TextureInfo texture_info = texture_infos[instance_index];
  vec2 tex_size =
      vec2(textureSize(texture_samplers[texture_info.texture_id], 0));
  color_output =
      frag_color * object_settings[instance_index].color *
      texture(texture_samplers[texture_info.texture_id],
              (vec2(texture_info.x, texture_info.y) +
               frag_tex_coord * vec2(texture_info.width, texture_info.height)) /
                  vec2(tex_size));
}
