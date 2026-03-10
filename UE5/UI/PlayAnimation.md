# UMG 的 Animation

官方文档：https://www.unrealengine.com/zh-CN/tech-blog/performance-at-scale-sequencer-in-unreal-engine-4-26

从 UE5 开始，数据的计算和更新是 ECS 框架进行计算的，这样计算的优势就是缓存友好

传统的 OOP 做法：遍历 UMovieSceneTrack → 遍历 UMovieSceneSection → 读取 FMovieSceneFloatChannel → 对 Widget 属性赋值。每帧都在做大量对象遍历和虚函数调用，Cache 不友好

ECS 的做法是

1. 每个需要被动画驱动的 **属性通道** （Opcity、Transofrm.X 等）都变成 ECS 中的一个 `Entity`
2. Entity 上挂着 `FloatResultComponent`（存当前帧的计算值）、`BoundObjectComponent`（绑定的 Widget 对象）、`PropertyTagComponent`（对应哪个属性） 等 `Component`
3. 每帧 `BlenderSystem`、`OuputSystem` 等 `System` 批量扫描所有 `Entity`，一次性完成插值计算和属性写入

- `UWidgetAnimation` 是数据层，提供 `Tracks`、`Sections`、`Channels` 关键帧数据
- `UUMGSequenceTickManager` 是全局调度中心
  - `UMovieSceneEntitySystemLinker` 提供 ECS 世界
    - `FEntityManager` 缓存所有的 Entity 和 Component
    - `FSystemGraph` 保存所有 System 的注册与调度图
  - `FMovieSceneEntitySystemRunner`求值调度器
- `UUMGSequencePlayer` 单个动画的控制器
  - `FMovieSceneRootEvalutionTemplateInstance` 数据 与 ECS 的桥梁

## 常见对象

### UWidgetAnimation

```cpp
class UWidgetAnimation : public UMovieSceneSequence
{
    UPROPERTY()
	TObjectPtr<UMovieScene> MovieScene;

	/**  */
	UPROPERTY()
	TArray<FWidgetAnimationBinding> AnimationBindings;
};
```

- 通过 `MovieScene` 存储所有关键帧数据
- 通过 `AnimationBindings` 存储绑定的 `Widget`

> `FWidgetAnimationBinding` 通过存储 `SlotName` 和 `WidgetName` 来获取具体的 `Widget` 对象

在 `UMovieScene::ObjectBindings` 对象中存储该对象对应的所有 `UMovieSceneTrack` 每个 Track 对应一个对象上的属性

注意 `UMovieSceneTrack::GetAllSections(...)` 函数，该函数会返回所有的 `Section`，注意需要继承 `UMovieSceneTrack` 重写 `GetAllSections` 函数，因为每个 `trace` 都维护自己的 `Section` 

`UMovieSceneSection::SectionRange` 保存着激活的时间范围，`UMovieSceneSection::ChannelProxy` 保存着所有的 Channel 信息

> `Player->Tick` 中调用 `Player::UpdateInternal` 注册更新请求到 `FMovieSceneEntitySystemRunner::UpdateQueue` 队列中，用于后续 `UUMGSequenceTickManager` 调用 `Runner` 执行更新

### UUMGSequencePlayer

`UUMGSequencePlayer` 是连接 `PlayAnimation` 和 `ECS 执行`（Runner::Flush）的中间层

Player 本身不做插值计算，它的核心职责是

1. 维护时间游标：知道动画当前播到第几帧
2. 处理边界逻辑：循环、PingPong、到达终点
3. 向 Runner 委托：把 **当前时间 + 播放状态** 封装成 `FMovieSceneContext`，提交给 ECS 处理

在 `UUMGSequencePlayer::PlayInternal` 和 `UUMGSequencePlayer::UpdateInternal` 函数中都有这么一段逻辑

1. 计算 Context
2. 向 Runner 中提交更新请求，并绑定回调函数，`PlayInternal` 中绑定 `OnBegunPlay`，`UpdateInternal` 中绑定 `OnComplete`
3. 如果是 `SynchronousRunner` 也就是要求同步计算，则直接调用 `Runner` 的 `Flush`

