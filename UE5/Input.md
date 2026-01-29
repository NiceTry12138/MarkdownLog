# 输入系统

## 收集事件

```
FWindowsApplication::RegisterClassW(HINSTANCE__ * const HInstance, HICON__ * const HIcon) 
FWindowsApplication::FWindowsApplication(HINSTANCE__ * const HInstance, HICON__ * const IconHandle) 
FWindowsApplication::CreateWindowsApplication(HINSTANCE__ * const InstanceHandle, HICON__ * const) 
FWindowsPlatformApplicationMisc::CreateApplication() 
FSlateApplication::Create() 
FEngineLoop::PreInitPreStartupScreen(const wchar_t * CmdLine) 
FEngineLoop::PreInit(const wchar_t *) 
EnginePreInit(const wchar_t *) 
```

从调用堆栈可以看到，根据 WinMain 的使用方法，想要监听窗口事件需要设置 WNDCLASS 或者 WNDCLASSEX 的 lpfnWndProc 属性

```cpp
WNDCLASS wc;
FMemory::Memzero( &wc, sizeof(wc) );
wc.style = CS_DBLCLKS; // We want to receive double clicks
wc.lpfnWndProc = AppWndProc;
wc.cbClsExtra = 0;
wc.cbWndExtra = 0;
wc.hInstance = HInstance;
wc.hIcon = HIcon;
wc.hCursor = NULL;	// We manage the cursor ourselves
wc.hbrBackground = NULL; // Transparent
wc.lpszMenuName = NULL;
wc.lpszClassName = FWindowsWindow::AppWindowClass;
```

这里设置的就是 `AppWndProc` 对应的就是 `FWindowsApplication::AppWndProc` 函数，最终会触发到 `FWindowsApplication::ProcessMessage` 函数中

```cpp
int32 FWindowsApplication::ProcessMessage( HWND hwnd, uint32 msg, WPARAM wParam, LPARAM lParam )
```

- hwnd 表示窗口句柄
- msg 表示消息类型
  - 键盘: WM_KEYDOWN / WM_KEYUP / WM_CHAR / WM_SYSKEYDOWN ...
  - 鼠标: WM_MOUSEMOVE / WM_LBUTTONDOWN / WM_LBUTTONDBLCLK / WM_MOUSEWHEEL ...
  - 触摸: WM_TOUCH
  - 窗口: WM_SIZE / WM_SETFOCUS / WM_KILLFOCUS 等
- wParam，`word parameter` 一个附加信息表，大小等同指针，含义完全取决于 msg
  - `WM_KEYDOWN / WM_KEYUP`: 那么 `wParam` 是虚拟键码 (`VK_*`)，例如 `VK_CONTROL`、`A` 等
  - `WM_MOUSEWHEEL`: 那么 `wParm` 高位是滚轮 delta，低位是滚轮修饰器/鼠标键状态(`MK_SHIFT`/`MK_CONTROL`/`MK_LBUTTON`...)
  - `WM_XBUTTONDOWN/UP`: 高位指示是 `XBUTTON1` 还是 `XBUTTON2`
  - `WM_CHAR`: `wPrarm` 是输入字符码（UTF-16 code unit）
- `lPraram`，`long parameter` 另一个附加信息参数，大小等同指针，具体作用同样取决于 msg
  - 鼠标位置: 很多鼠标信息把 x/y 打包在 lParam 里
  - `WMK_KEYDOWN/UP`: lParam 的各 bit 表述重复次数、扫描码、扩展键标志、之前是否按下、是否为释放等
  - `WM_SIZE`: 低位/高位 分表表示新宽高
  - `WM_MOUSEMOVE`: 同样携带 x/y


在 `ProcessMessage` 函数中有一个巨大的 `switch` 用于根据 `msg` 的值，对数据直接处理处理，或者转发给 `DeferMessage`延迟处理，或者交给 `DefWindowProc` 处理

