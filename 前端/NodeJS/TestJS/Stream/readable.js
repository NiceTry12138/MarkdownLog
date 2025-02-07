const fs = require(`fs`)

// 传统读取
// fs.readFile('./tempfile.txt', {encoding: "utf8"}, (err, data) => {

// })

// 通过流读取

const reader = fs.createReadStream(`./tempfile.txt`, {
    start: 3,               // 从第三个字节开始读取
    end: 6,                 // 读到第六个字节
    highWaterMark: 2,       // 每次读取2个字节
    encoding: `utf8`        // 通过 utf8 字节
})

reader.on(`data`, (data) => {
    console.log(data)

    reader.pause()      // 暂停读取
    setTimeout(() => {
        reader.resume()
    }, 100)         // 0.1 s之后恢复读取
})

reader.on(`close`, () => {
    console.log(`file closed`)
})

reader.on(`open`, () => {
    console.log(`file opened`)
})