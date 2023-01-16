#include "grassland/font/util.h"

#include "grassland/util/util.h"
namespace grassland::font {
void FTErrorHandler(FT_Error err,
                    const char *file,
                    int line,
                    const char *code) {
  if (err) {
    LAND_ERROR("({}:{} #{}) {}.", file, line, err, code);
  }
}
}  // namespace grassland::font