`DefWindowProc` 是让窗口做默认行为，`DefWindowProc` 是 Win32 的默认窗口过程实现，通常在 `WinProc` 里不处理某些消息的时候，就会把它交给 `DefWindowProc` 让系统完成默认行为

> 拖拽移动、系统菜单、系统及快捷键 等

`DeferMessage` 则是 UE 的行为，把消息 **延迟到稍后统一处理**

UE 把当前收到的 Windows 消息（连同鼠标位置、`RawInputFlags`）塞进一个队列（`DeferredMessages`），然后在后续的某个阶段再批量处理

1. 避免在 WndProc 回调里做复杂逻辑，WndProc 可能在非常敏感的时机被调用，重入/时序问题多
2. 保证输入处理发生在 UE 期望的线程/时机
3. 可以合并/过滤/重排（例如鼠标移动消息非常频繁，UE可以做一些策略；方便统一处理捕获、双击、IME等状态机）
4. 与 Slate/Viewport 的路由时序对其（先更新窗口/焦点/捕获状态，再分发输入）

> `RawInputFlags` 看起来是 `MOUSE_MOVE_RELATIVE` 和 `MOUSE_MOVE_ABSOLUTE` 二选一，表示鼠标传入的鼠标坐标是相对移动还是绝对移动，默认值是 `MOUSE_MOVE_RELATIVE`

## 处理事件

在 `FWindowsApplication::ProcessDeferredEvents` 函数中 `DeferredMessages` 队列进行处理

```cpp
TArray<FDeferredWindowsMessage> EventsToProcess( DeferredMessages );

DeferredMessages.Empty();
for( int32 MessageIndex = 0; MessageIndex < EventsToProcess.Num(); ++MessageIndex )
{
    const FDeferredWindowsMessage& DeferredMessage = EventsToProcess[MessageIndex];
    ProcessDeferredMessage( DeferredMessage );
}

CheckForShiftUpEvents(VK_LSHIFT);
CheckForShiftUpEvents(VK_RSHIFT);
```

### 对 IME 的处理

`IME` 是什么？

`IME` = `Input Method Editr` 输入法编辑器，在 Windows 上它是一个系统级组件，用来把 **键盘案件序列** 转换成 **最终文本**

> 典型场景就是 中文输入法

不同于英文直接产生字符，IME 往往有一个 **组合输入** (Composition) 的过程

1. 按下 n i h a o，并不会立刻产生最终文本 **你好**
2. 系统会产生一段 正在编辑的组合串
3. 你选择候选词、移动光标、确认后，才最终把字符提交给应用

这个过程还设计

1. 候选窗位置（跟随光标）
2. 组合串高亮/分段
3. 输入法状态切换（中/英、全/半角）
4. 语言/键盘布局切换(`WM_INPUTLANGCHANGE`)

```cpp
case WM_INPUTLANGCHANGEREQUEST:
case WM_INPUTLANGCHANGE:
case WM_IME_SETCONTEXT:
case WM_IME_NOTIFY:
case WM_IME_REQUEST:
case WM_IME_STARTCOMPOSITION:
case WM_IME_COMPOSITION:
case WM_IME_ENDCOMPOSITION:
case WM_IME_CHAR:
    {
        if(TextInputMethodSystem.IsValid())
        {
            TextInputMethodSystem->ProcessMessage(hwnd, msg, wParam, lParam);
        }
        if (msg == WM_INPUTLANGCHANGE)
        {
            MessageHandler->OnInputLanguageChanged();
        }
        return 0;
    }
    break;
```

如果 msg 是 IME 类型的信息，会统一交给 `TextInputMeshodSystem` 处理，这里对应 Slate 的文本输入法上下文（组合输入、候选床、CompositionString 等）

而 `WM_INPUTLANGCHANGE` 表示语言切换通知，让 Slate/编辑器 UI 更新输入语言状态

> 编辑器模式下这里 `MessageHandler` 类型是 `FSlateApplication`，在 Win11 下按 win + 空格 即可切换输入法
>
> 编辑器模式下这里 `TextInputMethodSystem` 类型是 `FWindowsTextInputMethodSystem`


