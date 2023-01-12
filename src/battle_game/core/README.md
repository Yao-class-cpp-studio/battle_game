# Core

这个文件夹下的内容是整个项目的核心框架��1�7
[GameCore](game_core.h) 类是扢�有游戏��辑集中协调的地方��1�7

在这个框架中，游戏内容主要分为四类元素：单位（[Units](unit.h)）��障碍物（[Obstacles](obstacle.h)）��子弹（[Bullets](bullet.h)）和粒子（[Particles](particle.h)）��1�7
这四类元素的基类统一继承自对象类型（[Object](object.h)），他们都有丢�定特定的预设功能，用于方便你实现自己的想法��1�7

## Object

对象类的声明位于 [object.h](object.h) 文件中，
你可以看到其包含了以下成员变量：

```c++
class Object {
    ...
protected:
    GameCore *game_core_{nullptr};
    glm::vec2 position_{0.0f};  // offset from the origin (0, 0)
    float rotation_{0.0f};      // angle in radians
    uint32_t id_{0};
};
```

- game_core_
  - 这是丢�个指向游戏核心类的指针，会在对象被创建时赋����当对象在运行过程中霢�要与游戏内其他元素交互时＄1�7
你可以��过这个指针调用游戏核心的相应功能函数��1�7
  - 约定：被用于游戏中的对象，game_core_ 丢�定指向合法的对象，且在过程中不更改其值��1�7
  - 约定：game_core_ 丄1�7 nullptr 时，则当前对象是在进行一些游戏内容外的测试，此时该对象游戏��辑部分代码不会也不应该被调用��1�7
- id_
  - 表示对象的编号，同类元素的不同实体拥有不同的编号，不同类型元素的实体可能拥有相同编号〄1�7
  例如，属于单佄1�7 Unit 子类的实体，本变量表示的值为**单位编号**（Unit ID），以此为类比，适用于其他三类元素��1�7
  - 约定：任何类型元素的编号 0 为保留编号，用于丢�些特殊判断的处理〄1�7
- position_, rotation_
  - 游戏中每丢�个对象都拥有其位置信恄1�7
  - position_ 表示对象相对于游戏内坐标原点的偏移量
  - glm::vec2 昄1�7 glm 数学运算库中表示单精度浮点二维向量的类型，这个数学库内包含了四维及以下向量��矩阵的类型定义和常用功能函数��1�7
  - rotation_ 表示对象在场景内相比于默认朝向，逆时针旋转的角度

除成员变量外，所有对象都应包含两部分功能＄1�7

```c++
class Object {
public:
    ...
    virtual void Update() = 0;
    virtual void Render() = 0;
};
```


- Update
  - 这个函数用于更新对象的状怄1�7
  - 对象的主要��辑代码都应写在这里
  - 这个函数会在每一个游戏帧（Tick）更新时被调甄1�7
- Render
  - 这个函数用于绘制对象的视觉效构1�7
  - 你可以调甄1�7 `src/battle_game/graphics/grahics.h` 中的函数进行画面的绘制，
  前往 [src/battle_game/graphics/README.md](../graphics/README.md) 查看相关绘图函数的使用方法��1�7

为了方便构��，我们可以想象扢�有对象都根据其位置信息拥有一个自己的**本地空间**坐标系，
相对亄1�7**世界空间**坐标系，圄1�7**本地空间**坐标系上进行计算会带来许多便利��1�7
因此 Object 类还提供了如下两个函数用于辅助计箄1�7

- LocalToWorld
  - 传入参数丄1�7**本地空间**坐标
  - 返回对应的1�7**世界空间**坐标
- WorldToLocal
  - 传入参数丄1�7**世界空间**坐标
  - 返回对应的1�7**本地空间**坐标

## Unit

单位类声明在 [unit.h](unit.h) 中，单位表示游戏中进行主动动作的对象，所有具体单位的实现都应继承自此基类〄1�7

### 成员变量

- player_id_
  - 这个变量表示单位扢�有��的**玩家编号**（Player ID），用于区分敌我
- health_
  - 这个变量表示单位的生命��1�7
  - 取��范围为 [0, 1]，即剩余生命相对于最大生命��生命��的比例〄1�7
  - 单位实际生命值为 `GetMaxHealth() * health_`
  - 这样定义是为了方便动态地对最大声明��进行调整，以实现一些复杂机制��1�7
  - 该��归 0 时单位死亡��1�7
