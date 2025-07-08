# GC

## 创建一个新的 UObject

在 `UObject` 的基类是 `UObjectBaseUtility`， `UObjectBaseUtility` 的基类是 `UObjectBase`

`UObjectBase` 的构造函数中就做了一个事情 `AddObject`，最终会将该对象添加到 `GUObjectArray`

```cpp
void UObjectBase::AddObject(FName InName, EInternalObjectFlags InSetInternalFlags, int32 InInternalIndex, int32 InSerialNumber)
{
	NamePrivate = InName;
	EInternalObjectFlags InternalFlagsToSet = InSetInternalFlags;
    
    // 设置 Tag ... do something ...

	GUObjectArray.AllocateUObjectIndex(this, InternalFlagsToSet, InInternalIndex, InSerialNumber);
	check(InName != NAME_None && InternalIndex >= 0);
	HashObject(this);
	check(IsValidLowLevel());
}
```

`GUObjectArray` 是一个全局唯一的 `FUObjectArray`，也就是说所有创建的 `UObject` 对象都会保存在这个数组里面

![](Image/001.png)

在 `StaticAllocateObject` 全局函数中

首先通过全局的 `GUObjectAllocator` 分配内存

```cpp
int32 Alignment	= FMath::Max( 4, InClass->GetMinAlignment() );
Obj = (UObject *)GUObjectAllocator.AllocateUObject(TotalSize,Alignment,GIsInitialLoad);
```

然后通过 `placement new` 对内存区域进行构造函数

```cpp
if (!bSubObject)
{
	FMemory::Memzero((void *)Obj, TotalSize);
	new ((void *)Obj) UObjectBase(const_cast<UClass*>(InClass), InFlags|RF_NeedInitialization, InternalSetFlags, InOuter, InName, OldIndex, OldSerialNumber);
}
```

在构造函数中，将该对象设置到 `GUObjectArray` 中

## GC

明牌 UE 的 GC 机制是 **标记-清除** 算法

1. 加锁，保持所有对象的引用关系不变
2. 标记所有的 `UObject` 为 **不可达**
3. 遍历根对象列表 `GUObjectArray`，跟对象引用到的对象去除 **不可达** 标记
4. 收集所有仍然标记为 **不可达** 的对象，全部删除

```cpp
void CollectGarbage(EObjectFlags KeepFlags, bool bPerformFullPurge)
{
	if (GIsInitialLoad)
	{
		UE_LOG(LogGarbage, Log, TEXT("Skipping CollectGarbage() call during initial load. It's not safe."));
		return;
	}
	AcquireGCLock();
	UE::GC::CollectGarbageInternal(KeepFlags, bPerformFullPurge);
}

FORCEINLINE void CollectGarbageInternal(EObjectFlags KeepFlags, bool bPerformFullPurge)
{
	const double StartTime = FPlatformTime::Seconds();

	if (bPerformFullPurge)
	{
		CollectGarbageFull(KeepFlags);
	}
	else
	{
		CollectGarbageIncremental(KeepFlags);
	}

	GTimingInfo.LastGCDuration = FPlatformTime::Seconds() - StartTime;

	CSV_CUSTOM_STAT(GC, Count, 1, ECsvCustomStatOp::Accumulate);
}
```

### AcquireGCLock

通过 `AcquireGCLock` 加锁，暂停其他线程对 UOBject 对象的引用关系产生影响

```cpp
void AcquireGCLock()
{
	// 信息收集
	FGCCSyncObject::Get().GCLock();
	// 输出 GC 所用时间
}
```

本质就是在执行 `FGCCSyncObject` 的 `GCLock` 函数

首先将 `GCWantsToRunCounter` 值加 1，表示想要进行 GC 操作

> 在 `IsGarbageCollectionWaiting` 全局函数中返回 `GCWantsToRunCounter` 是否不为 0。该函数在 `AsyncLoading` 中被使用

随后就是让当前线程 `Sleep`，直到 `AsyncCounter` 值归零

`AsyncCounter` 值用于表示其他线程是否有阻塞GC的操作还在执行，当 `AsyncCounter` 归零表示可以执行 GC，此时顺便清空 `GCWantsToRunCounter` 的值

让 `GCCounter` 自增，表示当前正在执行 GC 操作，会在 `GCUnlock()` 函数中自减

