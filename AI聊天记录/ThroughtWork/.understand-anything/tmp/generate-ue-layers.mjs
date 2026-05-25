#!/usr/bin/env node
// Deterministic layer generator for Unreal Engine.
// Reads assembled-graph.json, partitions file-level nodes into UE-specific
// architectural layers based on filePath. Writes layers.json.
//
// Usage: node generate-ue-layers.mjs

import fs from 'node:fs';
import path from 'node:path';

const projectRoot = process.cwd();
const interDir = path.join(projectRoot, '.understand-anything', 'intermediate');
const graphPath = path.join(interDir, 'assembled-graph.json');
const outPath = path.join(interDir, 'layers.json');

console.log('Loading assembled-graph.json...');
const g = JSON.parse(fs.readFileSync(graphPath, 'utf8'));
console.log(`Loaded ${g.nodes.length} nodes.`);

const fileLevelTypes = new Set([
  'file', 'config', 'document', 'service', 'pipeline',
  'table', 'schema', 'resource', 'endpoint',
]);

// Layer definitions, evaluated top-to-bottom (first match wins).
const layerDefs = [
  {
    id: 'layer:engine-core',
    name: '引擎核心',
    description: 'Unreal Engine 核心模块（Source/Runtime/Core 及紧密耦合的基础库），提供内存管理、字符串、容器、数学、日志、序列化等最底层服务，被几乎所有其他模块依赖。',
    match: (p) => p.startsWith('Source/Runtime/Core/')
      || p.startsWith('Source/Runtime/CoreUObject/')
      || p.startsWith('Source/Runtime/Engine/Classes/')
      || p.startsWith('Source/Runtime/Engine/Public/')
      || p.startsWith('Source/Runtime/Engine/Private/')
      || p.startsWith('Source/Runtime/Launch/')
      || p.startsWith('Source/Runtime/TraceLog/')
      || p.startsWith('Source/Runtime/Cbor/'),
  },
  {
    id: 'layer:engine-rendering',
    name: '渲染与RHI',
    description: '图形渲染管线、RHI（渲染硬件接口）、Shader 系统、材质系统等图形相关的运行时模块。',
    match: (p) => p.startsWith('Source/Runtime/RHI')
      || p.startsWith('Source/Runtime/Renderer/')
      || p.startsWith('Source/Runtime/RenderCore/')
      || p.startsWith('Source/Runtime/D3D')
      || p.startsWith('Source/Runtime/Vulkan')
      || p.startsWith('Source/Runtime/OpenGL')
      || p.startsWith('Source/Runtime/MetalRHI')
      || p.startsWith('Source/Runtime/Slate')
      || p.startsWith('Source/Runtime/SlateCore')
      || p.startsWith('Source/Runtime/UMG'),
  },
  {
    id: 'layer:engine-runtime',
    name: '引擎运行时',
    description: '其他运行时模块：物理、动画、音频、网络、AI、输入、媒体、平台抽象等。',
    match: (p) => p.startsWith('Source/Runtime/'),
  },
  {
    id: 'layer:editor',
    name: '编辑器',
    description: 'UnrealEd 及其依赖的编辑器专用模块，包括关卡编辑、蓝图编辑、内容浏览器、各种资产编辑器等。',
    match: (p) => p.startsWith('Source/Editor/'),
  },
  {
    id: 'layer:developer-tools',
    name: '开发者工具',
    description: '调试、性能分析、自动化测试、源码控制、目标设备管理等开发者工具模块。',
    match: (p) => p.startsWith('Source/Developer/'),
  },
  {
    id: 'layer:programs',
    name: '独立程序',
    description: '随引擎构建的独立可执行程序：UnrealHeaderTool、ShaderCompileWorker、UnrealLightmass、AutomationTool 等。',
    match: (p) => p.startsWith('Source/Programs/'),
  },
  {
    id: 'layer:third-party',
    name: '第三方代码',
    description: '内置的第三方库源码（如 SymsLib 等）。',
    match: (p) => p.startsWith('Source/ThirdParty/') || /\/ThirdParty\//.test(p),
  },
  {
    id: 'layer:plugins-runtime',
    name: '运行时插件',
    description: 'Plugins/Runtime 以及 2D、Animation、FX、Media、Online 等运行时类插件，扩展引擎运行时能力。',
    match: (p) => p.startsWith('Plugins/Runtime/')
      || p.startsWith('Plugins/2D/')
      || p.startsWith('Plugins/Animation/')
      || p.startsWith('Plugins/FX/')
      || p.startsWith('Plugins/Media/')
      || p.startsWith('Plugins/Messaging/')
      || p.startsWith('Plugins/Online/')
      || p.startsWith('Plugins/AI/')
      || p.startsWith('Plugins/Cameras/')
      || p.startsWith('Plugins/Compositing/')
      || p.startsWith('Plugins/EnhancedInput/')
      || p.startsWith('Plugins/MovieScene/')
      || p.startsWith('Plugins/NNE/')
      || p.startsWith('Plugins/Compression/')
      || p.startsWith('Plugins/Chaos'),
  },
  {
    id: 'layer:plugins-editor',
    name: '编辑器插件',
    description: 'Plugins/Editor 以及 MeshPainting、Importers、Bridge、Fab、LightWeightInstancesEditor 等编辑器扩展插件。',
    match: (p) => p.startsWith('Plugins/Editor/')
      || p.startsWith('Plugins/MeshPainting/')
      || p.startsWith('Plugins/Importers/')
      || p.startsWith('Plugins/Interchange/')
      || p.startsWith('Plugins/Bridge/')
      || p.startsWith('Plugins/Fab/')
      || p.startsWith('Plugins/LightWeightInstancesEditor/')
      || p.startsWith('Plugins/BlueprintFileUtils/')
      || p.startsWith('Plugins/AudioInsights/')
      || p.startsWith('Plugins/ChaosClothAssetEditor/')
      || p.startsWith('Plugins/ChaosVD/'),
  },
  {
    id: 'layer:plugins-experimental',
    name: '实验性 / 开发者插件',
    description: 'Plugins/Experimental、Plugins/Developer、Plugins/Enterprise 等仍在开发迭代或面向特定行业场景的插件。',
    match: (p) => p.startsWith('Plugins/Experimental/')
      || p.startsWith('Plugins/Developer/')
      || p.startsWith('Plugins/Enterprise/')
      || p.startsWith('Plugins/VirtualProduction/'),
  },
  {
    id: 'layer:plugins-other',
    name: '其他插件',
    description: '其余尚未归类的引擎自带插件。',
    match: (p) => p.startsWith('Plugins/'),
  },
  {
    id: 'layer:configuration',
    name: '配置',
    description: 'Config/ 目录下的引擎 ini 配置，按平台与子系统组织，决定运行时默认行为。',
    match: (p) => p.startsWith('Config/'),
  },
  {
    id: 'layer:platforms',
    name: '平台扩展',
    description: 'Platforms/ 目录下的特定平台扩展（如 VisionOS）。',
    match: (p) => p.startsWith('Platforms/'),
  },
  {
    id: 'layer:build-tools',
    name: '构建与工具',
    description: 'Build/、Extras/、Binaries/DotNET 等构建脚本、批处理与辅助工具。',
    match: (p) => p.startsWith('Build/') || p.startsWith('Extras/') || p.startsWith('Binaries/'),
  },
  {
    id: 'layer:documentation',
    name: '文档',
    description: '工程文档、说明文件、Markdown 文章。',
    match: (p) => p.startsWith('Documentation/') || /\.(md|markdown)$/i.test(p) || /README/i.test(p),
  },
  {
    id: 'layer:shaders',
    name: '着色器',
    description: 'Shaders/ 目录下的引擎着色器源码。',
    match: (p) => p.startsWith('Shaders/'),
  },
  {
    id: 'layer:misc',
    name: '其他',
    description: '不属于以上分类的杂项文件。',
    match: () => true,
  },
];

const layerNodes = new Map(layerDefs.map(l => [l.id, []]));

for (const n of g.nodes) {
  if (!fileLevelTypes.has(n.type)) continue;
  const p = n.filePath || '';
  for (const def of layerDefs) {
    if (def.match(p)) {
      layerNodes.get(def.id).push(n.id);
      break;
    }
  }
}

const layers = layerDefs
  .map(def => ({
    id: def.id,
    name: def.name,
    description: def.description,
    nodeIds: layerNodes.get(def.id),
  }))
  .filter(l => l.nodeIds.length > 0);

fs.writeFileSync(outPath, JSON.stringify(layers, null, 2), 'utf8');
console.log(`Wrote ${layers.length} layers:`);
for (const l of layers) console.log(`  ${l.id.padEnd(28)} ${l.nodeIds.length} 个文件`);
