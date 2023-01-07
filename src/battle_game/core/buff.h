#pragma once
#include "map"
#include "vector"

namespace battle_game {
class Buff {
 public:
  Buff(const char *buffName,
       float time,
       bool issupportmanylayer,
       bool isinfinitetime,
       int removewhendie)
      : buffname_(buffName),
        time_(time),
        issupportmanylayer_(issupportmanylayer),
        isinfinitetime_(isinfinitetime),
        removewhendie_(removewhendie) {
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