使用 `IsGCLocked()` 可以通关判断 `GCCounter` 的值是否不为零来判断是否正在执行 GC 操作

```cpp
void GCLock()
{
	// GCWantsToRunCounter 值加 1
	SetGCIsWaiting();

	bool bLocked = false;
	do
	{
		// Sleep 线程，直到 AsyncCounter 值归零
		FPlatformProcess::ConditionalSleep([&]()
		{
			return AsyncCounter.GetValue() == 0;
		});
		{
			FScopeLock CriticalLock(&Critical);
			if (AsyncCounter.GetValue() == 0)
			{
				GCUnlockedEvent->Reset();
				int32 GCCounterValue = GCCounter.Increment();
				FPlatformMisc::MemoryBarrier();
				ResetGCIsWaiting();		// 重置 GCWantsToRunCounter 值为 0
				bLocked = true;
			}
		}
	} while (!bLocked);
}
```

当上述流程执行完毕之后，就可以开始 GC 操作了

### CollectGarbageInternal

```cpp
FORCEINLINE void CollectGarbageInternal(EObjectFlags KeepFlags, bool bPerformFullPurge)
{
	const double StartTime = FPlatformTime::Seconds();

	if (bPerformFullPurge)
	{
		CollectGarbageFull(KeepFlags);
	}
	else
	{
		CollectGarbageIncremental(KeepFlags);
	}

	GTimingInfo.LastGCDuration = FPlatformTime::Seconds() - StartTime;

	CSV_CUSTOM_STAT(GC, Count, 1, ECsvCustomStatOp::Accumulate);
}
```

根据 `bPerformFullPurge` 判断是否需要全量垃圾回收，全量回收执行 `CollectGarbageFull`，增量回收执行 `CollectGarbageIncremental`

```cpp
FORCENOINLINE static void CollectGarbageFull(EObjectFlags KeepFlags)
{
	// 信息收集
	CollectGarbageImpl<true>(KeepFlags);
}

FORCENOINLINE static void CollectGarbageIncremental(EObjectFlags KeepFlags)
{
	// 信息收集
	CollectGarbageImpl<false>(KeepFlags);
}

template<bool bPerformFullPurge>
void CollectGarbageImpl(EObjectFlags KeepFlags)
{ // ...
}
```

通过 `template` 的方式，生成两个 `CollectGarbageImpl` 模板函数，减少判断 if-else

### CollectGarbageImpl

#### 可达性分析

标记所有可达对象，使用根集递归遍历

```cpp
FRealtimeGC GC;
{
	// 信息收集
	GC.PerformReachabilityAnalysis(KeepFlags, Options);
	// 日志打印
}

// 二次分析 一般用于调试 检测非法引用（仅调试模式启用）
if (GC.Stats.bFoundGarbageRef && GGarbageReferenceTrackingEnabled > 0)
{
	// 信息收集
	GC.PerformReachabilityAnalysis(KeepFlags, Options);
	// 日志打印
}
```

`FRealtimeGC::PerformReachabilityAnalysis` 函数分为两个阶段，一个是标记，一个可达性分析

##### 标记

在开始之前，先把 `GGCObjectReferencer` 内容添加到 `InitialObjects` 数组中

> `GGCObjectReferencer` 存储的就是继承自 `FGCObject` 的非 `UObject` 对象

```cpp
if (FPlatformProperties::RequiresCookedData() && GUObjectArray.IsDisregardForGC(FGCObject::GGCObjectReferencer))
{
	InitialObjects.Add(FGCObject::GGCObjectReferencer);
}
```

```cpp
const EGCOptions OptionsForMarkPhase = Options & ~EGCOptions::WithPendingKill;
(this->*MarkObjectsFunctions[GetGCFunctionIndex(OptionsForMarkPhase)])(KeepFlags);
```

上述是标记代码，使用 `MarkObjectsFunctions` 函数指针，该函数指针初始在构造函数中

```cpp
typedef void(FRealtimeGC::*MarkObjectsFn)(EObjectFlags);
MarkObjectsFn MarkObjectsFunctions[4];

FRealtimeGC()
{
	MarkObjectsFunctions[GetGCFunctionIndex(EGCOptions::None)] = &FRealtimeGC::MarkObjectsAsUnreachable<false>;
	MarkObjectsFunctions[GetGCFunctionIndex(EGCOptions::Parallel | EGCOptions::None)] = &FRealtimeGC::MarkObjectsAsUnreachable<true>;

	// do something else ...
}
```

