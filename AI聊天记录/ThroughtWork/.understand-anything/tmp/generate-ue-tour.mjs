#!/usr/bin/env node
// Deterministic guided-tour generator for Unreal Engine.
// Picks representative nodes from the assembled graph based on UE's well-known
// architecture and writes tour.json.
//
// Usage: node generate-ue-tour.mjs

import fs from 'node:fs';
import path from 'node:path';

const projectRoot = process.cwd();
const interDir = path.join(projectRoot, '.understand-anything', 'intermediate');
const graphPath = path.join(interDir, 'assembled-graph.json');
const outPath = path.join(interDir, 'tour.json');

const g = JSON.parse(fs.readFileSync(graphPath, 'utf8'));
const idSet = new Set(g.nodes.map(n => n.id));

// Helper: pick first existing node from a list of candidate IDs.
function pick(...ids) {
  return ids.filter(id => idSet.has(id));
}

// Helper: pick first N file IDs whose path matches a prefix.
function pickByPrefix(prefix, limit = 3, suffix = null) {
  const out = [];
  for (const n of g.nodes) {
    if (!n.filePath || !n.filePath.startsWith(prefix)) continue;
    if (suffix && !n.filePath.endsWith(suffix)) continue;
    out.push(n.id);
    if (out.length >= limit) break;
  }
  return out;
}

