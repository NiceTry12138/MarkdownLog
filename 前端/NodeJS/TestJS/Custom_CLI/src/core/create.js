const program = require("commander")
const {createProjectAction, addCpmAction} = require('./actions')

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

    program
        .command(`addCpn <name>`)
        .description(`创建指定类型的文件模板 根据名称进行替换`)
        .action(addCpmAction)
}

module.exports = createCommand