# 背包系统 Inventory

## 定义单个装备信息

首先定义 `ULyraInventoryItemFragment` 和 `ULyraInventoryItemDefinition` 用于配置装备信息

定义 `ULyraInventoryItemFragment`，用于定义装备 `Item` 具备的一个片段（信息），并提供实例创建的接口

```cpp
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class LYRAGAME_API ULyraInventoryItemFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(ULyraInventoryItemInstance* Instance) const {}
};
```

定义 `ULyraInventoryItemDefinition` 用于定义装备 `Item` 的配置信息，包括最基本的**显示名称**和**片段信息数组**，一个装备 `item` 可以具备多种**片段信息** 

```cpp
UCLASS(Blueprintable, Const, Abstract)
class ULyraInventoryItemDefinition : public UObject
{
	GENERATED_BODY()

public:
    // ... do something 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display, Instanced)
	TArray<TObjectPtr<ULyraInventoryItemFragment>> Fragments;

    // ... do something 
};
```

Lyra 提供多种片段信息

| 片段信息类型 | 作用 | 详细说明 |
| --- | --- | --- |
| UInventoryFragment_EquippableItem | 可装备物品 | 将物品标记为"可装备"并关联装备定义 |
| UInventoryFragment_PickupIcon | 拾取物外观 | 控制物品作为地面可拾取物时的外观 |
| UInventoryFragment_QuickBarIcon | 快捷栏图标 | 管理物品在玩家快捷栏中的视觉表现 |
| UInventoryFragment_SetStats | 属性设置 | 定义物品的基础属性（如攻击力、防御值等），使用 GameplayTag 作为属性键，支持灵活扩展 |
| UInventoryFragment_ReticleConfig | 准星配置 | 配置武器瞄准时的准星UI，允许多种准星类型（如：基础瞄准镜、高级狙击镜等） |

具体使用时，表现效果如下图所示，这就是一个 `ULyraInventoryItemDefinition` 的定义，并且配置了多种片段信息，这里定义的是一个 手枪(`Pistol`) 的装备信息

![](Image/001.png)

扩展性很强，各自模块基于 `ULyraInventoryItemFragment` 定义自己模块需要的配置就可以适配任何系统

一个装备需要至少一个配置文件，对于成百上千种装备的游戏来说，配置的工作量是不太好接受的，毕竟策划最喜欢的还是拉配置表

## 运行时添加状态

前面说明了如何配置一个装备的各种信息，那么在运行时如何利用这些信息呢？

每个人的装备都是自己管理的，所以需要在角色身上绑定相关信息，于是直接定义 `ULyraInventoryManagerComponent` 挂载在角色身上，用于存储、管理装备信息

> 无需坐标等信息，所以直接继承 `UActorComponent` 即可

由于装备是一个列表，所以需要维护一个容器，用于存储装备信息，所以定义了给一个 `FLyraInventoryList` 类作为容器，定义 `FLyraInventoryEntry` 类作为容器中的一项

那么函数的调用顺序就很明显了，对外通过 `ULyraInventoryManagerComponent` 调用接口，再通过一系列的判断之后，调用 `FLyraInventoryList` 对容器种的 `FLyraInventoryEntry` 进行增、删、改、查的操作

下面提供 `FLyraInventoryList` 和 `FLyraInventoryEntry` 的定义

```cpp
struct FLyraInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FLyraInventoryEntry()
	{}

	FString GetDebugString() const;

private:
	friend FLyraInventoryList;
	friend ULyraInventoryManagerComponent;

	UPROPERTY()
	TObjectPtr<ULyraInventoryItemInstance> Instance = nullptr;

	UPROPERTY()
	int32 StackCount = 0;

	UPROPERTY(NotReplicated)
	int32 LastObservedCount = INDEX_NONE;
};

struct FLyraInventoryList : public FFastArraySerializer
{
	GENERATED_BODY()

    // do something .....

private:
	friend ULyraInventoryManagerComponent;

private:
	// Replicated list of items
	UPROPERTY()
	TArray<FLyraInventoryEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};
```

不过通过 `FLyraInventoryEntry` 可以发现，其存储装备信息的是一个名为 `ULyraInventoryItemInstance` 的对象

`ULyraInventoryItemInstance` 的实现比较简单，存储着 `ULyraInventoryItemDefinition` 信息和 `GameplayTag` 的相关信息

```cpp
class ULyraInventoryItemInstance : public UObject
{
	GENERATED_BODY()

    // do something ...

	friend struct FLyraInventoryList;

private:
	UPROPERTY(Replicated)
	FGameplayTagStackContainer StatTags;

	// The item definition
	UPROPERTY(Replicated)
	TSubclassOf<ULyraInventoryItemDefinition> ItemDef;
};

```

`ItemDef` 存储着配置信息，用于其他对象查询配置内容，因此会对外提供 `FindFragmentByClass` 的方法，通过类名进行查询对应的 `ULyraInventoryItemFragment` 配置信息

`StatTags` 则记录着 `GameplayTag` 和对应的值信息，看上面关于配置的图片，在 `UInventoryFragment_SetStats` 中可以配置 `InitialItemStats`，这些值会在实例化的时候设置到 `StatTags` 中，给可能会要用的模块使用

正因为存在 `StatTags` 等运行时信息，所以需要专门定义 `ULyraInventoryItemInstance` 来存储运行时信息和配置信息，并对外提供方便使用的接口

![](Image/002.png)

> 类之间的基本关联关系如上图所示


