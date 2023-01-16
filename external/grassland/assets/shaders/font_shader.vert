#version 450

layout(location = 0) in vec2 vert_pos;

struct UniformObject {
  mat4 trans_mat;
  vec4 color;
};

layout(binding = 0) readonly buffer uniform_buffer_object {
  UniformObject uniform_objects[];
};

layout(location = 0) out vec4 frag_color;

void main() {
  UniformObject ubo = uniform_objects[gl_InstanceIndex];
  frag_color = ubo.color;
  gl_Position = ubo.trans_mat * vec4(vert_pos, 0.0, 1.0);
}
