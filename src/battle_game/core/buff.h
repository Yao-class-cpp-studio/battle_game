#pragma once
#include "map"
#include "vector"

namespace battle_game {
class Buff {
 public:
  Buff(const char *BuffName,
       float Time,
       bool IsSupportManyLayer,
       bool IsInfiniteTime,
       int RemoveWhenDie)
      : buffname_(BuffName),
        time_(Time),
        issupportmanylayer_(IsSupportManyLayer),
        isinfinitetime_(IsInfiniteTime),
        removewhendie_(RemoveWhenDie) {
  }
  float Gettime() {
    return time_;
  }
  bool Getissupportmanylayer() {
    return issupportmanylayer_;
  }
  bool Getisinfinitetime() {
    return isinfinitetime_;
  }
  int Getremovewhendie() {
    return removewhendie_;
  }

 private:
  const char *buffname_;
  float time_;
  bool issupportmanylayer_;
  bool isinfinitetime_;
  int removewhendie_;
};
}  // namespace battle_game