const steps = [
  {
    order: 1,
    title: '项目总览',
    description: '从顶层配置入手，了解 Unreal Engine 引擎仓库的整体形态：核心源码位于 Source/，引擎插件位于 Plugins/，可配置参数集中在 Config/。先浏览引擎根级的 Base 配置文件，掌握引擎默认行为的入口。',
    nodeIds: pick('config:Config/Base.ini', 'config:Config/BaseEngine.ini', 'config:Config/BaseGame.ini', 'config:Config/BaseEditor.ini'),
  },
  {
    order: 2,
    title: '引擎启动入口',
    description: '引擎可执行程序的入口点位于 Source/Runtime/Launch。这里负责模块加载、引擎循环（GameEngine / EditorEngine）的创建与销毁，是阅读引擎初始化流程的起点。',
    nodeIds: pickByPrefix('Source/Runtime/Launch/', 5),
  },
  {
    order: 3,
    title: '引擎核心 Core',
    description: 'Source/Runtime/Core 提供内存分配、容器、字符串、数学、文件 IO、日志、Tick、Stat 等所有引擎模块依赖的基础设施。建议先阅读 CoreMinimal.h、PlatformAtomics、HAL（硬件抽象层）以理解平台抽象。',
    nodeIds: pickByPrefix('Source/Runtime/Core/Public/', 8),
  },
  {
    order: 4,
    title: 'UObject 反射系统',
    description: 'CoreUObject 实现 UE 的反射、垃圾回收、序列化系统，是蓝图、网络复制、资产系统、编辑器属性面板的统一基石。理解 UObject、UClass、FProperty 是阅读引擎任何上层模块的前提。',
    nodeIds: pickByPrefix('Source/Runtime/CoreUObject/Public/', 8),
  },
  {
    order: 5,
    title: 'Engine 模块 — 游戏世界',
    description: 'Source/Runtime/Engine 定义 Actor、Component、World、Level、GameMode 等核心游戏对象，是 gameplay 框架的实现层。同时承担资产（StaticMesh、Material、Texture 等）的运行时表达。',
    nodeIds: pickByPrefix('Source/Runtime/Engine/Classes/', 10),
  },
  {
    order: 6,
    title: '渲染管线 RHI + Renderer',
    description: 'RHI 是平台无关的渲染硬件接口；Renderer 在其之上实现延迟/前向渲染、阴影、光照等渲染算法；各 *RHI（D3D11/12、Vulkan、Metal）模块提供平台后端实现。',
    nodeIds: [
      ...pickByPrefix('Source/Runtime/RHI/Public/', 3),
      ...pickByPrefix('Source/Runtime/Renderer/Private/', 3),
      ...pickByPrefix('Source/Runtime/RenderCore/Public/', 2),
    ],
  },
  {
    order: 7,
    title: 'Slate + UMG UI 框架',
    description: 'Slate 是 UE 自研的 immediate-mode UI 框架，编辑器界面全部基于它构建；UMG 在 Slate 上提供面向 gameplay 的 widget blueprint。',
    nodeIds: [
      ...pickByPrefix('Source/Runtime/SlateCore/Public/', 3),
      ...pickByPrefix('Source/Runtime/Slate/Public/', 3),
      ...pickByPrefix('Source/Runtime/UMG/Public/', 2),
    ],
  },
  {
    order: 8,
    title: '编辑器 UnrealEd',
    description: 'Source/Editor/UnrealEd 是编辑器主模块，承载关卡编辑、Actor 操控、内容浏览器、菜单系统等。编辑器其他子模块（PropertyEditor、SceneOutliner、BlueprintGraph 等）围绕它扩展。',
    nodeIds: pickByPrefix('Source/Editor/UnrealEd/', 8),
  },
  {
    order: 9,
    title: '开发者工具',
    description: 'Source/Developer 下的工具集合：Insights / Trace（性能与内存分析）、AutomationController（自动化测试）、SourceControl（版本控制集成）等，仅在非 Shipping 构建下编译。',
    nodeIds: [
      ...pickByPrefix('Source/Developer/TraceInsights/', 3),
      ...pickByPrefix('Source/Developer/AutomationController/', 2),
      ...pickByPrefix('Source/Developer/SourceControl/', 2),
    ],
  },
  {
    order: 10,
    title: '独立程序 Programs',
    description: 'Source/Programs 下是随引擎构建的独立可执行程序：UnrealHeaderTool（UHT，生成反射代码）、ShaderCompileWorker（着色器编译进程）、UnrealLightmass（光照烘焙）、AutomationTool 等。',
    nodeIds: [
      ...pickByPrefix('Source/Programs/UnrealHeaderTool/', 3),
      ...pickByPrefix('Source/Programs/ShaderCompileWorker/', 2),
      ...pickByPrefix('Source/Programs/UnrealLightmass/', 2),
    ],
  },
  {
    order: 11,
    title: '插件生态：运行时插件',
    description: 'Plugins/Runtime 与 Plugins/Animation、Plugins/FX、Plugins/Online 等承载了大量可选运行时能力（Niagara 粒子、增强输入、在线子系统、控制绑定 Animation Rigging 等），通过 .uplugin 描述启用。',
    nodeIds: [
      ...pickByPrefix('Plugins/Runtime/', 3),
      ...pickByPrefix('Plugins/Animation/', 2),
      ...pickByPrefix('Plugins/FX/Niagara/', 2),
      ...pickByPrefix('Plugins/EnhancedInput/', 1),
    ],
  },
  {
    order: 12,
    title: '插件生态：编辑器与实验性',
    description: 'Plugins/Editor、Plugins/Experimental、Plugins/Developer 等提供编辑器扩展与新功能孵化（Chaos 物理、虚拟制作、Interchange 资产导入框架等）。',
    nodeIds: [
      ...pickByPrefix('Plugins/Editor/', 2),
      ...pickByPrefix('Plugins/Experimental/ChaosCaching/', 2),
      ...pickByPrefix('Plugins/Interchange/', 2),
      ...pickByPrefix('Plugins/VirtualProduction/', 1),
    ],
  },
];

// Filter out empty nodeIds and ensure all referenced IDs exist.
const finalSteps = steps
  .map(s => ({ ...s, nodeIds: s.nodeIds.filter(id => idSet.has(id)) }))
  .filter(s => s.nodeIds.length > 0);

// Re-number after filtering
finalSteps.forEach((s, i) => { s.order = i + 1; });

fs.writeFileSync(outPath, JSON.stringify(finalSteps, null, 2), 'utf8');
console.log(`Wrote ${finalSteps.length} tour steps:`);
for (const s of finalSteps) console.log(`  ${s.order}. ${s.title} (${s.nodeIds.length} 个节点)`);
