const path = require('path');

const bastPath = 'Temp';
const rootPath = "/User/root/Temp"
const filename = 'test.txt'

// resolve 用于路径拼接
const fileFullPath = path.resolve(bastPath, filename)
const fileFullPath2 = path.resolve(rootPath, filename)

console.log(fileFullPath);  // 以 当前运行文件夹为相对路径
console.log(fileFullPath2); // 以 根目录 为绝对路径

// 获取路径信息
const TempFilePath = "/User/root/config.txt"
const TempFileDir = "/User/root/config"

console.log(path.dirname(TempFilePath))     // /User/root
console.log(path.dirname(TempFileDir))      // /User/root

console.log(path.basename(TempFilePath))    // 文件名 config.txt

console.log(path.extname(TempFilePath))     // 文件后缀 .txt

// join 路径拼接
const TempBasePath = '/User/root'
const TempFileName = 'acb.txt'

console.log(path.join(TempBasePath, TempFileName))

// resolve 路径拼接 对比 join resolve 会多判断判断路径字符串开头是否有 "/" "./" "../"
console.log(path.resolve(TempBasePath, TempFileName))

