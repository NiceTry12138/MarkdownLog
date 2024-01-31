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

1. **Meadow Mapping**

Ronald C. Arkin 是一位美国计算机科学家，他在 1987 年发表了一篇论文 **《Path planning for a vision-based autonomous robot》** ，介绍了 `Meadow mapping` 的原理和实现

Meadow mapping 是一种用于生成导航网格的方法，它将自由空间分解成凸多边形的集合，然后以每个凸多边形的边缘中点作为寻路节点，使用 A* 算法进行寻路