综上，对 IME 的处理顺序大概就是

1. Win32 发送 IME 信息
2. FWindowsApplication 只负责接收并把 IME 信息转发给 文本输入发系统接口
3. Slate 负责把组合串画出来、维护光标/选取、告诉 IME 候选窗位置、最终把 commit 的字符串写入控件

从 `FWindowsTextInputMethodSystem` 的源码中可以看到其实现了两套 API：IMM 和 TSF

- IMM，Input Method Manager，传统 Win32 IME API
- TSF，Text Services Framework，更现代的文本服务框架

```cpp
enum class EAPI
{
    Unknown,
    IMM,
    TSF
} CurrentAPI;
```

`FWindowsTextInputMethodSystem` 还维护了 `ActiveContext` 和 `ContextToInternalContextMap`，用于把 **引擎的文本输入上下文** 映射到 **Windows 侧需要的内部状态**（窗口句柄、TSF Context、IMM Composition 状态等）

`FWindowsTextInputMethodSystem` 只依赖 `ITextInputMethodContext`，Slate 的可输入文本控件会实现/提供一个 `ITextInputMethodContext`，当 Slate 某个文本框获得焦点的时候，会调用 `TextInputMethodSystem::ActivateContext`

参考 `FSlateEditableTextLayout::EnableTextInputMethodContext` 函数

```cpp
void FSlateEditableTextLayout::EnableTextInputMethodContext()
{
	ITextInputMethodSystem* const TextInputMethodSystem = FSlateApplication::Get().GetTextInputMethodSystem();
	if (TextInputMethodSystem)
	{
        // 做很多迁前置处理，最后将 TextInputMethodContext 注册给 TextInputMethodSystem
		TextInputMethodSystem->ActivateContext(TextInputMethodContext.ToSharedRef());
	}
}
```

### 对 Char 的处理

当 `msg` 类型是 `WM_CHAR` 时，表示本次输入的是一个字符

```cpp
const TCHAR Character = IntCastChecked<TCHAR>(wParam);
const bool bIsRepeat = ( lParam & 0x40000000 ) != 0;
MessageHandler->OnKeyChar( Character, bIsRepeat );
```

### 对 WM_KEYUP/WM_SYSKEYUP/WM_KEYDOWN/WM_SYSKEYDOWN 对键盘按键的处理

> 假设，当前按下的是 Ctrl + A

根据 wParam 和 lParam 得到具体的按键信息

比如 wParam 只能得到 `VK_MENU` (alt)、`VK_CONTROL` (ctrl)、`VK_SHIFT` (shift)、`VK_CAPITAL` (大小写切换)

通过 lParam 可以确定是 VK_LMENU、VK_RMENU、VK_LCONTROL、VK_RCONTROL、VK_LSHIFT、VK_RSHIFT、VK_CAPITAL

同时维护 `ModifierKeyState`，并用它修正 repeat 语义，如果按住左 ctrl 不放，系统可能不断发 Keydown Repeat

这里为什么要修正 repeat ?

`Windows` 对 `WM_KEYDOWN` 的 `lParam` 的第 30 位表示是否 `repeat`

- `lParam & 0x40000000`
  - 0 之前是 up
  - 1 之前已经 down

但是 Windows 的消息是以 **总键** 的情况上报，也就是不区分 `LCtrl` 和 `RCtrl`，所以需要自己维护一下按键信息

另外 Windows 在某些情况下不会给你 干净的 up/down 序列

- Alt 相关系统键：会被系统菜单、窗口激活等机制干扰，消息序列可能不完全符合 down-repeat-up 
- 焦点切换/窗口失活：按住 ctrl 切换到别的窗口再切回来， Windows 可能导致状态不同步，UE需要用自己的状态来保证一致性
- 左右修饰键同时按下

当然这里不止处理了 alt、ctrl、shift 键，普通的按键也会处理

### 鼠标按键、双击、抬起

