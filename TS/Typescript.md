<!--
 * @Version:
 * @Autor: LC
 * @Date: 2022-02-08 13:06:14
 * @LastEditors: LC
 * @LastEditTime: 2022-02-08 18:24:20
 * @Description: Typescript
-->

# TypeScript

- TypeScript
  - 加强版的Javascript，让JS更加安全
  - JS拥有的特性，TS都支持，紧随ECMAScript标准
  - 增加了类型约束，同时扩展一些语法：枚举、元组等
  - TS在实现新特性的同时，保持和ES标准的同步
  - TS最终会被编译成JS代码，不用担心兼容性

- **错误出现的越早越好**
  - 能在**写代码的时候**发现的错误，就不要**代码编译时**发现
  - 能在**编代码编译期**发现的错误，就不要在**代码运行时**发现
  - 能在**开发阶段**发现的错误，就不要在**测试阶段**发现错误
  - 能在**测试阶段**发现的错误，就不要**上线后**才发现

## 类型

### 数据类型

| 类型   | 作用                           |
| ------ | ------------------------------ |
| string | Typescript中的字符串类型       |
| String | Javascript的字符串包装类的类型 |
| number | Typescript中的数字类型         |
| Number | JavaScript的数字包装类的类型   |

| 类型            | 作用                                                                                | -              |
| --------------- | ----------------------------------------------------------------------------------- | -------------- |
| number          | 数字类型，不区分int和double，统一为number                                           | 基本数据类型   |
| boolean         | 仅又true和false                                                                     | 基本数据类型   |
| string          | 字符串类型，可以用单引号或双引号表示                                                | 基本数据类型   |
| Array           | 数组类型(最好类型固定，不要数组中存放多种类型)                                      | 非基本数据类型 |
| null、undefined |                                                                                     |                |
| Symbol          | 符号                                                                                |                |
| any             | 无法确定变量的类型，并且可能会发生改变的时候使用any                                 | TS特有，JS没有 |
| unknown         | 用于描述类型不确定的变量，unknown只能赋值给unknown，any可以赋值给任意类型           | TS特有，JS没有 |
| void            | 通常用来指定函数没有返回值，那么返回值就是void类型，可以把null和undefined赋值给void | TS特有，JS没有 |
| never           | 永远不会发生的值，如果函数死循环或者抛出异常就不会返回任何值包括void                | TS特有，JS没有 |
| 联合类型        | `let n = string | number` 用 `|` 来表示`n`可以是`string`也可以是`number`类型        |                |
| tuple           | 元组类型                                                                            |                |

```typescript
let num : number = 123;
num = 222;

let num1 : number = 100;        // 十进制
let num2 : number = 0b111;      // 二进制
let num3 : number = 0o234;      // 八进制
let num4 : number = 0x29abc;    // 十六进制
// --------------------------------------------------------------
let flag : boolean = true;
flag = (20 > 40);
// --------------------------------------------------------------
let name = "2345";
let name1 = `name : ${name}`;   // 格式化字符串
// --------------------------------------------------------------
let names = [];                 // 默认是any类型数组
let names1 : Array<string> = [];// 不推荐，在react jsx中有冲突
let names2 : string[] = [];     // 推荐
// --------------------------------------------------------------
let info = {
    name : "w",
    age : 1
}
// --------------------------------------------------------------
let n1 : null : null;
let n2 : undefined : undefined;
// --------------------------------------------------------------
let message : any = "hello";    // any运行时不会报错，如果是string就报错了
message = 123;
let anyarray : any[] = [];
// --------------------------------------------------------------
function sum(n1 : number, n2 : number) {
    console.log("hello");
}
let resule = sum(1, 2);         // void类型

// --------------------------------------------------------------
function loopFun() : never {
    while(true){
        console.log("123");
    }
}
function loopFun1() : never{
    throw new Error();
}

// 针对类型做特殊处理，此时忘记处理boolean，所以赋值给never值，此时会报错，做提示之用
function handleMessage(message : string | number | boolean) {
    switch(typeof message){
        case 'string':
            break;
        case 'number':
            break;
        default :
            const check : never = message;
    }
}
handleMessage(true);
// --------------------------------------------------------------
const info:any[] = ["q", 1, 1.0];
let val = info[0];  // val 为any类型，不够安全
const info2 : [string, number, numer] = ["x", 10, 1.0]; // 元组可以确定各个类型
// --------------------------------------------------------------

```

