#!/usr/bin/env node

const program = require("commander")
const helpOptions = require('./src/core/help')
const createCommand = require('./src/core/create')

program.version(require('./package.json').version)  // 从 package.json 中获取 version 信息

helpOptions()
createCommand()

program.parse(process.argv)