```cpp
case WM_LBUTTONDBLCLK:
case WM_LBUTTONDOWN:
case WM_MBUTTONDBLCLK:
case WM_MBUTTONDOWN:
case WM_RBUTTONDBLCLK:
case WM_RBUTTONDOWN:
case WM_XBUTTONDBLCLK:
case WM_XBUTTONDOWN:
case WM_LBUTTONUP:
case WM_MBUTTONUP:
case WM_RBUTTONUP:
case WM_XBUTTONUP:
```

这里从 lParam 解码坐标，再转屏幕坐标，最后触发 `OnMouseDown/Up/DoubleClick`

通过 msg 解析出

- `MouseButton` 鼠标的 左、中、右、辅助键1，辅助键2
- `bMouseUp` 鼠标抬起、按下
- `bDoubleClick` 是否是双击

从 `lParam` 中获取鼠标坐标，然后转换成屏幕坐标

```cpp
POINT CursorPoint;
CursorPoint.x = GET_X_LPARAM(lParam);
CursorPoint.y = GET_Y_LPARAM(lParam); 

ClientToScreen(hwnd, &CursorPoint);

const FVector2D CursorPos(CursorPoint.x, CursorPoint.y);
```

最后根据 `bMouseUp` 和 `bDoubleClick` 分发不同的事件

```cpp
if (bMouseUp)
{
    return MessageHandler->OnMouseUp( MouseButton, CursorPos ) ? 0 : 1;
}
else if (bDoubleClick)
{
    MessageHandler->OnMouseDoubleClick( CurrentNativeEventWindowPtr, MouseButton, CursorPos );
}
else
{
    MessageHandler->OnMouseDown( CurrentNativeEventWindowPtr, MouseButton, CursorPos );
}
```

### WM_INPUT

用于处理高精度鼠标/相对位移

```cpp
if( DeferredMessage.RawInputFlags == MOUSE_MOVE_RELATIVE )
{
    MessageHandler->OnRawMouseMove(DeferredMessage.X, DeferredMessage.Y);
}
else
{
    // Absolute coordinates given through raw input are simulated using MouseMove to get relative coordinates
    MessageHandler->OnMouseMove();
}
```

- `RawInput` 可能给相对位移（FPS视角常用），UE 直接走 `OnRawMouseMove(dx, dy)`
- 如果是绝对最表 RawInput，UE 反而用 `OnMouseMove` 走常规路径来模拟相对

> 这就是在游戏里开启 高精度鼠标模式 时，视角旋转更稳定的底层来源

### WM_NCMOUSEMOVE, WM_MOUSEMOVE

普通鼠标移动，走 `OnMouseMove`

> 返回值 0，1 用于告诉上层是否处理

### WM_MOUSEWHEEL

处理鼠标滚轮事件

```cpp
const SHORT WheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
MessageHandler->OnMouseWheel(WheelDelta * (1/120.f), CursorPos);
```

`Windwos` 一个滚轮通常是 120，UE 归一化滚轮步长浮点值，交给 `Slate`/`Viewport` 做滚动/缩放等

## 存储按键事件

当你在 `UPlayerInput::InputKey` 函数打个断点，就可以看到整个从 WinMain 到 事件触发的流程了

```
UPlayerInput::InputKey
UEnhancedPlayerInput::InputKey
APlayerController::InputKey
UGameViewportClient::InputKey
FSceneViewport::OnMouseButtonDown
SViewport::OnMouseButtonDown
FSlateApplication::RoutePointerDownEvent::__l5::<lambda_2>::operator
FSlateApplication::RoutePointerDownEvent
FSlateApplication::ProcessMouseButtonDownEvent
FSlateApplication::OnMouseDown
FWindowsApplication::ProcessDeferredMessage
FWindowsApplication::DeferMessage
FWindowsApplication::ProcessMessage
UPlayerInput::InputKey
UEnhancedPlayerInput::InputKey
APlayerController::InputKey
UGameViewportClient::InputKey
FSceneViewport::OnMouseButtonDown
SViewport::OnMouseButtonDown
FSlateApplication::RoutePointerDownEvent::__l5::<lambda_2>::operator
FSlateApplication::RoutePointerDownEvent
FSlateApplication::ProcessMouseButtonDownEvent
FSlateApplication::OnMouseDown
FWindowsApplication::ProcessDeferredMessage
FWindowsApplication::DeferMessage
FWindowsApplication::ProcessMessage
```

