# FlowGraph

## 全新的资产类型

> 前置内容：https://zhuanlan.zhihu.com/p/639752004

蓝图编辑器的作用主要就是为了帮助使用者方便的进行数据配置

> 我觉得蓝图本身就是一种配置

虚幻提供了为自定义类型扩展编辑界面的方法

本质上来说，这一套流程分为三个部分

1. UObject，也就是资源对象本体所代表的数据
2. 编辑器类，UObject 对应的编辑器类，可能存储一些额外的数据
3. UI类，编辑器类对应的显示类，用于方便用户使用

### 定于原始数据类型 

对 `FlowGraph` 插件来说，`UFlowAsset` 就是那个用于存储资源对象原始数据的类，所有的操作都是基于这个类来运行的

```cpp
UCLASS(BlueprintType, hideCategories = Object)
class FLOW_API UFlowAsset : public UObject
```

#### 关联 Asset

如何基于这个 `UObject` 创建资产(`Asset`) 呢？】

根据 **前置内容** 来看，继承 `IAssetTypeActions` 来定义 Asset 的效果

在 `FlowGraph` 的插件中，使用 `IAssetTypeActions` 是下面这样的

```cpp
class FLOWEDITOR_API FAssetTypeActions_FlowAsset : public FAssetTypeActions_Base
{
public:
	virtual FText GetName() const override;
	virtual uint32 GetCategories() override;
	virtual FColor GetTypeColor() const override { return FColor(255, 196, 128); }

	virtual UClass* GetSupportedClass() const override;
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;

	virtual void PerformAssetDiff(UObject* OldAsset, UObject* NewAsset, const FRevisionInfo& OldRevision, const FRevisionInfo& NewRevision) const override;
};
```

| 函数名称 | 作用 | 示例 |
| --- | --- | --- |
| GetName | 显示名称 | "Flow Asset" |
| GetCategories | 所属类型 | static_cast<EAssetTypeCategories::Type>(0) |
| GetTypeColor | 资产下面的颜色条  | FColor(255, 196, 128) |
| GetSupportedClass | 该资产关联的 Class | UFlowAsset::StaticClass() |
| OpenAssetEditor | 打开资产编辑面板 |  |

> `FAssetTypeActions_Base` 是继承自 `IAssetTypeActions` 的，预先定义好很多默认内容

与此同时，UE5 也提供了新的方法，来定义 `Asset`，其名为 `UAssetDefinition`

```cpp
class FLOWEDITOR_API UAssetDefinition_FlowAsset : public UAssetDefinition
{
	GENERATED_BODY()

public:
	virtual FText GetAssetDisplayName() const override;
	virtual FLinearColor GetAssetColor() const override;
	virtual TSoftClassPtr<UObject> GetAssetClass() const override;
	virtual TConstArrayView<FAssetCategoryPath> GetAssetCategories() const override;
	virtual FAssetSupportResponse CanLocalize(const FAssetData& InAsset) const override;

	virtual EAssetCommandResult OpenAssets(const FAssetOpenArgs& OpenArgs) const override;
	virtual EAssetCommandResult PerformAssetDiff(const FAssetDiffArgs& DiffArgs) const override;
};
```

| 函数名称 | 作用 | 示例 |
| --- | --- | --- |
| GetAssetDisplayName | 显示名称 | "Flow Asset" |
| GetAssetColor | 资产下面的颜色条 | FColor(255, 196, 128) |
| GetAssetClass | 资产关联的 Class | UFlowAsset::StaticClass() |
| GetAssetCategories | 所属类型 | { FFLowAssetCategoryPaths::Flow } |
| OpenAssets | 打开资产对应的编辑面板 |  |

在 `FlowGraph` 中，无论是 `OpenAssets` 还是 `OpenAssetEditor`，统一调用的接口都是

```cpp
const FFlowEditorModule* FlowModule = &FModuleManager::LoadModuleChecked<FFlowEditorModule>("FlowEditor");
FlowModule->CreateFlowAssetEditor(OpenArgs.GetToolkitMode(), OpenArgs.ToolkitHost, FlowAsset);
```

#### 创建 Asset

这里倒是与 **前置知识** 相同，通过继承 `UFactory` 来 Asset 对象

