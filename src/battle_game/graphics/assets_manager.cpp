#include "battle_game/graphics/assets_manager.h"

namespace battle_game {
AssetsManager *AssetsManager::GetInstance() {
  static AssetsManager assets_manager;
  return &assets_manager;
}

uint32_t AssetsManager::RegisterModel(const std::vector<ObjectVertex> &vertices,
                                      const std::vector<uint32_t> &indices) {
  assets_synced_ = false;
  models_.emplace_back(vertices, indices);
  return models_.size() - 1;
}

AssetsManager::AssetsManager() {
  textures_.emplace_back(1, 1, glm::vec4{1.0f});
  textures_.emplace_back(kParticleTextureSize, kParticleTextureSize);
  current_texture_id_ = 1;
  texture_infos_.push_back({0, 0, 1, 1, 0});
  models_.emplace_back(
      std::vector<ObjectVertex>{
          {{-1.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
          {{-1.0f, -1.0f}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
          {{1.0f, 1.0f}, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
          {{1.0f, -1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}}},
      std::vector<uint32_t>{0, 1, 2, 1, 2, 3});
}

TextureInfo AssetsManager::GetTextureSpace(uint32_t width, uint32_t height) {
  TextureInfo texture_info{};
  texture_info.width = width;
  texture_info.height = height;
  width++;
  height++;
  while (current_head_x_ + width >= kParticleTextureSize ||
         current_head_y_ + height >= kParticleTextureSize) {
    if (current_head_x_ + width >= kParticleTextureSize) {
      current_head_x_ = 0;
      current_head_y_ = current_bottom_y_;
    } else if (current_head_y_ + height >= kParticleTextureSize) {
      current_head_x_ = 0;
      current_head_y_ = 0;
      current_bottom_y_ = 0;
      current_texture_id_ = textures_.size();
      textures_.emplace_back(kParticleTextureSize, kParticleTextureSize);
    }
  }
  current_bottom_y_ = std::max(current_bottom_y_, current_head_y_ + height);
  texture_info.texture_id = current_texture_id_;
  texture_info.x = current_head_x_;
  texture_info.y = current_head_y_;
  current_head_x_ += width;
  return texture_info;
}

uint32_t AssetsManager::RegisterTexture(const Texture &particle_texture) {
  uint32_t result = texture_infos_.size();
  auto texture_info = GetTextureSpace(particle_texture.GetWidth(),
                                      particle_texture.GetHeight());
  texture_infos_.push_back(texture_info);
  auto &texture = textures_[texture_info.texture_id];
  auto x = std::lround(texture_info.x);
  auto y = std::lround(texture_info.y);
  auto buffer = particle_texture.GetBuffer();
  auto tex_buffer = texture.GetBuffer();
  for (auto dy = 0; dy < particle_texture.GetHeight(); dy++) {
    std::memcpy(tex_buffer + (dy + y) * texture.GetWidth() + x,
                buffer + dy * particle_texture.GetWidth(),
                particle_texture.GetWidth() * sizeof(glm::vec4));
  }
  assets_synced_ = false;
  return result;
}
}  // namespace battle_game
