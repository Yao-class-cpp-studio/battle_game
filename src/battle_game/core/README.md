# Core

这个文件夹下的内容是整个项目的核心框架。
[GameCore](game_core.h) 类是所有游戏逻辑集中协调的地方。

在这个框架中，游戏内容主要分为四类元素：单位（[Units](unit.h)）、障碍物（[Obstacles](obstacle.h)）、子弹（[Bullets](bullet.h)）和粒子（[Particles](particle.h)）。
这四类元素的基类统一继承自对象类型（[Object](object.h)），他们都有一定特定的预设功能，用于方便你实现自己的想法。

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
  - 这是一个指向游戏核心类的指针，会在对象被创建时赋值。当对象在运行过程中需要与游戏内其他元素交互时，
  你可以通过这个指针调用游戏核心的相应功能函数。
  - 约定：被用于游戏中的对象，game_core_ 一定指向合法的对象，且在过程中不更改其值。
  - 约定：game_core_ 为 nullptr 时，则当前对象是在进行一些游戏内容外的测试，此时该对象游戏逻辑部分代码不会也不应该被调用。
- id_
  - 表示对象的编号，同类元素的不同实体拥有不同的编号，不同类型元素的实体可能拥有相同编号。
  例如，属于单位 Unit 子类的实体，本变量表示的值为**单位编号**（Unit ID），以此为类比，适用于其他三类元素。
  - 约定：任何类型元素的编号 0 为保留编号，用于一些特殊判断的处理。
- position_, rotation_
  - 游戏中每一个对象都拥有其位置信息
  - position_ 表示对象相对于游戏内坐标原点的偏移量
  - glm::vec2 是 glm 数学运算库中表示单精度浮点二维向量的类型，这个数学库内包含了四维及以下向量、矩阵的类型定义和常用功能函数。
  - rotation_ 表示对象在场景内相比于默认朝向，逆时针旋转的角度

除成员变量外，所有对象都应包含两部分功能：

```c++
class Object {
public:
    ...
    virtual void Update() = 0;
    virtual void Render() = 0;
};
```


- Update
  - 这个函数用于更新对象的状态
  - 对象的主要逻辑代码都应写在这里
  - 这个函数会在每一个游戏帧（Tick）更新时被调用
- Render
  - 这个函数用于绘制对象的视觉效果
  - 你可以调用 `src/battle_game/graphics/grahics.h` 中的函数进行画面的绘制，
  前往 [src/battle_game/graphics/README.md](../graphics/README.md) 查看相关绘图函数的使用方法。

为了方便构思，我们可以想象所有对象都根据其位置信息拥有一个自己的**本地空间**坐标系，
相对于**世界空间**坐标系，在**本地空间**坐标系上进行计算会带来许多便利。
因此 Object 类还提供了如下两个函数用于辅助计算

- LocalToWorld
  - 传入参数为**本地空间**坐标
  - 返回对应的**世界空间**坐标
- WorldToLocal
  - 传入参数为**世界空间**坐标
  - 返回对应的**本地空间**坐标

## Unit

单位类声明在 [unit.h](unit.h) 中，单位表示游戏中进行主动动作的对象，所有具体单位的实现都应继承自此基类。

### 成员变量

- player_id_
  - 这个变量表示单位所有者的**玩家编号**（Player ID），用于区分敌我
- lifebar_*
  - 这些变量保存了生命条的设置，请通过set来修改
- fadeout_health_
  - 保存生命条渐变的起始位置。（一般不需要修改）
- status_
  - 这是一个类，存储了所有 Unit 所具有的基本属性，以便编写辅助影响
  - 内含以下成员函数
    - max_health_
      - 单位的最大生命值基准，默认 100.0
      - 暂时**仅可在单位定义时修改**
    - health_
      - 单位的生命比率，初始默认 1.0
      - 取值范围为 [0, 1]，即剩余生命相对于最大生命值生命值的比例
      - 单位实际生命值为 `max_health_ * health_`
      - 这样定义是为了方便动态地对最大生命值进行调整，以实现一些复杂机制。
      - 该值归 0 时单位死亡。
    - attack_
      - 单位攻击力，初始默认 1.0
      - 作为伤害倍率的默认影响参数
    - defence_
      - 单位防御力，初始默认 0.0
      - 作为伤害减免的默认影响参数
    - speed_
      - 单位移动速度，初始默认 1.0
      - 作为速度倍率的默认影响参数

