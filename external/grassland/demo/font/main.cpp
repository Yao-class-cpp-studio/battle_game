#include "app.h"
#include "grassland/font/font.h"

int main() {
  grassland::font::Factory factory("../fonts/NotoSansSC-Light.otf");
  FontViewer font_viewer(factory.GetChar(L'G'));
  font_viewer.Run();
}
