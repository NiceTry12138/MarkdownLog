# 移动组件

## collide and slide 算法

> https://zhuanlan.zhihu.com/p/685714685

> https://news.16p.com/842025.html

> https://www.cnblogs.com/walterwhiteJNU/p/15720810.html

> https://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/guide/Manual/CharacterControllers.html#moving-a-character-controller

角色移动是一个复杂的计算过程，为了让游戏操作更加顺畅，往往需要非常多的细节处理，这些特殊的移动处理逻辑叫做 `collide and slide`

正如 `Phys-x` 物理引擎的 SDK 文档中所说，如果使用物理引擎来控制角色移动存在很多问题

- 缺乏连续碰撞检测，经典物理引擎使用离散碰撞检测，因此会出现`tunneling effect`
  - `tunneling effect` 当角色移动过快的时候，可能会穿过墙壁，因此需要限制角色最大速度
  - 即使没有 `tunneling`，当角色在角落向前推时，会出现角色抖动的情况，因为物理引擎不断将其前后移动到略有不同的位置
- 无法直接控制，**刚体**通常由**脉冲**或**力**控制，无法直接将角色移动到最终位置
- 摩擦问题，当角色在斜坡上时，通常不应该滑动，因此会设置无限大的摩擦力，当这也导致角色无法向前移动
- 不受控制的跳跃，当角色快速移动并于物体碰撞时，角色不应该被弹开，比如角色从高空落下时，落地双腿完全，并且不会弹跳。但是由于 `restitution` 的存在，会产生回弹效果

由于以上这些问题的存在，角色移动不能直接使用 **物理引擎** 来控制

### 碰撞查询

虚幻引擎将碰撞检测的信息封装成 `FHitResult`，虽然下面这些理论知识与虚幻引擎无关，为了方便参数解释，还是提前说明 

| FHitResult 的属性 | 解释 |
| --- | --- |
| bBlockingHit |  是否发生碰撞 |
| Time |  碰撞后实际移动距离除以检测移动距离 |
| Distance |  碰撞后实际移动距离 |
| Location |  碰撞后最终位置 |
| ImpactPoint |  碰撞接触点 |
| Normal |  碰撞切面法向量 |
| ImpactNormal |  碰撞切面法向量（非胶囊体和球体检测与Normal不同） |
| TraceStart |  检测开始位置 |
| TraceEnd |  检测结束位置 |
| bStartPenetrating |  是否在检测开始就有渗透情况 |
| PenetrationDepth |  渗透深度 |

`Sweep` 检测

> `Sweep` 扫过、掠过、大范围伸展

> https://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/guide/3.3.4/Manual/GeometryQueries.html#sweeps

![](Image/002.jpg)

以当前坐标为起点，以当前 **速度** 和 `Delta` 计算下一帧的理论坐标为终点，进行检测

如果中间检测到碰撞，则无法移动到终点坐标，从碰撞点计算角色下一帧真实坐标

- `TraceStart` 表示 检测起点，值为当前坐标
- `TraceEnd` 表示 检测终点，值为理论下一帧坐标
- `ImpactPoint` 表示 碰撞接触点
- `Location` 表示 下一帧的实际坐标
- `Distance` 表示 下一帧的实际坐标与当前坐标之间的距离
- `Time` 表示 $\frac{Distance}{TraceEnd-TraceStart}$，是一个 0~1 之间的值

--------

`InitialOverlaps` 检测

![](Image/003.jpg)

也就是在**开始位置**就检测到了重叠

这个时候 `bStartPenetrating` 值为 true，表示检测到了 `InitialOverlaps` 

`Penetrating` 表示重叠深度

> `Penetrating`： 渗透、贯穿、穿过

--------



## UE 的移动组件

> https://zhuanlan.zhihu.com/p/650314172

![](Image/001.jpg)

角色移动组件分层设计，各自负责不同的职责

| 组件 | 作用 |
| --- | --- |
| UMovementComponent | 基础移动 |
| UNavMovementComponent | 导航集成 |
| UPawnMovementComponent | 输入响应 |
| UCharacterMovementComponent | 角色物理 |

