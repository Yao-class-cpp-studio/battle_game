
struct GlobalSettings {
  mat4 world_to_camera;
};

struct ObjectSettings {
  mat4 local_to_world;
  vec4 color;
};

struct TextureInfo {
  float x;
  float y;
  float width;
  float height;
  int texture_id;
};
