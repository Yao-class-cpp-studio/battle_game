# Project 2: Battle Game

[![Linux Build](https://github.com/Yao-class-cpp-studio/battle_game/actions/workflows/linux-build.yml/badge.svg)](https://github.com/Yao-class-cpp-studio/battle_game/actions/workflows/linux-build.yml)

经过一个学期的学习，我们已经一起学习了许多编程知识，大家的编程技能想必也有了显著的提高。
这个项目将会给大家一个自由展现自己编程能力的平台，为课程画上一个圆满的句号。
与历次作业和第一次大作业单打独斗的体验不同，这个项目将由参与课程的所有同学共同完成，
你需要和同学们相互配合，让所有人的代码配合到一起，体验共同开发的乐趣。

这个项目的目标是制作一款内容丰富的对战小游戏。
同学们可以自由地发挥自己的创造力，设计各种奇妙有趣的游戏内容，添加绚丽的视觉效果。
项目结束后，我们的课程团队将会把同学们设计创造的内容进行整合并发布，因此你的设计将有机会被很多人体验到。
期待大家的成果。。

## 任务

设计本项目的目的是希望大家通过本项目体验和学习软件工程的思维和流程。

### 1、创建分叉（Fork）并提交 PR （Pull Request）【5 pts】

在一个团队合作的项目中，每个人提交的每一行代码都可能引发无法预料的后果，你的每一行代码在进入主仓库之前都需要经过严格的审核。
你不拥有对主仓库写权限，在编写你贡献的新代码之前，你需要先从主仓库创建一个属于你自己的分叉（Fork）。在 GitHub 中，
实现这个操作只需要点击仓库主页面右上角的 `Fork` 按钮，并根据跳转页面的提示创建你的分叉。

在你的分叉仓库里，你可以对其进行任意的改动，这些都不会影响到主仓库的内容安全。

在你写完并提交了新的代码后，你可以通过创建 PR （Pull Request）的方式申请将你分叉仓库中新的代码合并到主仓库。
你可以通过仓库页面导航栏的 `Pull requests` 按钮进入 PR 页面。
在 PR 页面内，你可以点击偏右上方绿色的 `New pull request` 按钮进入 Pull Request 创建页面。
接下来根据页面中的提示和选项，选择你的分叉仓库和你分叉仓库里更新代码的分支（branch）。
在选择完成后，点击 `Create pull request` 按钮提交你的 PR。

这一子任务不需要你提交有意义的代码，你只需要正确地创建分叉，随意提交一些改动，然后正确提交一个 PR 即可，然后请进入下一个子任务。

### 2、创建一个新的游戏单位类型 【10 pts】

本游戏的框架涉及 4 种类型的元素：单位（Units）、障碍物（Obstacles）、子弹（Bullets）和粒子（Particles）。
这些元素共同组成了游戏内容。每种类型的元素都有一个基类的定义，所有相应类型的设计实现都应通过继承对应类型元素的基类的方式实现。
不同类型的元素也有一些符合各自定位的虚函数声明，你在实现的过程中需要给出相应的定义。
更多框架相关信息的说明请前往 `src/battle_game/core/` 目录下的 [README.md](src/battle_game/core/README.md) 文件查阅。

在几种类型的元素中，单位是玩家主要操控的对象，即玩家在游戏内可以扮演的角色。
这个子任务要求你在项目中创建一个由你进行维护的游戏单位类型，你可以自由地设计你的游戏单位的功能和特性，但你至少需要完成最基础的功能。
在本次作业发布时，游戏框架已经给出了一个游戏单位的实现范例，即 `src/battle_game/core/units/` 目录下的 `tiny_tank.h,cpp`。
这个范例实现的是一个坦克单位，可以用键盘进行移动，用鼠标进行瞄准和射击。
作为最基础的要求，你需要仿制这个坦克单位，并复现其功能。
如果你希望实现一些别样的游戏单位，你可以自行创造，但你的单位至少需要包含：
正确的外观、合理的移动逻辑以及至少一种射击飞射物（Bullet）的功能。

这是你需要做的具体步骤：
1. 在 `src/battle_game/core/units/` 目录下仿照 `tiny_tank.h` 和 `tiny_tank.cpp` 的模式新建一对用于编写你的游戏单位的 `.h` 和 `.cpp` 文件。
你可以自行选取合适的名字。为了避免和他人的命名产生冲突，你可以在文件名中添加带有个人特征的标记，如：常用网名、学号等。
2. 在你创建的代码文件里，你需要定义一个新的类型，使其继承自 `Unit` 类型。这个类型名可以任意选取。
同样的，你可以添加带有个人特征的标记以避免与其他同学冲突。
3. 实现功能：如果你希望快速获得这部分分数，你可以参考 `tiny_tank` 的实现完成一个你的坦克的功能。
如果你希望设计创造性的单位，你可以参考 `tiny_tank` 的逻辑设计，编写你自己的单位逻辑。
4. 在实现完成你的单位类型后，给你的作品签上自己的名字吧！你需要重载 `UnitName` 和 `Author` 两个函数，
前者是你创造的单位的名称，后者是作者的名称，也就是你的名字，你可以随意地用真实姓名或者网名。
5. 最后你需要将定义了你的单位的头文件添加到 `src/battle_game/core/units/units.h` 中，
然后前往 `src/battle_game/core/selectable_units.cpp` 将你的单位添加到可选单位列表。


一切做完后，编译并运行程序，你会看到你的单位已经出现在左上角的可选单位列表中了。你可以从列表中选择你的单位，
用 `自毁` 按钮消灭当前默认创建的单位，等待 5 秒后重生出的新单位就是你的作品了！

如果你的单位运行起来一切正常，你可以将你的代码推送到你的分叉仓库中，你之前创建的 PR 会自动同步你更新的内容。
这时候你就可以找助教验收这个子任务的成果了。

如果你发现你的 PR 被 clang-format 检查卡住了，这是因为你的代码风格不标准。
你不需要手调代码风格，这样会很累。你可以采取如下自动的方式：

1. 下载 pre-commit：
```shell
pip install pre-commit
```
2. 将 pre-commit 的钩子加入到 git commit 中
```shell
pre-commit install # 在仓库主目录下运行
```
3. 重新提交你修改的文件：
```shell
git commit -am "Your Comments"
```
此时 pre-commit 会帮助你自动修改代码风格。
如果报错了，就将上放指令再敲一遍，直到提交成功。
4. 推送到远端仓库

### 3、提出一个新功能的想法到 Issues 列表 【5 pts】

有时你有一个很好的想法，但是没有办法立刻实现，
或者你发现项目中存在一个 Bug，希望以后去修复。
这时你可以利用 GitHub 的 Issues 功能先把问题记下来。

项目的构建需要大家发挥自己的想象力和创造力，
你既是乙方，也是甲方。
这个子任务要求你在 GitHub Issues 栏中提交一个有效的 Issue。
你需要在 Issue 中尽可能详细的说明问题的需求，最好同时给出一个可行的实现思路。
如果前面已经有同学提过类似或者相同的问题，那就不要再重复提交了。
我们不在意你提的问题是复杂还是简单，只要是一个有效的问题，你都可以直接拿到这个子任务的 5 分！

### 4、自由创造 【10 pts】

在同学们提出很多好的 idea 后，我们希望能够把他们一一变为现实。
最后的这 10 分完全由你自行发挥。
你可以从 Issues 列表中选择一个你认为自己可以解决的部分，你可以申领这个 Issue，然后提交 PR 实现并关闭之。
你可以和同学合作完成，在提交 PR 时请说明每个人贡献了哪部分代码。
每一个被成功解决的 Issue 的所有参与者都可以获得 5 分本子任务的分数，当然，满分 10 分封顶。
你如果希望多做一些贡献那就更好了，毕竟，这个项目就是为了大家能够体会编程的乐趣！
Good Luck and Have Fun！

## 后记

我们希望同学们通过这次作业体会到编程的乐趣，在过程中创造快乐，最终我们会一起呈现一个完整的作品，这离不开每一位同学的贡献。
你的每一行代码都是重要的，也是有意义的。希望多年以后，这段旅程能够成为同学们共同的美好回忆。
