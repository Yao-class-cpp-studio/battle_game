#include "battle_game/core/player.h"

#include "battle_game/core/game_core.h"

namespace battle_game {
const char *BuffType[] = {"Stunning"};
const int BN1 = sizeof(BuffType) / sizeof(BuffType[0]);
float Time[BN1] = {1.5};
bool IsSupportManyLayer[BN1] = {false};
bool IsInfiniteTime[BN1] = {false};
int RemoveWhenDie[BN1] = {0};
Player::Player(GameCore *game_core, uint32_t id)
    : game_core_(game_core), id_(id) {
  buffs_[0].push_back(0);
}

void Player::AddBuff(int buff) {
  if (!IsSupportManyLayer[buff]) {
    buffs_[buff].pop_back();
  }
  buffs_[buff].push_back(uint32_t(kTickPerSecond * Time[buff]));
}

void Player::RebirthUpdateBuff() {
  for (int i = 0; i < BN1; i++) {
    int tmp = RemoveWhenDie[i];
    if (!tmp) {
      if (tmp < 0) {
        buffs_[i].clear();
      } else {
        if (buffs_[i].size() > tmp) {
          for (int j = 0; j < tmp; j++) {
            buffs_[i].pop_back();
          }
        } else {
          buffs_[i].clear();
        }
      }
    }
    int layer = buffs_[i].size();
    uint32_t time = kTickPerSecond * Time[i];
    for (int j = 0; j < layer; j++) {
      buffs_[i][j] = time;
    }
  }
}

void Player::UpdateBuff() {
  for (int i = 0; i < BN1; i++) {
    if (!IsInfiniteTime[i]) {
      int layer = buffs_[i].size();
      if (layer) {
        auto it = buffs_[i].cbegin();
        for (int j = 0; j < layer; j++) {
          if (!buffs_[i][j]) {
            buffs_[i].erase(it);
          } else {
            buffs_[i][j]--;
          }
          it++;
        }
      }
    }
  }
}

void Player::Update() {
  auto primary_unit = game_core_->GetUnit(primary_unit_id_);
  if (!primary_unit) {
    if (!resurrection_count_down_) {
      resurrection_count_down_ = kTickPerSecond * 5;  // Respawn after 5 seconds
    }
    resurrection_count_down_--;
    if (!resurrection_count_down_) {
      RebirthUpdateBuff();
      primary_unit_id_ = game_core_->AllocatePrimaryUnit(id_);
    }
  } else {
    UpdateBuff();
  }
}
}  // namespace battle_game
