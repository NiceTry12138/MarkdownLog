# Unlua

源码部分直接从 `github` 上的 `Tencent/Unlua` 项目中复制过来，并提供 [BP_PlayerController_C](./BP_PlayerController_C.lua) 作为实例代码

## 绑定 lua 和 UOBject

### 准备工作

从 [UnLuaManager.cpp](./Source/UnLua/Private/UnLuaModule.cpp) 文件开始，当模块加载完毕之后会监听 UObject 的创建事件 和 Error 事件用于日志输出

```cpp
OnHandleSystemErrorHandle = FCoreDelegates::OnHandleSystemError.AddRaw(this, &FUnLuaModule::OnSystemError);
OnHandleSystemEnsureHandle = FCoreDelegates::OnHandleSystemEnsure.AddRaw(this, &FUnLuaModule::OnSystemError);
GUObjectArray.AddUObjectCreateListener(this);
GUObjectArray.AddUObjectDeleteListener(this);
```

由于 `FUnLuaModule` 继承了 `FUObjectArray::FUObjectCreateListener` 和 `FUObjectArray::FUObjectDeleteListeners`，通过重写 `NotifyUObjectCreated` 和 `NotifyUObjectDeleted` 可以知道全局状态下 `UObject` 的创建和删除

当一个 UObject 被创建之后，会将其与 lua 对象进行绑定

```cpp
Env->TryBind(Object);
Env->TryReplaceInputs(Object);
```

> `TryReplaceInputs` 是为了应对当 `UObject` 是 `UInputComponent` 的时候，注册处理输入事件

所以重点还是 `FLuaEnv::TryBind` 的逻辑上

### FLuaEnv::TryBind

1. 如果 `Class->ImplementsInterface(InterfaceClass)` 判断该类实现了 `UUnLuaInterface` 接口，说明该类需要绑定 lua 对象，通过 `ModuleLocator->Locate(Object)` 获取该类对应模块名
2. 如果没有实现 `UUnLuaInterface` 接口，则通过 `GLuaDynamicBinding` 判断该类有没有设定绑定的模块名

```lua
local img = NewObject(WidgetClass, self, nil, "Tutorials.IconWidget")
img:AddToViewport()
img:RandomPosition()
```

> `NewObject` 参数列表对应 类、Outer、Name、ModuleName

参考这段 `lua` 代码，虽然调用的是 `NewObject`，但这是在 `lua` 层，本质上调用的是 `Global_NewObject` 函数，在该函数中通过构造 `FScopedLuaDynamicBinding` 对象将 ModuleName 等数据设置给 `GLuaDynamicBinding`

```cpp
    FScopedLuaDynamicBinding Binding(L, Class, UTF8_TO_TCHAR(ModuleName), TableRef);
#if ENGINE_MAJOR_VERSION <= 4 && ENGINE_MINOR_VERSION < 26
    UObject* Object = StaticConstructObject_Internal(Class, Outer, Name);
#else
    FStaticConstructObjectParameters ObjParams(Class);
    ObjParams.Outer = Outer;
    ObjParams.Name = Name;
    UObject* Object = StaticConstructObject_Internal(ObjParams);
```

得到 `ModuleName` 和 `UObject*` 之后，通过 `UUnLuaManager::Bind` 执行绑定逻辑

### UUnLuaManager::Bind

在 `UUnLuaManager::Bind` 函数中主要是三个函数的调用

- `BindClass(Class, InModuleName, Error)`
- `Env->GetObjectRegistry()->Bind(Class)`
- `Env->GetObjectRegistry()->Bind(Object)`

`BindClass` 主要做了两个事情

1. 通过 `ModuleName` 获取 `Table` 的引用，并保存到 `Classes` 映射表中
2. 函数 `Hook`，遍历 `Table` 中所有的函数名，如果 `UClass` 中存在同名函数，则替换 `UClass` 的 `UFCuntion` 的执行指针为 `UnLua` 的 `execScriptCallLua`

```cpp
const auto Ref = luaL_ref(L, LUA_REGISTRYINDEX);
lua_settop(L, Top);

auto& BindInfo = Classes.Add(Class);
BindInfo.Class = Class;
BindInfo.ModuleName = InModuleName;
BindInfo.TableRef = Ref;
```

