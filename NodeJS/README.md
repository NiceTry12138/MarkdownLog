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

### CommonJS

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

#### 测试案例1

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

#### 测试案例2

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

#### 测试案例3

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

#### 测试案例4

```js
// bar.js
exports = 123

```

```js
// main.js
console.log(require(`./bar`)) // 输出 {}
```

根据 main.js 的输出可以得出结论， `module.exports = exports` 赋值是在文件一开始就做了，如果赋值是在文件最后做的话 `main.js` 应该输出 `123` 才对

#### 关于 require 的细节

[官方文档中 require 的查找细节](https://nodejs.org/dist/latest-v18.x/docs/api/modules.html#all-together)

require 是一个**函数**，可以帮助引入一个文件(模块)中导入的对象

一些比较常用的规则(规则很多不全部介绍)，导入格式：`require(X)`

1. 如果 X 是一个核心模块，比如`path`、`http`、`fs`等
   - 直接返回核心模块，并停止查找（优先查找核心模块）
2. 如果 X 是以 `./` 或者 `../` 或 `/`(根目录) 开头的
   - 说明是查找文件或者文件夹
   - 如果是查找文件
     - 如果有后缀名，按照后缀名的格式查找对应的文件
     - 如果没有后缀名，按照如下顺序查找
       - 直接查找文件 X
       - 查找 X.js 文件
       - 查找 X.json 文件
       - 查找 X.node 文件
   - 如果没有找到对应文件，那么将 X 看成是目录
     - 查找目录下的 index 文件
       - 查找 X/index.js 文件
       - 查找 X/index.json 文件
       - 查找 X/index.node 文件
   - 如果都没有找到，那就报错：not found

3. 如果直接是一个 X ，不是路径也不是一个核心模块
   - 优先查找是否是核心模块
   - 然后在运行 `require` 函数的js文件同级目录的 node_modules 中查找
   - 然后在上一层级的 node_modules 文件夹中查找
   - 更上一级的 node_modules 文件夹中查找
   - 直至查找到根目录位置

假设当前运行 `require` 函数的是 `main.js` 文件，其路径是 `/User/codewhy/Desktop/Node/TestCode/04_learn_node/05_javascript-module/02_commonjs/main.js` 那么其 `require` 的查找路径如下

![](Image/014.png)

```js
console.log(module.path)
console.log(module.paths)
```

通过 `module.paths` 即可获得查找路径

#### 模块的加载过程

1. 模块在被第一次引入时，模块中的js代码会被运行一次

```js
// bar.js
console.log(`bar`)
```

```js
// main.js
require("./bar")

console.log("main")
```

其打印顺序是先打印 `bar` 再打印 `main` 

`CommondJS` 的加载是同步的，也就是说等到 `require` 加载的模块执行完毕之后，才会执行后续代码

`CommondJS` 的加载规则用在服务器上不会出现什么问题，因为文件都在本地，同步加载不会影响模块的执行

`CommondJS` 的加载规则如果用在浏览器中会出现大问题，必须等到 `require` 的文件下载完毕才能加载，会严重阻碍当前模块的运行

2. 模块被多次引入时，会缓存，最终只加载(运行)一次

`module` 对象有一个属性叫做 `loaded` ，值为 `false` 表示没有被加载，为 `true` 表示被加载了

```js
// bar.js
console.log(`bar`)
```

```js
// foo.js
require("./bar")

console.log(`foo`)
```

```js
// main.js
require("./bar")
require("./foo")

console.log(module.children)
console.log(module.loaded)
```

最终只会输出一个 `bar`，说明了一个模块只会被加载一次

关注一下 main.js 中的输出

- `module.children` 输出了子模块的 `module` 信息，其中就包括其 `loaded` 的值
- `module.loaded` 表示当前模块的是否被加载完毕，当前模块没有全部执行完毕，所以 `loaded` 的值是 `false`

> module.children 中存储了所有加载的子模块的 module

3. 如果存在循环引用，如何处理？

![](Image/015.png)

Node 采用的是深度优先算法，也就是 main => aaa => ccc => ddd => eee => bbb，按照这个顺序加载文件

```js
// main.js

console.log("main")
require("./foo")
console.log("main finish")
```

```js
// foo.js

console.log("foo")
require("./bar")
console.log("foo finish")
```

```js
// bar.js

console.log("bar")
require("./main")
console.log("bar")
```

![](Image/016.png)

#### 对应 node 代码

当前使用的 node 版本为 v16.13.2，不同版本目录可能不同

![](Image/017.png)

模块相关内容在 `modules` 文件夹中，其包含了两种加载规则 `cjs` 和 `esm`

```js
Module.prototype.require = function(id) {
  validateString(id, 'id');
  if (id === '') {
    throw new ERR_INVALID_ARG_VALUE('id', id,
                                    'must be a non-empty string');
  }
  requireDepth++;
  try {
    return Module._load(id, this, /* isMain */ false);
  } finally {
    requireDepth--;
  }
};
```

在 `Module` 的原型上添加了 `require` 函数，其本质就是调用了 `Module._load` 函数

```js
Module._load = function(request, parent, isMain) {
    if(parent) {
        // 如果存在父模块，说明当前模块被加载过，则直接从 Module._cache[filename] 中获取缓存的 module 对象
        return cachedModule.exports;
    }

    // ... to some thing
    return module.exports;
};
```

### ESModule

`ESModule` 使用了 `import` 和 `export` 关键字，采用编译器的**静态分**析，同时也加入了**动态引用**

`ESModule` 中 `export` 负责将模块内的内容导出，`import` 负责从其他模块导入内容

使用 `ESModule` 将自动采用严格模式 `use strick`

[什么是严格模式？](https://developer.mozilla.org/zh-CN/docs/Web/JavaScript/Reference/strict_mode)

#### named export 有名字的导出

常用的导出主要有三种

1. 在想导出的对象前面加上 export 关键字

```js
export const name = "bar";
```

2. 统一导出

```js
const name = "bar";
const sayHello = function(name) {
    console.log("hello");
}

export {
    name,
    sayHello
}
```

需要注意， `export {}` 后面的 `{}` 并不是一个Object对象，而是放置要导出的变量的引用列表

3. 导出时可以给变量起别名

```js
const name = "bar";
const sayHello = function(name) {
    console.log("hello");
}

export {
    name as FName,
    sayHello as FSayHello
}
```

常用的导入方式也有三种

1. 使用 `import` 关键字

```js
import { name, sayHello } from "./bar.js"
```

> 必须指定确定文件后缀，原生ESM不会像 CJS 去搜索文件

2. 起别名

```js
import { name as FName, sayHello as FSayHello } from "./bar.js"
```

3. 通过 `* as ` 

```js
import * as bar from "./bar.js"

console.log(bar.name);
bar.sayHello();
```

> 本质来看就是将 bar.js 中导出的东西放置到 bar 对象中，作为属性进行调用

`export` 和 `import` 还可以结合使用

```js
// foo.js
export { name, sayHello } from "./bar.js"
```

通过上面的写法可以直接在在 `foo.js` 中导出 `bar.js` 中的内容，而不用先写 `import` 导入 `bar.js` 的内容， 再写 `export` 导出刚导入的 `bar.js` 的内容

上面这种写法一般用在自己开发或者封装一个功能库的时候，通常希望将暴露的所有接口放到一个文件中

比如模块中有 `mathUtil.js` 文件里面有三四个工具函数，有 `format.js` 里面有一个工具函数，希望把这些工具函数暴露给其他模块使用，但是其他模块又不知道我自己模块的内部文件名，所以一般在模块中有一个 `index.js` 专门负责导出

#### default export 默认导出

`export` 时不需要使用 `{}` 来指定名称，导入时也不需要使用 `{}`

```js
// bar.js

export default function() {
    console.log("hello world")
}
```

```js
// main.js
import format from 'bar.js'

format();
```

一个模块中，只能有一个默认导出

如上代码所示，直接导出对应函数，在 main 中也直接使用对应，因为只能有一个默认导出，所以导入的时候就知道导出的是什么

#### import 函数

通过 import 加载一个模块时不可以将其放到逻辑代码中，比如

```js
if(falg) {
    import * as bar from './bar.js';
}
else {
    import * as bar from './foo.js';
}
```

上述代码会报错，因为依赖关系是在解析的时候就确定了的，没有等到运行时。解析的时候 `flag` 值并没有确定，所以这个时候会报错

> 之前 `cjs` 的 `require` 是一个函数，是运行阶段时处理的，所以 cjs 可以通过 `if-else` 进行处理

由于 webpack 支持 ESM 和 CJS，所以在 webpack 的环境下可以直接使用 `require` 来进行条件判断式的模块导入

如果在纯 ESM 环境下运行，可以使用 `import(模块名)` 来条件判断式的加载模块

```js
if(flag) {
    import('./bar.js').then(res => {
        console.log(res.name)
    }).catch(err => {
        // 错误处理
    })
}
```

注意，此时 `import()` 是一个函数，只有函数才能在运行时执行。使用 `import()` 函数本质上返回的就是一个 `Promise` 

#### ESModule 加载过程

ESModule 加载 JS 文件的过程是编译(解析)时加载的，并且是异步的

- 编译(解析)时加载，意味着 `import` 不能和运行时相关的内容放在一起使用
  - 比如 import from 后的路径不能动态设置
  - 比如 import 不能放在 if 语句中判断执行

```js
// bar.js

let name = "bar"

setTimeout(() => {
    name = "aaa"
}, 1000);

export {
    name
}
```

```js
// main.js
import { name } from './bar.js'

setTimeout(() => {
    console.log(name)   // 输出 aaa
}, 2000)
```

如果与 CJS 一样 `bar.js` 导出的是一个对象，那么 `main.js` 应该输出 `bar`，但是这里输出的是 `aaa`，说明 ESM 导出的是**变量的引用**

根据 ESM 的解释，创建了一块内存空间，名为模块环境记录(module environment record) 用于绑定(bindings)导出数据，并且是实时绑定。这一系列操作都是在 JS引擎 解析的时候进行处理的

```js
// main.js
import { name } from './bar.js'

setTimeout(() => {
    name = "bbb"
}, 1000)
```

上面修改其他模块变量的操作，会直接报错。因为 import 的变量是 const 的，所以不能修改

但是，众所周知，JS对const对象只封装了一层，也就是说可以通过下面的操作进行值的修改

```js
// bar.js

let obj = {
  name: "bar",
  age: 18
}

setTimeout(() => {
  console.log(obj.name) // 输出 main
}, 1000)

export {
  obj
}
```

```js
// main.js

import { obj } from './bar.js'

console.log(obj.name)

obj.name = "main"
```

## 常见的内置模块解析

