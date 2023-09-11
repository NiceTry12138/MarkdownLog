# NodeJS 的学习

![](./Image/001.png)

## NodeJs介绍

什么是NodeJS?

官方定义是：`NodeJs Is a JavaScript Runtime Built On Chrome's v8 JavaScript Engine`  
NodeJs 是一个基于 V8 Javascript 引擎的 Javascript 运行时环境  

![](Image/002.png)

上图是一个渲染引擎的工作流程

1. 首先将 HTML 通过 HTMLParser 解析生成 Dom 树
2. 将 StyleSheets 通过 CSSParser 解析成 StyleRules
3. 将 StyleRules 附加到 Dom 树上，生成 RenderTree，也就是渲染树
4. RenderTree 和 Layout(布局) 结合之后就可以进行绘制

> 前端渲染引擎是浏览器中负责将 HTML、CSS 和 JavaScript 转换为可视化页面的核心组件

Javascript 可以操作上面的 Dom 树，所以为了执行 Javascript 代码的逻辑，需要一个解释器来解释 JavaScript 代码并将其翻译成机器语言执行

NodeJS 中运行 JavaScript 代码是通过 V8 引擎的

- V8 是用 C++ 编写的 Google 开源高性能 JavaScript 和 WebAssembly 引擎
- 它实现 ECMAScript 和 WebAssembly，并且实现Windows、MacOS和Linux的跨平台
- V8 可以独立运行，也可以嵌入到任何 C++ 应用程序中

![](Image/003.png)

> V8 的运行流程 

NodeJS 不仅仅只有 V8 引擎，V8 只是帮助 JS 代码执行，除此之外 NodeJS 还需要处理文件系统读/写、网络IO、加密、压缩解压缩等操作

![](Image/004.png)

> NodeJS 和 浏览器 的差异

![](Image/005.png)

> NodeJS 的架构图

- 上图左上角的 APPLICATION 表示用户所写的 Javascript 代码
- 用户写的 JS 代码交给 V8 来执行
- V8 执行的代码的时候会通过 `NODE.JS BINDINGS` 也就是 NodeJS 的 API 来连接 LIBUV
- LibUV 中包括事件循环、文件系统、网络等核心模块

> LibUV 是用 C语言 编写的库

**安装 NodeJS**

安装完毕之后，可以测试使用 NodeJS 来运行 JS 代码

直接在命令行中输入 node + 要运行的JS文件即可

![](Image/006.png)

## Node的REPL

什么是REPL？

REPL 是 `Read-Event-Print Loop` 的简称，即 **读取-求值-输出的循环**
REPL 是一个简单的、交互式的编程环境

比如直接在命令行输入 python，就是进入 python 的 REPL 交互环境

node 与 python 类似，在命令行中直接输入 node，就是进入 Node 的 REPL 的交互环境

![](Image/007.png)

> process 翻译过来就是进程，在 node 中是一个全局变量，存储了很多有用信息

## Node 程序传递参数

按照之前所讲，Node 运行 JS 文件只需要通过 `node 文件名.js` 即可执行对应文件

那么如果想要给执行的js文件传递参数又该如何处理？

`console.log(process)` 在 node 执行的 js 文件中输出 process 的内容

然后使用 `node 文件名.js t1 t2 t3` 去执行指定的 js 文件

> 这里 t1、t2、t3 就是模拟参数输入

在 process 中可以找到 argv 属性，它的值是一个数组

```json
{
    argv: [
        'E:\\nodejs\\node.exe',
        'F:\\KS-TS\\MarkdownLog\\NodeJS\\TestJS\\Test_02.js',
        't1',
        't2',
        't3'
    ],
    execArgv: []
}
```
 
- `'E:\\nodejs\\node.exe'` 表示 node 可执行程序所在路径
- `'F:\\KS-TS\\MarkdownLog\\NodeJS\\TestJS\\Test_02.js'` 表示执行的js文件所在路径
- 剩下的就是命令行传入的参数了

