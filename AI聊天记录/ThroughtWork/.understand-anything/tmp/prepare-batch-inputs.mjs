#!/usr/bin/env node
// Helper: emit per-batch input files for file-analyzer subagents.
// Usage: node prepare-batch-inputs.mjs <startBatch> <endBatch>
// Writes:
//   .understand-anything/tmp/ua-file-analyzer-input-<N>.json (for extract-structure.mjs)
//   .understand-anything/tmp/ua-neighbormap-<N>.json         (cross-batch neighbors for LLM)
import fs from 'node:fs';
import path from 'node:path';

const projectRoot = process.cwd();
const batchesPath = path.join(projectRoot, '.understand-anything', 'intermediate', 'batches.json');
const tmpDir = path.join(projectRoot, '.understand-anything', 'tmp');
fs.mkdirSync(tmpDir, { recursive: true });

const start = parseInt(process.argv[2] || '1', 10);
const end = parseInt(process.argv[3] || '50', 10);

console.log(`Reading ${batchesPath}...`);
const batches = JSON.parse(fs.readFileSync(batchesPath, 'utf8'));
console.log(`Loaded ${batches.totalBatches} batches.`);

const all = batches.batches;
let written = 0;
for (let i = start; i <= end && i <= all.length; i++) {
  const b = all[i - 1];
  if (!b || b.batchIndex !== i) {
    console.warn(`Batch ${i}: index mismatch (got ${b ? b.batchIndex : 'undefined'})`);
  }
  const input = {
    projectRoot,
    batchFiles: b.files,
    batchImportData: b.batchImportData || {},
  };
  fs.writeFileSync(
    path.join(tmpDir, `ua-file-analyzer-input-${i}.json`),
    JSON.stringify(input, null, 2),
    'utf8',
  );
  fs.writeFileSync(
    path.join(tmpDir, `ua-neighbormap-${i}.json`),
    JSON.stringify(b.neighborMap || {}, null, 2),
    'utf8',
  );
  written++;
}
console.log(`Wrote ${written} batch input pairs (${start}..${Math.min(end, all.length)}).`);
