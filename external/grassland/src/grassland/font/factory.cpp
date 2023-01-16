#include "grassland/font/factory.h"
#include FT_OUTLINE_H
#include "algorithm"
#include "grassland/util/util.h"
#include "set"

namespace {
const int arc_precision = 3;
const int size_scale = 128;
const float inv_size = 1.0f / (64.0f * float(size_scale));

int cross(const glm::ivec2 &a, const glm::ivec2 &b) {
  return a.x * b.y - a.y * b.x;
}

int dot(const glm::ivec2 &v, const glm::ivec2 &u) {
  return v.x * u.x + v.y * u.y;
}

int dot(const glm::ivec3 &v, const glm::ivec3 &u) {
  return v.x * u.x + v.y * u.y + v.z * u.z;
}

int signal(int v) {
  if (v < 0)
    return -1;
  else if (v > 0)
    return 1;
  else
    return 0;
}

struct Segment {
  glm::ivec2 u;
  glm::ivec2 v;
};

struct Line {
  glm::ivec2 n;
  int d;
  explicit Line(const Segment &segment) : n(segment.v - segment.u) {
    std::swap(n.x, n.y);
    n.x = -n.x;
    d = -dot(n, segment.v);
  }
  int operator()(const glm::ivec2 &v) const {
    return dot(v, n) + d;
  }
};

bool operator<(const glm::ivec2 &v0, const glm::ivec2 &v1) {
#define COMPARE(x) \
  if (v0.x < v1.x) \
    return true;   \
  if (v1.x < v0.x) \
    return false;
  COMPARE(x);
  COMPARE(y);
#undef COMPARE
  return false;
}

struct Triangle {
  glm::ivec2 v0;
  glm::ivec2 v1;
  glm::ivec2 v2;
  bool operator<(const Triangle &triangle) const {
#define COMPARE(x)    \
  if (x < triangle.x) \
    return true;      \
  if (triangle.x < x) \
    return false;
    COMPARE(v0);
    COMPARE(v1);
    COMPARE(v2);
#undef COMPARE
    return false;
  }
  [[nodiscard]] bool IsInside(glm::ivec2 v) const {
    auto line2 = Line{Segment{v0, v1}};
    auto line0 = Line{Segment{v1, v2}};
    auto line1 = Line{Segment{v2, v0}};
    return (std::abs(signal(line0(v0)) - signal(line0(v))) < 2) &&
           (std::abs(signal(line1(v1)) - signal(line1(v))) < 2) &&
           (std::abs(signal(line2(v2)) - signal(line2(v))) < 2) &&
           (v != v0 && v != v1 && v != v2);
  }
};

glm::ivec2 rotate90(const glm::ivec2 &v) {
  return glm::ivec2{v.y, -v.x};
}

glm::ivec3 construct_line(const glm::ivec2 &v, const glm::ivec2 &u) {
  auto n = rotate90(u - v);
  return {n, -dot(n, v)};
}

int directional_scan(const glm::ivec2 &v,
                     const glm::ivec2 &u,
                     const glm::ivec2 &p) {
  auto area = cross(v, u);
  auto rv = rotate90(v);
  auto ru = rotate90(u);
  auto line = construct_line(v, u);
  if (signal(dot(v, ru)) * signal(dot(ru, p)) < 0) {
    return 0;
  }
  if (signal(dot(rv, u)) * signal(dot(rv, p)) <= 0) {
    return 0;
  }
  if (area < 0) {
    if (signal(dot(line, glm::ivec3{p, 1.0f})) * signal(line.z) <= 0) {
      return 0;
    }
    return -1;
  } else {
    if (signal(dot(line, glm::ivec3{p, 1.0f})) * signal(line.z) < 0) {
      return 0;
    }
    return 1;
  }
}

bool inside(const std::vector<glm::ivec2> &cycle, const glm::ivec2 &p) {
  int scan_counter = 0;
  for (int i = 0; i < cycle.size(); i++) {
    int j = i + 1;
    if (j == cycle.size()) {
      j = 0;
    }
    scan_counter += directional_scan(cycle[i], cycle[j], p);
  }
  return scan_counter;
}

auto rotate_vector(std::vector<glm::ivec2> &contour, int offset) {
  int sz = int(contour.size());
  for (int i = 0; i < offset; i++) {
    contour.push_back(contour[i]);
  }
  for (int i = 0; i < sz; i++) {
    contour[i] = contour[i + offset];
  }
  contour.resize(sz);
};

auto contour_area(const std::vector<glm::ivec2> &contour) {
  int area = 0;
  int sz = int(contour.size());
  for (int i = 1; i < sz - 1; i++) {
    area += cross(contour[i] - contour[0], contour[i + 1] - contour[0]);
  }
  return area;
}

auto is_intersect(const Segment &seg0, const Segment &seg1) {
  Line line0{seg0};
  Line line1{seg1};
  return (std::abs(signal(line0(seg1.u)) - signal(line0(seg1.v))) == 2) &&
         (std::abs(signal(line1(seg0.u)) - signal(line1(seg0.v))) == 2);
}

void merge_contours(std::vector<glm::ivec2> &outer_contour,
                    std::vector<glm::ivec2> &inner_contour,
                    const std::vector<std::vector<glm::ivec2>> &contours) {
  auto outer_area = contour_area(outer_contour);
  std::reverse(inner_contour.begin(), inner_contour.end());

  int outer_index = -1;
  int inner_index = -1;
  int dist = 0x7fffffff;

  for (int i = 0; i < outer_contour.size(); i++) {
    int i_1 = (i - 1 + int(outer_contour.size())) % int(outer_contour.size());
    int i1 = (i + 1) % int(outer_contour.size());
    for (int j = 0; j < inner_contour.size(); j++) {
      if (std::abs(signal(cross(outer_contour[i] - outer_contour[i_1],
                                inner_contour[j] - outer_contour[i_1])) -
                   signal(outer_area)) == 2)
        continue;
      if (std::abs(signal(cross(outer_contour[i] - inner_contour[j],
                                outer_contour[i1] - inner_contour[j])) -
                   signal(outer_area)) == 2)
        continue;
      Segment seg{outer_contour[i], inner_contour[j]};
      bool pass = true;
      for (auto &contour : contours) {
        for (int k = 0; k < contour.size(); k++) {
          int k1 = (k + 1) % int(contour.size());
          if (is_intersect(seg, Segment{contour[k], contour[k1]})) {
            pass = false;
            break;
          }
        }
        if (!pass)
          break;
      }
      if (pass) {
        auto this_dist = dot(seg.u - seg.v, seg.u - seg.v);
        if (this_dist < dist) {
          dist = this_dist;
          outer_index = i;
          inner_index = j;
        }
      }
    }
  }

  rotate_vector(outer_contour, outer_index);
  rotate_vector(inner_contour, inner_index);
  outer_contour.push_back(outer_contour[0]);
  inner_contour.push_back(inner_contour[0]);
  for (auto &v : inner_contour) {
    outer_contour.push_back(v);
  }
}

void triangulation(std::vector<glm::vec2> &triangles,
                   std::vector<glm::ivec2> &outline) {
  int area = 0;
  for (size_t i = 1; i < outline.size() - 1; i++) {
    area += cross(outline[i] - outline[0], outline[i + 1] - outline[0]);
  }
  int direction = signal(area);

  if (!direction) {
    LAND_ERROR("Outline has no area.");
  }

  while (outline.size() > 2) {
    int sz = int(outline.size());
    int result_index = sz;
    int result_area = -1;
    int mid_index = 0;
    int prev_index, next_index;
    Triangle triangle{};
    while (mid_index < sz) {
      prev_index = (mid_index - 1 + sz) % sz;
      next_index = (mid_index + 1) % sz;
      triangle = Triangle{outline[prev_index], outline[mid_index],
                          outline[next_index]};

      auto triangle_direction =
          signal(cross(outline[mid_index] - outline[prev_index],
                       outline[next_index] - outline[prev_index]));
      if (triangle_direction == -direction) {
        mid_index++;
        continue;
      }
      bool pass = true;
      auto shortcut = Segment{outline[prev_index], outline[next_index]};
      for (int i = 0; i < sz - 2; i++) {
        int j = (next_index + i) % sz;
        int k = (next_index + i + 1) % sz;
        if (is_intersect(shortcut, Segment{outline[j], outline[k]})) {
          pass = false;
          break;
        }
        if (i && triangle_direction) {
          if (triangle.IsInside(outline[j])) {
            pass = false;
            break;
          }
        }
      }
      if (pass) {
        auto this_area = std::abs(
            cross(triangle.v1 - triangle.v0, triangle.v2 - triangle.v0));
        if (this_area > result_area) {
          result_area = this_area;
          result_index = mid_index;
        }
      }
      mid_index++;
    }

    mid_index = result_index;

    prev_index = (mid_index - 1 + sz) % sz;
    next_index = (mid_index + 1) % sz;
    triangle =
        Triangle{outline[prev_index], outline[mid_index], outline[next_index]};
    if (result_area > 0) {
      triangles.emplace_back(float(triangle.v0.x) * inv_size,
                             float(triangle.v0.y) * inv_size + 0.125f);
      triangles.emplace_back(float(triangle.v1.x) * inv_size,
                             float(triangle.v1.y) * inv_size + 0.125f);
      triangles.emplace_back(float(triangle.v2.x) * inv_size,
                             float(triangle.v2.y) * inv_size + 0.125f);
    }
    for (int i = mid_index; i < sz - 1; i++) {
      outline[i] = outline[i + 1];
    }
    outline.pop_back();
  }
}
}  // namespace

