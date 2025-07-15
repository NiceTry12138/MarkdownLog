# UI

在虚幻中，实现 UI 的方法就是通过 `Slate` 弯沉

方便内存管理和使用，UMG 使用 `UObject` 来持有 `Slate` 控件

比如， UImage 持有 SImage 控件

通常来说真正实现功能的方法都在 `Slate` 控件中

`Slate` 中常见控件分为三种

- `SLeafWidget` 不包含子节点的叶子节点，比如 `STextBlock`
- `SPanel` 可以包含多个子节点的，比如 `SVerticalBox`
- `SCompoundWidget` 只能包含一个子节点，比如 `SButton`

## 渲染

![](Image/002.png)

在 `FEngineLoop::Tick` 中，触发 `FSlateApplication::Get().Tick` 函数，用于 `Slate` 渲染

`Slate` 的渲染逻辑是每帧重新渲染所有的控件，这当然会带来大量的性能浪费，因为某些控件的变化频率并没有那么高，无需每帧更新，`SInvalidationPanel` 则是当控件的内容发生变化时，只需重新渲染发生变化的部分，而不是整个面板

于是在 `PaintInvalidationRoot` 函数中出现了这么一段内容

```cpp
if (!Context.bAllowFastPathUpdate || bNeedsSlowPath || GSlateIsInInvalidationSlowPath)
	{
		GSlateIsOnFastUpdatePath = false;
		bNeedsSlowPath = false;
		CachedViewOffset = Context.ViewOffset;
		{
			if (Context.bAllowFastPathUpdate)
			{
				TGuardValue<bool> InSlowPathGuard(GSlateIsInInvalidationSlowPath, true);
				BuildFastPathWidgetList(RootWidget);
			}
			// Repopulates cached element lists
			CachedMaxLayerId = PaintSlowPath(Context);
		}
		Result.bRepaintedWidgets = true;
	}
	else if (!FastWidgetPathList->IsEmpty())
	{
		// We should not have been supplied a different root than the one we generated a path to
		check(RootWidget == FastWidgetPathList->GetRoot().Pin());
		Result.bRepaintedWidgets = PaintFastPath(Context);
	}
```

根据是否启用 `bAllowFastPathUpdate` 来决定执行 `PaintSlowPath` 全量计算，还是 `PaintFastPath` 快速计算

> 不过后续流程都是根据 `PaintSlowPath` 进行参考

在 `PaintSlowPath` 中以 `SWidget::PaintWindow` 为入口，开始深度遍历所有的 `Slate` 控件

在 `SWindow` 调用到 `SWindow::Paint`，`SWindow` 继承自 `SWidget` 且没有重写 `Paint`，最后还是调用到 `SWidget::Paint` 

### PrePass

![](Image/005.png)

在调用 `DrawWindowAndChildren` 绘制之前，会调用 `DrawPrepass`

```cpp
// Prepass the window
DrawPrepass( DrawOnlyThisWindow );
```

根据注释，可以知道这是在绘制之前的流程

1. 执行自定义流程 `CustomPrepass`
2. 遍历子节点，对其执行 `Prepass_ChildLoop`
3. 缓存设计分辨率 `CacheDesiredSize` 

```cpp
void SWidget::Prepass_Internal(float InLayoutScaleMultiplier)
{
	PrepassLayoutScaleMultiplier = InLayoutScaleMultiplier;

	bool bShouldPrepassChildren = true;
	if (bHasCustomPrepass)
	{
		bShouldPrepassChildren = CustomPrepass(InLayoutScaleMultiplier);
	}

	if (bCanHaveChildren && bShouldPrepassChildren)
	{
		FChildren* MyChildren = this->GetChildren();
		const int32 NumChildren = MyChildren->Num();
		Prepass_ChildLoop(InLayoutScaleMultiplier, MyChildren);
		ensure(NumChildren == MyChildren->Num());
	}

	{
		CacheDesiredSize(PrepassLayoutScaleMultiplier.Get(1.0f));
		bNeedsPrepass = false;
	}
}
```

要自定义设计分辨率，需要重写 `ComputeDesiredSize` 函数，这是 `SWidget` 的纯虚函数，每个子类都要重写

```cpp
virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const = 0;
```

计算每个控件占据的控件大小，是一个自下而上的计算过程

- 不含子项的控件基于其本质属性计算和缓存其所需的大小
- 组合其他控件的控件使用特殊逻辑决定其所需的大小

