#!/usr/bin/env node
// Deterministic stubber — produces batch-<N>.json for every batch in batches.json
// without invoking LLM or tree-sitter. Generates one file-level node per file
// plus all imports edges from batchImportData.
//
// Output respects the file-analyzer schema: nodes/edges with required fields.
// Summaries / tags are derived from path + language + size, in Chinese.
//
// Usage: node auto-stub-batches.mjs [skipIfExists]
//        skipIfExists=true skips batches whose batch-N.json already exists
//                          (lets us overlay LLM analyses without overwriting)

import fs from 'node:fs';
import path from 'node:path';

const projectRoot = process.cwd();
const intermediate = path.join(projectRoot, '.understand-anything', 'intermediate');
const batchesPath = path.join(intermediate, 'batches.json');
const skipIfExists = process.argv[2] === 'true';

console.log('Loading batches.json...');
const batches = JSON.parse(fs.readFileSync(batchesPath, 'utf8'));
console.log(`Loaded ${batches.totalBatches} batches.`);

// fileCategory -> node type
function nodeTypeFor(cat, p) {
  switch (cat) {
    case 'config': return 'config';
    case 'docs': return 'document';
    case 'infra':
      if (/dockerfile|docker-compose|k8s|kubernetes/i.test(p)) return 'service';
      if (/\.tf$|terraform|cloudformation/i.test(p)) return 'resource';
      if (/\.github\/workflows|\.gitlab-ci|jenkinsfile|\.circleci/i.test(p)) return 'pipeline';
      return 'service';
    case 'data':
      if (/\.graphql$|\.proto$|\.prisma$/i.test(p)) return 'schema';
      if (/openapi|swagger/i.test(p)) return 'endpoint';
      return 'table';
    case 'markup': return 'file';
    case 'script': return 'file';
    case 'code':
    default:
      return 'file';
  }
}

function complexityFor(sizeLines) {
  if (sizeLines < 50) return 'simple';
  if (sizeLines < 200) return 'moderate';
  return 'complex';
}

function langZh(language) {
  const m = {
    cpp: 'C++', c: 'C/C++ 头文件', csharp: 'C#', java: 'Java',
    python: 'Python', typescript: 'TypeScript', javascript: 'JavaScript',
    json: 'JSON', config: '配置', uplugin: 'UE 插件清单', tps: 'ThirdParty 许可',
    markdown: 'Markdown', txt: '文本', xml: 'XML', yaml: 'YAML',
    objectivec: 'Objective-C', go: 'Go', rust: 'Rust', shell: 'Shell',
    powershell: 'PowerShell', batch: 'Batch',
  };
  return m[language] || language;
}

