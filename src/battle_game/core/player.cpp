#include "battle_game/core/player.h"

#include "battle_game/core/game_core.h"

namespace battle_game {
Player::Player(GameCore *game_core, uint32_t id)
    : game_core_(game_core), id_(id) {
  BuffList_.push_back(
      battle_game::Buff("stunning", 1.5, false, false, 0));  // BuffID 0
  buffs_[0].push_back(1);
}

void Player::AddBuff(int buff) {
  if (!BuffList_[buff].IsSupportManyLayer_) {
    buffs_[buff].pop_back();
  }
  buffs_[buff].push_back(uint32_t(kTickPerSecond * BuffList_[buff].Time_));
}

void Player::RebirthUpdateBuff() {
  int size = BuffList_.size();
  for (int i = 0; i < size; i++) {
    int tmp = BuffList_[i].RemoveWhenDie_;
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
    uint32_t time = kTickPerSecond * BuffList_[i].Time_;
    for (int j = 0; j < layer; j++) {
      buffs_[i][j] = time;
    }
  }
}

void Player::UpdateBuff() {
  int size = BuffList_.size();
  for (int i = 0; i < size; i++) {
    if (!BuffList_[i].IsInfiniteTime_) {
      int layer = buffs_[i].size();
      if (layer) {
        auto it = buffs_[i].cbegin();
        int cnt = 0;
        for (int j = 0; j < layer; j++) {
          if (!buffs_[i][j - cnt]) {
            buffs_[i].erase(it);
            cnt++;
          } else {
            buffs_[i][j - cnt]--;
            it++;
          }
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