```cpp
// 前面是计算 Context 的步骤 
Runner->QueueUpdate(Context, RootTemplateInstance.GetRootInstanceHandle(), MoveTemp(OnComplete), UpdateFlags);

if (Runner == SynchronousRunner || !UE::UMG::GAsyncAnimationControlFlow)
{
	Runner->Flush();
	ApplyLatentActions(); // PlayInternal 没有调用这个函数
}
```

`Runner` 分为两种，一种是 `SynchronousRunner` 这个是同步计算，用于要求立刻更新值的情况；另一种是使用 `UUMGSequenceTickManager::GetRunner(...)`  获得的所有 `UserWidget` 公用的 `Runner`

> `UpdateInternal` 是在 `Tick` 函数中调用的

`UUserWidget::PlayAnimation` 被调用的时候，会就会执行 `PlayInternal` 函数，在这里会根据 `UWidgetAnimation` 是否是 `BlockingEvaluation` 来决定使用自己新建 Runner  还是使用全局共享的 Runner

### UUMGSequenceTickManager

挂载在 GEngine 上，全局唯一一个实例。由 FSlateApplication 的 Pre/PostTick 事件驱动

它将 **多个 Widget 的多个动画** 的求值合并为一次 `Flush`

```cpp
UPROPERTY(transient)
TMap<TWeakObjectPtr<UUserWidget>, FSequenceTickManagerWidgetData> WeakUserWidgetData;

UPROPERTY(transient)
TObjectPtr<UMovieSceneEntitySystemLinker> Linker;

TSharedPtr<FMovieSceneEntitySystemRunner> Runner;
```

持有 `Linker` 和 `Runner`，同时缓存了 `UserWidget` 和其对应的播放动画的信息

在 `UUMGSequenceTickManager::Get` 函数中，这是经典的 UE 的单例写法，绑定了 `FSlateApplication` 的 `PreTick` 和 `PostTick`

```cpp
FSlateApplication& SlateApp = FSlateApplication::Get();
FDelegateHandle PreTickHandle = SlateApp.OnPreTick().AddUObject(TickManager, &UUMGSequenceTickManager::TickWidgetAnimations);
check(PreTickHandle.IsValid());
TickManager->SlateApplicationPreTickHandle = PreTickHandle;

FDelegateHandle PostTickHandle = SlateApp.OnPostTick().AddUObject(TickManager, &UUMGSequenceTickManager::HandleSlatePostTick);
check(PostTickHandle.IsValid());
TickManager->SlateApplicationPostTickHandle = PostTickHandle;
```

在 `UUMGSequenceTickManager::TickWidgetAnimations` 函数中

1. 遍历所有的 UserWidget
   -  如果该 `UserWidget` 被 `GC`、**动画结束**、 `UserWidget` 对应的 `Slate` 控件无效，则将其从 `WeakUserWidgetData` 中删除
   -  其他情况，调用 `UserWidget::TickActionsAndAnimation` 来遍历的 `UserWidget::ActiveSequencePlayers`，调用所有的 `Player::Tick` 函数 
2. 调用 `ForceFlush(...)` 让 Runner 计算更新
3. 再次遍历所有的 UserWidget，清除无效对象

在 `UUMGSequenceTickManager::HandleSlatePostTick` 函数中

1. `Runner->HasQueuedUpdates()` 判断 Runner 中是否还残余请求
2. 如果有残余请求，则 `ForceFlush(...)` 处理这些残余请求，算是保底措施

### FMovieSceneEntitySystemRunner

```cpp
enum class ERunnerFlushState
{
	None           = 0,				// 
	Start          = 1 << 0,		// 做一些一次性准备（比如统计、调试标志、外部回调的 guard）
	Import         = 1 << 1,		// 把本帧涉及的 entities/components 导入到实体管理器（EntityManager）
	Spawn          = 1 << 2,		// 处理需要“生成/创建”的对象（MovieScene Spawnable、绑定对象代理等）或者把“需要存在”的对象状态准备到位
	Instantiation  = 1 << 3,		// 实例化 track/section 的运行态对象（例如 track instance、system 需要的运行数据）,建立 entities 之间的链接与缓存
	Evaluation     = 1 << 4,		// 系统按依赖顺序执行任务（scheduler 执行 tasks），计算曲线、采样、混合、约束，把结果写回
	Finalization   = 1 << 5,		// 清理、收尾、提交延迟结果，触发某些外部事件/回调
	EventTriggers  = 1 << 6,		// 在重入窗口内安全地触发事件
	PostEvaluation = 1 << 7,		// 对 SequenceInstance 调用 Post Evaluation 回调
	End            = 1 << 8,		// 与 Start 对称的 收尾，重置外部播放器/监听器状态

	// Signifies that, during the Finalization task, there were still outstanding tasks and we need to perform another iteration
	LoopEval       = Import | Spawn | Instantiation | Evaluation | Finalization | EventTriggers | PostEvaluation | End,

	// Initial flush state
	Everything     = Start | LoopEval,
};
```