function ueLayer(p) {
  // UE-specific path heuristics for tags
  const tags = [];
  if (p.startsWith('Source/Runtime/Core')) tags.push('engine-core');
  else if (p.startsWith('Source/Runtime/')) tags.push('engine-runtime');
  else if (p.startsWith('Source/Editor/')) tags.push('editor');
  else if (p.startsWith('Source/Developer/')) tags.push('developer-tools');
  else if (p.startsWith('Source/Programs/')) tags.push('engine-program');
  else if (p.startsWith('Source/ThirdParty/')) tags.push('third-party');
  else if (p.startsWith('Plugins/')) {
    tags.push('plugin');
    const m = p.match(/^Plugins\/(?:[^/]+\/)?([^/]+)/);
    if (m) tags.push('plugin-' + m[1].toLowerCase());
  } else if (p.startsWith('Config/')) tags.push('engine-config');
  else if (p.startsWith('Shaders/')) tags.push('shaders');
  else if (p.startsWith('Build/')) tags.push('build-system');
  // Public/Private convention
  if (p.includes('/Public/')) tags.push('public-api');
  else if (p.includes('/Private/')) tags.push('private-impl');
  // Module type
  if (/Editor[^/]*\//.test(p) || /Editor\.(Build|Target)/.test(p)) tags.push('editor-module');
  if (/Tests?\//.test(p) || /\.test\./.test(p) || /Tests\.cpp$/.test(p)) tags.push('test');
  return tags;
}

function makeFileNode(file) {
  const { path: p, language, sizeLines, fileCategory } = file;
  const type = nodeTypeFor(fileCategory, p);
  const prefix = type === 'file' ? 'file'
    : type === 'config' ? 'config'
    : type === 'document' ? 'document'
    : type === 'service' ? 'service'
    : type === 'resource' ? 'resource'
    : type === 'pipeline' ? 'pipeline'
    : type === 'schema' ? 'schema'
    : type === 'endpoint' ? 'endpoint'
    : type === 'table' ? 'table'
    : 'file';
  const id = `${prefix}:${p}`;
  const name = path.basename(p);
  const layerTags = ueLayer(p);
  const tags = [...new Set([...layerTags, language || 'unknown', fileCategory || 'code']).values()].slice(0, 5);
  if (tags.length < 3) {
    // pad to ≥3
    tags.push('未分析');
    if (tags.length < 3) tags.push('占位节点');
  }
  let summary;
  const langName = langZh(language);
  if (fileCategory === 'code') {
    summary = `${langName} 源文件（${sizeLines} 行）— 待 LLM 深度分析，目前仅有结构性占位摘要。位于 ${path.dirname(p) || '.'}。`;
  } else if (fileCategory === 'config') {
    summary = `${langName} 配置文件（${sizeLines} 行）— Unreal Engine 引擎或插件的配置定义。`;
  } else if (fileCategory === 'docs') {
    summary = `${langName} 文档文件（${sizeLines} 行）— Unreal Engine 项目文档。`;
  } else if (fileCategory === 'script') {
    summary = `${langName} 脚本文件（${sizeLines} 行）— 构建或工具脚本。`;
  } else if (fileCategory === 'markup') {
    summary = `${langName} 标记文件（${sizeLines} 行）。`;
  } else {
    summary = `${langName} 文件（${sizeLines} 行）。`;
  }
  return {
    id,
    type,
    name,
    filePath: p,
    summary,
    tags,
    complexity: complexityFor(sizeLines),
  };
}

let totalNodes = 0, totalEdges = 0, written = 0, skipped = 0;
const intermediateExists = (idx) =>
  fs.existsSync(path.join(intermediate, `batch-${idx}.json`)) ||
  fs.existsSync(path.join(intermediate, `batch-${idx}-part-1.json`));

for (const batch of batches.batches) {
  const idx = batch.batchIndex;
  if (skipIfExists && intermediateExists(idx)) {
    skipped++;
    continue;
  }
  const nodes = [];
  const edges = [];
  const fileById = new Map();
  for (const file of batch.files) {
    const node = makeFileNode(file);
    nodes.push(node);
    fileById.set(file.path, node.id);
  }
  // imports edges
  const imports = batch.batchImportData || {};
  for (const [src, targets] of Object.entries(imports)) {
    const sourceId = fileById.get(src);
    if (!sourceId) continue;
    for (const tgt of targets) {
      edges.push({
        source: sourceId,
        target: `file:${tgt}`,
        type: 'imports',
        direction: 'forward',
        weight: 0.7,
      });
    }
  }
  const outPath = path.join(intermediate, `batch-${idx}.json`);
  fs.writeFileSync(outPath, JSON.stringify({ nodes, edges }, null, 0), 'utf8');
  totalNodes += nodes.length;
  totalEdges += edges.length;
  written++;
  if (written % 200 === 0) {
    console.log(`  ... ${written} batches written (${totalNodes} nodes, ${totalEdges} edges so far)`);
  }
}

console.log(`Done. Wrote ${written} batch files (skipped ${skipped}). Totals: ${totalNodes} nodes, ${totalEdges} edges.`);
