# 感知 AIPerception

通常来说 `AIPerceptionComponent` 应该放在 `AIController` 上。Pawn 提供接口用于控制行为，至于何时调用接口由 AIController 来决定

AI 感知系统分为五个部分

1. AIPerceptionSystem: AI感知系统 核心数据处理 信息传递
2. AISense: 提供具体 AI 感知
3. AISenseConfig: 配置 AISense 所需数据
4. AIStimulus: 感知系统中的刺激
5. AIPerceptionComponent: 配置 AISenseConfig

## 感知配置 UAISenseConfig

![](Image/002.png)

![](Image/001.png)

```cpp
class UAISenseConfig : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sense, AdvancedDisplay)
	FColor DebugColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sense, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float MaxAge;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sense)
	uint32 bStartsEnabled : 1;

	mutable FString CachedSenseName;

	AIMODULE_API virtual TSubclassOf<UAISense> GetSenseImplementation() const;

    // other functions ...
}
```

`MaxAge` 表示当该 `AISense` 被触发之后，多长时间遗忘，如果值为 0 表示从不遗忘

`UAISenseConfig` 只是用于配置，真正的功能实现是在 `GetSenseImplementation` 返回的实例对象

```cpp
TSubclassOf<UAISense> UAISenseConfig_Damage::GetSenseImplementation() const
{
    // TSubclassOf<UAISense_Damage> Implementation;
	return Implementation;
}

TSubclassOf<UAISense> UAISenseConfig_Sight::GetSenseImplementation() const 
{ 
    // TSubclassOf<UAISense_Sight> Implementation;
	return *Implementation; 
}

TSubclassOf<UAISense> UAISenseConfig_Touch::GetSenseImplementation() const
{
	return UAISense_Touch::StaticClass();
}
```

> `UAISenseConfig_Sight` 的 `GetSenseImplementation` 返回的是 `*Implementation` 就是 `UClass`，然后再构造新的 `TSubclassOf<UAISense>` 作为返回值

在 `UAIPerceptionComponent` 就存储着这些配置的数组

```cpp
UPROPERTY(EditDefaultsOnly, Instanced, Category = "AI Perception")
TArray<TObjectPtr<UAISenseConfig>> SensesConfig;
```

## AIPerceptionComponent

Actor 的生命周期

1. UObject::PostLoad，当 Actor 被拖到场景中
2. UActorComponent:OnComponentCreated 当 Actor 被创建出来时调用
3. AActor::PreRegisterAllComponents
   1. UActorComponent::RegisterComponent
4. AActor::PostRegisterAllComponents
5. AActor::PostActorCreated
6. AActor::UserConstructionScript
7. AActor::OnConstruction
8. AActor::PostActorConstruction
9.  AActor::PreInitializeComponents
10. AActor::InitializeComponents
    1. AActorComponent::Activate
    2. AActorComponent::InitializeComponent
11. AActor::PostInitializeComponents
12. AActor::OnActorSpawned
13. AActor::BeginPlay

在 `UAIPerceptionComponent::OnRegister` 中会将自己注册到 `UAIPerceptionSystem` 中

```cpp
void UAIPerceptionComponent::OnRegister()
{
    // 注册 EndPlay
    Owner->OnEndPlay.AddUniqueDynamic(this, &UAIPerceptionComponent::OnOwnerEndPlay);
    // 其他设置

	UAIPerceptionSystem* AIPerceptionSys = UAIPerceptionSystem::GetCurrent(GetWorld());

    for (auto SenseConfig : SensesConfig)
    {
        if (SenseConfig)
        {
            RegisterSenseConfig(*SenseConfig, *AIPerceptionSys);
        }
    }

    AIPerceptionSys->UpdateListener(*this);
    
    // 其他设置
}
```

当 `OnOwnerEndPlay` 或者 `OnUnregister` 的时候会执行 `CleanUp`

```cpp
void UAIPerceptionComponent::CleanUp()
{
    // 其他操作

    // UnregisterListener
    UAIPerceptionSystem* AIPerceptionSys = UAIPerceptionSystem::GetCurrent(GetWorld());
    if (AIPerceptionSys != nullptr)
    {
        AIPerceptionSys->UnregisterListener(*this);
        AActor* MutableBodyActor = GetMutableBodyActor();
        if (MutableBodyActor)
        {
            AIPerceptionSys->UnregisterSource(*MutableBodyActor);
        }
    }

    // 取消事件监听
    Owner->OnEndPlay.RemoveDynamic(this, &UAIPerceptionComponent::OnOwnerEndPlay);
}
```

### RegisterSenseConfig

`UAIPerceptionComponent::RegisterSenseConfig` 核心逻辑就下面这三步

