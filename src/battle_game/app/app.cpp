#include "battle_game/app/app.h"

#include <string>

#include "battle_game/core/object.h"
#include "battle_game/graphics/util.h"
namespace battle_game {
using asio::ip::tcp;
static void HelpMarker(const char *desc) {
  ImGui::TextColored(ImVec4(1, 1, 0, 1), "(?)");
  if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort)) {
    ImGui::BeginTooltip();
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    ImGui::TextUnformatted(desc);
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
  }
}

App::App(const AppSettings &app_settings,
         GameCore *game_core,
         asio::io_context &io_context)
    : game_core_(game_core),
      io_context_(io_context),
      resolver_(io_context_),
      worker_(asio::make_work_guard(io_context_)),
      thread_([this]() { io_context_.run(); }) {
  vulkan::framework::CoreSettings core_settings;
  core_settings.window_title = "Battle Game";
  core_settings.window_width = app_settings.width;
  core_settings.window_height = app_settings.height;
  core_ = std::make_unique<vulkan::framework::Core>(core_settings);
  SetGlobalCore(core_.get());
}

void App::Run(const Mode &mode) {
  OnInit();
  Reset(mode);

  while (!glfwWindowShouldClose(core_->GetWindow())) {
    if (should_reset_) {
      Reset(chosen_mode_);
    }
    if (should_start_) {
      Start();
    }
    OnLoop();
    glfwPollEvents();
  }

  core_->GetDevice()->WaitIdle();

  OnClose();
}

void App::OnInit() {
  frame_image_ = std::make_unique<vulkan::framework::TextureImage>(
      core_.get(), core_->GetFramebufferWidth(), core_->GetFramebufferHeight(),
      VK_FORMAT_B8G8R8A8_UNORM);

  core_->SetFrameSizeCallback([this](int width, int height) {
    frame_image_->Resize(width, height);
    BuildRenderNodes();
  });

  device_global_settings_ =
      std::make_unique<vulkan::framework::DynamicBuffer<GlobalSettings>>(
          core_.get(), 1);
  device_texture_infos_ =
      std::make_unique<vulkan::framework::DynamicBuffer<TextureInfo>>(
          core_.get(), 1048576);
  device_object_settings_ =
      std::make_unique<vulkan::framework::DynamicBuffer<ObjectSettings>>(
          core_.get(), 1048576);

  linear_sampler_ =
      std::make_unique<vulkan::Sampler>(core_->GetDevice(), VK_FILTER_LINEAR);
  nearest_sampler_ =
      std::make_unique<vulkan::Sampler>(core_->GetDevice(), VK_FILTER_NEAREST);

  SyncDeviceAssets();
  BuildRenderNodes();

  core_->ImGuiInit(frame_image_.get(), "../../fonts/NotoSansSC-Regular.otf",
                   20.0f);
}

void App::Reset(const Mode &mode) {
  mode_ = mode;
  chosen_mode_ = mode;
  should_reset_ = false;
  should_start_ = false;
  running_ = false;
  message_ = "";
  if (server_) {
    server_->Close();
    server_.reset();
  }
  if (client_) {
    client_->Close();
    client_.reset();
  }
  game_core_->Reset();
  if (mode == kOffline) {
    should_start_ = true;
  } else {
    Stop();
    try {
      if (mode == kClient) {
        message_ = u8"连接中...";
        static char buffer[10];
        buffer[0] = '\0';
        sprintf(buffer, "%hu", port_);
        client_ = std::make_shared<Client>(this);
        client_->Connect(resolver_.resolve(ip_, buffer));
      } else if (mode == kServer) {
        server_ = std::make_shared<Server>(
            io_context_, tcp::endpoint(tcp::v4(), port_), &input_data_queue_);
        server_->Build();
        message_ = u8"成功创建服务器";
      }
    } catch (std::exception &e) {
      message_ = std::string(u8"错误：") + std::string(e.what());
      client_.reset();
      server_.reset();
    }
  }
}

void App::Start() {
  if (running_) {
    return;
  }
  begin_time_ = std::chrono::steady_clock::now();
  updated_step_ = 0;
  SetScene();
  while (!input_data_queue_.empty()) {
    input_data_queue_.pop();
  }
  running_ = true;
  should_start_ = false;
}

void App::Stop() {
  running_ = false;
}

void App::OnLoop() {
  OnUpdate();
  OnRender();
}

void App::OnClose() {
  io_context_.stop();
  thread_.join();
}

