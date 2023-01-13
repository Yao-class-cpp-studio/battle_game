#pragma once
#include "battle_game/core/bullet.h"
#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/input_data.h"
#include "battle_game/core/obstacle.h"
#include "battle_game/core/obstacles/obstacles.h"
#include "battle_game/core/particle.h"
#include "battle_game/core/particles/particles.h"
#include "battle_game/core/player.h"
#include "battle_game/core/unit.h"
#include "battle_game/core/units/units.h"
#include "battle_game/graphics/graphics.h"
#include "functional"
#include "grassland/grassland.h"
#include "map"
#include "queue"
#include "random"
#include "vector"

namespace battle_game {
constexpr int kTickPerSecond = 60;
constexpr float kSecondPerTick = 1.0f / float(kTickPerSecond);
class GameCore {
 public:
  GameCore();

  void SetScene();

  template <class UnitType, class... Args>
  void AddPrimaryUnitAllocationFunction(Args... args);

  void GeneratePrimaryUnitList();
  uint32_t AllocatePrimaryUnit(uint32_t player_id);
  [[nodiscard]] std::vector<const char *> GetSelectableUnitList() const;
  [[nodiscard]] const std::vector<bool> &GetSelectableUnitListSkill() const {
    return selectable_unit_list_skill_;
  }

  void Update();
  void Render();

  template <class UnitType, class... Args>
  uint32_t AddUnit(uint32_t player_id, Args... args) {
    auto unit_index = unit_index_++;
    units_[unit_index] =
        std::make_unique<UnitType>(this, unit_index, player_id, args...);
    return unit_index;
  }

  template <class ObstacleType, class... Args>
  uint32_t AddObstacle(glm::vec2 position,
                       float rotation = 0.0f,
                       Args... args) {
    auto obstacle_index = obstacle_index_++;
    obstacles_[obstacle_index] = std::make_unique<ObstacleType>(
        this, obstacle_index, position, rotation, args...);
    return obstacle_index;
  }

  template <class BulletType, class... Args>
  uint32_t AddBullet(uint32_t unit_id,
                     uint32_t player_id,
                     glm::vec2 position,
                     float rotation = 0.0f,
                     float damage_scale = 1.0f,
                     Args... args) {
    if (IsOutOfRange(position)) {
      return 0;
    }
    auto bullet_index = bullet_index_++;
    bullets_[bullet_index] =
        std::make_unique<BulletType>(this, bullet_index, unit_id, player_id,
                                     position, rotation, damage_scale, args...);
    return bullet_index;
  }
  template <class ParticleType, class... Args>
  uint32_t AddParticle(glm::vec2 position,
                       float rotation = 0.0f,
                       Args... args) {
    if (IsOutOfRange(position)) {
      return 0;
    }
    auto particle_index = particle_index_++;
    particles_[particle_index] = std::make_unique<ParticleType>(
        this, particle_index, position, rotation, args...);
    return particle_index;
  }

  uint32_t AddPlayer();

  [[nodiscard]] Unit *GetUnit(uint32_t unit_id) const;
  [[nodiscard]] Bullet *GetBullet(uint32_t bullet_id) const;
  [[nodiscard]] Particle *GetParticle(uint32_t particle_id) const;
  [[nodiscard]] Obstacle *GetObstacle(uint32_t obstacle_id) const;
  [[nodiscard]] Player *GetPlayer(uint32_t player_id) const;
  [[nodiscard]] const std::map<uint32_t, std::unique_ptr<Unit>> &GetUnits()
      const {
    return units_;
  }
  [[nodiscard]] const std::map<uint32_t, std::unique_ptr<Bullet>> &GetBullets()
      const {
    return bullets_;
  }
  [[nodiscard]] const std::map<uint32_t, std::unique_ptr<Particle>>
      &GetParticles() const {
    return particles_;
  }
  [[nodiscard]] const std::map<uint32_t, std::unique_ptr<Obstacle>>
      &GetObstacles() const {
    return obstacles_;
  }
  [[nodiscard]] const std::map<uint32_t, std::unique_ptr<Player>> &GetPlayers()
      const {
    return players_;
  }

  void SetRenderPerspective(uint32_t player_id);
  [[nodiscard]] uint32_t GetRenderPerspective() const;
  [[nodiscard]] glm::vec4 GetPlayerColor(uint32_t player_id) const;