1. 获取 UAISenseConfig 对应的 UAISense 实际运行类
2. 将 UAISense 注册到  AIPercetionSystem 中
3. 将 UAISense 对应的 SenseID 添加到 PerceptionFilter 中
4. 缓存 UAISense 对应的 MaxAge

```cpp
const TSubclassOf<UAISense> SenseImplementation = SenseConfig.GetSenseImplementation();
const FAISenseID SenseID = AIPerceptionSys.RegisterSenseClass(SenseImplementation);
PerceptionFilter.AcceptChannel(SenseID);
SetMaxStimulusAge(SenseID, SenseConfig.GetMaxAge());
```

`PerceptionFilter` 是什么？

```cpp
struct FPerceptionChannelAllowList
{
	typedef int32 FFlagsContainer;
	FFlagsContainer AcceptedChannelsMask;

	FORCEINLINE_DEBUGGABLE FPerceptionChannelAllowList& AcceptChannel(FAISenseID Channel)
	{
		AcceptedChannelsMask |= (1 << Channel);
		return *this;
	}

    // 其他功能函数
}
```

结构非常简单， `FPerceptionChannelAllowList` 用于维护一个 int32 的属性，该属性用于做位运算


参考 `AcceptChannel` 函数实现，每个 `AISenseID` 都对应 `AcceptedChannelsMask` 32 位中的一个

那么，使用位运算就可以很方便的对 AISense 进行过滤，或者判断是否需要对该 AISense 做出反应

> `ShouldRespondToChannel` 函数的作用就是过滤

### 事件

所有感知事件的触发都是通过事件的方式，传递出去的

| 事件 | 作用 |
| --- | --- |
| OnPerceptionUpdated |  |
| OnTargetPerceptionForgotten |  |
| OnTargetPerceptionUpdated |  |
| OnTargetPerceptionInfoUpdated |  |

## AIPerceptionSystem





## 感知行为实现 UAISense

### UAISenseID

```cpp
struct FAISenseCounter : FAIBasicCounter<uint8>
{};
typedef FAINamedID<FAISenseCounter> FAISenseID;
```

这里出现两个结构体 `FAINamedID` 和 `FAIBasicCounter`

```cpp
template<typename TCounterType>
struct FAIBasicCounter
{
	typedef TCounterType Type;
protected:
	Type NextAvailableID;
public:
	FAIBasicCounter() : NextAvailableID(Type(0)) {}
	Type GetNextAvailableID() { return NextAvailableID++; }
	uint32 GetSize() const { return uint32(NextAvailableID); }
    // 其他工具函数
}
```

`FAIBasicCounter` 就是维护一个 int 类型的属性

```cpp
template<typename TCounter>
struct FAINamedID
{
	const typename TCounter::Type Index;
	const FName Name;
private:
	static AIMODULE_API TCounter Counter;
public:
	FAINamedID(const FName& InName)
		: Index(GetCounter().GetNextAvailableID()), Name(InName)
	{}

    // 其他函数
}
```

`FAINamedID` 的内容，总共有三个属性

1. Index 用于表示 ID
2. Name 用于表示对应的 AISense 的名称
3. `Counter` 就是 `FAIBasicCounter`

注意，`Counter` 属性是 `static` 的，也就是所有的 `AISenseID` 通用的

每次调用 `GetCounter().GetNextAvailableID()` 给 `FAISenseID` 的 `Index` 赋值的时候都会让 `NextAvailableID` 的值 +1

综上所述，简单来说

`FAIBasicCounter`，就是一个 uint8 的整数，每次创建一个全新的 `FAISenseID` 的时候都会让这个值 +1 

但是，不是每次都会创建 `FAISenseID` 

在 `UAIPerceptionSystem::RegisterSenseClass` 函数中可以看到如何获取 AISense 的 AISenseID 的

```cpp
FAISenseID SenseID = UAISense::GetSenseID(SenseClass);

static FAISenseID GetSenseID(const TSubclassOf<UAISense> SenseClass) { return SenseClass ? ((const UAISense*)SenseClass->GetDefaultObject())->SenseID : FAISenseID::InvalidID(); }
```

没错，是从 CDO 中获取 AISenseID 的

这也就意味着，每种类型的 FAISence 对应的 FAISenseID 是相同的

如果是一个全新类型的 FAISense，它没有有效的 SenseID，那么会通过来创建一个新的 FAISenseID

```cpp
FAISenseID UAISense::UpdateSenseID()
{
	if (SenseID.IsValid() == false)
	{
		SenseID = FAISenseID(GetFName());
	}
	return SenseID;
}
```
 
> 注意，这里的构造函数，使得 Counter.NextAvailableID 的数值 +1

所以说，总共有多少个 FAISense，那么其对应的 AISenseID 最大值就是多少

这也是为什么 `PerceptionFilter` 可以只用一个 int32 来过滤 FAISense，毕竟一个项目不可能创建 2^32 中类型的 FAISense