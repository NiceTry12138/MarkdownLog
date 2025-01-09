# AI

## 关于 AI 寻路

### 路径规划

[Navigation](../../Navigation/README.md) 是 Unreal 和 Unity 都在使用的经典导航网格生成算法，通过体素化、凸多边形划分、寻路、路径优化 等几步就可以实现 AI 智能体的路径搜索功能

关于寻路算法有很多种：A*、JSP、广搜等

### AI 避障

https://www.red3d.com/cwr/steer/gdc99/

https://www.bilibili.com/video/BV1Nb421n7qJ

https://www.bilibili.com/video/BV1yCqfYuEh1

AI 路径规划 和 AI 避障 属于两个不同业务范畴，路径规划决定大的方向，避障更属于角色行为

所以对于 AI 系统来说，一般将**路径规划**和**避障**拆分实现

> Unreal 的 CharacterMovement 支持 RVO 避障，不过需要勾选


## 关于 AI 决策

- 状态机
- 行为树
- GOAP

## 关于 Unreal 中的 AI 模块

### 如何让 AI 动起来

#### 通过 `UAITask_MoveTo` 请求移动

通过 `UAITask_MoveTo::AIMoveTo` 函数可以窥探一二

![](Image/001.png)

```cpp
UFUNCTION(BlueprintCallable, Category = "AI|Tasks", meta = (AdvancedDisplay = "AcceptanceRadius,StopOnOverlap,AcceptPartialPath,bUsePathfinding,bUseContinuousGoalTracking,ProjectGoalOnNavigation", DefaultToSelf = "Controller", BlueprintInternalUseOnly = "TRUE", DisplayName = "Move To Location or Actor"))
static AIMODULE_API UAITask_MoveTo* AIMoveTo(AAIController* Controller, FVector GoalLocation, AActor* GoalActor = nullptr,
    float AcceptanceRadius = -1.f, EAIOptionFlag::Type StopOnOverlap = EAIOptionFlag::Default, EAIOptionFlag::Type AcceptPartialPath = EAIOptionFlag::Default,
    bool bUsePathfinding = true, bool bLockAILogic = true, bool bUseContinuousGoalTracking = false, EAIOptionFlag::Type ProjectGoalOnNavigation = EAIOptionFlag::Default);
```

通过 `DisplayName = "Move To Location or Actor"` 可以知道该函数在蓝图中显示的名字是 `Move To Location Or Actor`

在该函数中，将创建了一个 `UAITask_MoveTo` 对象，将各种输入参数构建成 `FAIMoveRequest` 对象，最后将 `FAIMoveRequest` 和输入的 `Controller` 设置到 `UAITask_MoveTo` 对象中

- 创建 `FAIMoveRequest` 

```cpp
FAIMoveRequest MoveReq;
if (InGoalActor)
{
    MoveReq.SetGoalActor(InGoalActor);
}
else
{
    MoveReq.SetGoalLocation(InGoalLocation);
}

MoveReq.SetAcceptanceRadius(AcceptanceRadius);
MoveReq.SetReachTestIncludesAgentRadius(FAISystem::PickAIOption(StopOnOverlap, MoveReq.IsReachTestIncludingAgentRadius()));
MoveReq.SetAllowPartialPath(FAISystem::PickAIOption(AcceptPartialPath, MoveReq.IsUsingPartialPaths()));
MoveReq.SetUsePathfinding(bUsePathfinding);
MoveReq.SetProjectGoalLocation(FAISystem::PickAIOption(ProjectGoalOnNavigation, MoveReq.IsProjectingGoal()));
if (Controller)
{
    MoveReq.SetNavigationFilter(Controller->GetDefaultNavigationFilterClass());
}
```

- 设置 `UAITask_MoveTo`

```cpp
MyTask->SetUp(Controller, MoveReq);
MyTask->SetContinuousGoalTracking(bUseContinuousGoalTracking);

if (bLockAILogic)
{
    MyTask->RequestAILogicLocking();
}
```

在创建完 `UAITask_MoveTo` 之后，会自动执行 `Activate` 方法

```cpp
void UAITask_MoveTo::Activate()
{
	Super::Activate();

	UE_CVLOG(bUseContinuousTracking, GetGameplayTasksComponent(), LogGameplayTasks, Log, TEXT("Continuous goal tracking requested, moving to: %s"),
		MoveRequest.IsMoveToActorRequest() ? TEXT("actor => looping successful moves!") : TEXT("location => will NOT loop"));

	MoveRequestID = FAIRequestID::InvalidRequest;
	ConditionalPerformMove();
}
```

在 `ConditionalPerformMove` 函数中，检查 `Controller` 能否更新 `Path` 状态

- 如果可以，则直接调用 `PerformMove`
- 如果不可以，则启动定时器，定时重复调用 `ConditionalPerformMove` 检查是否可行

```cpp
void UAITask_MoveTo::ConditionalPerformMove()
{
	if (MoveRequest.IsUsingPathfinding() && OwnerController && OwnerController->ShouldPostponePathUpdates())
	{
		UE_VLOG(GetGameplayTasksComponent(), LogGameplayTasks, Log, TEXT("%s> can't path right now, waiting..."), *GetName());
		OwnerController->GetWorldTimerManager().SetTimer(MoveRetryTimerHandle, this, &UAITask_MoveTo::ConditionalPerformMove, 0.2f, false);
	}
	else
	{
		MoveRetryTimerHandle.Invalidate();
		PerformMove();
	}
}
```

