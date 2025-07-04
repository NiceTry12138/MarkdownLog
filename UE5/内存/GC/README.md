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


