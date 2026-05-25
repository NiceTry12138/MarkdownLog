# README

通过 [Understand-Anything](https://github.com/Lum1104/Understand-Anything) 生成 UE 5.4.4 项目的索引内容

- 中间内容生成到 `.understand-anything` 目录中
- 模块关系内容生成到 `Understand` 目录中
- 通过 `Understand-Anything` 生成的内容保存到 `Understand_Through` 中

运行时目录如下

```
-- UnrealEngine-5.4.4-release
  -- Engine
    -- .understand-anything
    -- Binaries
    -- Build
    -- Documentation
    -- Extras
    -- Platforms
    -- Plugins
    -- Programs
    -- Shaders
    -- Source
    -- ThroughtWork
        -- Understand_Through
        -- Understand
  -- Samples
  -- Templates
```

提示词大概如下

```
现在基于 ThroughtWork/Understand 目录的阅读记录，尤其是 knowledge-graph.json，

1. UE 编辑器的框架结构、设计
2. 以 Actor的蓝图编辑器为例，详细拆解蓝图编辑器的各个模块的实现方案
  - Viewport 预览界面如何实现，属性的修改如何影响 Viewport 的现实，如何在 Viewprot 控制镜头移动
  - Details 界面如何实现，如何现实各个属性，如何通过修改 Details 真正的去修改对象都是属性
  - 函数、Events 界面如何实现，为什么可以在界面右键出现节点选项、选择节点之后添加了 Node 保存在哪里、节点之间的连接关系如何保存、连接起来的节点运行的时候具体是怎么实现逻辑的
  - 点击编译之后，编译逻辑是什么？如何知道节点的运行顺序，如何知道节点编译是否合规
  - 点击 FInd 如何查找节点的
  - 界面右键出现的选项按钮是怎么定义的？
  - 节点之间能否连接是如何判断的？
  - 在运行时，这些节点数据怎么存储的？
3. 如何对编辑器进行扩展，比如我想要创建一个新的按钮，放在菜单栏的 Window 中；比如新建一个资产类型，在 Browser 界面右键可以选择并创建这个新的资产类型；比如给新的资产类型一个全新的编辑界面，比如行为树的 UBehaviourTree 的界面


对所有问题都做详尽的解释说明，整理说明内容为 Markdown 格式，命名 UE-Editor-Dev.md 到 ThroughtWork/Understand_Through/Editor-Dev 目录中，对于执行过程、包处理、内存检查、内存处理 等通过图片可以更好说明的内容，全部都要提供 SVG 图片 保存到 UE-Dev.md 同级的 Image 目录中

注意：
- 禁止通过函数名、属性名、注释推测函数、属性、类的作用，必须阅读源码
- md 文件中在合适的位置要引用 SVG 图片，而不是在最后统一使用 SVG 图片
- 把我作为一个纯新手，刚刚接触 UE 开发，有一点 C++ 基础的大学生进行解释，确保内容完善，对于可能存疑的地方提供现实例子；对于专有名词，先解释专有名字再作后续解释；对于数据的处理确保对每个处理步骤详细全面的讲解，每一步做了哪些操作
```