所以，本质上是执行 `FRealtimeGC::MarkObjectsAsUnreachable` 函数，只是根据是否多线程来选择数组中的执行函数罢了

```cpp
TArray<TArray<UObject*>, TInlineAllocator<32>> ObjectsToSerializeArrays;
ObjectsToSerializeArrays.SetNum(NumThreads);

ParallelFor( TEXT("GC.MarkUnreachable"),NumThreads,1,
	[&ObjectsToSerializeArrays, &ClustersToDissolveList, &KeepClusterRefsList,
		KeepFlags, NumberOfObjectsPerThread, NumThreads, MaxNumberOfObjects, bIsRerun = Stats.bFoundGarbageRef] (int32 ThreadIndex)
		{}, !bParallel ? EParallelForFlags::ForceSingleThread : EParallelForFlags::None);
```

- `NumThreads` 是可以运行的线程数量
- `ObjectsToSerializeArrays` 用于存储各个线程计算出来的 `UObject*`

通过 `ParallelFor` 启动多个线程，通过 `ThreadIndex` 来分块计算 `GUObjectArray` 中的 `UObject*`

```cpp
int32 FirstObjectIndex = ThreadIndex * NumberOfObjectsPerThread + GUObjectArray.GetFirstGCIndex();
int32 NumObjects = (ThreadIndex < (NumThreads - 1)) ? NumberOfObjectsPerThread : (MaxNumberOfObjects - (NumThreads - 1) * NumberOfObjectsPerThread);
int32 LastObjectIndex = FMath::Min(GUObjectArray.GetObjectArrayNum() - 1, FirstObjectIndex + NumObjects - 1);

for (int32 ObjectIndex = FirstObjectIndex; ObjectIndex <= LastObjectIndex; ++ObjectIndex)
{}
```

那么对一个 `UObject` 进行进行标记的真正部分就在循环体中，通过 `ObjectIndex` 可以直接从 `GUObjectArray` 获取对应的 `UObject`

```cpp
FUObjectItem* ObjectItem = &GUObjectArray.GetObjectItemArrayUnsafe()[ObjectIndex];
UObject* Object = (UObject*)ObjectItem->Object;
```

得到 `UObject` 第一步就是清除可达性标记

```cpp
ObjectItem->ClearFlags(EInternalObjectFlags::ReachableInCluster);
```

- 如果 `UObject` 是一个根对象，直接加入到 `LocalObjectsToSerialize`，如果它属于 **集群** 或者 **集群根** ，也加入到 `KeepClusterRefsList` 
- 如果 `UObject` 是一个集群对象，并且有 `FastKeepFlags` 也加入到 `LocalObjectsToSerialize` 和 `KeepClusterRefsList`

> `FastKeepFlags = EInternalObjectFlags::GarbageCollectionKeepFlags`

如果 `UObject` 是 **普通对象** 或者 **集群根** 

```cpp
if (ObjectItem->HasAnyFlags(FastKeepFlags))
{
	bMarkAsUnreachable = false;
}
else if (!ObjectItem->IsPendingKill() && KeepFlags != RF_NoFlags && Object->HasAnyFlags(KeepFlags))
{
	bMarkAsUnreachable = false;
}
```

符合上述条件，就不会被标记为 **不可达**

如果是 `ClusterRoot` 也就是 **集群根** 对象，它是 `PendingKill` 或者 `Garbage`，那么这个集群应该被解散，会被加入到 `ClustersToDissolveList` 数组

```cpp
PRAGMA_DISABLE_DEPRECATION_WARNINGS
else if (ObjectItem->HasAnyFlags(EInternalObjectFlags::PendingKill | EInternalObjectFlags::Garbage) && ObjectItem->HasAnyFlags(EInternalObjectFlags::ClusterRoot))
PRAGMA_ENABLE_DEPRECATION_WARNINGS
{
	ClustersToDissolveList.Push(ObjectItem);
}
```

不可达的对象会被加入到打上不可达的标记，其他会加入到 `LocalObjectsToSerialize` 和 `KeepClusterRefsList` 数组中

```cpp
if (!bMarkAsUnreachable)
{
	LocalObjectsToSerialize.Add(Object);
	if (ObjectItem->HasAnyFlags(EInternalObjectFlags::ClusterRoot))
		KeepClusterRefsList.Push(ObjectItem);
}
else
{
	ObjectItem->SetFlags(EInternalObjectFlags::Unreachable);
}
```

