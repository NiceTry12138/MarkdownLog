<!--
 * @Version: 
 * @Autor: LC
 * @Date: 2022-01-20 10:45:55
 * @LastEditors: LC
 * @LastEditTime: 2022-01-20 21:27:32
 * @Description: file content
-->
# JavaScipt语法

迷惑的知识点：
1. 作用域
    - 作用域的理解
    - 作用域提升
    - 块级作用域
    - 作用域链
    - AO、GO、VO等概念

2. 函数、闭包  
    - 闭包的访问规则
    - 闭包的内存泄露
    - 函数中的this的指向

3. 面向对象
    - Javascript面向对象
    - 继承
    - 原型
    - 原型链等

4. ES的新特性
    - ES6、7、8、9、10、11、12

5. 其他知识
    - 事件循环
    - 微任务
    - 宏任务
    - 内存管理
    - Promise
    - await
    - async
    - 防抖
    - 节流等等

## 浏览器的工作原理和V8引擎

![浏览器工作原理](./Image/1.png)

> 从图中可知 js、css文件并不是跟index.html文件一起下载下来的，而是需要时才会下载

**浏览器内核**来解析下载下来的文件

| 内存    | 使用                                                                 |
| ------- | -------------------------------------------------------------------- |
| Gecko   | 早期被Netscape和Mozilla Firefox浏览器使用                            |
| Trident | 微软开发，被IE4~IE11浏览器使用，但Edge转向使用Blink                  |
| Webkit  | 苹果给予KHTML开发、开源的，用于Sfari，Google Chrome之前也在用        |
| Blink   | Webkit的一个分支，Google开发，目前应用于Google Chrome、Edge、Opera等 |
| 。。。  | 。。。                                                               |

**浏览器内核**常指浏览器的**排版引擎**  

> 排版引擎（layout engine），也成为浏览器引擎（browser engine）、页面渲染引擎（rendering engine）或样板引擎

![浏览器工作流程](./Image/2.png)

- 浏览器工作流程
  1. 通过`HTML Parser`把HTML文件解析成`DOM Tree`
  2. HTML解析的时候遇到JavaScript标签时，停止解析HTML转而去加载和执行Javascript代码
  3. Javascript代码可以对`Dom`进行操作从而修改`Domt Tree`，这执行Javascript代码就是**js引擎**
  4. CSS文件通过`CSS Parse`解析成`Style Rules`css规则
  5. 将`Style Rules`和`Dom Tree`结合(`Attachment`)到一起，生成渲染树(`Render Tree`)
  6. 绘制(`Painting`)到界面上显示(`Display`)出来

> JavaScript代码通过**JavaScript引擎**来执行

### 认识JavaScript引擎，V8引擎的原理

| 引擎           | 使用                                                               |
| -------------- | ------------------------------------------------------------------ |
| SpiderMonkey   | 第一款JavaScript引擎，有Brendan Eich开发                           |
| Chakra         | 微软开发，用于IE浏览器                                             |
| JavascriptCore | WebKit中的JavaScript引擎，由Apple公司开发                          |
| V8             | Google开发的强大JavaScript引擎，也帮助Chrome从众多浏览器中脱颖而出 |
| 。。。         | 。。。                                                             |

1. V8引擎是用C++编写的Goole开源高性能JavaScript和WebAssembly引擎，他用于Chrome和Node.js等
2. 它实现`ECMAScript`和`WebAssembly`，并在Windows7或更高版本，MacOS 10.12+和使用x64、IA-32、ARM或MIPS处理器的linux系统上运行
3. V8可以独立运行，也可以嵌入到任何C++应用程序中 

![Js引擎的处理](./Image/3.png)