### 函数的参数类型

```typescript
function sum(num1: number, num2: number) : numer{
    return num1 + num2;
}
```

和变量的类型注解一样，一般而言不需要编写返回类型注解，因为TypeScript会根据return返回值推断函数的返回类型（看个人喜好，编写了可读性更强）

```typescript
const names = ["1", "2", "3"];
names.forEach(function(item) {

});
```

一些成员函数可以不写数据类型，因为`names`是`string[]`，所以`item`肯定是`string`类型，Typescript会推导出来

```typescript
// 对象类型
function printPoint(point : {x: number, y:number, z?: number}){

}

printPoint({ x: 1, y: 2});
printPoint({ x: 1, y: 2, z: 3 });
printPoint({ x: 1, y: 2, z: 3, q : 4 });    // Error
```

> z ?: number 表示可选数据类型，如果没有`z`打印是`undefined`

`printPoint`目标是一个`Object`对象，该对象存在键`x`和`y`，并且都是`number`类型，目标对象可以存在名为`z`的key，也可以不存在`z`

### 组合类型

Typescript的类型系统允许我们使用多种运算符，从现有类型中构建新类型

- 一种组合类型：联合类型(Union Type)
   - 联合类型是由两个或者多个其他类型组成的类型
   - 表示可以是这些类型中的任何一个值
   - 联合类型中的每个类型被称之为联合成员(union's memeber)
   - 使用时必须小心类型

```typescript
// id 就是联合类型
function PrintID(id : number | string){
    console.log(id);
}
```

- 可选类型：可以理解为这个参数是 `目标类型 | undefined`的联合类型

```typescript
function foo(message ?: string){
    console.log(message);
}

function foo1(message : string | undefined){
    console.log(message);
}

foo1(undefined);
```

- 类型别名

```typescript
type UnionType = string | number | boolean;
function PrintID(id : UnionType){

}
type PointType = {
    x : number,
    y : number,
    z ?: number
}
funtion PrintPoint(point : PointType){

}
```

### 类型断言 as

有时候TypeScript无法获取具体的类型信息，这个时候需要使用**类型断言**(Type Assertions)
通过类型断言，可以把普遍的类型转变成具体类型

> `document.getElementById`TS只知道返回HTMLElement，但并不知道具体类型

```typescript
const el = document.getElementById("img");  // 其实获取到的是img标签
el.src = "图片地址";    // 直接设置img标签的报错

const el1 = document.getElementById("img") as HTMLImageElement;  // 其实获取到的是img标签
el1.src = "图片地址";    // 直接设置img标签的报错

class Person {
}

class Student extends Person {
    studying() {
        console.log("s");
    }
}

function sayHello(p: Person) {
    (p as Student).studying();
}

let s = new Student();
sayHello(s);

// as 的奇淫巧计 别瞎用
const message = "hello";
const num : number = (message as any) as number;
```

### 非空类型断言

```typescript
function printMessage(message ?: string){
    console.log(message.length);
}
printMessage("hello");
printMessage(); // Error message是undefined的

function printMessage2(message ?: string){
    console.log(message?.length);
}

```

上述代码`printMessage`不够严谨，`message`有`undefined`的可能
为了解决上述代码的不够严谨的问题，引入**可选链**(就是`printMessage2`中`.?`)

```typescript
type Person = {
    name: string,
    friend?: {
        name: string,
        age?: number,
        girlFriend?: {
            name: string
        }
    }
}

function test(p: Person) {
    console.log(p.name);
    console.log(p.friend?.name);
    console.log(p.friend?.age);
    console.log(p.friend?.girlFriend?.name);
}

let info: Person = {
    name: "x",
    friend: {
        name: "y",
        girlFriend: {
            name: "z"
        }
    }
}

test(info);
```

使用**可选链**可以省去复杂的`undefined`嵌套判断

> 是Javascript在ES11中添加的功能，非TS增加的功能

### !!和??的作用

- !!操作符
  - 将一个其他类型转换成`boolean`类型
  - 类似`Boolean(变量)`的方式
- ??操作符
  - 控制合并操作符是一个逻辑操作符
  - 当操作符的左侧是null或者undefined是，返回其右侧操作数，否则返回左侧操作数

```typescript
const message = "hello";
const flag = Boolean(message);
console.log(flag);
const flag1 = !!message;
console.log(flag1);
const flag2 = (!(!message));
console.log(flag2);
```

> `(!!message)`的操作可以理解为两部分：`(!message)`将`message`转换成`boolean`类型并取反，再通过`!`把值转换回来

```typescript
const message = "321";
const result = message ?? "123";
console.log(result);
```

### 字面量类型

TS中字面量类型的类型和值必须相同

```javascript
let message : "hello" = "hello";
let msg : 123 = 123;
```

> 通过上述代码可见，字符串也可以当作数据类型

虽然看起来不知道有什么用，但是配合联合类型就可以完成enum的功能

```typescript
type Alignment = 'left' | 'right' | 'center' | 'top';
let align : Alignment = 'left';
align = 'right';

type Method = 'Get' | 'Post';
function requests(url : string, method : Method){}

let options = {
    url : "",
    method : "Post"
}

requests(options.url, options.method);   // Error : options.method是字符串类型，不一定是"Post"/"Get"
requests(options.url, options.method as Method);

type Requested = {
    url: string,
    method: Method
};

let options2 : Requested = {
    url : "",
    method : "Post"
}
requests(options2.url, options2.method);
```

> 通过字面量类型和联合类型，可以限制变量的值的内容

### 类型缩小

- 类型缩小
  - 类型缩小的英文：Type Narrowing
  - 通过类似`typeof padding === "number"`的判断语句，来改变TypeScript的执行路径
  - 在给定的执行路径中，可以缩小比声明时更小的类型，过程称之为**缩小**
  - `typeof padding === "number"`称之为**类型保护(type guards)**

- 常见的类型保
  - typeof
  - 平等缩小(===, !==,==, !=, switch)
  - instanceof
  - in
  - ...

> 就是逐渐缩小变量的类型的范围的过程

```typescript
// typeof
function printID(id : number | string){
    if (typeof id === 'string'){
        // 从联合类型到确认为string类型
        console.log(id.toUpperCase());
    } else {
        // 从联合类型到确认为number类型
        console.log(id);
    }
}

// 平等缩小
type Alignment = 'left' | 'right' | 'center' | 'top';
function printDirection(direction : Alignment){
    switch(direction){
        case 'left':
            break;
        case 'right':
            break;
        case 'center':
            break;
        case 'top':
            break;
    }
}

// instanceof 判断对象类型
function printTime(time : string | Date){
    if (time instanceof Date){
        console.log(time.toUTCString());
    } else {
        console.log(string);
    }
}

// in
type Fish = {
    swimming: () => void;
}
type Dog = {
    running: () => void;
}
function walk(animal : Fish | Dog){
    if ('swimming' in animal){
        // 判断为Fish类型
        animal.swimming();
    } else {
        animal.running();
    }
}
```

### 函数类型

1. 函数作为参数时

```javascript
function foo(){

}

function bar(fn : ()=>void){

}

bar(foo);
```

> `·()=>void`是函数类型，不是匿名函数

2. 函数作为变量

```typescript
type AddFnType = (num1 : number, num2 : number) => number  ;
let add : AddFnType = (num1 : number, num2 : number) => {
    return num1 + num2;
}
```

3. 参数的可选类型

可选参数`name ?: type`必须写在最后，本质其实就是`name : type | undefined`，所以name其实可能为undefined，需要对name为undefined的情况做处理

```typescript
function foo(x: number, y?:number){

}
```

4. 参数默认值

```typescript
function foo(x : number, y : number = 100){
    console.log(x, y);
}
function foo1(x : number = 20, y : number){
    console.log(x, y);
}
foo(20);
foo1(undefined, 30);
```

5. 剩余参数

使用`...nums : number[]`作为剩余参数
JS部分有讲

```typescript
function sum(...nums : number[]) : number{
    if(nums.length == 1){
        return nums[0];
    }
    let val = nums[0];
    nums = nums.slice(1);
    return val + sum(...nums);
}
console.log(sum(1, 2, 3, 4, 5, 6));
```