### 成员函数

- SetPosition
  - 设置单位位置的函数
  - 为了游戏帧更新时的数据一致性，一般不直接调用，而是用 GameCore 中的 PushEventMoveUnit 函数添加单位移动事件
- SetRotation
  - 设置单位朝向的函数
  - 为了游戏帧更新时的数据一致性，一般不直接调用，而是用 GameCore 中的 PushEventRotateUnit 函数添加单位旋转事件
- GetDamageScale
  - 获取单位的伤害倍率，默认为 status\_.attack\_
  - 你可以编写这部分的计算逻辑以实现高倍率攻击力属性、光环等效果
  - 单位造成伤害的功能都应考虑此函数的影响
- GetDamageReduction
  - 获取单位的伤害减免，默认为 1/(1+status\_.defence\_)
  - 你可以编写这部分的计算逻辑以实现高倍率防御力属性、光环等效果
  - **该函数在伤害计算时自动调用，单位不宜主动调用此函数**
- GetSpeedScale
  - 获取单位的移动速度倍率，默认为 status\_.speed\_
  - 你可以编写这部分的计算逻辑以实现加速、减速相关的属性、光环功能
- GetMaxHealth
  - 返回最大生命值
- Set/GetLifeBar*
  - 修改/获取各种生命条设置
- RenderLifeBar
  - 这是一个虚函数
  - 渲染该对象对应的生命条
- Hide/ShowLifeBar
  - 隐藏/显示生命条
- RenderHelper
  - 这是一个虚函数
  - 仅在该单位所有者玩家的视角中，渲染该对象用于辅助的一些视觉效果（例如子弹射出的预计轨迹）
- IsHit
  - 这是一个虚函数
  - 用于判断若一个事件发生于传入参数**世界空间**坐标 `position`，该事件是否会对当前单位产生影响
  - 可以理解为定义命中体积的函数
  - 通常用于判断子弹、飞射物是否命中了当前单位
  - 你可以在子类实现中通过 `override` 定义不同的命中判断逻辑
- GenerateBullet
  - 这是一个模板函数
  - 由于子弹类的生成包含了许多类似于发出者**单位编号**、所属**玩家编号**相关的固定信息，我们希望把基本信息相关的内容省略，从而只通过传入关键信息即可生成一颗子弹，
  该函数可以帮助你通过只填写新生成的子弹对象的位置、朝向、伤害倍率等关键参数，自动根据单位信息进行补全并添加一个子弹生成事件。
  - 实现类似于“开火”一类的技能可以使用该函数
  - 函数的实现位于 `src/battle_game/core/game_core.h` 中

### 相关类

