#!/usr/bin/env node
/**
 * generate-module-graphs.mjs
 * 从全量 knowledge-graph.json 切片，为每个 UE 模块生成独立的 knowledge-graph.json
 * 输出: ThroughtWork/Understand/Modules/<module-key>/knowledge-graph.json
 */
import fs from 'node:fs';
import path from 'node:path';

const ROOT = process.cwd();
const KG_PATH  = path.join(ROOT, '.understand-anything', 'knowledge-graph.json');
const OUT_BASE = path.join(ROOT, 'ThroughtWork', 'Understand', 'Modules');

// ── 1. 加载全量图 ──────────────────────────────────────────────────────────────
console.log('Loading knowledge-graph.json …');
const fullGraph = JSON.parse(fs.readFileSync(KG_PATH, 'utf8'));
console.log(`  nodes=${fullGraph.nodes.length}, edges=${fullGraph.edges.length}`);

// ── 2. 发现插件根路径（以 .uplugin 文件为锚点）─────────────────────────────────
const pluginRoots = []; // [{ prefix, key }] — prefix 末尾带 '/'
for (const node of fullGraph.nodes) {
  const fp = (node.filePath || '').replace(/\\/g, '/');
  if (fp.startsWith('Plugins/') && fp.endsWith('.uplugin')) {
    const dir = fp.split('/').slice(0, -1).join('/');        // e.g. Plugins/FX/Niagara
    const parts = dir.split('/');                             // ['Plugins','FX','Niagara']
    const key = 'Plugin-' + parts.slice(1).join('-');        // Plugin-FX-Niagara
    pluginRoots.push({ prefix: dir + '/', key });
  }
}
// 按前缀长度降序，最长（最精确）优先
pluginRoots.sort((a, b) => b.prefix.length - a.prefix.length);

function pluginKey(fp) {
  for (const { prefix, key } of pluginRoots) {
    if (fp.startsWith(prefix)) return key;
  }
  // 回退：用路径前两/三段
  const parts = fp.split('/');
  if (parts.length >= 3) return 'Plugin-' + parts[1] + '-' + parts[2];
  return 'Plugin-' + parts[1];
}

