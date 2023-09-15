const { promisify } = require('util')
const download = promisify(require('download-git-repo'))
const { commandSpawn } = require('../utils/terminal')

const { vueRepo } = require('../config/repo-config')

const createProjectAction = async (project) => {
    // 1. clone 项目，使用 download-git-repo 下载项目
    // 使用 promisify 将 download 回调转换成 promise, 然后使用 async 和 await编写
    await download(vueRepo, project, {clone: true})

    const command = process.platform === 'win32' ? 'npm.cmd' : 'npm';
    // 2. 执行 npm install
    await commandSpawn(command, ['install'], { cwd: `./${project}`})

    // 3. 运行 npm run server
    await commandSpawn(command, ['run', `serve`], { cwd: `./${project}`})
}

module.exports = {
    createProjectAction
}