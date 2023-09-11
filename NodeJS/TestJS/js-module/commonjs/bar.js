// 一个文件就是一个模块

const name = "bar.js"

const age = 10

const obj = {
    name: "bar",
    age: 10
}

let message = "my name is bar.js"

function barFunc(name) {
    console.log("hello " + name);
}

exports.name = name
exports.age = age
exports.obj = obj

setInterval(() => {
    console.log(obj.name)
    console.log(age)
}, 1000);

console.log(module)