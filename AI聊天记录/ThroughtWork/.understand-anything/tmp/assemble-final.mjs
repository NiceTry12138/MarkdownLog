#!/usr/bin/env node
// Assemble the final knowledge-graph.json by combining:
//   - assembled-graph.json (nodes + edges)
//   - layers.json
//   - tour.json
// Writes to .understand-anything/knowledge-graph.json
import fs from 'node:fs';
import path from 'node:path';

const projectRoot = process.cwd();
const ua = path.join(projectRoot, '.understand-anything');
const inter = path.join(ua, 'intermediate');
const scan = JSON.parse(fs.readFileSync(path.join(inter, 'scan-result.json'), 'utf8'));
const g = JSON.parse(fs.readFileSync(path.join(inter, 'assembled-graph.json'), 'utf8'));
const layers = JSON.parse(fs.readFileSync(path.join(inter, 'layers.json'), 'utf8'));
const tour = JSON.parse(fs.readFileSync(path.join(inter, 'tour.json'), 'utf8'));

// Languages present
const langs = [...new Set(scan.files.map(f => f.language).filter(Boolean))]
  .sort((a, b) => a.localeCompare(b));

const out = {
  version: '1.0.0',
  project: {
    name: 'UnrealEngine-5.4',
    languages: langs,
    frameworks: ['Unreal Engine 5'],
    description: 'Unreal Engine 5.4 游戏引擎源代码 — 包含运行时（Source/Runtime）、编辑器（Source/Editor）、开发者工具、独立程序（Source/Programs）以及大量随引擎分发的插件（Plugins/）。',
    analyzedAt: new Date().toISOString(),
    gitCommitHash: 'release-5.4.4',
  },
  nodes: g.nodes,
  edges: g.edges,
  layers,
  tour,
};

const outPath = path.join(inter, 'assembled-graph.json');
fs.writeFileSync(outPath, JSON.stringify(out), 'utf8');
console.log('Wrote final assembled graph: ' + outPath);
console.log('  nodes=' + out.nodes.length + ', edges=' + out.edges.length + ', layers=' + out.layers.length + ', tour=' + out.tour.length);