鼠标事件从 `ProcessDeferredMessage` 被解析，调用给 `FSlateApplication`，传递给 `SViewport`，最后给到 `APlayerController`

### 从 Windows 到 Slate

- 在 ` FSlateApplication::ProcessMouseButtonDownEvent` 首先通过 预处理器 进行处理

这类东西包括：编辑器工具、调试输入、某些全局拦截器等。如果返回 `true` 表示 `Slate` 已经处理，不会再走 `widget` 路由，更不会下发到游戏

```cpp
if (InputPreProcessors.HandleMouseButtonDownEvent(*this, MouseEvent))
{
    return true;
}
```

- 捕获(Capture) 优先：有捕获就不会命中测试

如果某个 widget 捕获了鼠标（拖拽、按住滑动条等），事件会强制路由到捕获者路径，而不是鼠标下方的控件

```cpp
if (SlateUser->HasCapture(MouseEvent.GetPointerIndex()))
{
    FWidgetPath MouseCaptorPath = SlateUser->GetCaptorPath(...);
    Reply = FEventRouter::Route<...>(..., MouseCaptorPath, ... OnPreviewMouseButtonDown ...);

    if (!Reply.IsEventHandled())
    {
        Reply = FEventRouter::Route<...>(..., MouseCaptorPath, ... OnMouseButtonDown/OnTouchStarted ...);
    }
}
```

- 没捕获：命中测试（HitTest）得到 FWidgetPath，再路由

通过 `LocateWindowUnderMouse` 进行 `HitTest`，构建从 `SWindow` 根到叶子控件的路径 `FWidgetPath`

```cpp
FWidgetPath WidgetsUnderCursor = LocateWindowUnderMouse( MouseEvent.GetScreenSpacePosition(), GetInteractiveTopLevelWindows(), false, SlateUser->GetUserIndex());
PopupSupport.SendNotifications( WidgetsUnderCursor );
FScopedSwitchWorldHack SwitchWorld(WidgetsUnderCursor);
bInGame = FApp::IsGame();
Reply = RoutePointerDownEvent(WidgetsUnderCursor, MouseEvent);
```

- Slate 的路由实现 `RoutePointerDownEvent`，由 Tunnel + Bubble 两个阶段

- Slate 路由的 Tunnel 阶段，从跟节点到叶子节点

主要用于 预览/拦截，典型是 父控件想要先拦截（比如 ScrollBox、Overlay等）

```cpp
FReply Reply = FEventRouter::Route<FReply>( this, FEventRouter::FTunnelPolicy( WidgetsUnderPointer ), TransformedPointerEvent, []( const FArrangedWidget TargetWidget, const FPointerEvent& Event )
{
    const FReply TempReply = TargetWidget.Widget->OnPreviewMouseButtonDown(TargetWidget.Geometry, Event);
#if WITH_SLATE_DEBUGGING
    FSlateDebugging::BroadcastInputEvent(ESlateDebuggingInputEvent::PreviewMouseButtonDown, &Event, TempReply, TargetWidget.Widget);
#endif
    return TempReply;
}, ESlateDebuggingInputEvent::PreviewMouseButtonDown);
```

> 注意这里是 `FEventRouter::FTunnelPolicy`

- Slate 路由的 Bubble 从叶子节点冒泡的根

典型是叶子控件（SButton、SViewport） 先处理；如果它不处理，父容器可能处理

