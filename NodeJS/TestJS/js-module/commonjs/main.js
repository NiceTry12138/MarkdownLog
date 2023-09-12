// 获得整个对象
const bar = require('./bar')
const foo = require('./foo')

console.log(bar.name)
console.log(bar.age)

// 通过解构获得对象对应的属性
const {name, age} = require('./bar')

console.log(name)
console.log(age)

setTimeout(() => {
    bar.obj.name = "main"
    bar.age = 0;
}, 1500);

console.log(module.exports === exports) // true

console.log(module)

console.log(module.paths)
