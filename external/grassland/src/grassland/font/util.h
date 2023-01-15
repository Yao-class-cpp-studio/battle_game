#pragma once
#include "ft2build.h"
#include "glm/glm.hpp"
#include FT_FREETYPE_H
namespace grassland::font {
void FTErrorHandler(FT_Error err, const char *file, int line, const char *code);
#define FTCall(x) grassland::font::FTErrorHandler(x, __FILE__, __LINE__, #x)
}  // namespace grassland::font