- `MovementComponent` 为基类，提供基本的更新坐标逻辑。通常用于 **可推动的物理道具**
  - 设置需要更新的组件(`UpdateComponent`)，通常是根组件，比如 `ACharacter` 的根**胶囊体组件**
  - 设置需要更新的组件(`UpdatedPrimitive`)，用于物理交互，如果 `UpdateComponent` 能转换成 `UPrimitiveComponent` 会直接使用 `UpdateComponent`
  - 设置 `MoveComponentFlags`，用于控制更新行为的精细开关
  - `Velocity` 存储实时移动速度向量，是驱动组件运动的核心数据
  - `PlaneConstraintNormal` 定义移动约束平面的法线方向(`(0,1,1) 限制 Y 轴移动`)
  - `PlaneConstraintOrigin` 定义移动约束平面的原点坐标，用于计算组件与平面的空间关系

```cpp
UpdatedComponent = IsValid(NewUpdatedComponent) ? NewUpdatedComponent : NULL;
UpdatedPrimitive = Cast<UPrimitiveComponent>(UpdatedComponent);
```

- `NavMovementComponent` 提供了AI寻路用的一些接口。通常用于 AI 控制的非人形物体
    - `NavAgentProps` 定义导航代理的物理特性和移动能力(如：半径、高度、最大速度、加速度等)，用于路径计算和碰撞检测
    - `FixedPathBrakingDistance` 定义减速到停止的距离，由 `bUseFixedBrakingDistanceForPaths` 控制是否启用
    - `PathFollowingComp` 处理路径跟随逻辑，通常是 `UPathFollowingComponent`

- `PawnMovementComponent` 定义了接受输入的接口。其Owner必须为APawn子类。通常用于自定义载具等

### MoveUpdateComponent

`UMovementComponent::MoveUpdateComponent` 是真正执行物体移动逻辑的函数接口，并且该函数不是虚函数，子类无法重写

```cpp
FORCEINLINE_DEBUGGABLE bool UMovementComponent::MoveUpdatedComponent(const FVector& Delta, const FRotator& NewRotation, bool bSweep, FHitResult* OutHit, ETeleportType Teleport)
{
	return MoveUpdatedComponentImpl(Delta, NewRotation.Quaternion(), bSweep, OutHit, Teleport);
}

bool UMovementComponent::MoveUpdatedComponentImpl( const FVector& Delta, const FQuat& NewRotation, bool bSweep, FHitResult* OutHit, ETeleportType Teleport)
{
	if (UpdatedComponent)
	{
		const FVector NewDelta = ConstrainDirectionToPlane(Delta);
		return UpdatedComponent->MoveComponent(NewDelta, NewRotation, bSweep, OutHit, MoveComponentFlags, Teleport);
	}

	return false;
}
```

对的，该函数直接调用 `UpdatedComponent` 的 `MoveComponent`，让组件自己更新自己

不过 `USceneComponent::MoveComponentImpl` 是虚函数，可以被子类重写，所以说根据设置的 `UpdatedComponent` 的不同，最后执行的移动逻辑也不相同

#### USceneComponent::MoveComponentImpl

对于 `USceneComponent::MoveComponentImpl` 的实现逻辑是比较简单的

1. 检查能否移动，组件的 `Mobility` 必须是 `Movable`
2. 调用 `ConditionalUpdateComponentToWorld` 确保组件的 `Transform` 已经更新
3. 检测是否是零位移 `Delta.IsZero()`，零位移表示不用移动，也就无需后续计算
4. 更新坐标和旋转 `InternalSetWorldLocationAndRotation`
5. 如果更新成功，则进行重叠检测 `UpdateOverlaps`，主要是递归更新 `AttachedChild`，并且更新自己的 `PhysicsVolume` 信息

`InternalSetWorldLocationAndRotation` 逻辑相对简单

1. 传入新的坐标、旋转参数
2. 如果存在父节点，基于父节点的坐标信息，更新传入的坐标和旋转信息
3. 判断是否存在坐标修改、旋转修改
4. 如果存在 坐标 或者 旋转 修改，更新 `RelativeLocation` 和 `RelativeRotation` 的值
5. 根据 `bCanEverAffectNavigation` 更新导航网格的信息

#### UPrimitiveComponent::MoveComponentImpl

前面说过 `MoveComponentImpl` 是一个虚函数

在 `UMovementComponent` 中的 `UpdatedComponent` 通常被设置为对象的根组件，而 `ACharacter` 的根组件是 `UCapsuleComponent`

`UCapsuleComponent` -> `UShapeComponent` -> `UPrimitiveComponent`

所以对于 `ACharacter` 来说，更新坐标执行的是 `UPrimitiveComponent::MoveComponentImpl`


