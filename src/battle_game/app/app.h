#pragma once
#include "battle_game/app/app_settings.h"
#include "battle_game/app/device_model.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"
#include "grassland/grassland.h"

namespace battle_game {
using namespace grassland;
class App {
 public:
  explicit App(const AppSettings &app_settings, GameCore *game_core);
  void Run();

 private:
  void OnInit();
  void OnLoop();
  void OnClose();

  void OnUpdate();
  void OnRender();

  void BuildRenderNodes();
  void SyncDeviceAssets();

  void CaptureInput();
  void UpdateDrawCommands();
  void UpdateDynamicBuffer();
  void UpdateImGui();

  void SetScene();
  [[nodiscard]] glm::mat4 GetCameraTransform(float fov_y) const;

  GameCore *game_core_{nullptr};

  std::unique_ptr<vulkan::framework::Core> core_;
  std::unique_ptr<vulkan::framework::TextureImage> frame_image_;

  std::vector<DeviceModel> device_models_;
  std::vector<std::unique_ptr<vulkan::framework::TextureImage>>
      device_particle_textures_;
  std::unique_ptr<vulkan::framework::DynamicBuffer<GlobalSettings>>
      device_global_settings_;
  std::unique_ptr<vulkan::framework::DynamicBuffer<TextureInfo>>
      device_texture_infos_;
  std::unique_ptr<vulkan::framework::DynamicBuffer<ObjectSettings>>
      device_object_settings_;
  std::unique_ptr<vulkan::Sampler> linear_sampler_;
  std::unique_ptr<vulkan::Sampler> nearest_sampler_;

  std::unique_ptr<vulkan::framework::RenderNode> render_node_;

  uint32_t my_player_id_{0};
  uint32_t enemy_player_id_{0};
  float fov_y_{10.0f};
};
}  // namespace battle_game
