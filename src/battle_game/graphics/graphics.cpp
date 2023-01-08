#include "battle_game/graphics/graphics.h"

namespace battle_game {
namespace {
std::vector<ObjectSettings> object_settings_;
std::vector<TextureInfo> texture_infos_;
std::vector<uint32_t> model_ids_;
TextureInfo current_texture_info_;
uint32_t current_texture_id_;
ObjectSettings current_object_settings_;
glm::vec2 current_position_;
glm::vec2 current_scale_;
float current_rotation_;
AssetsManager *mgr{};

void UpdateObjectSettingsTransformMatrix() {
  current_object_settings_.local_to_world =
      glm::translate(glm::mat4{1.0f}, glm::vec3{current_position_, 0.0f}) *
      glm::rotate(glm::mat4{1.0f}, current_rotation_,
                  glm::vec3{0.0f, 0.0f, 1.0f}) *
      glm::scale(glm::mat4{1.0f}, glm::vec3{current_scale_, 1.0f});
}
}  // namespace

void NewFrame() {
  object_settings_.clear();
  texture_infos_.clear();
  model_ids_.clear();
  mgr = AssetsManager::GetInstance();
  current_texture_id_ = 0;
  current_texture_info_ = mgr->GetTextureInfos()[0];
  current_object_settings_ = {glm::mat4{1.0f}, glm::vec4{1.0f}};
  current_position_ = {0.0f, 0.0f};
  current_rotation_ = 0.0f;
  current_scale_ = glm::vec2{1.0f};
}

const std::vector<ObjectSettings> &GetObjectSettings() {
  return object_settings_;
}

const std::vector<TextureInfo> &GetTextureInfos() {
  return texture_infos_;
}

const std::vector<uint32_t> &GetModelIds() {
  return model_ids_;
}

void SetColor(const glm::vec4 &color) {
  current_object_settings_.color = color;
}

glm::vec4 GetColor() {
  return current_object_settings_.color;
}

void SetPosition(glm::vec2 position) {
  current_position_ = position;
  UpdateObjectSettingsTransformMatrix();
}

void SetRotation(float rotation) {
  current_rotation_ = rotation;
  UpdateObjectSettingsTransformMatrix();
}

void SetTexture(uint32_t texture_id) {
  current_texture_id_ = texture_id;
  current_texture_info_ = mgr->GetTextureInfos()[texture_id];
}

uint32_t GetTexture() {
  return current_texture_id_;
}

uint32_t RegisterTexture(const std::string &file_path) {
  static std::map<std::string, uint32_t> map_file_path_index;
  if (!map_file_path_index.count(file_path)) {
    map_file_path_index[file_path] =
        mgr->RegisterTexture(Texture::Load(file_path));
  }
  return map_file_path_index.at(file_path);
}

uint32_t SetTexture(const std::string &file_path) {
  auto id = RegisterTexture(file_path);
  SetTexture(id);
  return id;
}

void DrawModel(uint32_t model_id) {
  texture_infos_.push_back(current_texture_info_);
  object_settings_.push_back(current_object_settings_);
  model_ids_.push_back(model_id);
}

void DrawTexture(const std::string &file_path) {
  auto bak_texture_id = GetTexture();
  SetTexture(file_path);
  DrawModel(0);
  SetTexture(bak_texture_id);
}

void SetScale(glm::vec2 scale) {
  current_scale_ = scale;
  UpdateObjectSettingsTransformMatrix();
}

glm::vec2 GetScale() {
  return current_scale_;
}

void SetTransformation(glm::vec2 position, float rotation, glm::vec2 scale) {
  current_position_ = position;
  current_rotation_ = rotation;
  current_scale_ = scale;
  UpdateObjectSettingsTransformMatrix();
}
}  // namespace battle_game