namespace grassland::font {
Factory::Factory(const char *font_file_path) {
  FTCall(FT_Init_FreeType(&library_));
  FTCall(FT_New_Face(library_, font_file_path, 0, &face_));
  FTCall(FT_Set_Pixel_Sizes(face_, 0, size_scale));
}

Factory::~Factory() {
  FTCall(FT_Done_Face(face_));
  FTCall(FT_Done_FreeType(library_));
}

const Mesh &Factory::GetChar(Char_T c) {
  if (!loaded_fonts.count(c)) {
    LoadChar(c);
  }
  return loaded_fonts.at(c);
}

void Factory::LoadChar(Char_T c) {
  FTCall(FT_Load_Char(face_, c, FT_LOAD_DEFAULT));
  FT_Outline outline = face_->glyph->outline;
  struct Outlines {
    std::vector<std::vector<glm::ivec2>> outlines;
    std::vector<glm::ivec2> buffer;
    glm::ivec2 pos{0};
    int precision;
  } outlines{};
  outlines.precision = arc_precision;

  FT_Outline_Funcs interface {};

  interface.move_to = [](const FT_Vector *to, void *user) -> int {
    auto *outlines = static_cast<Outlines *>(user);
    glm::ivec2 vec_to{to->x, to->y};
    if (!outlines->buffer.empty()) {
      outlines->outlines.push_back(outlines->buffer);
      outlines->buffer.clear();
    }
    outlines->pos = vec_to;
    return 0;
  };

  interface.line_to = [](const FT_Vector *to, void *user) -> int {
    auto *outlines = static_cast<Outlines *>(user);
    outlines->pos = {to->x, to->y};
    outlines->buffer.push_back(outlines->pos);
    return 0;
  };

  interface.conic_to = [](const FT_Vector *control, const FT_Vector *to,
                          void *user) -> int {
    auto *outlines = static_cast<Outlines *>(user);
    glm::ivec2 vec_control{control->x, control->y};
    glm::ivec2 vec_to{to->x, to->y};
    auto num_frac = (1 << arc_precision);
    for (int i = 1; i < num_frac; i++) {
      auto comp_i = num_frac - i;
      outlines->buffer.push_back((outlines->pos * comp_i * comp_i +
                                  2 * vec_control * i * comp_i +
                                  vec_to * i * i) >>
                                 (2 * arc_precision));
    }
    outlines->buffer.push_back(vec_to);
    outlines->pos = vec_to;
    return 0;
  };
  interface.cubic_to = [](const FT_Vector *control1, const FT_Vector *control2,
                          const FT_Vector *to, void *user) -> int {
    auto *outlines = static_cast<Outlines *>(user);
    glm::ivec2 vec_control1{control1->x, control1->y};
    glm::ivec2 vec_control2{control2->x, control2->y};
    glm::ivec2 vec_to{to->x, to->y};
    auto num_frac = (1 << arc_precision);
    for (int i = 1; i < num_frac; i++) {
      auto comp_i = num_frac - i;
      outlines->buffer.push_back((outlines->pos * comp_i * comp_i * comp_i +
                                  vec_control1 * comp_i * comp_i * i * 3 +
                                  vec_control2 * comp_i * i * i * 3 +
                                  vec_to * i * i * i) >>
                                 (3 * arc_precision));
    }
    outlines->buffer.push_back(vec_to);
    outlines->pos = vec_to;
    return 0;
  };

  FTCall(FT_Outline_Decompose(&outline, &interface, &outlines));
  if (!outlines.buffer.empty()) {
    outlines.outlines.push_back(outlines.buffer);
    outlines.buffer.clear();
  }

  for (auto &cycle : outlines.outlines) {
    int area = 0;
    for (int i = 0; i < cycle.size(); i++) {
      int j = i + 1;
      if (j == cycle.size()) {
        j = 0;
      }
      area += cross(cycle[i], cycle[j]);
    }
    if (area < 0) {
      std::reverse(cycle.begin(), cycle.end());
    }
  }

  std::set<uint32_t> out_most_set;
  std::set<uint32_t> alive_set;
  std::set<uint32_t> process_set;
  std::set<uint32_t> next_turn_set;
  for (int i = 0; i < outlines.outlines.size(); i++) {
    alive_set.insert(i);
  }
  process_set = alive_set;
  while (!process_set.empty()) {
    for (auto i : process_set) {
      int father = -1;
      for (auto j : alive_set) {
        if (j == i) {
          continue;
        }
        if (inside(outlines.outlines[j], outlines.outlines[i][0])) {
          if (father == -1) {
            father = j;
          } else {
            father = -2;
            break;
          }
        }
      }
      if (father == -1) {
        out_most_set.insert(i);
      } else if (father == -2) {
        next_turn_set.insert(i);
      } else {
        merge_contours(outlines.outlines[father], outlines.outlines[i],
                       outlines.outlines);
        alive_set.erase(i);
        outlines.outlines[i].clear();
      }
    }
    process_set = next_turn_set;
    next_turn_set.clear();
  }

  std::vector<glm::vec2> vertices;
  for (auto i : out_most_set) {
    triangulation(vertices, outlines.outlines[i]);
  }
  loaded_fonts.insert(std::make_pair(
      c, Mesh(vertices, float(face_->glyph->advance.x) * inv_size)));
}

Mesh Factory::GetString(const std::wstring &wide_str) {
  Mesh mesh{};
  for (auto wc : wide_str) {
    auto &char_mesh = GetChar(wc);
    for (auto index : char_mesh.indices) {
      mesh.indices.push_back(index + mesh.vertices.size());
    }
    for (auto vertex : char_mesh.vertices) {
      vertex.x += mesh.advection;
      mesh.vertices.push_back(vertex);
    }
    mesh.advection += char_mesh.advection;
  }

  if (mesh.indices.empty()) {
    mesh.vertices = {{0.0f, 0.0f}};
    mesh.indices = {0, 0, 0};
  }
  return mesh;
}

}  // namespace grassland::font
