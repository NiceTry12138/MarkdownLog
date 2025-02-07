const fs = require('fs')

const filepath = './tempfil.txt'

const fileContent = '测试内容....'

// 回调写入文件
fs.writeFile(filepath, fileContent, (err) => {
    console.log(err)
});

fs.readFile(filepath, {encoding: 'utf-8'}, (err, data) => {
    if(err) {
        console.log(err)
    }
    else {
        console.log(data)   // 输出 buffer 十六进制
    }
})