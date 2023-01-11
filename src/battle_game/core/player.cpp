#include "battle_game/core/player.h"

#include "battle_game/core/game_core.h"

namespace battle_game {
Player::Player(GameCore *game_core, uint32_t id)
    : game_core_(game_core), id_(id) {
  bufflist_.push_back(
      battle_game::Buff("stunning", 100, 1.5, false, false, 0));  // BuffID 100
  buffs_[100].push_back(1);
}

void Player::AddBuff(int buff) {
  int number = bufflist_.size();
  int index = -1;
  for (int i = 0; i < number; i++) {
    if (bufflist_[i].Getbuffid() == buff)
      index = i;
  }
  if (index > -1) {
    if (!bufflist_[index].Getissupportmanylayer()) {
      buffs_[buff].clear();
    }
    buffs_[buff].push_back(
        uint32_t(kTickPerSecond * bufflist_[index].Gettime()));
  }
}

void Player::RebirthUpdateBuff() {
  int size = bufflist_.size();
  for (int i = 0; i < size; i++) {
    int tmp = bufflist_[i].Getremovewhendie();
    int id = bufflist_[i].Getbuffid();
    if (!tmp) {
      if (tmp < 0) {
        buffs_[id].clear();
      } else {
        if (buffs_[id].size() > tmp) {
          for (int j = 0; j < tmp; j++) {
            buffs_[id].pop_back();
          }
        } else {
          buffs_[id].clear();
        }
      }
    }
    int layer = buffs_[id].size();
    uint32_t time = kTickPerSecond * bufflist_[i].Gettime();
    for (int j = 0; j < layer; j++) {
      buffs_[id][j] = time;
    }
  }
}

void Player::UpdateBuff() {
  int size = bufflist_.size();
  for (int i = 0; i < size; i++) {
    if (!bufflist_[i].Getisinfinitetime()) {
      int id = bufflist_[i].Getbuffid();
      int layer = buffs_[id].size();
      if (layer) {
        auto it = buffs_[id].cbegin();
        int cnt = 0;
        for (int j = 0; j < layer; j++) {
          if (!buffs_[id][j - cnt]) {
            buffs_[id].erase(it);
            cnt++;
          } else {
            buffs_[id][j - cnt]--;
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
  } 
  UpdateBuff();
}
}  // namespace battle_game