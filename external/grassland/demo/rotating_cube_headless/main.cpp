#include "app.h"
#include "grassland/util/logging.h"
#include "vulkan/vulkan.h"

int main() {
  App app(1280, 720, "Grassland App");
  app.Run();
}