`ERunnerFlushState` 不是 **单一状态机的当前状态** 那种枚举，而是一个 bitmask（按位组合）。 用来描述 `FMovieSceneEntitySystemRunner` 这次 `Flush` 还需要跑哪些阶段（tasks/phases）

在 `FMovieSceneEntitySystemRunner::Flush` 函数中，直接设置当前状态为 `ERunnerFlushState::Everything`

```cpp
void FMovieSceneEntitySystemRunner::Flush(double BudgetMs)
{
	using namespace UE::MovieScene;

	// If we're not currently evaluating, start by flushing everything
	if (FlushState == ERunnerFlushState::None)
	{
		FlushState = ERunnerFlushState::Everything;
	}

	FlushOutstanding(BudgetMs);
}
```

在 `FMovieSceneEntitySystemRunner::FlushNext(...)` 根据 `FlushState` 的值执行对应的函数

| 状态 | 对应执行的函数 |
| --- | --- |
| ERunnerFlushState::Start | StartEvaluation |
| ERunnerFlushState::Import | GameThread_UpdateSequenceInstances |
| ERunnerFlushState::Spawn | GameThread_SpawnPhase |
| ERunnerFlushState::Instantiation | GameThread_InstantiationPhase |
| ERunnerFlushState::Evaluation | GameThread_EvaluationPhase |
| ERunnerFlushState::Finalization | GameThread_EvaluationFinalizationPhase |
| ERunnerFlushState::EventTriggers | GameThread_EventTriggerPhase |
| ERunnerFlushState::PostEvaluation | GameThread_PostEvaluationPhase |
| ERunnerFlushState::End | EndEvaluation |

从 `Flush` 函数可以看到真正的执行体是 `FlushOutstanding(...)`

```cpp
void FMovieSceneEntitySystemRunner::FlushOutstanding(double BudgetMs, UE::MovieScene::ERunnerFlushState TargetState)
{
	UMovieSceneEntitySystemLinker* Linker = GetLinker();
    
	if (TargetState == ERunnerFlushState::None)
	{
		TargetState = ERunnerFlushState::Everything;
	}

    const double BudgetSeconds = BudgetMs / 1000.f;
	if (!bRequireFullFlush && BudgetSeconds > 0.0)
	{
		double StartTime = FPlatformTime::Seconds();
		do
		{
			if (!FlushNext(Linker))
			{
				break;
			}
		}
		while (FPlatformTime::Seconds() - StartTime < BudgetSeconds && EnumHasAnyFlags(FlushState, TargetState));
	}
	else
	{
		while (EnumHasAnyFlags(FlushState, TargetState))
		{
			if (!FlushNext(Linker))
			{
				break;
			}
		}
	}

	bRequireFullFlush = false;
}
```

- 如果 `BudgetMs` 传入值，表示限制 Flush 的时间，如果一次 Flush 非常耗时，那么会通过 分帧 的方式来优化
- 如果 `BudgetMs` 没有值，则默认为 0，表示没有计算时间限制，那么通过 `while` 来执行 `FlushNext` 处理所有的 `State`



## 播放动画

### 准备阶段

从 `PlayerAnimation` 开始窥探播放动画的准备过程

```cpp
UUMGSequencePlayer* UUserWidget::PlayAnimation(UWidgetAnimation* InAnimation, float StartAtTime, int32 NumberOfLoops, EUMGSequencePlayMode::Type PlayMode, float PlaybackSpeed, bool bRestoreState)
{
	SCOPED_NAMED_EVENT_TEXT("Widget::PlayAnimation", FColor::Emerald);

	UUMGSequencePlayer* Player = GetOrAddSequencePlayer(InAnimation);
	if (Player)
	{
		Player->Play(StartAtTime, NumberOfLoops, PlayMode, PlaybackSpeed, bRestoreState);

		OnAnimationStartedPlaying(*Player);

		UpdateCanTick();
}

	return Player;
}
```