// ── 3. 按模块分组节点 ─────────────────────────────────────────────────────────
function moduleKey(filePath) {
  if (!filePath) return null;
  const fp = filePath.replace(/\\/g, '/');
  // Source/{Runtime,Editor,Developer,Programs}/<ModuleName>/…
  const src = fp.match(/^Source\/(Runtime|Editor|Developer|Programs)\/([^/]+)\//);
  if (src) return `Source-${src[1]}-${src[2]}`;
  // Plugins/…
  if (fp.startsWith('Plugins/')) return pluginKey(fp);
  // Platforms/<Platform>/…
  const plat = fp.match(/^Platforms\/([^/]+)\//);
  if (plat) return `Platforms-${plat[1]}`;
  return null;
}

const groupedNodes = new Map(); // key → node[]
for (const node of fullGraph.nodes) {
  const key = moduleKey(node.filePath);
  if (!key) continue;
  if (!groupedNodes.has(key)) groupedNodes.set(key, []);
  groupedNodes.get(key).push(node);
}
console.log(`  Discovered ${groupedNodes.size} modules`);

// ── 4. 建立边的快速索引 ────────────────────────────────────────────────────────
const edgesBySource = new Map();
for (const edge of fullGraph.edges) {
  if (!edgesBySource.has(edge.source)) edgesBySource.set(edge.source, []);
  edgesBySource.get(edge.source).push(edge);
}

// ── 5. 辅助函数 ───────────────────────────────────────────────────────────────
const COMPLEXITY_ORDER = { complex: 0, moderate: 1, simple: 2 };
function pickByComplexity(nodes, max) {
  return [...nodes]
    .sort((a, b) => (COMPLEXITY_ORDER[a.complexity] || 1) - (COMPLEXITY_ORDER[b.complexity] || 1))
    .slice(0, max)
    .map(n => n.id);
}

function nodeLanguages(nodes) {
  const LANG_TAGS = new Set(['cpp','c','csharp','python','javascript','typescript',
    'json','config','batch','shell','markdown','java','swift','kotlin','uplugin']);
  const langs = new Set();
  for (const n of nodes) {
    for (const t of (n.tags || [])) {
      if (LANG_TAGS.has(t)) langs.add(t);
    }
  }
  return langs.size ? [...langs].sort() : ['cpp'];
}

function buildLayers(nodes) {
  const layers = [
    { id: 'layer:module-config', name: '模块配置',  description: '构建脚本（Build.cs）和插件描述文件（.uplugin），定义模块依赖与编译参数。', nodeIds: [] },
    { id: 'layer:public-api',    name: '公共接口',  description: 'Public/ 与 Classes/ 目录下的头文件，定义模块对外暴露的 API 与数据类型。',  nodeIds: [] },
    { id: 'layer:private-impl',  name: '私有实现',  description: 'Private/ 目录下的实现文件（.cpp/.h），包含模块内部逻辑。',                 nodeIds: [] },
    { id: 'layer:tests',         name: '测试文件',  description: 'Tests/ 目录下的自动化测试代码。',                                          nodeIds: [] },
    { id: 'layer:other',         name: '其他文件',  description: '配置、脚本及其他支撑文件。',                                               nodeIds: [] },
  ];
  const [cfg, pub, priv, tst, oth] = layers;
  for (const n of nodes) {
    const fp = (n.filePath || '').replace(/\\/g, '/');
    if (fp.match(/\.(Build\.cs|uplugin|uproject)$/)) {
      cfg.nodeIds.push(n.id);
    } else if (fp.includes('/Public/') || fp.includes('/Classes/')) {
      pub.nodeIds.push(n.id);
    } else if (fp.includes('/Private/')) {
      priv.nodeIds.push(n.id);
    } else if (fp.includes('/Tests/') || fp.includes('/Test/') || fp.includes('/Specs/')) {
      tst.nodeIds.push(n.id);
    } else {
      oth.nodeIds.push(n.id);
    }
  }
  return layers.filter(l => l.nodeIds.length > 0);
}

function buildTour(moduleKeyStr, nodes) {
  const tour = [];
  let order = 1;

  // Step 1: Build.cs / .uplugin
  const cfgNodes = nodes.filter(n => (n.filePath || '').match(/\.(Build\.cs|uplugin)$/));
  if (cfgNodes.length) {
    tour.push({
      order: order++, title: '模块配置',
      description: '通过构建脚本与插件描述文件了解模块的依赖关系和编译参数。',
      nodeIds: cfgNodes.slice(0, 3).map(n => n.id),
    });
  }

  // Step 2: Public headers
  const pubHeaders = nodes.filter(n => {
    const fp = (n.filePath || '').replace(/\\/g, '/');
    return (fp.includes('/Public/') || fp.includes('/Classes/')) && /\.(h|hpp)$/.test(fp);
  });
  if (pubHeaders.length) {
    tour.push({
      order: order++, title: '公共接口（Public API）',
      description: '对外暴露的核心头文件，涵盖主要类型声明与接口定义。',
      nodeIds: pickByComplexity(pubHeaders, 8),
    });
  }

  // Step 3: Complex .cpp implementations
  const implNodes = nodes.filter(n => /\.cpp$/.test(n.filePath || ''));
  if (implNodes.length) {
    tour.push({
      order: order++, title: '核心实现',
      description: '模块中最关键的 .cpp 实现文件，包含核心算法与业务逻辑。',
      nodeIds: pickByComplexity(implNodes, 8),
    });
  }

  // Fallback
  if (tour.length === 0 && nodes.length > 0) {
    tour.push({
      order: 1, title: '模块文件',
      description: '模块中的主要文件。',
      nodeIds: pickByComplexity(nodes, 6),
    });
  }
  return tour;
}

function buildDescription(key, count) {
  const parts = key.split('-');
  if (key.startsWith('Source-')) {
    const CAT = { Runtime: '运行时', Editor: '编辑器', Developer: '开发者工具', Programs: '独立程序' };
    const cat = CAT[parts[1]] || parts[1];
    const mod = parts.slice(2).join('-');
    return `Unreal Engine 5.4 ${cat}模块 ${mod}，共 ${count} 个文件。`;
  }
  if (key.startsWith('Plugin-')) {
    const name = parts.slice(1).join(' / ');
    return `Unreal Engine 5.4 插件 ${name}，共 ${count} 个文件。`;
  }
  if (key.startsWith('Platforms-')) {
    return `Unreal Engine 5.4 平台扩展 ${parts.slice(1).join('-')}，共 ${count} 个文件。`;
  }
  return `Unreal Engine 5.4 模块 ${key}，共 ${count} 个文件。`;
}

// ── 6. 生成并写出每个模块 ─────────────────────────────────────────────────────
if (!fs.existsSync(OUT_BASE)) fs.mkdirSync(OUT_BASE, { recursive: true });

let processed = 0;
const total = groupedNodes.size;
const skipped = [];

for (const [key, nodes] of groupedNodes) {
  if (nodes.length === 0) { skipped.push(key); continue; }

  // Internal edges only (source ∈ module)
  const nodeIdSet = new Set(nodes.map(n => n.id));
  const internalEdges = [];
  for (const node of nodes) {
    for (const edge of (edgesBySource.get(node.id) || [])) {
      if (nodeIdSet.has(edge.target)) internalEdges.push(edge);
    }
  }

  const graph = {
    version: '1.0.0',
    project: {
      name: key.replace(/^(Source-|Plugin-|Platforms-)/, '').replace(/-/g, ' '),
      languages: nodeLanguages(nodes),
      frameworks: ['Unreal Engine 5'],
      description: buildDescription(key, nodes.length),
      analyzedAt: new Date().toISOString(),
      gitCommitHash: 'release-5.4.4',
    },
    nodes,
    edges: internalEdges,
    layers: buildLayers(nodes),
    tour: buildTour(key, nodes),
  };

  const dir = path.join(OUT_BASE, key);
  fs.mkdirSync(dir, { recursive: true });
  fs.writeFileSync(path.join(dir, 'knowledge-graph.json'), JSON.stringify(graph), 'utf8');

  processed++;
  if (processed % 25 === 0 || processed === total) {
    process.stdout.write(`\r  [${processed}/${total}] processed…`);
  }
}

process.stdout.write('\n');
console.log(`\n✓ Generated ${processed} module knowledge graphs → ${OUT_BASE}`);
if (skipped.length) console.log(`  Skipped (empty): ${skipped.join(', ')}`);

// ── 7. 写出模块索引 ───────────────────────────────────────────────────────────
const index = [];
for (const [key, nodes] of groupedNodes) {
  if (!nodes.length) continue;
  const parts = key.split('-');
  let category = 'other';
  if (key.startsWith('Source-Runtime-'))   category = 'source-runtime';
  else if (key.startsWith('Source-Editor-')) category = 'source-editor';
  else if (key.startsWith('Source-Developer-')) category = 'source-developer';
  else if (key.startsWith('Source-Programs-')) category = 'source-programs';
  else if (key.startsWith('Plugin-'))      category = 'plugin';
  else if (key.startsWith('Platforms-'))   category = 'platforms';
  index.push({
    key,
    name: key.replace(/^(Source-|Plugin-|Platforms-)/, '').replace(/-/g, ' '),
    category,
    nodeCount: nodes.length,
    path: `Modules/${key}/knowledge-graph.json`,
  });
}
index.sort((a, b) => a.category.localeCompare(b.category) || a.name.localeCompare(b.name));

fs.writeFileSync(
  path.join(ROOT, 'ThroughtWork', 'Understand', 'modules-index.json'),
  JSON.stringify({ total: index.length, modules: index }, null, 2),
  'utf8'
);
console.log(`✓ modules-index.json written (${index.length} entries)`);
