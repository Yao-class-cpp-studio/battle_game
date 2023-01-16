#include "battle_game/core/game_core.h"

namespace battle_game {

GameCore::GameCore() {
  auto mgr = AssetsManager::GetInstance();
  boundary_model_ = mgr->RegisterModel(
      std::vector<ObjectVertex>{
          {{-1.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
          {{1.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
          {{-1.0f, -1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 0.0f}},
          {{1.0f, -1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 0.0f}}},
      std::vector<uint32_t>{0, 1, 2, 1, 2, 3});
  SetScene();
  GeneratePrimaryUnitList();
}

/*
 * Update for 1 game tick.
 * Order: obstacles, bullets, units, particles
 * */
void GameCore::Update() {
  for (auto &player : players_) {
    player.second->Update();
  }

  for (auto &obstacle : obstacles_) {
    obstacle.second->Update();
  }
  for (auto &bullet : bullets_) {
    if (IsOutOfRange(bullet.second->GetPosition())) {
      PushEventRemoveBullet(bullet.first);
      continue;
    }
    bullet.second->Update();
  }
  for (auto &units : units_) {
    units.second->Update();
  }
  for (auto &particle : particles_) {
    if (IsOutOfRange(particle.second->GetPosition())) {
      PushEventRemoveParticle(particle.first);
      continue;
    }
    particle.second->Update();
  }
  ProcessEventQueue();
}

/*
 * Render the objects
 * Order: obstacles, bullets, units, particles, life bars, helper
 * */
void GameCore::Render() {
  auto observer = GetPlayer(render_perspective_);
  if (observer) {
    auto observing_unit = GetUnit(observer->GetPrimaryUnitId());
    if (observing_unit) {
      SetCamera(observing_unit->GetPosition(), 0.0f);
    }
  }

  SetColor();
  SetTexture();
  SetTransformation(
      glm::vec2{boundary_low_.x, (boundary_low_.y + boundary_high_.y) * 0.5f},
      glm::radians(-90.0f),
      {(boundary_high_.y - boundary_low_.y) * 0.5f, 0.1f});
  DrawModel(boundary_model_);
  SetTransformation(
      glm::vec2{boundary_high_.x, (boundary_low_.y + boundary_high_.y) * 0.5f},
      glm::radians(90.0f), {(boundary_high_.y - boundary_low_.y) * 0.5f, 0.1f});
  DrawModel(boundary_model_);
  SetTransformation(
      glm::vec2{(boundary_low_.x + boundary_high_.x) * 0.5f, boundary_low_.y},
      glm::radians(0.0f), {(boundary_high_.x - boundary_low_.x) * 0.5f, 0.1f});
  DrawModel(boundary_model_);
  SetTransformation(
      glm::vec2{(boundary_low_.x + boundary_high_.x) * 0.5f, boundary_high_.y},
      glm::radians(180.0f),
      {(boundary_high_.x - boundary_low_.x) * 0.5f, 0.1f});
  DrawModel(boundary_model_);

  for (auto &obstacle : obstacles_) {
    obstacle.second->Render();
  }
  for (auto &bullet : bullets_) {
    bullet.second->Render();
  }
  for (auto &units : units_) {
    units.second->Render();
  }
  for (auto &particle : particles_) {
    particle.second->Render();
  }
  for (auto &units : units_) {
    units.second->RenderLifeBar();
  }
  if (observer) {
    auto observing_unit = GetUnit(observer->GetPrimaryUnitId());
    if (observing_unit) {
      observing_unit->RenderHelper();
    }
  }
}

uint32_t GameCore::AddPlayer() {
  uint32_t player_id = player_index_++;
  players_[player_id] = std::make_unique<Player>(this, player_id);
  return player_id;
}

glm::vec4 GameCore::GetPlayerColor(uint32_t player_id) const {
  if (render_perspective_ == 0) {
    return glm::vec4{0.5f, 1.0f, 0.5f, 1.0f};
  } else if (render_perspective_ == player_id) {
    return glm::vec4{1.0f, 1.0f, 1.0f, 1.0f};
  } else {
    return glm::vec4{1.0f, 0.5f, 0.5f, 1.0f};
  }
}

void GameCore::SetRenderPerspective(uint32_t player_id) {
  render_perspective_ = player_id;
}

uint32_t GameCore::GetRenderPerspective() const {
  return render_perspective_;
}

void GameCore::ProcessEventQueue() {
  while (!event_queue_.empty()) {
    event_queue_.front()();
    event_queue_.pop();
  }
}

bool GameCore::IsBlockedByObstacles(glm::vec2 p) const {
  if (IsOutOfRange(p)) {
    return true;
  }
  for (auto &obstacle : obstacles_) {
    if (obstacle.second->IsBlocked(p)) {
      return true;
    }
  }
  return false;
}

Obstacle *GameCore::GetBlockedObstacle(glm::vec2 p) const {
  if (!IsOutOfRange(p)) {
    for (auto &obstacle : obstacles_)
      if (obstacle.second->IsBlocked(p)) {
        return obstacle.second.get();
      }
  }
  return nullptr;
}

void GameCore::PushEventMoveUnit(uint32_t unit_id, glm::vec2 new_position) {
  event_queue_.emplace([this, unit_id, new_position]() {
    auto unit = GetUnit(unit_id);
    if (unit) {
      unit->SetPosition(new_position);
    }
  });
}

void GameCore::PushEventRotateUnit(uint32_t unit_id, float new_rotation) {
  event_queue_.emplace([this, unit_id, new_rotation]() {
    auto unit = GetUnit(unit_id);
    if (unit) {
      unit->SetRotation(new_rotation);
    }
  });
}

Unit *GameCore::GetUnit(uint32_t unit_id) const {
  if (!units_.count(unit_id)) {
    return nullptr;
  }
  return units_.at(unit_id).get();
}

Bullet *GameCore::GetBullet(uint32_t bullet_id) const {
  if (!bullets_.count(bullet_id)) {
    return nullptr;
  }
  return bullets_.at(bullet_id).get();
}

Particle *GameCore::GetParticle(uint32_t particle_id) const {
  if (!particles_.count(particle_id)) {
    return nullptr;
  }
  return particles_.at(particle_id).get();
}

Obstacle *GameCore::GetObstacle(uint32_t obstacle_id) const {
  if (!obstacles_.count(obstacle_id)) {
    return nullptr;
  }
  return obstacles_.at(obstacle_id).get();
}

Player *GameCore::GetPlayer(uint32_t player_id) const {
  if (!players_.count(player_id)) {
    return nullptr;
  }
  return players_.at(player_id).get();
}

void GameCore::SetCamera(glm::vec2 position, float rotation) {
  camera_position_ = position;
  camera_rotation_ = rotation;
}

void GameCore::PushEventDealDamage(uint32_t dst_unit_id,
                                   uint32_t src_unit_id,
                                   float damage) {
  event_queue_.emplace([=]() {
    auto unit = GetUnit(dst_unit_id);
    if (unit) {
      unit->SetHealth(unit->GetHealth() - damage / unit->GetMaxHealth());
      if (unit->GetHealth() <= 0.0f) {
        PushEventKillUnit(dst_unit_id, src_unit_id);
      }
    }
  });
}

void GameCore::PushEventRemoveObstacle(uint32_t obstacle_id) {
  event_queue_.emplace([=]() {
    if (obstacles_.count(obstacle_id)) {
      obstacles_.erase(obstacle_id);
    }
  });
}

void GameCore::PushEventRemoveBullet(uint32_t bullet_id) {
  event_queue_.emplace([=]() {
    if (bullets_.count(bullet_id)) {
      bullets_.erase(bullet_id);
    }
  });
}

void GameCore::PushEventRemoveParticle(uint32_t particle_id) {
  event_queue_.emplace([=]() {
    if (particles_.count(particle_id)) {
      particles_.erase(particle_id);
    }
  });
}

void GameCore::PushEventRemoveUnit(uint32_t unit_id) {
  event_queue_.emplace([=]() {
    if (units_.count(unit_id)) {
      units_.erase(unit_id);
    }
  });
}

void GameCore::PushEventKillUnit(uint32_t dst_unit_id, uint32_t src_unit_id) {
  event_queue_.emplace([=]() { PushEventRemoveUnit(dst_unit_id); });
}

float GameCore::RandomFloat() {
  return std::uniform_real_distribution<float>()(random_device_);
}

int GameCore::RandomInt(int low_bound, int high_bound) {
  return std::uniform_int_distribution<int>(low_bound,
                                            high_bound)(random_device_);
}

void GameCore::SetScene() {
  AddObstacle<obstacle::Block>(glm::vec2{-3.0f, 4.0f});
  AddObstacle<obstacle::River>(glm::vec2{3.0f, 0.0f});
  AddObstacle<obstacle::ReboundingBlock>(glm::vec2{-10.0f, -10.0f},
                                         0.78539816339744830961566084581988f);
  AddObstacle<obstacle::ReboundingBlock>(glm::vec2{10.0f, -10.0f},
                                         0.78539816339744830961566084581988f);
  AddObstacle<obstacle::ReboundingBlock>(glm::vec2{10.0f, 10.0f},
                                         0.78539816339744830961566084581988f);
  AddObstacle<obstacle::ReboundingBlock>(glm::vec2{-10.0f, 10.0f},
                                         0.78539816339744830961566084581988f);
  respawn_points_.emplace_back(glm::vec2{0.0f}, 0.0f);
  respawn_points_.emplace_back(glm::vec2{3.0f, 4.0f}, glm::radians(90.0f));
  boundary_low_ = {-10.0f, -10.0f};
  boundary_high_ = {10.0f, 10.0f};
}

uint32_t GameCore::AllocatePrimaryUnit(uint32_t player_id) {
  auto player = GetPlayer(player_id);
  if (!player) {
    return 0;
  }
  auto unit_id =
      primary_unit_allocation_functions_[player->SelectedUnit()](player_id);
  auto unit = GetUnit(unit_id);
  auto respawn_point =
      respawn_points_[RandomInt(0, int(respawn_points_.size()) - 1)];
  unit->SetPosition(respawn_point.first);
  unit->SetRotation(respawn_point.second);
  return unit_id;
}

glm::vec2 GameCore::RandomOnCircle() {
  auto theta = RandomFloat() * glm::pi<float>() * 2.0f;
  return {std::sin(theta), std::cos(theta)};
}

glm::vec2 GameCore::RandomInCircle() {
  auto theta = RandomFloat() * glm::pi<float>() * 2.0f;
  auto length = std::sqrt(RandomFloat());
  return {std::sin(theta) * length, std::cos(theta) * length};
}

bool GameCore::IsOutOfRange(glm::vec2 p) const {
  return p.x < boundary_low_.x || p.x > boundary_high_.x ||
         p.y < boundary_low_.y || p.y > boundary_high_.y;
}

std::vector<const char *> GameCore::GetSelectableUnitList() const {
  std::vector<const char *> result;
  for (auto &selectable_unit : selectable_unit_list_) {
    result.emplace_back(selectable_unit.data());
  }
  return result;
}
}  // namespace battle_game
