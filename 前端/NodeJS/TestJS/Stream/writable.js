const fs = require(`fs`)

// 普通写文件操作
// fs.writeFile(`./tempfile.txt`, "asdfrgs", {falg:`a`}, (err) => {
// })

//  Stream 写入
const writer = fs.createWriteStream(`./tempfile.txt`, {
    flags : "r+",
    start : 4,
    encoding : "utf8"
})

writer.write('你好啊', (err) => {
    if(err) {
        console.log(err)
        return;
    }
    console.log(`写入成功`)
})

writer.write('你好啊', (err) => {
    if(err) {
        console.log(err)
        return;
    }
    console.log(`写入成功`)
})

writer.on(`close`, () => {
    console.log(`file closed`)
})

// 需要手动关闭文件
// writer.close();
writer.end("最后的内容")