- Skill（技能类）
  
  - units支持加入技能。
  
  - 为了方便玩家操作，技能应当使用键盘快捷键完成。特别地，由于本游戏使用W/A/S/D控制转向，为方便起见，技能采用按键E/Q/R完成。我们规定E/Q/R表示的技能强度递增，并建议按照E/Q/R的顺序依次实现技能（可不足3个，但主动技能一般不会超过3个）。此外，P表示被动技能，这一技能不需要用户输入。
  
  - 用户通常希望从UI界面获取技能的简略信息。因此，如果您不希望技能被展示在UI界面中，请使用ADD_SELECTABLE_UNIT_WITHOUT_SKILL()进行调用（如果您没有设置技能，则不会显示任何信息。因此技能界面向前兼容）。此外，您需要维护一个名称为skill_的信息存储库，它已经是您的units类型中的protected类型。它的格式为std::vector<battle_game::Skill> 。其中Skill是一个用于交互的结构体。
  
  - units支持子弹切换界面显示。一个units可以拥有不止一种射击的子弹，并且子弹切换通常需要一定的冷却时间。如果您不希望展示子弹界面，您只需要留空即可。 如果您希望展示子弹界面，请在skill_里加入type=B的一个元素。如果有不止一个子弹，请不要多次添加，而是填写当前的bullet_type和一共的bullet_total_number。子弹界面对您的输入具有一定的适应性，例如，如果您只有一种子弹，将不会展示切换信息；如果您的冷却时间为0，将不会展示冷却进度。
  
  - ```cpp
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
  
  - 你需要在name中填写技能名称，description为技能简述（若有），src为技能图示路径（若有），time_remain为技能冷却时间，time_total为技能冷却总时间，type为技能类型，function为技能调用的接口（可选择不提供）。若选择提供，使用格式为example.function=SKILL_ADD_FUNCTION(YourUnits::YourFunction)。
  
  - 使用示例请参考inferno_tank类型。技能显示页面可能会持续更新，但可以承诺skill_这一交互容器会保持不变。也即技能显示页面的更新会自动兼容您的数据，您无须再次编写。如果您发现了显示页面的BUG或者希望增加更多内容（如您可能希望加入用户状态，如加速/灼烧等），欢迎联系XuGW-Kevin。

- Effect（效果类）

  - 为了存储延时性伤害、参数类影响，我们将其统称为“状态效果”，主要处理对状态 statue\_ 内参数的持续影响

  - 每个单位均存储 Effect 链表，存储所具有的所有状态

  - 定义如下：

  - ```cpp
    class Effect{
        virtual std::string Name() const;         // 效果名，用于 GUI 显示
        virtual std::string Description() const;  // 描述，用于 GUI 显示
        virtual void Influence(Unit::Status &);   // 具体实现，对状态的影响函数
        virtual uint32_t TickRemain() const;      // 持续时间（单位：帧），可用于 GUI 显示
        virtual void TickPass();                  // 每过一帧时，调用此函数，用于调整该效果的时间参数
        virtual bool ShouldRemove() const;        // 判断该状态效果是否结束
    
        uint32_t src_unit_id_;                    // 效果来源，对伤害溯源有作用
        Effect(uint32_t src_uint_id);             // 唯一的初始化定义，声明 Effect 必须标注来源的单位
      }
    };
    ```

  - 执行逻辑

    - 回合开始时，对所有单位，遍历其所有状态，将 `status_` 带入 `Effect::Influence` 中运行
      - 在 `Influence` 中，你可以对 `status_` 进行修改
    - 回合开始时，对所有单位，遍历其所有状态，调用 `Effect::TickPass`，表示该效果的存在时间多了一帧
    - 回合结束时，对所有单位，遍历其所有状态，调用 `Effect::ShouldRemove`。若返回真，该状态从该单位中删除
    - 不难发现，回合过程中获得的状态将从下一帧开始生效

  - 您需要定义 `Effect` 的子类，定义这 6 个函数，以完成**对一个状态效果的定义**。

    - 当然，您可以在您的子类中定义其它变量或函数，用以辅助完成更复杂的内容。

  - 你可以在 `Unit` 的成员函数内调用 `PUSH_EFFECT(效果类构造函数)`，以**让一个单位获得该状态**

  - 存储位置：

    - 所有 `Effect` 的子类存储在 `effects` 文件夹下，其中所有效果类的声明需要包含在 `effects/effects.h` 中。

## Obstacle

障碍物类声明在 [obstacle.h](obstacle.h) 中，该类对象主要用于组成游戏场景。

- GetSurfaceNormal
  - 给定向量的始点与终点信息，返回向量与物体表面相交处对应的表面单位法向量信息。我们约定其返回值的第一项为给定线段与表面的交点，第二项为单位法向量的方向。
  - 主要用于实现子弹的反弹。对于障碍物（对于某些子弹）不应该返回对应表面单位法向量的情况，我们约定返回值中单位法向量的方向设为 (0,0)。

### 成员函数

- IsBlocked
  - 这是一个虚函数
  - 该函数用于判断传入参数**世界空间**坐标 `position` 是否被该障碍物阻挡
  - 你可以在子类实现中通过 `override` 定义不同的障碍物作用模式

## Bullet

子弹类声明在 [bullet.h](bullet.h) 文件中，该类对象主要用于表示会对游戏性产生影响的临时对象。如：子弹

相比于子弹这个名称，或许用“飞射物”一词更符合其广义的含义。

### 成员变量

- unit_id_
  - 表示该飞射物的创建者的**单位编号**（Unit ID），用于可能实现的“经验值系统”或相关机制
- player_id_
  - 表示该飞射物的所属玩家的**玩家编号**（Player ID），用于区分阵营
- damage_scale_
  - 伤害倍率，用于存储该子弹被创建时被赋予的伤害倍率，以作用于命中时的实际伤害


## Particle

粒子类声明在 [particle.h](particle.h) 文件中，该类对象主要用于表示不会对游戏性产生影响的临时对象，用于提升视觉体验。

## GameCore

游戏核心提供了许多用于方便游戏内元素进行交互的函数功能

### 对象访问

通过游戏核心对象你可以访问其他任意仍旧存在于游戏中的对象：

- GetUnit
  - 根据输入的**单位编号**（Unit ID），返回对应单位对象的指针
  - 若**单位编号**指向的单位已经被删除或编号不合法，则返回 `nullptr`
- GetUnits
  - 获取所有现存的单位
- GetObstacle
    - 根据输入的**障碍物编号**（Obstacle ID），返回对应障碍物对象的指针
    - 若**障碍物编号**指向的障碍物已经被删除或编号不合法，则返回 `nullptr`
- GetObstacles
  - 获取所有现存的障碍物
- GetBlockedObstacle
  - 判断传入参数**世界空间**坐标 `position` 是否被某障碍物阻挡。如果是，返回对应障碍物对象的指针
- GetBullet
    - 根据输入的**子弹编号**（Bullet ID），返回对应子弹对象的指针
    - 若**子弹编号**指向的子弹已经被删除或编号不合法，则返回 `nullptr`
- GetBullets
  - 获取所有现存的子弹
- GetParticle
    - 根据输入的**粒子编号**（Particle ID），返回对应粒子对象的指针
    - 若**粒子编号**指向的粒子已经被删除或编号不合法，则返回 `nullptr`
- GetParticles
  - 获取所有现存的粒子

### 事件队列

由于一个游戏帧在更新的过程中，不同对象的更新函数存在被调用的先后顺序。
为了数据一致性，我们不希望先被调用的更新函数产生的影响立刻影响到后续被调用的更新函数，
例如：当两个单位可以同时击杀对方时，先被调用的对象如果立即对对方产生了伤害，那么后发单位直接死亡，会影响到游戏公平。
因此，我们希望游戏对象的更新逻辑部分将可能影响到其他对象的事件延后结算，因此我们引入了事件队列机制：

在所有游戏对象的更新函数调用完成后，事件队列会依次执行其中的内容。
我们希望保证：事件与事件之间的相对顺序对计算结果没有干扰。

总而言之，这是一个保证游戏逻辑一致性的机制，已经提供的事件函数如下：

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
  - 压入一个单位移动事件
- PushEventRotateUnit
  - 压入一个单位旋转事件
- PushEventDealDamage
  - 压入一个伤害事件
- PushEventKillUnit
  - 压入一个击杀事件
  - 这种事件通常通过伤害事件产生
- PushEventRemoveObstacle
  - 压入一个障碍物移除事件
- PushEventRemoveBullet
  - 压入一个子弹移除事件
- PushEventRemoveParticle
  - 压入一个粒子移除事件
- PushEventRemoveUnit
  - 压入一个单位移除事件
  - 这种事件通常通过击杀事件产生
- PushEventGenerateBullet
  - 压入一个子弹生成事件
  - 和 Unit 类中的 GenerateBullet 函数相联系

### 随机变量

游戏中我们常常会见到随机机制，为了方便日后我们加入联机功能，我们需要使随机机制具有一致性。
因此我们希望你编写的代码中所有涉及到的随机变量都通过以下函数获得：

- RandomFloat
  - 产生一个区间 [0, 1] 内均匀分布的随机浮点数
- RandomInt
  - 产生一个区间 [low_bound, high_bound] 内均匀分布的随机整数
- RandomOnCircle
  - 产生一个单位圆圆周上均匀分布的随机向量
- RandomInCircle
  - 产生一个单位圆内部均匀分布的随机向量
