#!/usr/bin/env node
// 用法: node query.mjs <命令> [参数]
//   node query.mjs layers              -- 列出所有层及节点数
//   node query.mjs layer engine-core   -- 列出某层的所有文件
//   node query.mjs search Renderer     -- 按路径关键词搜索节点
//   node query.mjs deps Source/Runtime/Core/Private/Math/UnrealMath.cpp  -- 查依赖
//   node query.mjs tour                -- 显示导览

import fs from 'node:fs';
const g = JSON.parse(fs.readFileSync(new URL('./knowledge-graph.json', import.meta.url)));

const [,, cmd, ...args] = process.argv;

if (cmd === 'layers') {
  g.layers.forEach(l => {
    const name = l.id.replace('layer:', '');
    console.log(`  ${name.padEnd(30)} ${l.nodeIds.length} 个节点`);
    if (l.description) console.log(`    ${l.description}`);
  });
} else if (cmd === 'layer') {
  const layerId = `layer:${args[0]}`;
  const layer = g.layers.find(l => l.id === layerId);
  if (!layer) { console.error('未找到层:', args[0]); process.exit(1); }
  const nodeMap = new Map(g.nodes.map(n => [n.id, n]));
  console.log(`层: ${layer.id}  (${layer.nodeIds.length} 个节点)`);
  if (layer.description) console.log(`描述: ${layer.description}\n`);
  layer.nodeIds.forEach(id => {
    const n = nodeMap.get(id);
    if (n) console.log(`  [${n.complexity}] ${n.filePath}`);
  });
} else if (cmd === 'search') {
  const kw = args[0]?.toLowerCase();
  if (!kw) { console.error('缺少关键词'); process.exit(1); }
  const found = g.nodes.filter(n => n.filePath?.toLowerCase().includes(kw) || n.name?.toLowerCase().includes(kw));
  console.log(`找到 ${found.length} 个节点 (关键词: ${kw}):`);
  found.slice(0, 50).forEach(n => console.log(`  [${n.complexity}] ${n.filePath}`));
  if (found.length > 50) console.log(`  ... 还有 ${found.length - 50} 个`);
} else if (cmd === 'deps') {
  const path = args[0];
  const node = g.nodes.find(n => n.filePath === path || n.id === `file:${path}`);
  if (!node) { console.error('未找到节点:', path); process.exit(1); }
  console.log(`节点: ${node.filePath}`);
  console.log(`  tags: ${node.tags?.join(', ')}, complexity: ${node.complexity}`);
  const out = g.edges.filter(e => e.source === node.id);
  const inc = g.edges.filter(e => e.target === node.id);
  console.log(`\n依赖的文件 (${out.length}):`);
  out.slice(0, 20).forEach(e => console.log('  → ' + e.target.replace('file:', '')));
  console.log(`\n被引用的文件 (${inc.length}):`);
  inc.slice(0, 20).forEach(e => console.log('  ← ' + e.source.replace('file:', '')));
} else if (cmd === 'tour') {
  const nodeMap = new Map(g.nodes.map(n => [n.id, n]));
  g.tour.forEach(step => {
    console.log(`\n=== Step ${step.order}: ${step.title} ===`);
    console.log(step.description);
    console.log('关键节点:');
    step.nodeIds.forEach(id => {
      const n = nodeMap.get(id);
      if (n) console.log(`  → ${n.filePath}`);
    });
  });
} else {
  console.log(`用法: node query.mjs <命令> [参数]
  layers              列出所有架构层
  layer <层名>        列出某层的所有文件 (如: layer engine-core)
  search <关键词>     按路径搜索节点 (如: search Niagara)
  deps <文件路径>     查看文件的依赖关系
  tour                显示导览步骤`);
}