void App::OnUpdate() {
  UpdateImGui();
  NewFrame();
  CaptureInput();
  UpdateDrawCommands();
  UpdateDynamicBuffer();

  auto mgr = AssetsManager::GetInstance();
  if (!mgr->GetSyncState()) {
    SyncDeviceAssets();
  }
}

void App::OnRender() {
  core_->BeginCommandRecord();
  frame_image_->ClearColor({0.8f, 0.8, 0.8f, 1.0f});

  auto &model_ids = GetModelIds();
  render_node_->BeginDraw();
  for (int obj_id = 0; obj_id < model_ids.size(); obj_id++) {
    auto &device_model = device_models_[model_ids[obj_id]];
    render_node_->DrawDirect(device_model.vertex_buffer.get(),
                             device_model.index_buffer.get(),
                             device_model.index_buffer->Size(), obj_id);
  }
  render_node_->EndDraw();
  core_->TemporalSubmit();
  core_->ImGuiRender();
  core_->Output(frame_image_.get());
  core_->EndCommandRecordAndSubmit();
}

void App::BuildRenderNodes() {
  render_node_ = std::make_unique<vulkan::framework::RenderNode>(core_.get());
  render_node_->AddColorAttachment(frame_image_.get(), true);
  render_node_->AddShader("../../shaders/render.vert.spv",
                          VK_SHADER_STAGE_VERTEX_BIT);
  render_node_->AddShader("../../shaders/render.frag.spv",
                          VK_SHADER_STAGE_FRAGMENT_BIT);
  render_node_->VertexInput({VK_FORMAT_R32G32_SFLOAT, VK_FORMAT_R32G32_SFLOAT,
                             VK_FORMAT_R32G32B32A32_SFLOAT});
  render_node_->AddBufferBinding(
      device_texture_infos_.get(),
      VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
  std::vector<std::pair<vulkan::framework::TextureImage *, vulkan::Sampler *>>
      texture_sampler_pairs;
  for (auto &device_particle_texture : device_particle_textures_) {
    texture_sampler_pairs.emplace_back(device_particle_texture.get(),
                                       linear_sampler_.get());
  }
  render_node_->AddUniformBinding(
      texture_sampler_pairs,
      VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
  render_node_->AddUniformBinding(
      device_global_settings_.get(),
      VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
  render_node_->AddBufferBinding(
      device_object_settings_.get(),
      VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
  render_node_->BuildRenderNode();
}

void App::SyncDeviceAssets() {
  auto mgr = AssetsManager::GetInstance();
  auto &particle_textures = mgr->GetTextures();
  while (device_particle_textures_.size() < particle_textures.size()) {
    auto &particle_texture =
        particle_textures[device_particle_textures_.size()];
    device_particle_textures_.push_back(
        std::make_unique<vulkan::framework::TextureImage>(
            core_.get(), particle_texture.GetWidth(),
            particle_texture.GetHeight()));
  }
  for (int i = 0; i < device_particle_textures_.size(); i++) {
    auto &particle_texture = particle_textures[i];
    auto host_buffer = particle_texture.GetBuffer();
    vulkan::Buffer upload_buffer(core_->GetDevice(),
                                 sizeof(glm::vec4) *
                                     particle_texture.GetWidth() *
                                     particle_texture.GetHeight(),
                                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    std::memcpy(upload_buffer.Map(), host_buffer,
                sizeof(glm::vec4) * particle_texture.GetWidth() *
                    particle_texture.GetHeight());
    upload_buffer.Unmap();
    vulkan::UploadImage(core_->GetCommandPool(),
                        device_particle_textures_[i]->GetImage(),
                        &upload_buffer);
  }

  auto &models = mgr->GetModels();
  while (device_models_.size() < models.size()) {
    auto &model = models[device_models_.size()];
    auto &vertices = model.GetVertices();
    auto &indices = model.GetIndices();
    device_models_.push_back(
        {std::make_unique<vulkan::framework::StaticBuffer<ObjectVertex>>(
             core_.get(), vertices.size()),
         std::make_unique<vulkan::framework::StaticBuffer<uint32_t>>(
             core_.get(), indices.size())});
    auto &device_model = *device_models_.rbegin();
    device_model.vertex_buffer->Upload(vertices.data());
    device_model.index_buffer->Upload(indices.data());
  }

  mgr->GetSyncState() = true;
}

void App::UpdateDrawCommands() {
  if (running_) {
    auto current_time = std::chrono::steady_clock::now();
    auto time_passed =
        double((current_time - begin_time_) / std::chrono::nanoseconds(1)) *
        1e-9;
    uint64_t target_update_step = std::lround(time_passed / kSecondPerTick);
    while (updated_step_ < target_update_step &&
           (mode_ == kOffline || !input_data_queue_.empty())) {
      if (!input_data_queue_.empty()) {
        const auto &data = input_data_queue_.front();
        for (uint32_t i = 1; i < data.size(); ++i) {
          game_core_->GetPlayer(i)->SetInputData(data[i].input_data);
          game_core_->GetPlayer(i)->SelectedUnit() = data[i].selected_unit;
        }
        input_data_queue_.pop();
      }
      game_core_->Update();
      updated_step_++;
    }
  }
  if (render_) {
    game_core_->Render();
  }
}

void App::UpdateDynamicBuffer() {
  auto &object_settings = GetObjectSettings();
  auto &texture_infos = GetTextureInfos();
  std::memcpy(&(*device_object_settings_)[0], object_settings.data(),
              object_settings.size() * sizeof(ObjectSettings));
  std::memcpy(&(*device_texture_infos_)[0], texture_infos.data(),
              texture_infos.size() * sizeof(TextureInfo));
  (*device_global_settings_)[0].world_to_camera = GetCameraTransform(fov_y_);
}

void App::CaptureInput() {
  if (mode_ == kOffline || mode_ == kClient) {
    auto window = core_->GetWindow();
    for (int i = 0; i < kKeyRange; i++) {
      input_data_.key_down[i] = (glfwGetKey(window, i) == GLFW_PRESS);
    }
    static bool mouse_button_state[kMouseButtonRange] = {};
    for (int i = 0; i < kMouseButtonRange; i++) {
      if (i == GLFW_MOUSE_BUTTON_LEFT && ImGui::GetIO().WantCaptureMouse) {
        input_data_.mouse_button_down[i] = false;
        continue;
      }
      input_data_.mouse_button_down[i] =
          (glfwGetMouseButton(window, i) == GLFW_PRESS);
      input_data_.mouse_button_clicked[i] =
          (input_data_.mouse_button_down[i] && !mouse_button_state[i]);
      mouse_button_state[i] = input_data_.mouse_button_down[i];
    }
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    xpos += 0.5f;
    ypos += 0.5f;
    input_data_.mouse_cursor_position =
        glm::inverse(GetCameraTransform(fov_y_)) *
        glm::vec4{
            (glm::vec2{xpos, ypos} / glm::vec2{width, height}) * 2.0f - 1.0f,
            0.0f, 1.0f};
  }
  if (mode_ == kOffline) {
    game_core_->GetPlayer(my_player_id_)->SetInputData(input_data_);
    game_core_->GetPlayer(my_player_id_)->SelectedUnit() = selected_unit_;
  }
}

void App::SetScene() {
  game_core_->Reset();
  if (mode_ == kOffline) {
    my_player_id_ = game_core_->AddPlayer();
    auto enemy_player_id = game_core_->AddPlayer();
    game_core_->SetRenderPerspective(my_player_id_);
  } else if (mode_ == kClient) {
    if (client_) {
      uint32_t player_cnt = client_->GetPlayerCount();
      for (uint32_t i = 0; i < player_cnt; ++i) {
        game_core_->AddPlayer();
      }
      my_player_id_ = client_->GetPlayerId();
    } else {
      my_player_id_ = 0;
    }
  } else if (mode_ == kServer) {
    my_player_id_ = 0;
  }
}

glm::mat4 App::GetCameraTransform(float fov_y) const {
  auto inv_fov_y = 1.0f / fov_y;
  return glm::scale(
             glm::mat4{1.0f},
             glm::vec3{inv_fov_y / ((float)core_->GetFramebufferWidth() /
                                    (float)core_->GetFramebufferHeight()),
                       -inv_fov_y, 1.0f}) *
         glm::inverse(
             glm::translate(glm::mat4{1.0f},
                            glm::vec3{game_core_->GetCameraPosition(), 0.0f}) *
             glm::rotate(glm::mat4{1.0f}, game_core_->GetCameraRotation(),
                         glm::vec3{0.0f, 0.0f, 1.0f}));
}

void App::UpdateImGui() {
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  ImGui::SetNextWindowPos(ImVec2{0.0f, 0.0f}, ImGuiCond_Once);
  ImGui::SetNextWindowBgAlpha(0.3f);
  if (ImGui::Begin(u8"调试窗口", nullptr,
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
                       ImGuiWindowFlags_AlwaysAutoResize)) {
    static const char *const mode_list[] = {u8"单机", u8"联机（客户端）",
                                            u8"联机（服务器）"};
    int mode_id = chosen_mode_;
    ImGui::Combo(u8"选择模式", &mode_id, mode_list, 3u);
    chosen_mode_ = Mode(mode_id);
    if (chosen_mode_ == kClient) {
      ImGui::InputText(u8"服务器IP", ip_, max_ip_length - 1);
      int port = port_;
      ImGui::InputInt(u8"端口", &port);
      port_ = port;
    }
    if (chosen_mode_ == kServer) {
      int port = port_;
      ImGui::InputInt(u8"端口", &port);
      port_ = port;
    }
    ImGui::SameLine();
    if (ImGui::Button(u8"确定并重启")) {
      should_reset_ = true;
    }

    if (!message_.empty()) {
      ImGui::Text("%s", message_.c_str());
    }

    if (mode_ == kServer) {
      ImGui::Checkbox(u8"渲染", &render_);
      if (server_) {
        ImGui::SameLine();
        if (server_->IsRunning()) {
          if (ImGui::Button(u8"开始")) {
            server_->Start();
            should_start_ = true;
          }
        } else {
          if (ImGui::Button(u8"停止")) {
            server_->Stop();
            Stop();
          }
        }
      }
    } else {
      render_ = true;
    }

    if (mode_ == kOffline || mode_ == kClient) {
      const auto &selectable_list = game_core_->GetSelectableUnitList();
      ImGui::Combo(u8"选择你的单位（重生后生效）", &selected_unit_,
                   selectable_list.data(), selectable_list.size());
    }

    auto player = game_core_->GetPlayer(my_player_id_);
    if (player) {
      auto selectable_list = game_core_->GetSelectableUnitList();
      auto selectable_list_skill = game_core_->GetSelectableUnitListSkill();
      if (mode_ == kOffline) {
        if (ImGui::Button(u8"自毁")) {
          auto unit = game_core_->GetUnit(player->GetPrimaryUnitId());
          if (unit) {
            game_core_->PushEventRemoveUnit(unit->GetId());
          }
        }
      }
      auto unit = game_core_->GetUnit(player->GetPrimaryUnitId());
      if (unit) {
        ImGui::Text(u8"生命值: %.1f / %.1f",
                    unit->GetHealth() * unit->GetMaxHealth(),
                    unit->GetMaxHealth());
        ImGui::ProgressBar(unit->GetHealth());
        for (int i = 0; i < selectable_list.size(); i++) {
          if (selectable_list[i] ==
              unit->UnitName() + std::string(" - By ") + unit->Author()) {
            if (selectable_list_skill[i] == true) {
              std::vector<Skill> skill_list = unit->GetSkills();
              for (int i = 0; i < skill_list.size(); i++) {
                if (skill_list[i].type == B) {
                  if (skill_list[i].bullet_total_number > 1) {
                    if (skill_list[i].time_total > 0 &&
                        skill_list[i].time_remain > 0) {
                      ImGui::Text(u8"当前子弹类型：%s ",
                                  skill_list[i].name.c_str());
                      if (!skill_list[i].description.empty()) {
                        ImGui::SameLine();
                        HelpMarker(skill_list[i].description.c_str());
                      }
                      ImGui::SameLine();
                      ImGui::Text(
                          u8" 第%d种 / 共%d种"
                          u8" 子弹切换冷却时间：%d "
                          u8"s "
                          u8"/ %d s",
                          skill_list[i].bullet_type,
                          skill_list[i].bullet_total_number,
                          skill_list[i].time_remain / 60,
                          skill_list[i].time_total / 60);
                      ImGui::ProgressBar((double)skill_list[i].time_remain /
                                         skill_list[i].time_total);
                    } else {
                      ImGui::Text(u8"当前子弹类型：%s ",
                                  skill_list[i].name.c_str());
                      if (!skill_list[i].description.empty()) {
                        ImGui::SameLine();
                        HelpMarker(skill_list[i].description.c_str());
                      }
                      ImGui::SameLine();
                      if (skill_list[i].switch_bullet) {
                        ImGui::Text(u8" 第%d种/共%d种 子弹可切换至",
                                    skill_list[i].bullet_type,
                                    skill_list[i].bullet_total_number);
                        {
                          ImGui::BeginGroup();
                          for (int j = 1;
                               j <= skill_list[i].bullet_total_number; j++) {
                            if (skill_list[i].bullet_type != j) {
                              if (ImGui::Button(std::to_string(j).c_str())) {
                                skill_list[i].switch_bullet(j);
                              }
                              if (j != skill_list[i].bullet_total_number)
                                ImGui::SameLine();
                            }
                          }
                          ImGui::EndGroup();
                        }
                      } else {
                        ImGui::Text(u8" 第%d种/共%d种 子弹可切换",
                                    skill_list[i].bullet_type,
                                    skill_list[i].bullet_total_number);
                      }
                    }
                  } else if (skill_list[i].bullet_total_number == 1) {
                    ImGui::Text(u8"子弹类型：%s", skill_list[i].name.c_str());
                    if (!skill_list[i].description.empty()) {
                      ImGui::SameLine();
                      HelpMarker(skill_list[i].description.c_str());
                    }
                  }
                }
              }
              for (int i = 0; i < skill_list.size(); i++) {
                if (skill_list[i].type == B)
                  continue;
                if (skill_list[i].time_remain) {
                  ImGui::Text(u8"%s ", skill_list[i].name.c_str());
                  if (!skill_list[i].description.empty()) {
                    ImGui::SameLine();
                    HelpMarker(skill_list[i].description.c_str());
                  }
                  ImGui::SameLine();
                  if (skill_list[i].type == E) {
                    ImGui::Text(u8" (按E键释放) 技能冷却时间: %d s / %d s",
                                skill_list[i].time_remain / 60,
                                skill_list[i].time_total / 60);
                  } else if (skill_list[i].type == Q) {
                    ImGui::Text(u8" (按Q键释放) 技能冷却时间: %d s / %d s",
                                skill_list[i].time_remain / 60,
                                skill_list[i].time_total / 60);
                  } else if (skill_list[i].type == battle_game::SkillType::R) {
                    ImGui::Text(u8" (按R键释放) 技能冷却时间: %d s / %d s",
                                skill_list[i].time_remain / 60,
                                skill_list[i].time_total / 60);
                  } else {
                    ImGui::Text(u8" (被动技能) 技能冷却时间: %d s / %d s",
                                skill_list[i].time_remain / 60,
                                skill_list[i].time_total / 60);
                  }
                  ImGui::ProgressBar((double)skill_list[i].time_remain /
                                     skill_list[i].time_total);
                } else {
                  if (skill_list[i].type == E) {
                    ImGui::Text(u8"%s ", skill_list[i].name.c_str());
                    if (!skill_list[i].description.empty()) {
                      ImGui::SameLine();
                      HelpMarker(skill_list[i].description.c_str());
                    }
                    ImGui::SameLine();
                    ImGui::Text(u8" (按E键释放) 技能可释放");
                    if (skill_list[i].function) {
                      if (ImGui::Button(u8"点击释放E技能")) {
                        skill_list[i].function();
                      }
                    }
                  } else if (skill_list[i].type == Q) {
                    ImGui::Text(u8"%s ", skill_list[i].name.c_str());
                    if (!skill_list[i].description.empty()) {
                      ImGui::SameLine();
                      HelpMarker(skill_list[i].description.c_str());
                    }
                    ImGui::SameLine();
                    ImGui::Text(u8" (按Q键释放) 技能可释放");
                    if (skill_list[i].function) {
                      if (ImGui::Button(u8"点击释放Q技能")) {
                        skill_list[i].function();
                      }
                    }
                  } else if (skill_list[i].type == battle_game::SkillType::R) {
                    ImGui::Text(u8"%s ", skill_list[i].name.c_str());
                    if (!skill_list[i].description.empty()) {
                      ImGui::SameLine();
                      HelpMarker(skill_list[i].description.c_str());
                    }
                    ImGui::SameLine();
                    ImGui::Text(u8" (按R键释放) 技能可释放");
                    if (skill_list[i].function) {
                      if (ImGui::Button(u8"点击释放R技能")) {
                        skill_list[i].function();
                      }
                    }
                  } else {
                    ImGui::Text(u8"%s ", skill_list[i].name.c_str());
                    if (!skill_list[i].description.empty()) {
                      ImGui::SameLine();
                      HelpMarker(skill_list[i].description.c_str());
                    }
                    ImGui::SameLine();
                    ImGui::Text(u8" (被动技能) 技能可释放");
                  }
                }
              }
            }
          }
        }
      } else {
        ImGui::Text(u8"已死亡,等待%d秒后复活。",
                    player->GetResurrectionCountDown() / kTickPerSecond);
      }
    }
    ImGui::End();
  }
  ImGui::Render();
}
}  // namespace battle_game
