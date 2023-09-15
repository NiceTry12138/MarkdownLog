const program = require("commander")
const {createProjectAction} = require('./actions')

const createCommand = () => {
    // custom-cli create demo
    program
        .command('create <project> [others...]')
        .description(`创建一个自定义项目`)
        .action((project, others) => {
            // console.log(project, others);
            // 将命令与操作拆分到不同的文件中
            createProjectAction(project);
        })
}

module.exports = createCommand