从 `PlayAnimation` 函数作为起点，可以发现数据保存在 `UWidgetAnimation` 中，将数据交给 `UUMGSequencePlayer` 进行播放

通过 `GetOrAddSequencePlayer` 可以发现，`UserWidget` 会将自己注册给 `UUMGSequenceTickManager` 对象，然后通过 `InitSequencePlayer` 来初始化 `UUMGSequencePlayer` 对象，将 `Animation` 和 `UserWidget` 保存到 `Player` 中

```cpp
UUMGSequencePlayer* UUserWidget::GetOrAddSequencePlayer(UWidgetAnimation* InAnimation)
{
    AnimationTickManager = UUMGSequenceTickManager::Get(this);
    AnimationTickManager->AddWidget(this);

    UUMGSequencePlayer* FoundPlayer = nullptr;
    // 从 ActiveSequencePlayers 查找有播放该 Animation 的 Player
    // 如果有 则 return FoundPlayer
    // 如果没有 则创建 NewPlayer 并添加到 ActiveSequencePlayers
    UUMGSequencePlayer* NewPlayer = NewObject<UUMGSequencePlayer>(this, NAME_None, RF_Transient);
    ActiveSequencePlayers.Add(NewPlayer);

    // 初始化 NewPlayer
    NewPlayer->InitSequencePlayer(*InAnimation, *this);

    return NewPlayer;
}
```

这里要注意下 `UUMGSequencePlayer` 继承了 `IMovieScenePlayer` 接口，在 `IMoviewScenePlayer` 的构造函数中将自己注册到 `UE::MovieScene::GGlobalPlayerRegistry` 全局数组中

```cpp
// class UUMGSequencePlayer : public UObject, public IMovieScenePlayer
IMovieScenePlayer::IMovieScenePlayer()
{
	FWriteScopeLock ScopeLock(UE::MovieScene::GGlobalPlayerRegistryLock);

	UE::MovieScene::GGlobalPlayerRegistry.Shrink();
	UniqueIndex = UE::MovieScene::GGlobalPlayerRegistry.Add(this);

	UE::MovieScene::GGlobalPlayerUpdateFlags.PadToNum(UniqueIndex + 1, false);
	UE::MovieScene::GGlobalPlayerUpdateFlags[UniqueIndex] = 0;
}
```

### 开始播放

当一切准备就绪之后，通过 `Player->Play(...)` 来播放动画，具体的实现逻辑在 `UUMGSequencePlayer::PlayInternal` 函数中

1. 首先选择  `ECS Runner`

- 默认使用 `TickManager->GetRunner()`,即全局共享的 `Runner`。这意味着该动画将与所有其他 UMG 动画一起在帧末尾批量求值（性能好）
- 如果动画资产被标记为 **阻塞求值** ，则创建一个独立的 `SynchronousRunner`。这种模式下，动画更新会立即发生，不与其他动画合并（性能差，但是可以立刻得到值）

```cpp
TSharedPtr<FMovieSceneEntitySystemRunner> RunnerToUse = TickManager ? TickManager->GetRunner() : nullptr;
if (EnumHasAnyFlags(Animation->GetFlags(), EMovieSceneSequenceFlags::BlockingEvaluation))
{
    SynchronousRunner = MakeShared<FMovieSceneEntitySystemRunner>();
    RunnerToUse = SynchronousRunner;
}
```

2. 通过 `RootTemplateInstance` 将 `UWidgetAnimation`（面向对象数据）编译或链接到 `ECS` 系统中。它会在 `UMovieSceneEntitySystemLinker` 中注册这个 `Sequence` 实例，建立 `InstanceHandle`

```cpp
RootTemplateInstance.Initialize(*Animation, *this, nullptr, RunnerToUse);

// 如果通过 bRestoreState 要求 播放完恢复原状，会在这里 记录该属性的当前值，以便运行后还原
if (bInRestoreState)
{
    RootTemplateInstance.EnableGlobalPreAnimatedStateCapture();
}
```

3. 设置播放速度、方向（正向/反向/乒乓）、循环次数，计算起始时间 `TimeCursorPosition`