- lifebar_*
  - 这些变量保存了生命条的设置，请��过set来修攄1�7
- fadeout_health_
  - 保存生命条渐变的起始位置。（丢�般不霢�要修改）

### 成员函数

- SetPosition
  - 设置单位位置的函敄1�7
  - 为了游戏帧更新时的数据一致��，丢�般不直接调用，��是甄1�7 GameCore 中的 PushEventMoveUnit 函数添加单位移动事件
- SetRotation
  - 设置单位朝向的函敄1�7
  - 为了游戏帧更新时的数据一致��，丢�般不直接调用，��是甄1�7 GameCore 中的 PushEventRotateUnit 函数添加单位旋转事件
- GetDamageScale
  - 获取单位的伤害��率，默认为 1.0
  - 你可以编写这部分的计算��辑以实现高倍率攻击力属性��光环等效果
  - 单位造成伤害的功能都应��虑此函数的影响
- GetSpeedScale
  - 获取单位的移动��度倍率，默认为 1.0
  - 你可以编写这部分的计算��辑以实现加速��减速相关的属����光环功胄1�7
- BasicMaxHealth
  - 这是丢�个虚函数
  - 朢�大生命��基准，默认丄1�7 100.0
  - 表示生命值基硢�数��1�7
  - 你可以在不同单位的实现中通过 `override` 修改单位的基硢�生命倄1�7
- GetHealthScale
  - 生命值��率，默认为 1.0
  - 你可以编写这部分的计算��辑以实现增强或衰弱生命值的功能
- GetMaxHealth
  - 朢�大生命��1�7
  - 定义为基硢�生命值乘以生命����率
- Set/GetLifeBar*
  - 修改/获取各种生命条设罄1�7
- RenderLifeBar
  - 这是丢�个虚函数
  - 渲染该对象对应的生命杄1�7
- Hide/ShowLifeBar
  - 隐藏/显示生命杄1�7
- RenderHelper
  - 这是丢�个虚函数
  - 仅在该单位所有��玩家的视角中，渲染该对象用于辅助的丢�些视觉效果（例如子弹射出的预计轨迹）
- IsHit
  - 这是丢�个虚函数
  - 用于判断若一个事件发生于传入参数**世界空间**坐标 `position`，该事件是否会对当前单位产生影响
  - 可以理解为定义命中体积的函数
  - 通常用于判断子弹、飞射物是否命中了当前单佄1�7
  - 你可以在子类实现中��过 `override` 定义不同的命中判断��辑
- GenerateBullet
  - 这是丢�个模板函敄1�7
  - 由于子弹类的生成包含了许多类似于发出耄1�7**单位编号**、所屄1�7**玩家编号**相关的固定信息，我们希望把基本信息相关的内容省略，从而只通过传入关键信息即可生成丢�颗子弹，
  该函数可以帮助你通过只填写新生成的子弹对象的位置、朝向��伤害��率等关键参数，自动根据单位信息进行补全并添加一个子弹生成事件��1�7
  - 实现类似于��开火��一类的抢�能可以使用该函数
  - 函数的实现位亄1�7 `src/battle_game/core/game_core.h` 丄1�7
- Skill
  - units支持加入抢�能��1�7
  - 为了方便玩家操作，技能应当使用键盘快捷键完成。特别地，由于本游戏使用W/A/S/D控制转向，为方便起见，技能采用按键E/Q/R完成。我们规定E/Q/R表示的技能强度��增，并建议按照E/Q/R的顺序依次实现技能（可不趄1�73个，但主动技能一般不会超迄1�73个）。此外，P表示被动抢�能，这一抢�能不霢�要用户输入��1�7
  - 用户通常希望从UI界面获取抢�能的箢�略信息��因此，如果您不希望抢�能被展示在UI界面中，请使用ADD_SELECTABLE_UNIT_WITHOUT_SKILL()进行调用（如果您没有设置抢�能，则不会显示任何信息��因此技能界面向前兼容）。此外，您需要维护一个名称为skill_的信息存储库，它已经是您的units类型中的protected类型。它的格式为std::vector<battle_game::Skill> 。其中Skill是一个用于交互的结构体��1�7
  - units支持子弹切换界面显示。一个units可以拥有不止丢�种射击的子弹，并且子弹切换��常霢�要一定的冷却时间。如果您不希望展示子弹界面，您只霢�要留空即可��1�7 如果您希望展示子弹界面，请在skill_里加入type=B的一个元素��如果有不止丢�个子弹，请不要多次添加，而是填写当前的bullet_type和一共的bullet_total_number。子弹界面对您的输入具有丢�定的适应性，例如，如果您只有丢�种子弹，将不会展示切换信息；如果您的冷却时间丄1�70，将不会展示冷却进度〄1�7
