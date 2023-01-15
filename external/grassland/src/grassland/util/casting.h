#pragma once
#include <grassland/util/logging.h>

namespace grassland::util {
template <class dst_ty, class src_ty>
dst_ty *CastPtr(src_ty *ptr) {
  auto *result = dynamic_cast<dst_ty *>(ptr);
  LAND_ASSERT(result);
  return result;
}

class Instance {
 public:
  virtual ~Instance() = default;
  template <class _ty>
  _ty *QueryInterface() const {
    return CastPtr<_ty, Instance>(this);
  }
};
}  // namespace grassland::util
