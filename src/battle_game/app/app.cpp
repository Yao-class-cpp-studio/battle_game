#include "battle_game/app/app.h"

#include <string>

#include "battle_game/core/object.h"
#include "battle_game/graphics/util.h"
namespace battle_game {
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

App::App(const AppSettings &app_settings, GameCore *game_core) {
  game_core_ = game_core;
  vulkan::framework::CoreSettings core_settings;
  core_settings.window_title = "Battle Game";
  core_settings.window_width = app_settings.width;
  core_settings.window_height = app_settings.height;
  core_ = std::make_unique<vulkan::framework::Core>(core_settings);
  SetGlobalCore(core_.get());
}

void App::Run() {
  OnInit();

  while (!glfwWindowShouldClose(core_->GetWindow())) {
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

  SetScene();
  core_->ImGuiInit(frame_image_.get(), "../../fonts/NotoSansSC-Regular.otf",
                   20.0f);
}

void App::OnLoop() {
  OnUpdate();
  OnRender();
}

void App::OnClose() {
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
  static auto begin_time = std::chrono::steady_clock::now();
  auto current_time = std::chrono::steady_clock::now();
  auto time_passed =
      double((current_time - begin_time) / std::chrono::nanoseconds(1)) * 1e-9;
  static uint64_t updated_step = 0;
  uint64_t target_update_step = std::lround(time_passed / kSecondPerTick);
  while (updated_step < target_update_step) {
    game_core_->Update();
    updated_step++;
  }
  game_core_->Render();
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
  InputData input_data;
  auto window = core_->GetWindow();
  for (int i = 0; i < kKeyRange; i++) {
    input_data.key_down[i] = (glfwGetKey(window, i) == GLFW_PRESS);
  }
  static bool mouse_button_state[kMouseButtonRange] = {};
  for (int i = 0; i < kMouseButtonRange; i++) {
    if (i == GLFW_MOUSE_BUTTON_LEFT && ImGui::GetIO().WantCaptureMouse) {
      input_data.mouse_button_down[i] = false;
      continue;
    }
    input_data.mouse_button_down[i] =
        (glfwGetMouseButton(window, i) == GLFW_PRESS);
    input_data.mouse_button_clicked[i] =
        (input_data.mouse_button_down[i] && !mouse_button_state[i]);
    mouse_button_state[i] = input_data.mouse_button_down[i];
  }
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);
  int width, height;
  glfwGetWindowSize(window, &width, &height);
  xpos += 0.5f;
  ypos += 0.5f;
  input_data.mouse_cursor_position =
      glm::inverse(GetCameraTransform(fov_y_)) *
      glm::vec4{
          (glm::vec2{xpos, ypos} / glm::vec2{width, height}) * 2.0f - 1.0f,
          0.0f, 1.0f};
  game_core_->GetPlayer(my_player_id_)->SetInputData(input_data);
}

void App::SetScene() {
  my_player_id_ = game_core_->AddPlayer();
  auto enemy_player_id = game_core_->AddPlayer();
  game_core_->SetRenderPerspective(my_player_id_);
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
    auto player = game_core_->GetPlayer(my_player_id_);
    if (player) {
      auto selectable_list = game_core_->GetSelectableUnitList();
      auto selectable_list_skill = game_core_->GetSelectableUnitListSkill();
      ImGui::Combo(u8"选择你的单位（重生后生效）", &player->SelectedUnit(),
                   selectable_list.data(), selectable_list.size());
      if (ImGui::Button(u8"自毁")) {
        auto unit = game_core_->GetUnit(player->GetPrimaryUnitId());
        if (unit) {
          game_core_->PushEventRemoveUnit(unit->GetId());
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