```cpp
if (!Reply.IsEventHandled())
{
    Reply = FEventRouter::Route<FReply>(
        this,
        FEventRouter::FBubblePolicy(WidgetsUnderPointer),
        TransformedPointerEvent,
        [this](FArrangedWidget TargetWidget, const FPointerEvent& Event)
        {
            FReply TempReply = FReply::Unhandled();
            if (Event.IsTouchEvent()) TempReply = TargetWidget.Widget->OnTouchStarted(...);
            if (!Event.IsTouchEvent() || (!TempReply.IsEventHandled() && bTouchFallbackToMouse))
            {
                TempReply = TargetWidget.Widget->OnMouseButtonDown(...);
            }
            return TempReply;
        },
        ...
    );
}
```

所以每个函数都要返回 `FReply::Handled()` 或者 `FReply::Unhandled()` 用来判断是否处理了

### 从 Slate 到 UPlayerInput

因为承载游戏渲染画面的控件时 SViewport，当 Slate Widget 树中没有人消费的时候，最后的事件会触发给 `SViewport`

所以触发了 `SViewport::OnMouseButtonDown` 

```cpp
FReply SViewport::OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent )
{
	return ViewportInterface.IsValid() ? ViewportInterface.Pin()->OnMouseButtonDown(MyGeometry, MouseEvent) : FReply::Unhandled();
}
```

根据 `ViewportInterface` 的实现不同，最后按键事件会路由给不同的模块

比如这里触发的是 `FSceneViewport::OnMouseButtonDown`，通过 `UGameViewportClient::InputKey`，最后通过 `UPlayerInput::InputKey` 缓存了事件信息

## 事件触发优先级

当你在 **关卡蓝图** 和 `PlayerController` 都监听 `Keyboard Event P` 的时候，只会触发 `PlayerController` 中绑定的事件，而不会触发 关卡蓝图 中的事件

> 如果在 UMG 中也监听了 `Keyboard Event P` 那么只会触发 UMG 中的，因为事件先发送给 Slate 再给 PlayerController

从 `APlayerController::BuildInputStack` 中可以窥得原因

先把 `Pawn` 的 `InputComponent` 塞到 `InputStack` 中，再把 `Pawn` 上其他所有 `InputComponent` 塞到 `InputStack` 中

```cpp
APawn* ControlledPawn = GetPawnOrSpectator();

InputStack.Push(ControlledPawn->InputComponent);

for (UActorComponent* ActorComponent : ControlledPawn->GetComponents())
{
    UInputComponent* PawnInputComponent = Cast<UInputComponent>(ActorComponent);
    if (PawnInputComponent && PawnInputComponent != ControlledPawn->InputComponent)
    {
        InputStack.Push(PawnInputComponent);
    }
}
```

把所有关卡蓝图塞入 `InputStack` 中

```cpp
for (ULevel* Level : GetWorld()->GetLevels())
{
    ALevelScriptActor* ScriptActor = Level->GetLevelScriptActor();
    if (ScriptActor)
    {
        if (ScriptActor->InputEnabled() && ScriptActor->InputComponent)
        {
            InputStack.Push(ScriptActor->InputComponent);
        }
    }
}
```

最后把 APlayerController 的  InputComponent 塞入到 `InputStack`

```cpp
if (InputEnabled())
{
    InputStack.Push(InputComponent);
}

for (int32 Idx=0; Idx<CurrentInputStack.Num(); ++Idx)
{
    UInputComponent* IC = CurrentInputStack[Idx].Get();
    if (IC)
    {
        InputStack.Push(IC);
    }
    else
    {
        CurrentInputStack.RemoveAt(Idx--);
    }
}
```

根据堆栈的先进后出的原则，会优先处理 APlayerController 中的输入事件，再处理关卡蓝图中的输入事件，最后处理 Pawn 身上的输入事件

具体的触发逻辑在 `UPlayerInput::EvaluateInputDelegates` 函数中

```cpp
int32 StackIndex = InputComponentStack.Num()-1;

for ( ; StackIndex >= 0; --StackIndex)
{
    UInputComponent* const IC = InputComponentStack[StackIndex];
    // ...
}
```

这里注意下 `EvaluateInputDelegates` 函数的参数 `KeysWithEvents`，这个参数是外界传递进来的按键事件列表

