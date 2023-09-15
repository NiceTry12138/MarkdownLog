const program = require('commander')

const helpOptions = () => {
    program.option('-s, --separator <char>');

    program.option('-t, --Test');

    program.on('option:Test', function() {
        console.log("-s 命令触发")
    });
}

module.exports = helpOptions
