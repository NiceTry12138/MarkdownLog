#!/usr/bin/env node
// Patch every batch-N.json by injecting imports edges from scan-result.importMap.
// - Reads scan-result.json -> importMap: {filePath -> [resolvedTargets...]}
// - For each batch-N.json (or batch-N-part-K.json), finds file/config/document/etc
//   nodes whose filePath is in importMap and appends `imports` edges.
// - Skips edges already present (dedupe by source+target+type).
// - Preserves all existing nodes and edges (so user-edited or LLM-overlaid
//   batches keep their content).
//
// Usage: node patch-import-edges.mjs
import fs from 'node:fs';
import path from 'node:path';

const projectRoot = process.cwd();
const interDir = path.join(projectRoot, '.understand-anything', 'intermediate');
const scanPath = path.join(interDir, 'scan-result.json');

console.log('Loading scan-result.json...');
const scan = JSON.parse(fs.readFileSync(scanPath, 'utf8'));
const importMap = scan.importMap || {};
const importMapSize = Object.keys(importMap).length;
console.log(`importMap has ${importMapSize} entries.`);

// List batch files
const files = fs.readdirSync(interDir).filter(f => /^batch-\d+(-part-\d+)?\.json$/.test(f));
console.log(`Found ${files.length} batch files.`);

let totalAdded = 0;
let filesPatched = 0;

for (const fname of files) {
  const fpath = path.join(interDir, fname);
  let data;
  try {
    data = JSON.parse(fs.readFileSync(fpath, 'utf8'));
  } catch (e) {
    console.warn(`Skip ${fname}: invalid JSON (${e.message})`);
    continue;
  }
  if (!data || !Array.isArray(data.nodes) || !Array.isArray(data.edges)) {
    console.warn(`Skip ${fname}: missing nodes/edges`);
    continue;
  }
  // Build node-by-filePath index for this batch
  const fileNodeIdByPath = new Map();
  for (const n of data.nodes) {
    if (n.filePath && (n.type === 'file' || n.type === 'config' || n.type === 'document'
      || n.type === 'service' || n.type === 'pipeline' || n.type === 'schema'
      || n.type === 'resource' || n.type === 'endpoint' || n.type === 'table')) {
      fileNodeIdByPath.set(n.filePath, n.id);
    }
  }
  // Existing edge set for dedupe
  const edgeKey = (e) => `${e.source}\u0001${e.target}\u0001${e.type}`;
  const existing = new Set(data.edges.map(edgeKey));
  let added = 0;
  for (const [filePath, sourceId] of fileNodeIdByPath) {
    const targets = importMap[filePath];
    if (!Array.isArray(targets) || targets.length === 0) continue;
    for (const t of targets) {
      const edge = { source: sourceId, target: `file:${t}`, type: 'imports', direction: 'forward', weight: 0.7 };
      const k = edgeKey(edge);
      if (existing.has(k)) continue;
      existing.add(k);
      data.edges.push(edge);
      added++;
    }
  }
  if (added > 0) {
    fs.writeFileSync(fpath, JSON.stringify(data), 'utf8');
    totalAdded += added;
    filesPatched++;
  }
}

console.log(`Patched ${filesPatched} batch files. Added ${totalAdded} imports edges total.`);
