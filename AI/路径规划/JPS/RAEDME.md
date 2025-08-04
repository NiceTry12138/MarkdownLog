# JPS

> Jump Point Search

参考文档： https://zhuanlan.zhihu.com/p/500807732
参考代码： https://github.com/ai-winter/ros_motion_planning/blob/master/src/core/path_planner/path_planner/src/graph_planner/jps_planner.cpp

定义基本结构体 Node

```cpp
class JNode : public Node
{
  public:
    /* @brief Construct a new JNode object
     * @param x   节点对应 X 轴坐标
     * @param y   节点对应 Y 轴坐标
     * @param g   节点对应 到 StartNode 的 Cast
     * @param h   节点对应 到 EndNode 的 Cast
     * @param id  节点对应 的 ID
     * @param pid 节点对应 的 Parent 的 ID
     * @param fid 节点对应 强制邻居 Forced neighbor ID
     */
    JNode(int x = 0, int y = 0, double g = 0.0, double h = 0.0, int id = 0, int pid = -1, int fid = 0)
      : Node(x, y, g, h, id, pid), fid_(fid)
    {}  

    struct compare_cost
    {
        bool operator()(const Node& n1, const Node& n2) const
        {
            return (n1.g() + n1.h() > n2.g() + n2.h()) || ((n1.g() + n1.h() == n2.g() + n2.h()) && (n1.h() > n2.h()));
        };
    };
}
```