> 保存 BindInfo 到 Classes 中

```cpp
UnLua::LowLevel::GetFunctionNames(Env->GetMainState(), Ref, BindInfo.LuaFunctions);
ULuaFunction::GetOverridableFunctions(Class, BindInfo.UEFunctions);

// 用LuaTable里所有的函数来替换Class上对应的UFunction
for (const auto& LuaFuncName : BindInfo.LuaFunctions)
{
    UFunction** Func = BindInfo.UEFunctions.Find(LuaFuncName);
    if (Func)
    {
        UFunction* Function = *Func;
        ULuaFunction::Override(Function, Class, LuaFuncName);
    }
}
```

> 替换函数指针

`Env->GetObjectRegistry()->Bind(Class)` 就是在调用 `FObjectRegistry::Bind`

### FObjectRegistry::Bind

这一步是为了建立具体的 `UObject` 与 LuaTable 实例之间的联系

```cpp
Env->GetObjectRegistry()->Bind(Class);  // 确保类已绑定
Env->GetObjectRegistry()->Bind(Object); // 绑定具体对象
```

在 `Bind` 函数中，在 lua 层绑定 UObject

```cpp
lua_getfield(L, LUA_REGISTRYINDEX, REGISTRY_KEY);
lua_pushlightuserdata(L, Object);
lua_newtable(L); // create a Lua table ('INSTANCE')
PushObjectCore(L, Object); // push UObject ('RAW_UOBJECT')
lua_pushstring(L, "Object");
lua_pushvalue(L, -2);
lua_rawset(L, -4); // INSTANCE.Object = RAW_UOBJECT
```

> REGISTRY_KEY = "UnLua_ObjectMap"

一些前置知识点

1. `LUA_REGISTRYINDEX` 是 `Lua` 的注册表（registry），一个全局隐藏 table，C/C++ 常用它做全局缓存
2. `REGISTRY_KEY` 是一个字符串 `key`，用来在 `registry` 里取到绑定总表 `UnLua_ObjectMap`
3. `lightuserdata` 把 `UObject*` 当作 **裸指针值** 压入 `Lua`，当 `key` 使用
4. `lua_newtable` 是创建一个 `Table`

所以上面这段代码对应的逻辑就是

1. 从 `registry` 中获取名为 `UnLua_ObjectMap` 的 table，然后将 `UObject*` 作为 key，新创建的 table 作为 value 的键值对添加到 `UnLua_ObjectMap` 中
2. 新建 Table 的 INSTANCE，设置 `INSTANCE.Object` 值为 `PushObjectCore` 添加到栈中的 userdata

随后设置 INSTANCE 的 metatable

在 `Bind` 函数中，在 C++ 层缓存 lua 对象

```cpp
const auto Ret = luaL_ref(L, LUA_REGISTRYINDEX);
ObjectRefs.Add(Object, Ret);
```

## lua 层使用函数或者属性

```lua
function M:OnComponentHit_Sphere(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit)
	self:K2_DestroyActor()
end
```

以上面这段 lua 代码为例，当调用 `K2_DestroyActor` 时，如何知道调用的是 C++ 函数还是 lua 函数呢？

> 这里明显是调用 C++ 函数

相关的逻辑在 `FClassRegistry::PushMetatable` 函数中，该函数在绑定 lua 和 C++ 对象时通过 `PushObjectCore` 调用 `TryToSetMetatable` 进行设置

```cpp
luaL_newmetatable(L, MetatableName);
lua_pushstring(L, "__index");
lua_pushcfunction(L, Class_Index);
lua_rawset(L, -3);

lua_pushstring(L, "__newindex");
lua_pushcfunction(L, Class_NewIndex);
lua_rawset(L, -3);
```

- 在 lua 中 `__index` 是一个函数，用于在你读取一个 table 中不存在的属性（函数）时，应该怎么做，其有两个参数 `tbl` 和 `key` 分别表示 table 本体和操作的属性名
- 在 lua 中 `__newindex` 是一个函数，用于在你写入一个不存在的属性（函数）时，应该怎么做，其有三个参数 `tbl`、`key` 和 `value` 分别表示 table 本体、**新增属性名** 和 **赋值的属性值**

