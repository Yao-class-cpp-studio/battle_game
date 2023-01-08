#pragma once
#include "battle_game/graphics/model.h"
#include "battle_game/graphics/texture.h"
#include "battle_game/graphics/util.h"

namespace battle_game {
constexpr uint32_t kParticleTextureSize = 4096;
class AssetsManager {
 public:
  AssetsManager();
  static AssetsManager *GetInstance();
  uint32_t RegisterModel(const std::vector<ObjectVertex> &vertices,
                         const std::vector<uint32_t> &indices);
  uint32_t RegisterTexture(const Texture &particle_texture);

  std::vector<Model> &GetModels() {
    return models_;
  }
  [[nodiscard]] const std::vector<Model> &GetModels() const {
    return models_;
  }
  std::vector<Texture> &GetTextures() {
    return textures_;
  }
  [[nodiscard]] const std::vector<Texture> &GetParticleTextures() const {
    return textures_;
  }
  std::vector<TextureInfo> &GetTextureInfos() {
    return texture_infos_;
  }
  [[nodiscard]] const std::vector<TextureInfo> &GetTextureInfos() const {
    return texture_infos_;
  }

  bool &GetSyncState() {
    return assets_synced_;
  }
  [[nodiscard]] bool GetSyncState() const {
    return assets_synced_;
  }

 private:
  TextureInfo GetTextureSpace(uint32_t width, uint32_t height);
  std::vector<Texture> textures_;
  std::vector<TextureInfo> texture_infos_;
  uint32_t current_texture_id_;
  uint32_t current_head_x_{0};
  uint32_t current_head_y_{0};
  uint32_t current_bottom_y_{0};

  std::vector<Model> models_;
  bool assets_synced_{false};
};
}  // namespace battle_game