### ArrangeChildren

`ArrangeChildren` 函数负责计算子控件的几何信息并将其添加到 `ArrangedChildren` 列表中

`ArrangeChildren` 会在布局过程中被调用，子类可以重写 `OnArrangeChildren` 以实现自定义的子控件布局逻辑

```cpp
void SWidget::ArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren, bool bUpdateAttributes) const
{
	if (bUpdateAttributes)
	{
		FSlateAttributeMetaData::UpdateChildrenOnlyVisibilityAttributes(const_cast<SWidget&>(*this), FSlateAttributeMetaData::EInvalidationPermission::DelayInvalidation, false);
	}

	OnArrangeChildren(AllottedGeometry, ArrangedChildren);
}
```

这里 `OnArrangeChildren` 是一个纯虚函数，需要每个子类重写自己的内容

```cpp
virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const = 0;
```

以 `SCanvas::OnArrangeChildren` 为例

```cpp
void SCanvas::OnArrangeChildren( const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren ) const
{
    for (int32 ChildIndex = 0; ChildIndex < Children.Num(); ++ChildIndex)
    {
        // ... 做一些计算
        
        ArrangedChildren.AddWidget( AllottedGeometry.MakeChild(
            CurChild.GetWidget(),
            CurChild.GetPosition() + Offset,
            Size
        ));
    }
}
```

以 `SOverlay::OnPaint` 为例，展示使用 `ArrangeChildren` 的使用

```cpp
for (int32 ChildIndex = 0; ChildIndex < ArrangedChildren.Num(); ++ChildIndex)
{
    FArrangedWidget& CurArrangedWidget = ArrangedChildren[ChildIndex];

    const int32 CurWidgetsMaxLayerId =
        CurArrangedWidget.Widget->Paint(
            NewArgs,
            CurArrangedWidget.Geometry,
            MyCullingRect,
            OutDrawElements,
            MaxLayerId,
            InWidgetStyle,
            bChildrenEnabled);
}
```

将 `Widget` 的坐标偏移 和 大小 信息保存到 `ArrangedChildren` 中

### FGeometry

一个 UI 控件，需要计算自己的尺寸大小、位置坐标，这些信息都可以用 `FGeometry` 来表示

`FGeometry` 包含了控件的位置、大小、缩放等等各种信息，它在控件布局、事件处理、坐标变换、子控件传递以及支持复杂变换等方面发挥重要作用

![](Image/004.png)

成员属性比较简单

- `Size`: 表示控件在本地空间（Local Space）中的大小，即控件自身坐标系中的宽度和高度。这个字段主要用于描述控件的内部布局和尺寸
- `Scale`: 表示控件的缩放因子。这个字段用于描述控件在屏幕空间(Screen Space)中的缩放程度。这个值是累积的，包括了控件本身以及其所有父控件的缩放
- `AbsolutePosition`: 表示控件在屏幕空间中的位置，即控件相对于屏幕或应用程序窗口的左上角的坐标。这个字段主要用于描述控件在屏幕上的位置
- `Position`: 表示控件在本地空间中的位置。这个字段主要用于描述当前控件相对于其父控件的位置。
- `AccumulatedRenderTransform`: 表示从控件的本地空间到屏幕空间的累积渲染变换。这个变换包括了控件本身以及其所有父控件的渲染变换。这个字段主要用于在渲染过程中对控件应用复杂的变换，例如旋转、缩放和平移等
- `bHasRenderTransform`: 一个布尔值，表示控件是否具有渲染变换。这个字段用于在需要时快速检查控件是否具有渲染变换，以便在渲染过程中进行相应的处理

在 `Slate` 框架中，通常需要在本地空间和屏幕空间之间进行坐标转换。例如在处理控件的布局和点击事件时，需要将本地空间中的坐标转换为屏幕空间中的坐标，以便确定控件在屏幕上的实际位置

在这些计算的时候， `FGeometry` 可以发挥很大的作用

### Paint

根据几何信息（`AllottedGeometry`）和父级剪裁矩形（`MyCullingRect`）计算当前 `Widget` 的剪裁边界（`CullingBounds`）

```cpp
FSlateRect CullingBounds = CalculateCullingAndClippingRules(AllottedGeometry, MyCullingRect, bClipToBounds, bAlwaysClip, bIntersectClipBounds);
```