[抽象语法树在线生成网站](https://astexplorer.net/)

> `Parse`解析Javascript源代码（包括词法分析和语法分析）成抽象语法树(AST)  
> AST可以通过V8引擎中的`Ignition`库转换成字节码(bytecode)，不直接转换成机器码是为了根据运行环境做代码优化和环境适配等  

------

V8引擎本身的源码**非常复杂**，大概有超过100w行C++代码，通过了解它的架构，我们可以知道他是如何对Javascript执行的

- `Parse`模块会将Javascriptdiamagnetic转换成AST（抽象语法树），这是因为解释器并不直接认识JavaScript代码
  - 如果函数没有被调用，那么是不会转换成AST的
  - Parse的V8官方文档[https://v8.dev/blog/scanner](https://v8.dev/blog/scanner)

- `Ignition`是一个解释器，会将AST转换成ByteCode字节码
  - 同时会收集`TurboFan`优化所需要的信息（比如函数参数的类型信息，有了类型才能真实运算）
  - 如果函数只调用一次，`Ignition`会执行解释执行`ByteCode`
  - `Ignition`的V8官方文档[https://v8.dev/blog/ignition-interpreter](https://v8.dev/blog/ignition-interpreter)

- `TurboFan`是一个编译器，可以将字节码编译为CPU可以直接执行的机器码
  - 如果一个函数被多次调用，那么就会被标记为**热点函数**，那么会经过**TurboFan转换成优化的机器码，提高代码的执行性能**（直接执行函数机器码，比将字节码转换成机器码再执行更高效）
  - 但是，**机器码实际上也会被还原为ByteCode**，这是因为如果后续执行函数的过程中，**类型发生了变化（因为JS是弱类型语言，传入参数可以为number也可以为string）**，之前优化的机器码并不能正确的处理运算，就会逆向的转换成字节码
  - TurboFan的V8官方文档[https://v8.dev/blog/turbofan-jit](https://v8.dev/blog/turbofan-jit)

![V8引擎的解析图](./Image/4.png)

1. `Blink`是Chrome内核，在解析到JavaScript时将JS代码通过流(Stream)的方式传到V8引擎
2. 引擎中会将编码进行转换，再转换成`Scanner`（扫描器，做词法分析）
3. `Scanner`将代码转换成许多`Tokens`，再通过`Parse`解析转化成`AST`
    - `Parser`就是直接将`Tokens`转成AST树结构
    - `PreParser`称之为与解析
      - 并不是所有的JavaScript代码在一开始的时候就会被执行，所以一开始对所有JS代码进行解析会影响效率
      - V8引擎实现了**Lazy Parsing(延迟解析)**的方案，他的作用是**将不必要的函数进行预解析**，也就是只解析暂时需要的内容，而对**函数的全量解析**是在**函数被调用**才会进行
      - 比如在函数`Outer()`内部定义了`Inner()`函数，那么`Inner()`函数就会进行预解析

在`Parse`阶段时，V8引擎会自动创建`GlobalObject`对象，比如**Stirng、Date、Number、SetTimeout()、window...**等都是`GlobalObject`的成员属性，所以在JS代码中可以字节调用这些对象  

### 简易编译流程

```javascript
// 测试代码
var name = "why";
console.log(num1);
var num1 = 10;
var num2 = 20;
var result = num1 + num2;

foo(1)

function foo(num){
    var m = 10;
    var n = 20;
    console.log('foo');
}
```

1. 解析代码，v8引擎内部会创建一个`GlobalObject`对象

```javascript
var GlobalObject = {
    String : "类",
    Date : "类型",
    setTimeOut : "函数",
    name : undefined,
    num1 : undefined,
    num2 : undefined,
    result : undefined
    foo : 0xa00
}
```

> String、Date、setTimeOut是`GlobalObject`自带的对象  
> name、num1、num2、result是解析出来的变量并添加到`GlobalObject`中  
> 因为此时只是解析阶段，所以name、num1、num2、result都是**undefined**的  
> foo存储的函数地址

2. 运行代码

- 为了执行代码，v8引擎内部会有一个执行上下文栈（函数调用栈）(Execution Context Stack)  
- 因为上述例子是全局代码，v8提供全局执行上下文(Global Execution Contenxt)  
- 这些上下文中存在一个`VO(variable object)（变量对象）`，其中`VO`分为`GO`和`AO`两种，全局上下文中`VO = GO`，函数执行上下文中`VO = AO`  

- 代码从上下往下依次执行，从`VO`中取出目标对象并为其赋值  

![V8引擎的解析图](./Image/5.png)

> foo存储函数空间中存在一个父级作用域(parent scope)，可以获得父级作用域中的数据，当foo作用域中的使用的数据在foo作用域中没找到就往父级作用域(parent scope)中查找，如果一直没有最终会找到全局作用域

-----

```javascript
var message = "Hello Global";
function foo(){
    console.log(message);
}
function bar(){
    message = "Hello Bar";
    foo();
}

bar();      // 输出 Hello Global
```

1. 解析代码，v8引擎内部会创建一个`GlobalObject`对象

`foo`的父级作用域(`parent scope`)就是`GlobalObject`
`bar`的父级作用域(`parent scope`)也是`GlobalObject`

2. 执行代码

赋值。。。

![V8引擎的解析图](./Image/6.png)

------

```ts

function foo(){
    m = 100;
}
console.log(m);

function foo1(){
    var a = b = 10;
    /*
    等价于
    var a = 10;
    b = 10;
    */
}

foo1();
console.log(a);
console.log(b);

```

1. 首先，对于`foo`函数来说，如果没有用`var`或者`let`定义变量，则m会被直接定义到全局变量(GO)中，所以对于`console.log(m)`会输出100而不是报错
2. 对于`foo1`函数来说，`var a = b = 10;`会被理解为`var a = 10; b = 10;`，根据1的解释，b会被定义到全局变量中，而a还在`foo1`的AO中，所以最后`console.log(a)`会报错，而`console.log(b)`会输出10

### 环境变量和记录

每一个执行上下文会关联到一个环境变量(Variable Environment)中，在执行代码中变量和函数的声明会作为环境记录(Environment Record)添加到变量环境中  
对于函数来说，参数也会被作为环境记录添加到变量环境中  
所以对于上面的解释来说，**将不再是VO（变量环境），而是环境记录(VariableEnvironment)**，也就是说不一定是`O(object)`，只要是记录都行(map或者其他可以记录的类型)

## 内存管理

不管什么语言，在代码执行的过程中都需要分配内存，不同的是某些语言需要手动管理内存，某些编程语言可以帮助管理内存  

- 一般而言，内存管理存在如下的生命周期
  1. 分配你申请大小的内存
  2. 使用分配的内存
  3. 不需要使用时，释放内存

- JavaScript会在定义变量时为我们分配内存
  - JS对于基本数据类型内存的分配会在执行时，直接在栈空间进行分配
  - JS对于复杂数据类型内存的分配会在堆内存中开辟空间，并在将这块空间的指针返回值变量引用

因为**内存是有限**的，所以当**内存不再需要的时候**，我们需要**对其进行释放**，以便腾出**更多的内存空间**  
再手动管理内存的语言中，我们需要通过一些方式来释放不再需要的内存，比如free函数：  
    1. 手动管理的方式会**影响编写逻辑的代码的效率**
    2. 对开发者**要求较高**，不小心就会产生**内存泄漏**

1. 引用计数的垃圾回收算法
2. 标记清除的垃圾回收算法（JS使用）
   - 设置一个跟对象(Root Object)，垃圾回收器定期从这个根开始，找所有从根开始有引用到的对象，对于那些没有引用到的对象，就认为是不可用的对象
   - 可以解决引用计数的循环引用问题

JS引用使用标记清除算法，V8引擎为了更好的优化，它在算法的实现细节上也会结合一些其他的算法  

## 作用域、作用域提升、执行上下文内存管理和内存泄露