```cpp
UCLASS(HideCategories = Object)
class FLOWEDITOR_API UFlowAssetFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, Category = Asset)
	TSubclassOf<class UFlowAsset> AssetClass;

	virtual bool ConfigureProperties() override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

protected:
	// Parameterized guts of ConfigureProperties()
	bool ConfigurePropertiesInternal(const FText& TitleText);
};
```

当你在资源管理器中右键，会先触发 `ConfigureProperties`，根据返回的 bool 值判断能否创建

此时会创建一个虚拟的 Asset，当你确定文件名称之后，才会执行 `FactoryCreateNew` 用于表示一个 Asset 真正创建了

> 其实也很正常，毕竟在 **确定名称** 之后才能真正创建。当你右键创建资产，但是不按回车或者鼠标来设定名称，而是一直按 ESC，资产不会被创建

#### 创建 资产编辑器

```cpp
class FLOWEDITOR_API FFlowAssetEditor : public FAssetEditorToolkit, public FEditorUndoClient, public FGCObject, public FNotifyHook
```

提供了一套完整的框架来处理编辑器的生命周期、UI布局、菜单系统、撤销重做等功能

使用 `FEditorUndoClient` 来实现 Undo 功能

```cpp
void FFlowAssetEditor::PostUndo(bool bSuccess)
{
	HandleUndoTransaction();
}

void FFlowAssetEditor::PostRedo(bool bSuccess)
{
	HandleUndoTransaction();
}

void FFlowAssetEditor::HandleUndoTransaction()
{
	SetUISelectionState(NAME_None);
	GraphEditor->NotifyGraphChanged();
	FSlateApplication::Get().DismissAllMenus();
}
```

> `PostUndo` 和 `PostRedo` 都是 `FEditorUndoClient` 提供的接口

`HandleUndoTransaction` 本质是声明："状态已变，请更新显示"

使用 `FNotifyHook` 提供 **属性变化** 接口，也就是 `NotifyPostChange` 和 `NotifyPreChange` 函数

```cpp
void FFlowAssetEditor::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged)
{
	if (PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
	{
		GraphEditor->NotifyGraphChanged();
	}
}
```

> 这个属性可能是成员属性，也可能是 Graph 中某个 Node 的配置属性

当然，最重要的应该是基于 `FAssetEditorToolkit` 而实现的接口

使用 `FAssetEditorToolkit`，它是构建自定义资产编辑器的核心基类，用于创建类似蓝图编辑器、材质编辑器等专业编辑环境

```cpp
virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;

virtual void InitToolMenuContext(FToolMenuContext& MenuContext) override;
virtual void PostRegenerateMenusAndToolbars() override;
virtual void SaveAsset_Execute() override;
virtual void SaveAssetAs_Execute() override;
```

| 函数名称 | 作用 |
| --- | --- |
| RegisterTabSpawners | 义编辑器布局结构 |
| UnregisterTabSpawners | 编辑器关闭时反注册标签页 |
| InitToolMenuContext | 扩展编辑器菜单/工具栏 |
| PostRegenerateMenusAndToolbars | 菜单/工具栏刷新后执行额外操作 |
| SaveAsset_Execute | 执行"保存"命令的核心逻辑 |
| SaveAssetAs_Execute | 执行"另存为"命令 |

当打开了 `Standalone` 的编辑窗口，有哪些窗口？这个实现的地方就是 `RegisterTabSpawners` 中

```cpp
void FFlowAssetEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_FlowAssetEditor", "Flow Editor"));
	const auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(DetailsTab, FOnSpawnTab::CreateSP(this, &FFlowAssetEditor::SpawnTab_Details))
				.SetDisplayName(LOCTEXT("DetailsTab", "Details"))
				.SetGroup(WorkspaceMenuCategoryRef)
				.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"));

	InTabManager->RegisterTabSpawner(GraphTab, FOnSpawnTab::CreateSP(this, &FFlowAssetEditor::SpawnTab_Graph))
				.SetDisplayName(LOCTEXT("GraphTab", "Graph"))
				.SetGroup(WorkspaceMenuCategoryRef)
				.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "GraphEditor.EventGraph_16x"));

    // 其他 Tab 的创建
}
```


