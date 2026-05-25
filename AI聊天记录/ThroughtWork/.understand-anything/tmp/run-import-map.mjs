#!/usr/bin/env node
// Build import-map input from scan-result.json, run extract-import-map.mjs,
// then merge the import map back into scan-result.json so compute-batches.mjs
// can pick it up.
//
// Usage: node run-import-map.mjs <SKILL_DIR>
import fs from 'node:fs';
import path from 'node:path';
import { spawnSync } from 'node:child_process';

const projectRoot = process.cwd();
const skillDir = process.argv[2];
if (!skillDir) {
  console.error('Usage: node run-import-map.mjs <SKILL_DIR>');
  process.exit(1);
}

const interDir = path.join(projectRoot, '.understand-anything', 'intermediate');
const scanPath = path.join(interDir, 'scan-result.json');
const inputPath = path.join(interDir, 'import-map-input.json');
const outputPath = path.join(interDir, 'import-map-output.json');

console.log('Loading scan-result.json...');
const scan = JSON.parse(fs.readFileSync(scanPath, 'utf8'));
const codeFiles = scan.files.filter(f => f.fileCategory === 'code')
  .map(f => ({ path: f.path, language: f.language, fileCategory: f.fileCategory }));
console.log(`Preparing import-map input for ${codeFiles.length} code files.`);
fs.writeFileSync(inputPath, JSON.stringify({ projectRoot, files: codeFiles }), 'utf8');

console.log('Running extract-import-map.mjs ... (this may take a while)');
const t0 = Date.now();
const result = spawnSync(process.execPath, [
  path.join(skillDir, 'extract-import-map.mjs'),
  inputPath,
  outputPath,
], { stdio: 'inherit' });
const dt = ((Date.now() - t0) / 1000).toFixed(1);
console.log(`extract-import-map.mjs exited with code ${result.status} after ${dt}s.`);
if (result.status !== 0) process.exit(result.status || 1);

console.log('Merging importMap into scan-result.json...');
const out = JSON.parse(fs.readFileSync(outputPath, 'utf8'));
scan.importMap = out.importMap || {};
fs.writeFileSync(scanPath, JSON.stringify(scan), 'utf8');
console.log(`Done. importMap entries: ${Object.keys(scan.importMap).length}, stats: ${JSON.stringify(out.stats)}`);
