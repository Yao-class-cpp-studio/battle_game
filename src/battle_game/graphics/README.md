# Graphics

这部分代码你一般情况下不需要进行修改，
它们主要用于辅助你进行视觉效果的绘制

## 资产管理器

绘制图形需要涉及模型和纹理等资源，
这些绘图资源应避免重复加载以致占用不必要的额外内存空间。
因此我们引入资产管理器辅助管理绘图资源，并方便地进行重复利用。
你如果对绘图相关的知识不熟悉，可以不看这一章节的内容，利用下一章节中更直接的操作手段利用纹理进行绘制。

```c++
static AssetsManager *AssetsManager::GetInstance();
```

- 这个静态函数用于获得资产管理器对象的指针

```c++
uint32_t RegisterModel(const std::vector<ObjectVertex> &vertices,
                         const std::vector<uint32_t> &indices);
```

- 这个函数用于注册一个新的模型
- vertices 表示模型的顶点信息
- indices 表示模型的索引信息
- 返回值为注册的模型编号
- 同样的内容请只注册一次，以避免浪费资源

```c++
uint32_t RegisterTexture(const Texture &particle_texture);
```

- 这个函数用于注册一个新的纹理
- particle_texture 表示注册的纹理图案
- 返回值为注册的纹理编号，这个编号可以用于绘制操作前纹理的绑定
- 同样的内容请只注册一次，以避免浪费资源

## 全局函数定义

```c++
void SetColor(const glm::vec4 &color = glm::vec4{1.0f});
```

- 设置全局绘制颜色

```c++
glm::vec4 GetColor();
```

- 获取当前设置的全局绘制颜色

```c++
void SetPosition(glm::vec2 position);
```

- 设置全局绘制位置

```c++
void SetRotation(float rotation);
```

- 设置全局绘制朝向

```c++
glm::vec2 GetPosition();
```

- 获取全局绘制位置

```c++
float GetRotation();
```

- 获取全局绘制朝向

```c++
void SetScale(glm::vec2 scale);
```

- 设置全局模型拉伸倍率信息

```c++
glm::vec2 GetScale();
```

- 获取全局模型拉伸倍率信息

```c++
void SetTransformation(glm::vec2 position,
                       float rotation = 0.0f,
                       glm::vec2 scale = glm::vec2{1.0f});
```

- 统一设置变换信息
- 相当于一起调用 SetPosition, SetRotation, SetScale 函数

```c++
uint32_t RegisterTexture(const std::string &file_path);
```

- 根据图片文件注册一个新的纹理
- `file_path` 表示图片文件的路径
- 同样的内容请只注册一次，以避免浪费资源

```c++
uint32_t SetTexture(const std::string &file_path);
```

- 根据图片文件的路径设置绑定的纹理

```c++
void SetTexture(uint32_t texture_id = 0);
```

- 根据纹理注册编号设置绑定的纹理

```c++
uint32_t GetTexture();
```

- 获取当前绑定的纹理编号

```c++
void DrawModel(uint32_t model_id = 0);
```

- 根据当前绑定的全局设置，绘制对应编号的注册模型

```c++
void DrawTexture(const std::string &file_path);
```

- 根据当前绑定的全局设置，绘制 `file_path` 路径对应的图片到本地空间为 $[-1, 1]^2$ 的矩形区域内
