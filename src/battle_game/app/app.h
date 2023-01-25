#pragma once
#include "asio.hpp"
#include "battle_game/app/app_settings.h"
#include "battle_game/app/device_model.h"
#include "battle_game/app/server.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"
#include "grassland/grassland.h"

namespace battle_game {
constexpr uint16_t default_port = 8088;
constexpr size_t max_ip_length = 50;
using namespace grassland;
class App {
 public:
  enum Mode { kOffline, kClient, kServer };

  explicit App(const AppSettings &app_settings,
               GameCore *game_core,
               asio::io_context &io_context);
  void Run(const Mode &mode = kOffline);

 private:
  class Client : public std::enable_shared_from_this<Client> {
   public:
    Client(App *app);

    void Connect(const asio::ip::tcp::resolver::results_type &endpoint);
    void Close();
    uint32_t GetPlayerCount() const;
    uint32_t GetPlayerId() const;

   private:
    void DoReadHeader();
    void DoReadBody();
    void DoWrite();
    void DoStart();
    void DoStop();
    void RegisterTimer();
    void CloseTimer();
    void Write();
    void Quit();

    App *app_;
    asio::io_context &io_context_;
    asio::ip::tcp::socket socket_;
    std::unique_ptr<asio::steady_timer> timer_;
    std::queue<ByteString> write_messages_;
    MessageInitial init_message_;
    uint8_t buffer_[MessageInputData::length];
    std::vector<MessageInputData> input_data_;
  };

  void OnInit();
  void OnLoop();
  void OnClose();

  void Reset(const Mode &mode);
  void Start();
  void Stop();

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
  float fov_y_{10.0f};
  bool running_{false};
  bool should_reset_{false};
  bool should_start_{false};
  Mode mode_{kOffline}, chosen_mode_{kOffline};
  bool render_{true};
  std::queue<std::vector<MessageInputData>> input_data_queue_;
  InputData input_data_;
  int selected_unit_{0};
  std::chrono::time_point<std::chrono::steady_clock> begin_time_;
  uint64_t updated_step_{0};
  uint16_t port_{default_port};
  std::string message_{};
  char ip_[max_ip_length]{"localhost"};

  asio::io_context &io_context_;
  asio::executor_work_guard<asio::io_context::executor_type> worker_;
  asio::ip::tcp::resolver resolver_;
  std::thread thread_;
  std::shared_ptr<Server> server_;
  std::shared_ptr<Client> client_;
};
}  // namespace battle_game
