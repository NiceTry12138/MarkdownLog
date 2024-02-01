# Recast Navmesh

推荐文章

- 知乎 `Recast Navigation` 源码解析：https://zhuanlan.zhihu.com/p/484520809
- 知乎 `Recast Navigation` 基础：https://zhuanlan.zhihu.com/p/74537236
- `Recast Tutorial`：https://digestingduck.blogspot.com/2010/02/slides-from-past.html
  
重要论文

- Path Planning for a vision-based autonomous robot
- Simplified 3D Movement and Pathingfinding Using Navigation Meshes
- Volumetric cell-and-portal generation
- Crowdws In A Polygon Soup: Next-Gen Path Planning (GDC， 推荐)

[Recast Navigation 项目](https://github.com/recastnavigation/recastnavigation)

[学习教程来源](https://www.bilibili.com/video/BV19G4y187Fz)

## 导论

根据时间顺序，有这么几个重要概念被提出

| 里程碑 | 作者 | 时间 |
| --- | --- | --- |
| Meadow Mapping | Ronald C. Arkin | 1986 |
| Volumetric Cell-and-protal generation | Denis Haumont | 2003 |
| Corwds In A Polygon Soup: Next-Gen Path Planning | David Miles | 2006 |
| Recast Navigation | Mikko Mononen | 2008 |

### **Meadow Mapping**

Ronald C. Arkin 是一位美国计算机科学家，他在 1987 年发表了一篇论文 **《Path planning for a vision-based autonomous robot》** ，介绍了 `Meadow mapping` 的原理和实现

Meadow mapping 是一种用于生成导航网格的方法，它将自由空间分解成**凸多边形**的集合，然后以每个凸多边形的边缘中点作为寻路节点，使用 A* 算法进行寻路。它还可以根据不同的地形类型（如人行道，草地，砾石等）设置不同的代价函数，优化寻路结果。它是现代 Navmesh 系统的雏形。

[Path planning for a vision-based autonomous robot 论文](https://www.researchgate.net/publication/356666030_Path_Planning_for_Autonomous_Mobile_Robots_A_Review))

[路径导航算法示例](https://github.com/zhm-real/PathPlanning)

`Meadow Mapping` 提出一个概念：把世界**凸多边形化**

为什么是凸多边形？

1. 凸多边形内任意俩点连接不会超出多边形区域
2. 凸多边形边上的点便于做寻路最小单位

凸多边形内部不需要寻路

我们只需要将世界划分成多个凸多边形，那么就只需要研究凸多边形之间如何连通即可，凸多边形内部直接一根直线连接即可

于是我们将寻路问题转换为以凸多边形**边缘中心为节点**的图论搜索问题

为什么不用 `Grid` （网格） ？

对于大面积的开放区域来说，格子作为寻路基本单位则数量庞大；但是一个凸多边形可以是一个很大的区域，所以对于寻路来凸多边形的效率更高

所以一个寻路流程大概如下

1. 得到一个 3D 场景
2. 获得可行走区域 `Reachable Space`（以前是手动绘制，现在可以自动绘制）
3. 将可行走区域划分成多个凸多边形 `Convexify the Space`
4. 凸多边形为单位，继续路径查询 `Path Finding`
5. 路径改进，得到更为平滑的路径 

那么会有这么几个问题？

1. 如何自动绘制可行走区域
2. 如何区域划分凸多边形
3. 如何进行路径查询

**区域划分凸多边形**

![](Image/001.png)

如果区域是凸多边形，则直接结束算法

如果区域不是凸多边形，则找到一个凹的角（>180°），尝试将其与多边形内部其他的点连接起来，这个时候会得到两个新的区域，也就是上图的 A 和 B，然后对这两个区域再进行相同的算法，如此一来整个场景就都是凸多边形

- 如何判断一个多边形是凸多边形

检查是否存在大于 180° 的角