``` cpp
enum SkillType { E, Q, R, P, B };
struct Skill {
  std::string name;
  std::string description;
  std::string src;
  uint32_t time_remain;
  uint32_t time_total;
  uint32_t bullet_type;
  uint32_t bullet_total_number;
  SkillType type;
  std::function<void(void)> function;
};
```

 -
    - 你需要在name中填写技能名称，description为技能简述（若有），src为技能图示路径（若有），time_remain为技能冷却时间，time_total为技能冷却��时间，type为技能类型，function为技能调用的接口（可选择不提供）。若选择提供，使用格式为example.function=SKILL_ADD_FUNCTION(YourUnits::YourFunction)〄1�7
    - 使用示例请参考inferno_tank类型。技能显示页面可能会持续更新，但可以承诺skill_这一交互容器会保持不变��也即技能显示页面的更新会自动兼容您的数据，您无须再次编写��如果您发现了显示页面的BUG或��希望增加更多内容（如您可能希望加入用户状��，如加逄1�7/灼烧等），欢迎联系XuGW-Kevin〄1�7
## Obstacle

障碍物类声明圄1�7 [obstacle.h](obstacle.h) 中，该类对象主要用于组成游戏场景〄1�7

- GetSurfaceNormal
  - 给定向量的始点与终点信息，返回向量与物体表面相交处对应的表面单位法向量信息��我们约定其返回值的第一项为给定线段与表面的交点，第二项为单位法向量的方向��1�7
  - 主要用于实现子弹的反弹��对于障碍物（对于某些子弹）不应该返回对应表面单位法向量的情况，我们约定返回值中单位法向量的方向设为 (0,0)〄1�7

### 成员函数

- IsBlocked
  - 这是丢�个虚函数
  - 该函数用于判断传入参敄1�7**世界空间**坐标 `position` 是否被该障碍物阻挄1�7
  - 你可以在子类实现中��过 `override` 定义不同的障碍物作用模式

## Bullet

子弹类声明在 [bullet.h](bullet.h) 文件中，该类对象主要用于表示会对游戏性产生影响的临时对象。如：子弄1�7

相比于子弹这个名称，或许用��飞射物”一词更符合其广义的含义〄1�7

### 成员变量

- unit_id_
  - 表示该飞射物的创建��的**单位编号**（Unit ID），用于可能实现的��经验��系统��或相关机制
- player_id_
  - 表示该飞射物的所属玩家的**玩家编号**（Player ID），用于区分阵营
- damage_scale_
  - 伤害倍率，用于存储该子弹被创建时被赋予的伤害倍率，以作用于命中时的实际伤宄1�7


## Particle

粒子类声明在 [particle.h](particle.h) 文件中，该类对象主要用于表示不会对游戏��产生影响的临时对象，用于提升视觉体验��1�7

## GameCore

游戏核心提供了许多用于方便游戏内元素进行交互的函数功胄1�7

### 对象访问

通过游戏核心对象你可以访问其他任意仍旧存在于游戏中的对象＄1�7

- GetUnit
  - 根据输入的1�7**单位编号**（Unit ID），返回对应单位对象的指钄1�7
  - 苄1�7**单位编号**指向的单位已经被删除或编号不合法，则返回 `nullptr`
- GetUnits
  - 获取扢�有现存的单位
- GetObstacle
    - 根据输入的1�7**障碍物编叄1�7**（Obstacle ID），返回对应障碍物对象的指针
    - 苄1�7**障碍物编叄1�7**指向的障碍物已经被删除或编号不合法，则返囄1�7 `nullptr`