所以在调用 `self:K2_DestroyActor()` 时，会执行 `Class_Index` 函数

通过 `Class_Index` -> `GetField` -> `GetFieldInternal` 获取对应的 `Field`

```cpp
const char* ClassName = lua_tostring(L, -1);    // 从元表中取出类名
const char* FieldName = lua_tostring(L, 2);     // 取出 key = "K2_DestroyActor"
```

通过 `ClassName` 找到或创建 `ClassDesc`，然后在 `ClassDesc` 中注册 `FieldName` 这个字段

> 在 `FClassDesc::RegisterField` 函数中通过反射获取对应的 `FProperty` 或者 `UFunction`，并封装成 `FieldDesc` 存储到 `Fields` 中，方便下次查找

如果需要，则可以通过下面这段代码，将 UFunction 封装成 closure 并缓存到 metatable 中，下次直接命中，无需查表

```cpp
PushField(L, Field);
lua_pushvalue(L, 2);
lua_pushvalue(L, -2);
lua_rawset(L, -4);  // 写入元表缓存，下次直接命中
```

## 创建结构体

```lua
self.Velocity = UE.FVector()
```

在初始化 unlua 的时候，会调用 `UnLua::UELib::Open(...)` 进行下面这些操作

```cpp
lua_newtable(L);
lua_pushstring(L, "__index");
lua_pushcfunction(L, UE_Index);
lua_rawset(L, -3);

lua_pushvalue(L, -1);
lua_setmetatable(L, -2);

lua_pushvalue(L, -1);
lua_pushstring(L, REGISTRY_KEY);
lua_rawset(L, LUA_REGISTRYINDEX);

luaL_setfuncs(L, UE_Functions, 0);
lua_setglobal(L, NAMESPACE_NAME);
```

> `NAMESPACE_NAME` 等于 `"UE"` 

所以，当调用 `UE.FVector()` 的时候会调用到 `__index` 设置的 `UE_Index` 函数

单论 `FVector` 这个字符串，是 `F` 开头，会走到下面这段代码的 if 逻辑中

```cpp
const char Prefix = Name[0];
const auto& Env = UnLua::FLuaEnv::FindEnvChecked(L);
if (Prefix == 'U' || Prefix == 'A' || Prefix == 'F')
{
    const auto ReflectedType = UnLua::FClassRegistry::LoadReflectedType(Name + 1);
    if (!ReflectedType)
        return 0;

    if (ReflectedType->IsNative())
    {
        if (auto Struct = Cast<UStruct>(ReflectedType))
            Env.GetClassRegistry()->Register(Struct);
    }
    else
    {
        UE_LOG(LogUnLua, Warning, TEXT("attempt to load a blueprint type %s with UE namespace, use UE.UClass.Load or UE.UObject.Load instead."), UTF8_TO_TCHAR(Name));
        return 0;
    }
}
```

`ClassRegistry::Register` → `PushMetatable("FVector")` → 把 `FVector` 的元表注册到 Lua，并通过 `UELib::SetTableForClass` 写入 `UE["FVector"]`

在 `FClassRegistry::PushMetatable` 函数中，如果是 `ScriptStruct` 会设置 `__call` 为 `ScriptStruct_New`

```cpp
lua_pushstring(L, "__call");
lua_pushvalue(L, -2);
lua_pushcclosure(L, ScriptStruct_New, 1); // closure
lua_rawset(L, -4);
```

> `__call` 是为了让 table 像函数一样被调用。如果 obj 不是函数，但它的元表里有 `__call`，Lua 就会改为调用 `getmetatable(obj).__call(obj, ...)`

不过这是针对普通的结构体来说的，对于 `FVector` 等部分结构体来说，他们的 `__call` 另有设置

通过 [LuaLib_FVector.cpp](./Source/UnLua/Private/MathLib/LuaLib_FVector.cpp) 最后一部分可以看到关于 `FVector` 的一些注册逻辑

```cpp
BEGIN_EXPORT_REFLECTED_CLASS(FVector)
    ADD_CONST_FUNCTION_EX("Dot", unluaReal, operator|, const FVector&)
    ADD_CONST_FUNCTION_EX("Cross", FVector, operator^, const FVector&)
    ... // 一些其他的注册逻辑
END_EXPORT_CLASS()
```