```cpp
void UPlayerInput::ProcessInputStack(const TArray<UInputComponent*>& InputComponentStack, const float DeltaTime, const bool bGamePaused)
{
	ConditionalBuildKeyMappings();

	static TArray<TPair<FKey, FKeyState*>> KeysWithEvents;

	APlayerController* PlayerController = GetOuterAPlayerController();
	if (PlayerController)
	{
		PlayerController->PreProcessInput(DeltaTime, bGamePaused);
	}
	
    // 获取按键事件队列
	EvaluateKeyMapState(DeltaTime, bGamePaused, OUT KeysWithEvents);
    // 处理按键事件队列
	EvaluateInputDelegates(InputComponentStack, DeltaTime, bGamePaused, KeysWithEvents);

	if (PlayerController)
	{
		PlayerController->PostProcessInput(DeltaTime, bGamePaused);
	}
	
	FinishProcessingPlayerInput();

	TouchEventLocations.Reset();
	KeysWithEvents.Reset();
}
```

> 这个 `ProcessInputStack` 是 `Tick` 调用的，在 5.3 中是通过 `UEnhancedInputWorldSubsystem::TickPlayerInput` 每帧调用的

`EvaluateKeyMapState` 就是获取事件事件队列的地方，在该函数中主要是遍历 `KeyStateMap` 容器，对其进行解析

因此所有的按键事件数据都是存储在 `KeyStateMap` 容器中的，而 `KeyStateMap` 的数据是通过 `UPlayerInput::InputKey` 添加进来的

> `UPlayerInput::InputKey` 对应前文 **存储按键事件**

## Slate 控件树的 HitTest

可能存在多个窗口，所以先选中窗口，再做 HitTest

```cpp
TSharedPtr<SWindow> Window = FSlateWindowHelper::FindWindowByPlatformWindow(Windows, NativeWindowUnderMouse.ToSharedRef()); 
```

> 通过 OS 知道 鼠标 在哪个原生窗口

```cpp
for (int32 WindowIndex = Windows.Num() - 1; WindowIndex >= 0; --WindowIndex)
{ 
    const TSharedRef<SWindow>& Window = Windows[WindowIndex];
}
```

> 按 Slate 窗口的 Z-Order 从前往后找

### 真正的 HitTest

在函数 `LocateWidgetInWindow` 中调用 `Window->GetHittestGrid().GetBubblePath`

`HitTest` 的实现原理是： `FHittestGrid` 管理的 **空间网格** + **每格绘制顺序排列的 widget 列表** 

- 把窗口区域划分成很多网格
- 每个格子里面存 覆盖到这个格子的 Widget 索引列表
- 列表按 从后到前 排序（等价于绘制顺序/层级顺序），这样命中时可以从最前面的 Widget 开始测

在 `FWidgetData` 中存储关键数据

- 覆盖的格子范围：`UpperLeftCell` 和 `LowerRightCell`
- 排序 `PrimarySort` 和 `SecondarySort`

HitTestGrid 不是在点击时临时遍历 widget 树时算出来的，而是在 Slate 的绘制/布局阶段逐步填充的，参考 `SInvalidationPanel::OnPaint` 函数

```cpp
int32 SInvalidationPanel::OnPaint(...)
{
    ...
    const bool bHittestCleared = HittestGrid->SetHittestArea(Args.RootGrid.GetGridOrigin(), Args.RootGrid.GetGridSize(), Args.RootGrid.GetGridWindowOrigin());
    HittestGrid->SetOwner(this);
    HittestGrid->SetCullingRect(MyCullingRect);
    FPaintArgs NewArgs = Args.WithNewHitTestGrid(HittestGrid.Get());
    ...
    Args.GetHittestGrid().AddGrid(HittestGrid);
}
```

大概的流程就是

1. 把鼠标点映射到网格坐标
2. 取出该网格里候选的 widget 列表
3. 按 前到后 的顺序测试候选 widget
4. 对每个而候选 widget 做精确测试
5. 找到最前面的命中的 widget，回溯构建 bubble path


