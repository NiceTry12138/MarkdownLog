
# AStar

A* 算法结合了 Dijkstra 算法（保证找到最短路径）和 贪婪最佳优先搜索（优先探索目标方向）的优点

1. 像 Dijkstra 一样，它记录从起点到当前节点的实际移动代价（g(n)）
2. 像贪婪最佳优先搜索一样，它使用一个启发式函数（h(n)）来估计从当前节点到目标节点的剩余代价

参考代码如下：

https://github.com/ai-winter/ros_motion_planning/blob/master/src/core/path_planner/path_planner/src/graph_planner/astar_planner.cpp

```cpp
template <typename T>
class Node
{
protected:
  T x_, y_;       // x 轴坐标 y 轴坐标
  double g_;      // 从 StartNode 到 CurrentNode 的 Cast 
  double h_;      // 从 CurrentNode 到 EndNode 的 欧几里得距离 
  int id_;        // Node 节点的 ID
  int pid_;       // Node 节点的 父节点

public:
  
  // 提供排序的仿函数
  struct compare_cost
  {
    bool operator()(const Node& n1, const Node& n2) const
    {
      return (n1.g() + n1.h() > n2.g() + n2.h()) || ((n1.g() + n1.h() == n2.g() + n2.h()) && (n1.h() > n2.h()));
    };
  };
}
```

观察 Node 节点的代码，比较一个节点 Cast 的大小是将 g 值 + h 值进行比较，如果相等，则单纯比较 h 值

解析来是 A* 算法的实现

- `std::priority_queue<Node, std::vector<Node>, Node::compare_cost> open_list;` 使用 `priority_queue` 优先队列对插入的数据进行排序

结合 `Node::compare_cost` 的排序函数，可知 `open_list` 中 `Node` 是按 `Cast` 从小到大进行排序

每次都取出 `open_list` 中 Cast 最小的 Node，也就是第一个 Node

对 Node 周围的 8 个 Node 进行遍历，将其添加到 `open_list` 中

> 添加 Node 之前先判断其在 `open_list` 或者 `closed_list` 中是否已经存在，避免重复处理 Node 

```cpp
bool AStarPathPlanner::plan(const Point3d& start, const Point3d& goal, Points3d& path, Points3d& expand)
{
  double m_start_x, m_start_y, m_goal_x, m_goal_y;
  if ((!validityCheck(start.x(), start.y(), m_start_x, m_start_y)) ||
      (!validityCheck(goal.x(), goal.y(), m_goal_x, m_goal_y)))
  {
    return false;
  }

  Node start_node(m_start_x, m_start_y);
  Node goal_node(m_goal_x, m_goal_y);
  start_node.set_id(grid2Index(start_node.x(), start_node.y()));
  goal_node.set_id(grid2Index(goal_node.x(), goal_node.y()));

  // clear vector
  path.clear();
  expand.clear();

  // open list and closed list
  std::priority_queue<Node, std::vector<Node>, Node::compare_cost> open_list;
  std::unordered_map<int, Node> closed_list;

  open_list.push(start_node);

  // main process
  while (!open_list.empty())
  {
    // pop current node from open list
    auto current = open_list.top();
    open_list.pop();

    // current node does not exist in closed list
    if (closed_list.find(current.id()) != closed_list.end())
      continue;

    closed_list.insert(std::make_pair(current.id(), current));
    expand.emplace_back(current.x(), current.y());

    // goal found
    if (current == goal_node)
    {
      const auto& backtrace = _convertClosedListToPath<Node>(closed_list, start_node, goal_node);
      for (auto iter = backtrace.rbegin(); iter != backtrace.rend(); ++iter)
      {
        // convert to world frame
        double wx, wy;
        costmap_->mapToWorld(iter->x(), iter->y(), wx, wy);
        path.emplace_back(wx, wy);
      }
      return true;
    }

    // explore neighbor of current node
    for (const auto& motion : motions)
    {
      // explore a new node
      auto node_new = current + motion;
      node_new.set_g(current.g() + motion.g());
      node_new.set_id(grid2Index(node_new.x(), node_new.y()));

      // node_new in closed list
      if (closed_list.find(node_new.id()) != closed_list.end())
        continue;

      node_new.set_pid(current.id());

      // next node hit the boundary or obstacle
      // prevent planning failed when the current within inflation
      if ((node_new.id() < 0) || (node_new.id() >= map_size_) ||
          (costmap_->getCharMap()[node_new.id()] >= costmap_2d::LETHAL_OBSTACLE * obstacle_factor_ &&
           costmap_->getCharMap()[node_new.id()] >= costmap_->getCharMap()[current.id()]))
        continue;

      // if using dijkstra implementation, do not consider heuristics cost
      if (!is_dijkstra_)
        node_new.set_h(std::hypot(node_new.x() - goal_node.x(), node_new.y() - goal_node.y()));

      // if using GBFS implementation, only consider heuristics cost
      if (is_gbfs_)
        node_new.set_g(0.0);
      // else, g will be calculate through node_new = current + m

      open_list.push(node_new);
    }
  }

  return false;
}
```

`motions` 的内容如下，是为了方便遍历 上下左右 + 斜方向 总共 八方向 的 Node 节点而定义的一个数组

```cpp
const std::vector<Node> motions = {
    { 0, 1, 1.0 },          { 1, 0, 1.0 },           { 0, -1, 1.0 },          { -1, 0, 1.0 },
    { 1, 1, std::sqrt(2) }, { 1, -1, std::sqrt(2) }, { -1, 1, std::sqrt(2) }, { -1, -1, std::sqrt(2) },
};
```

> 分别对应 上、右、下、左、右上、右下、左上、左下