那么为了输出所有的参数，使用数组的遍历即可

```javascript
process.argv.forEach((val, index) => {
    console.log(`index = ${index} value = ${val}`)
})
```

![](Image/008.png)

- argc：argument counter的缩写，传递参数的个数
- argv：argument vector的缩写，传入的具体参数

> vector翻译过来是矢量的意思，在程序中表示的是一种数据结构

## Node 的输出

`console.log` 最常用的输入内容的方式

`console.clear` 清除输出

`console.trace` 跟踪，输出**调用栈**

[console的一些调用API](https://nodejs.org/dist/latest-v18.x/docs/api/console.html)

遇到问题直接查找 Node 的 API 文档，更加准全

## Node 常见的全局对象

全局对象可以在程序的任何位置都可以访问到

[官方文档中说明的Global对象](https://nodejs.org/dist/latest-v18.x/docs/api/globals.html)

> 并不是所有的全局对象都会被用到

模块化相关的 `exports`、`module`、`require()` 经常用到

web服务器会用到 `URL`

有一些**特殊的全局对象**是每个模块都私有一份的，而不是整个程序通用的，比如：`__dirname`、`__filename`、`exports`、`module`、`require()`

- `__dirname`：目录名称
- `__filename`：文件名称

![](Image/009.png)

> 但是 __dirname 和 __filename 等特殊全局对象在 REPL 环境下是无效值

![](Image/010.png)

还有一些比较**常用的全局对象**

- `process`：提供了 node 的进程相关的信息
  - 比如 node 的运行环境、参数信息等
  - 还可以一些环境变量读取到 process 的 env 中
- `console`：提供了简单的调试控制台
- 定时器函数： `setTimeout`、`setInterval`、`setImmediate`
  - `setImmediate`：它的作用是将回调函数放入事件循环的检查阶段，以便在当前一轮事件循环的末尾立即执行
  - 除此之外，`process.nextTick` 也可以在下一帧执行某个操作
- `global`对象

![](Image/011.png)

> 在 REPL 环境中，输入 `gloabl.` 然后双击 tab 即可得到 global 所有的属性

通过上面 global 的属性图可以看到，global中封装了很多常用属性，包括 `Date`、`Array`、`Set`、`Object`、`URL`、`v8` 等常用数据对象

为什么要将很多数据对象放在 `global` 中？因为方便获取，参考浏览器中的 window 对象，方便使用者调用

> global.process 就是全局对象 process `global.process === process`

![](Image/012.png)

参考 `node` 的源码，就是直接将 `process` 设置为 `global` 的属性

> global === globalThis，两个完全一样

## JavaScript 的模块化

基于node进行开发时，绝大多数情况都是编写 JS 代码

什么是模块化？

- 事实上模块化开发的最终目的是将程序分成**一个个小的结构**
- 不同结构编写属于**自己的逻辑代码**，并且有自己的作用域，不会影响到其他结构
- 每个结构可以将自己希望暴露的**变量**、**函数**、**对象**等导出给其他结构使用
- 每个结构可以通过某种方式，导入其他结构的**变量**、**函数**、**对象**等 

> 上面的**结构**就是**模块**；按照这种结构划分的过程，就是**模块化**开发的过程

`node` 中使用的模块规范是 `CommonJS`

`CommonJS`是一个**规范**，最初提出来是在浏览器以外的地方使用，并且当时被命名为`ServerJS`，后来为了体现它的广泛性，修改为`CommonJS`，简称为`CJS`

- `Node`是`CommonJS`在服务器端一个具有代表性的实现
- `Browserify`是`CommonJS`在浏览器中的一种实现
- `webpack`打包工具具备对`CommonJS`的支持和转换

`Node`中对 `CommonJS` 进行了支持和实现，帮助可以方便的进行模块化开发
- 在`Node`中每一个 JS 文件都是一个**单独的模块**
- 单独的模块中包括 `CommonJS` 规范的核心变量：`exports`、`module.exports`、`require`，可以使用这些变量进行模块化开发

`exports` 和 `module.exports` 负责导出，但是两者是不一样的

`require`函数可以帮助导入其他模块(自定义模块、系统模块、第三方模块)

### 测试案例1

为了证明一个JS文件就是一个模块

新建两个js文件：`bar.js` 和 `main.js`

在 `bar.js` 中定义属性和函数，在`main.js`中直接调用`bar.js`中定义的属性和函数

```js
// bar.js
const name = "bar.js"

const age = 10

let message = "my name is bar.js"

function barFunc(name) {
    console.log("hello " + name);
}
```

```js
// main.js
console.log(name)
console.log(age)
```

最后的结果就是报错，在`main.js`中并不能找到`name`属性

### 测试案例2

将前面 `bar.js` 中定义的属性和函数导出

还记得前面提到的全局对象吗？其中有一个叫 exports 的特殊全局对象，它是每个模块都有一个的对象

exports 是一个对象，那么就可以给对象添加属性，属性就会跟着 exports 对象一起被导出

```js
// bar.js
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
```

`main.js`需要导入对应的`bar.js`的`name`和`age`

```js
// 获得整个对象
const bar = require('./bar')

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
```

通过`require()` 会返回一个对象，这个对象就是 `bar.js` 的 `exports` 对象

参考 `bar.js` 中 `interval` 回调函数的输出，可以发现 obj 的属性被修改了(即使 obj 是 const 的)，因为 obj 是浅拷贝

但是 `bar.js` 中的 `age` 作为 `number`，是值传递，所以 `main.js` 不能对 `bar.js` 中的 `age` 产生修改

通过输出结果的变化，可以论证 `require('./bar')` 函数的返回值就是 `bar.js` 中的 `exports` 对象

> 每个模块的 `exports` 对象默认是一个空对象 `exports = {}`

所以单从 `require` 和 `exports` 来看，就是一个浅拷贝罢了

### 测试案例3

`module.exports` 是什么？

`CommonJS`中是没有 `module.exports` 的概念的，但是为了实现模块的导出，`node`中使用的是 `Module` 的类，每一个模块都是 `Module` 的一个实例，也就是 `module` 

> `let module = new Module()`

所以在 `Node` 中真正用于导出的其实根本不是 `exports`，而是 `module.exports`，因为 `module` 才是导出的真正实现者


```js
console.log(module.exports === exports)// true
console.log(module) // 查看对象的所有属性
```

![](Image/013.png)

> module 对象中有个 exports 属性，exports 属性存储了所有设置的导出对象

node 的逻辑大概是将 exports 赋值给 module 的对应属性中

```js
module.exports = exports
```

本质上是 `module.exprots` 在导出，为了验证这个观点，我们对 `module.exports` 做一些操作

```js
// bar.js
const name = "bar.js"

const age = 10

exports.name = name
exports.age = age

module.exports = {}
```

```js
// main.js
const bar = require('./bar')

console.log(bar.name)   // undefined
console.log(bar.age)    // undefined
```

命名 `bar.js` 的 `exports` 对象中存在 `name` 和 `age` 属性，但是 `main.js` 获得的对象却没有 `name` 和 `age` 属性

由此可见，导出本质上是导出 `module.exports` 而不是 `exports` 对象

那么 `exports` 对象有什么存在的必要呢？

因为 `CommonJS` 的规范要求必须有一个 `exports` 对象作为导出，`nodejs` 为了满足 `CommonJS` 做出了一种妥协

### 测试案例4

```js
// bar.js
exports = 123

```

```js
// main.js
console.log(require(`./bar`)) // 输出 {}
```

根据 main.js 的输出可以得出结论， `module.exports = exports` 赋值是在文件一开始就做了，如果赋值是在文件最后做的话 `main.js` 应该输出 `123` 才对

