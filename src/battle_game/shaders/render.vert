#version 460
#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_nonuniform_qualifier : enable

// clang-format off
#include "object_structs.glsl"
// clang-format on

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex_coord;
layout(location = 2) in vec4 color;
layout(location = 0) out flat int instance_index;
layout(location = 1) out vec2 frag_tex_coord;
layout(location = 2) out vec4 frag_color;

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
  instance_index = gl_InstanceIndex;
  frag_tex_coord = tex_coord;
  frag_color = color;
  ObjectSettings object_info = object_settings[instance_index];
  gl_Position = vec4(0.0, 0.0, 1.0, 0.0) +
                (global_settings.world_to_camera * object_info.local_to_world *
                 vec4(position.xy, 0.5, 1.0)) *
                    vec4(1.0, 1.0, -1.0, 1.0);
}