> `BEGIN_EXPORT_REFLECTED_CLASS` 宏会展开成 `FExportedFVectorHelper` 并通过静态单例的方式实现自动调用构造函数来注册 `FVector`

```cpp
// 程序启动时自动执行（静态对象构造）
struct FExportedFVectorHelper {
    static FExportedFVectorHelper StaticInstance;  // ← 静态单例
    FExportedFVectorHelper() {
        auto* Class = new TExportedClass<true, FVector>(
            "FVector", nullptr  // bIsReflected=true
        );
        UnLua::ExportClass(Class);  // ← 注册到全局 ReflectedClasses 表
    }
};
// IMPLEMENT_EXPORTED_CLASS 触发静态初始化
FExportedFVectorHelper FExportedFVectorHelper::StaticInstance;
```

在 `FVectorLib` 中存储了 `FVector` 的 `__call` 对应的是 `FVector_New` 函数

通过查看 `FVector_New` 函数可以看到，通过 `NewTypedUserdata` Lua 管理的堆上分配了一块内存空间，然后通过 `placement new` 的方法来调用 `FVector` 的构造函数

```cpp
static int32 FVector_New(lua_State* L)
{
    const int32 NumParams = lua_gettop(L);
    void* Userdata = NewTypedUserdata(L, FVector);

    switch (NumParams)
    {
    case 1:
        {
            new(Userdata) FVector(ForceInitToZero);
            break;
        }
    case 2: 
        // ... 其他逻辑
    }
    return 1;
}
```

由于 `self.Velocity = UE.FVector()` 这里是赋值逻辑，如果原来没有 `Velocity` 属性，则会触发 `__newindex` 逻辑

```cpp
GetField(L);
auto Ptr = lua_touserdata(L, -1);       // 走反射查找 "Velocity" → 得到 FPropertyDesc
auto Property = static_cast<TSharedPtr<UnLua::ITypeOps>*>(Ptr);
void* Self = GetCppInstance(L, 1);      // 获取对应 UObject 实例

// 将栈上的 FVector_userdata 写入对应 UObject 的 Velocity 属性槽
(*Property)->WriteValue_InContainer(L, Self, 3);
```

### 如果第二次赋值呢？  

`self.Velocity = UE.FVector(2, 2, 2)` 是怎么同步给 C++ 层的呢？

注意 `Class_NewIndex` 函数

```cpp
if (Type == LUA_TTABLE)
{
    lua_pushvalue(L, 2);
    lua_pushvalue(L, 3);
    lua_rawset(L, 1);

    //UE_LOG(LogUnLua, Warning, TEXT("%s: You are modifying metatable! Please make sure you know what you are doing!"), ANSI_TO_TCHAR(__FUNCTION__));
}
```

只有当找不到对应的 UE 属性的时候，才会写入 lua table 中

所以从始至终 lua table 中都没有 `Velocity` 属性，所以每次给 `Velocity` 赋值都会触发 `Class_NewIndex` 函数，赋值并将值同步给 C++ 层

### 读取结构体分量

如果是 `local x = self.Velocity.X` ？

依然是 `FClassRegistry::PushMetatable` 针对 `ScriptStruct` 有特殊处理

```cpp
lua_pushstring(L, "__index");
lua_pushcfunction(L, ScriptStruct_Index);
lua_rawset(L, -3);
```

也就是说获取结构体的属性值是通过 `ScriptStruct_Index` 来计算的

### 给结构体的分量赋值

如果是 `self.Velocity.X = 100` 是怎么处理的呢？

注意 `(*Property)->ReadValue_InContainer(L, Self, false)` 最后一个参数为 `false`，表示引用传递，也就是说 `self.Velocity` 直接指向 `UObject::Velocity` 的内存地址

`self.Velocity.X = 100` 在 lua 中被分为两个步骤

1. `local temp = self.Velocity`，触发 self 的 `__index`，也就是 `Class_Index`
2. `temp.X = 100`，触发 temp 的 `__newindex`，也就是 `Class_NewIndex`

> ScriptStruct 没有覆盖 `__newindex` 函数，所有跟 Class 走一个逻辑