- GetObstacles
  - 获取扢�有现存的障碍牄1�7
- GetBlockedObstacle
  - 判断传入参数**世界空间**坐标 `position` 是否被某障碍物阻挡��如果是，返回对应障碍物对象的指钄1�7
- GetBullet
    - 根据输入的1�7**子弹编号**（Bullet ID），返回对应子弹对象的指钄1�7
    - 苄1�7**子弹编号**指向的子弹已经被删除或编号不合法，则返回 `nullptr`
- GetBullets
  - 获取扢�有现存的子弹
- GetParticle
    - 根据输入的1�7**粒子编号**（Particle ID），返回对应粒子对象的指钄1�7
    - 苄1�7**粒子编号**指向的粒子已经被删除或编号不合法，则返回 `nullptr`
- GetParticles
  - 获取扢�有现存的粒子

### 事件队列

由于丢�个游戏帧在更新的过程中，不同对象的更新函数存在被调用的先后顺序��1�7
为了数据丢�致��，我们不希望先被调用的更新函数产生的影响立刻影响到后续被调用的更新函数＄1�7
例如：当两个单位可以同时击杀对方时，先被调用的对象如果立即对对方产生了伤害，那么后发单位直接死亡，会影响到游戏公平��1�7
因此，我们希望游戏对象的更新逻辑部分将可能影响到其他对象的事件延后结算，因此我们引入了事件队列机制：

在所有游戏对象的更新函数调用完成后，事件队列会依次执行其中的内容〄1�7
我们希望保证：事件与事件之间的相对顺序对计算结果没有干扰〄1�7

总��言之，这是丢�个保证游戏��辑丢�致��的机制，已经提供的事件函数如下＄1�7

```c++
void PushEventMoveUnit(uint32_t unit_id, glm::vec2 new_position);
void PushEventRotateUnit(uint32_t unit_id, float new_rotation);
void PushEventDealDamage(uint32_t dst_unit_id,
                           uint32_t src_unit_id,
                           float damage);
void PushEventKillUnit(uint32_t dst_unit_id, uint32_t src_unit_id);
void PushEventRemoveObstacle(uint32_t obstacle_id);
void PushEventRemoveBullet(uint32_t bullet_id);
void PushEventRemoveParticle(uint32_t particle_id);
void PushEventRemoveUnit(uint32_t unit_id);

template <class BulletType, class... Args>
void PushEventGenerateBullet(uint32_t unit_id,
                             uint32_t player_id,
                             glm::vec2 position,
                             float rotation = 0.0f,
                             float damage_scale = 1.0f,
                             Args... args);
```

- PushEventMoveUnit
  - 压入丢�个单位移动事仄1�7
- PushEventRotateUnit
  - 压入丢�个单位旋转事仄1�7
- PushEventDealDamage
  - 压入丢�个伤害事仄1�7
- PushEventKillUnit
  - 压入丢�个击杢�事件
  - 这种事件通常通过伤害事件产生
- PushEventRemoveObstacle
  - 压入丢�个障碍物移除事件
- PushEventRemoveBullet
  - 压入丢�个子弹移除事仄1�7
- PushEventRemoveParticle
  - 压入丢�个粒子移除事仄1�7
- PushEventRemoveUnit
  - 压入丢�个单位移除事仄1�7
  - 这种事件通常通过击杀事件产生
- PushEventGenerateBullet
  - 压入丢�个子弹生成事仄1�7
  - 咄1�7 Unit 类中的1�7 GenerateBullet 函数相联糄1�7

### 随机变量

游戏中我们常常会见到随机机制，为了方便日后我们加入联机功能，我们霢�要使随机机制具有丢�致����1�7
因此我们希望你编写的代码中所有涉及到的随机变量都通过以下函数获得＄1�7

- RandomFloat
  - 产生丢�个区闄1�7 [0, 1] 内均匢�分布的随机浮点数
- RandomInt
  - 产生丢�个区闄1�7 [low_bound, high_bound] 内均匢�分布的随机整敄1�7
- RandomOnCircle
  - 产生丢�个单位圆圆周上均匢�分布的随机向釄1�7
- RandomInCircle
  - 产生丢�个单位圆内部均匀分布的随机向釄1�7