```cpp
bIsBeginningPlay = true;
bRestoreState = bInRestoreState;
PlaybackSpeed = FMath::Abs(InPlaybackSpeed);
PlayMode = InPlayMode;

FFrameTime LastValidFrame(Duration-1, 0.99999994f);

if (PlayMode == EUMGSequencePlayMode::Reverse)
{
    // When playing in reverse count subtract the start time from the end.
    TimeCursorPosition = LastValidFrame - StartAtTime * AnimationResolution;
}
else
{
    TimeCursorPosition = StartAtTime * AnimationResolution;
}

// Clamp the start time and end time to be within the bounds
TimeCursorPosition = FMath::Clamp(TimeCursorPosition, FFrameTime(0), LastValidFrame);
EndTime = FMath::Clamp(EndAtTime * AnimationResolution, FFrameTime(0), LastValidFrame);

if ( PlayMode == EUMGSequencePlayMode::PingPong )
{
    // When animating in ping-pong mode double the number of loops to play so that a loop is a complete forward/reverse cycle.
    NumLoopsToPlay = 2 * InNumLoopsToPlay;
}
else
{
    NumLoopsToPlay = InNumLoopsToPlay;
}

NumLoopsCompleted = 0;
bIsPlayingForward = InPlayMode != EUMGSequencePlayMode::Reverse;

PlayerStatus = EMovieScenePlayerStatus::Playing;
```

4. 立即让动画生效

```cpp
const FMovieSceneContext Context(FMovieSceneEvaluationRange(AbsolutePlaybackStart + TimeCursorPosition, AbsolutePlaybackStart + TimeCursorPosition, AnimationResolution), PlayerStatus);

auto OnBegunPlay = [this]
{
    this->bIsBeginningPlay = false;
};

// We queue an update instead of immediately flushing the entire linker so that we don't incur a cascade of flushes on frames when multiple animations are played
// In rare cases where the linker must be flushed immediately PreTick, the queue should be manually flushed 
RunnerToUse->QueueUpdate(Context, RootTemplateInstance.GetRootInstanceHandle(), FSimpleDelegate::CreateWeakLambda(this, OnBegunPlay), UE::MovieScene::ERunnerUpdateFlags::Flush);

if (RunnerToUse == SynchronousRunner || !UE::UMG::GAsyncAnimationControlFlow)
{
    RunnerToUse->Flush();
}
```

### 更新

本质还是通过 `UUMGSequenceTickManager::TickWidgetAnimations` 函数，去调用 Runner 按照状态顺序去执行对应的函数

## 优化

结合 https://zhuanlan.zhihu.com/p/649173697 和 [官方 UMG 优化文档](https://dev.epicgames.com/documentation/zh-cn/unreal-engine/optimization-guidelines-for-umg-in-unreal-engine?application_version=5.0#%E5%8A%A8%E7%94%BB%E5%BC%80%E9%94%80) 

由于 UMG 的 Animation 是 ECS 方案，除了计算之外引入和很多额外的开销

| 开销来源 | 说明 |
| --- | --- |
| TickWidgetAnimations 遍历 | 遍历所有 Widget，即使动画静止也要检查 |
| FMovieSceneContext 构造 | 每帧为每个 Player 分配时间上下文对象 |
| ECS Flush 状态机 | 即使 Spawn/Instantiation 跳过，Import→Evaluation 的开销依然固定存在 |
| Slate 标脏传播 | 每次属性变更向上传播脏标记到整个 Widget 树 |
| UMovieSceneEntitySystemLinker 锁定/解锁 | LockDown() 和 ReleaseLockDown() 每帧执行一次 |

这些都是固定基础成本（Fixed Overhead），不随复杂度降低而减少。如果只是驱动一个按钮的透明度，也要走完整个流程

ECS 的优化目标是：当 UMG 动画数量非常多、属性通道非常密集时，批量计算的吞吐量

它的意义场景是：

- 100 个 Widget 同时播放复杂动画，每个有 30+ 个属性通道。
- 此时 OOP 逐帧遍历 3000 个 Channel 的 Cache Miss 代价显著，ECS 的列式内存布局可以大幅降低这个成本。

但是大部分场景下 HUD 上只有 几个或者十几个 动画，每个动画只修改几个属性，此时使用 ECS 的开销远大于其节省的计算成本

所以很多时候，更加推荐的是 手动设置曲线，然后通过蓝图的 Tick 去更新对应蓝图节点的属性