前情提要，`LocalObjectsToSerialize` 是 `ObjectsToSerializeArrays` 通过 `ThreadIndex` 得到的

每个线程将自己计算的内容存储到对应的数组中，通过引用的方式存储到 `ObjectsToSerializeArrays` 中

```cpp
TArray<UObject*>& LocalObjectsToSerialize = ObjectsToSerializeArrays[ThreadIndex];
```

由于 `ObjectsToSerializeArrays` 是一个 二维数组 `TArray<TArray<UObject*>, TInlineAllocator<32>>`，需要将其转换成 一维数组 然后进行后续计算，这个 一维数组 就是 `InitialObjects`

```cpp
InitialObjects.Reserve(InitialObjects.Num() + NumTotal + UE::GC::ObjectLookahead);
for (TArray<UObject*>& Objects : ObjectsToSerializeArrays)
{
	InitialObjects.Append(Objects);
}

ObjectsToSerializeArrays.Empty();
```

将所有的 `ClustersToDissolveList` 中所有的对象，也就是不可达的 **集群根** 对象，将该集群中所有的对象标记为不可达

```cpp
TArray<FUObjectItem*> ClustersToDissolve;
ClustersToDissolveList.PopAll(ClustersToDissolve);
for (FUObjectItem* ObjectItem : ClustersToDissolve)
{
	if (ObjectItem->HasAnyFlags(EInternalObjectFlags::ClusterRoot))
	{
		GUObjectClusters.DissolveClusterAndMarkObjectsAsUnreachable(ObjectItem);
		GUObjectClusters.SetClustersNeedDissolving();
	}
}
```

针对 `KeepClusterRefsList` 列表，该列表存储集群对象

遍历 `KeepClusterRefsList` 列表对象，如果对象可达，但是对象对应集群的根对象是不可达的，需要 **复活** 整个集群

顺便通过 `MarkReferencedClustersAsReachable` 通过遍历，将所有与该**集群**相关的**集群**都保留下来，存储到 `InitialObjects` 中

##### 可达性分析



```cpp
FContextPoolScope Pool;
FWorkerContext* Context = Pool.AllocateFromPool();
Context->InitialNativeReferences = GetInitialReferences(Options);
Context->SetInitialObjectsUnpadded(InitialObjects);

// 分析 InitialObjects 数组内的对象,它能达到的对象,去掉不可达标签
PerformReachabilityAnalysisOnObjects(Context, Options);
```

```cpp
virtual void PerformReachabilityAnalysisOnObjects(FWorkerContext* Context, EGCOptions Options) override
{
	(this->*ReachabilityAnalysisFunctions[GetGCFunctionIndex(Options)])(*Context);
}

typedef void(FRealtimeGC::*ReachabilityAnalysisFn)(FWorkerContext&);
ReachabilityAnalysisFn ReachabilityAnalysisFunctions[8];

ReachabilityAnalysisFunctions[GetGCFunctionIndex(EGCOptions::None)] = &FRealtimeGC::PerformReachabilityAnalysisOnObjectsInternal<EGCOptions::None | EGCOptions::None>;
ReachabilityAnalysisFunctions[GetGCFunctionIndex(EGCOptions::Parallel | EGCOptions::None)] = &FRealtimeGC::PerformReachabilityAnalysisOnObjectsInternal<EGCOptions::Parallel | EGCOptions::None>;

ReachabilityAnalysisFunctions[GetGCFunctionIndex(EGCOptions::None | EGCOptions::WithPendingKill)] = &FRealtimeGC::PerformReachabilityAnalysisOnObjectsInternal<EGCOptions::None | EGCOptions::WithPendingKill>;
ReachabilityAnalysisFunctions[GetGCFunctionIndex(EGCOptions::Parallel | EGCOptions::WithPendingKill)] = &FRealtimeGC::PerformReachabilityAnalysisOnObjectsInternal<EGCOptions::Parallel | EGCOptions::WithPendingKill>;
```

是的， `ReachabilityAnalysisFunctions` 又是函数指针，根据不同的 `EGCOptions` 选择不同的模板函数

无论如何，最后调用的是 `TFastReferenceCollector::ProcessObjectArray`


