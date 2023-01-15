#include <grassland/util/logging.h>

namespace grassland::util {
void assert_handler(bool result, const char *code) {
  if (!result) {
    LAND_ERROR("Grassland assert: {}", code);
  }
}
}  // namespace grassland::util