> 在 `AIController` 的 `ShouldPostponePathUpdates` 方法中就是检查 `PathFollowingComponent` 有没有被创建或者初始化

在 `PerformMove` 方法中，通过将 `MoveRequest` 对象设置给 `AIController`，并以此获得 `FNavPathSharedPtr` 对象 和 `FPathFollowingRequestResult` 状态

```cpp
// start new move request
FNavPathSharedPtr FollowedPath;
const FPathFollowingRequestResult ResultData = OwnerController->MoveTo(MoveRequest, &FollowedPath);

// ... do something
switch (ResultData.Code)
{
    // ..... do something 
	case EPathFollowingRequestResult::RequestSuccessful:
		MoveRequestID = ResultData.MoveId;
		PathFinishDelegateHandle = PFComp->OnRequestFinished.AddUObject(this, &UAITask_MoveTo::OnRequestFinished);
		SetObservedPath(FollowedPath);
    // ..... do something
}
```

所以说，`UAITask_MoveTo` 本质上不处理 AI 移动逻辑，而是交给 `AIController` 做，自己只是监听一个结果和状态而已

在 `PerformMove` 中 `UAITask_MoveTo` 监听了 `OnRequestFinished` 结束事件

在 `SetObservedPath` 中 `UAITask_MoveTo` 监听了导航中路径相关事件

- 导航路径事件枚举

```cpp
UENUM()
namespace ENavPathEvent
{
	enum Type : int
	{
		Cleared,
		NewPath,
		UpdatedDueToGoalMoved,
		UpdatedDueToNavigationChanged,
		Invalidated,
		RePathFailed,
		MetaPathUpdate,
		Custom,
	};
}
```

- 监听事件变换

```cpp
Path->EnableRecalculationOnInvalidation(false);
PathUpdateDelegateHandle = Path->AddObserver(FNavigationPath::FPathObserverDelegate::FDelegate::CreateUObject(this, &UAITask_MoveTo::OnPathEvent));
```

- 根据事件触发枚举，更新自身状态

```cpp
void UAITask_MoveTo::OnPathEvent(FNavigationPath* InPath, ENavPathEvent::Type Event)
{
	const static UEnum* NavPathEventEnum = StaticEnum<ENavPathEvent::Type>();
	switch (Event)
	{
	case ENavPathEvent::NewPath:
	case ENavPathEvent::UpdatedDueToGoalMoved:
	case ENavPathEvent::UpdatedDueToNavigationChanged:
            // ... Do Something
			FinishMoveTask(EPathFollowingResult::Aborted);
		break;

	case ENavPathEvent::Invalidated:
		ConditionalUpdatePath();
		break;

	case ENavPathEvent::Cleared:
	case ENavPathEvent::RePathFailed:
		FinishMoveTask(EPathFollowingResult::Aborted);
		break;

	case ENavPathEvent::MetaPathUpdate:
	default:
		break;
	}
}
```

在执行 `EndTask` 的时候(基类实现)，会触发 `OnDestroy` 方法，在 `OnDestroy` 方法中让 `AIController` 停止移动，清空 `Path` 引用

```cpp
ResetObservers();
ResetTimers();

if (MoveRequestID.IsValid())
{
    UPathFollowingComponent* PFComp = OwnerController ? OwnerController->GetPathFollowingComponent() : nullptr;
    if (PFComp && PFComp->GetStatus() != EPathFollowingStatus::Idle)
    {
        PFComp->AbortMove(*this, FPathFollowingResultFlags::OwnerFinished, MoveRequestID);
    }
}

Path = nullptr;
```

所以综上所述， `UAITask_MoveTo` 本身不做移动处理，将请求转发给 `AIController`，然后监听一下移动结束和移动状态，再将对应的状态透传出来

#### AIController 控制移动

`AIController` 控制智能体移动的入口就是 `AAIController::MoveTo`

首先需要注意 `FAIMoveRequest` 结构体

![](Image/001.png)

注意到 `Move To Location Or Actor` 可以传入 `Actor` 也可以传入 `Location`，所以 `FAIMoveRequest` 需要存储这些信息

在 `UAITask_MoveTo` 中设置就有这段代码

```cpp
if (InGoalActor)
{
    MoveReq.SetGoalActor(InGoalActor);
}
else
{
    MoveReq.SetGoalLocation(InGoalLocation);
}
```

所以如果同时设置了 `GoalActor` 和 `GoalLocation`，会使用 `GoalActor` 

在 `AAIController::MoveTo` 中会判断 `GoalActor` 的坐标是否有效

```cpp
if (MoveRequest.GetGoalLocation().ContainsNaN() || FAISystem::IsValidLocation(MoveRequest.GetGoalLocation()) == false)
{
    UE_VLOG(this, LogAINavigation, Error, TEXT("AAIController::MoveTo: Destination is not valid! Goal(%s)"), TEXT_AI_LOCATION(MoveRequest.GetGoalLocation()));
    bCanRequestMove = false;
}
```

然后会将 `GoalActor` 坐标映射到导航网格上，具体的代码在 `UNavigationSystemV1::ProjectPointToNavigation` 中

![](Image/003.png)

`DefaultQueryExtent` 的值默认是 `(50, 50, 50)`

![](Image/002.png)