  [[nodiscard]] bool IsOutOfRange(glm::vec2 p) const;
  [[nodiscard]] bool IsBlockedByObstacles(glm::vec2 p) const;
  [[nodiscard]] Obstacle *GetBlockedObstacle(glm::vec2 p) const;

  void PushEventMoveUnit(uint32_t unit_id, glm::vec2 new_position);
  void PushEventRotateUnit(uint32_t unit_id, float new_rotation);
  void PushEventDealDamage(uint32_t dst_unit_id,
                           uint32_t src_unit_id,
                           float damage);
  void PushEventKillUnit(uint32_t dst_unit_id, uint32_t src_unit_id);
  void PushEventRemoveObstacle(uint32_t obstacle_id);
  void PushEventRemoveBullet(uint32_t bullet_id);
  void PushEventRemoveParticle(uint32_t particle_id);
  void PushEventRemoveUnit(uint32_t unit_id);

  template <class BulletType, class... Args>
  void PushEventGenerateBullet(uint32_t unit_id,
                               uint32_t player_id,
                               glm::vec2 position,
                               float rotation = 0.0f,
                               float damage_scale = 1.0f,
                               Args... args) {
    event_queue_.emplace([=]() {
      AddBullet<BulletType>(unit_id, player_id, position, rotation,
                            damage_scale, args...);
    });
  }

  template <class ObstacleType, class... Args>
  void PushEventGenerateObstacle(glm::vec2 position,
                                 float rotation = 0.0f,
                                 Args... args) {
    event_queue_.emplace(
        [=]() { AddObstacle<ObstacleType>(position, rotation, args...); });
  }

  template <class ParticleType, class... Args>
  void PushEventGenerateParticle(glm::vec2 position,
                                 float rotation = 0.0f,
                                 Args... args) {
    event_queue_.emplace(
        [=]() { AddParticle<ParticleType>(position, rotation, args...); });
  }

  void ProcessEventQueue();

  void SetCamera(glm::vec2 position, float rotation = 0.0f);
  [[nodiscard]] glm::vec2 GetCameraPosition() const {
    return camera_position_;
  }
  [[nodiscard]] float GetCameraRotation() const {
    return camera_rotation_;
  }

  /*
   * Return a uniform random real number in range [0, 1]
   * */
  float RandomFloat();

  /*
   * Return a uniform random integer number in range [low_bound, high_bound]
   * */
  int RandomInt(int low_bound, int high_bound);
  glm::vec2 RandomOnCircle();
  glm::vec2 RandomInCircle();

 private:
  std::queue<std::function<void()>> event_queue_;

  std::map<uint32_t, std::unique_ptr<Unit>> units_;
  uint32_t unit_index_{1};
  std::map<uint32_t, std::unique_ptr<Bullet>> bullets_;
  uint32_t bullet_index_{1};
  std::map<uint32_t, std::unique_ptr<Particle>> particles_;
  uint32_t particle_index_{1};
  std::map<uint32_t, std::unique_ptr<Obstacle>> obstacles_;
  uint32_t obstacle_index_{1};
  std::map<uint32_t, std::unique_ptr<Player>> players_;
  uint32_t player_index_{1};

  uint32_t render_perspective_{
      0};  // This is a player id, defines which player is currently watching
           // the scene. 0 denote neutral.

  glm::vec2 camera_position_{0.0f};
  float camera_rotation_{0.0f};

  glm::vec2 boundary_low_{-10.0f, -10.0f};
  glm::vec2 boundary_high_{10.0f, 10.0f};
  uint32_t boundary_model_{};

  std::mt19937 random_device_{0};

  std::vector<std::pair<glm::vec2, float>> respawn_points_;
  std::vector<std::function<uint32_t(uint32_t)>>
      primary_unit_allocation_functions_;
  std::vector<std::string> selectable_unit_list_;
  std::vector<bool> selectable_unit_list_skill_;
};

template <class BulletType, class... Args>
void Unit::GenerateBullet(glm::vec2 position,
                          float rotation,
                          float damage_scale,
                          Args... args) {
  game_core_->PushEventGenerateBullet<BulletType>(
      id_, player_id_, position, rotation, damage_scale, args...);
}
}  // namespace battle_game
