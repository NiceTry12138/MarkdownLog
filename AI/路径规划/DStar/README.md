# DStar

DStar 也称 D* 算法

> DStar 是 Dynamic A Star 的简称

与 A* 算法类似，通过维护一个 OpenList 来对场景中的路径节点进行搜索，不同的地方是 D* 算法是从 **终点** 向 **起点** 进行查询

- 对 A* 来说，从 **起点** 进行搜索，所有搜索过的节点都记录的是到 **起点** 的最近距离
- 对 D* 来说，从 **终点** 进行搜索，所有搜索过的节点都记录的是到 **终点** 的最近距离

从终点开始搜索的优势是，当 AI 在移动过程中出现了原本可行走的路变成不可行走时，可以不用完全重新规划路线，最开始搜索过一次的信息能够继续使用

所以，通常来说 D* 算法分为两个阶段

1. 使用 A* 或者其他算法，找到从 终点 到 起点 的路径
2. 动态避障搜索，当移动过程中发现某个节点变成障碍物时，重新规划路线

## 代码分析

### Node 结构

每个节点有三种状态

- New: 从未被遍历查找过
- OPEN: 节点被查找中，正在 OpenList 中
- CLOSED: 节点从 OpenList 中被移除

```cpp
class DNode : public Node
{
public:
enum Tag
{
    NEW = 0,
    OPEN = 1,
    CLOSED = 2
};
/**
    * @brief Construct a new DNode object
    * @param x   坐标点 X
    * @param y   坐标点 Y
    * @param g   到达目标点的成本代价
    * @param h   根据启发式的代价
    * @param id  Node 节点 ID
    * @param pid Node 父节点 ID
    * @param t   状态枚举值
    * @param k   历史最小 g 值
    */
DNode(const int x = 0, const int y = 0, const double g = std::numeric_limits<double>::max(),
        const double h = std::numeric_limits<double>::max(), const int id = 0, const int pid = -1, const int t = NEW,
        const double k = std::numeric_limits<double>::max())
    : Node(x, y, g, h, id, pid), t_(t), k_(k)
{
}
private:
int t_;     // DNode's tag among enum Tag
double k_;  // DNode's k_min in history
};
```

默认情况下，每个节点的 k、h、g 都是 infinite， t 是 `DNode::New`

```cpp
new DNode(i, j, std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), grid2Index(i, j), -1, DNode::NEW, std::numeric_limits<double>::max());
```