将 `AllottedGeometry` 计算成桌面坐标

混合父级样式和当前 `Widget` 的透明度 

```cpp
FWidgetStyle ContentWidgetStyle = FWidgetStyle(InWidgetStyle).BlendOpacity(RenderOpacity);

FGeometry DesktopSpaceGeometry = AllottedGeometry;
DesktopSpaceGeometry.AppendTransform(FSlateLayoutTransform(Args.GetWindowToDesktopTransform()));
```

根据 `Flag` 指定定时逻辑，更新控件

```cpp
if (HasAnyUpdateFlags(EWidgetUpdateFlags::NeedsActiveTimerUpdate))
{
    // ... do something
    MutableThis->ExecuteActiveTimers(Args.GetCurrentTime(), Args.GetDeltaTime());
}

if (HasAnyUpdateFlags(EWidgetUpdateFlags::NeedsTick))
{
    // ... do something
    MutableThis->Tick(DesktopSpaceGeometry, Args.GetCurrentTime(), Args.GetDeltaTime());
}
```

> 这里执行 `Tick`，大部分控件的更新逻辑都在 `Tick` 中执行

将当前 `Widget` 压入绘制栈，缓存句柄

如果当前 `Widget` 可见并且支持命中测试，将其添加到命中测试网格 `FHittestGrid` 中

```cpp
OutDrawElements.PushPaintingWidget(*this, LayerId, PersistentState.CachedElementHandle);

if (bOutgoingHittestability)
{
    Args.GetHittestGrid().AddWidget(MutableThis, 0, LayerId, FastPathProxyHandle.GetWidgetSortOrder());
}
```

调用 `OnPaint` 由子类自行实现生成实际绘制指令

```cpp
int32 NewLayerId = OnPaint(UpdatedArgs, AllottedGeometry, CullingBounds, OutDrawElements, LayerId, ContentWidgetStyle, bParentEnabled);
```

### OnPaint

`SPanel` 和 `SCompoundWidget` 因为存在子节点，需要先调用 `PaintArrangedChildren` 计算每个子节点的坐标、大小

```cpp
ArrangeChildren(AllottedGeometry, ArrangedChildren);
```

> 如果是 `SLeafWidget` 则不用，毕竟没有子节点没必要计算

`SPanel` 和 `SCompoundWidget` 再对每个子节点调用 `Paint`

```cpp
for (int32 ChildIndex = 0; ChildIndex < ArrangedChildren.Num(); ++ChildIndex)
{
    const FArrangedWidget& CurWidget = ArrangedChildren[ChildIndex];

    if (!IsChildWidgetCulled(MyCullingRect, CurWidget))
    {
        const int32 CurWidgetsMaxLayerId = CurWidget.Widget->Paint(NewArgs, CurWidget.Geometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bShouldBeEnabled);
        MaxLayerId = FMath::Max(MaxLayerId, CurWidgetsMaxLayerId);
    }
    else
    {
        //SlateGI - RemoveContent
    }
}
```

不过前面都是收集数据、遍历节点，真正绘制的内容其实是下面这样，以 `SButton::OnPaint` 为例

```cpp
FSlateDrawElement::MakeBox(
    OutDrawElements,
    LayerId,
    AllottedGeometry.ToPaintGeometry(),
    BrushResource,
    DrawEffects,
    BrushResource->GetTint(InWidgetStyle) * InWidgetStyle.GetColorAndOpacityTint() * GetBorderBackgroundColor().GetColor(InWidgetStyle)
);
```

`FSlateDrawElement` 是 `Slate` 渲染系统的原子绘制单元，封装了 `GPU` 绘制所需的所有元数据

`OutDrawElements` 是由 `SWindow` 创建，用于收集绘制信息。通过遍历所有节点时，由每个节点各自添加各自的绘制指令到 `OutDrawElements` 中

![](Image/006.png)

## 事件触发

https://zhuanlan.zhihu.com/p/448050955

以点击事件为例

![](Image/001.jpg)

在 `FSlateApplication::RoutePointerDownEvent` 函数中，传入参数有两个

- `WidgetsUnderPointer` 用于表示特定的 `Widget` 路径，它包含了从最上层的 `SWindow` 到特定 `Widget` 的层次结构、几何信息和虚拟光标位置
- `PointerEvent` 点输入事件类， 用于处理鼠标以及移动设备的触摸按键



## ListView

![](Image/003.png)
