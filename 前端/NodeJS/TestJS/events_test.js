const EventEmitter = require('events')

// 创建发射器
const emitter = new EventEmitter();

// 监听事件： on 和 addListener 作用一样
emitter.on('click', (args) => {
    console.log(`listener1 clicked, args => ${args}`);
})

const listener2 = (arg1, arg2, arg3) => {
    console.log(`listener2 clicked, args =>`, arg1, arg2, arg3);
};

const listener3 = (args) => {
    console.log(`listener3 clicked, args => ${args}`);
};

emitter.on('click', listener2)

emitter.once('click', listener3)    // 只监听一次

// 发送事件
emitter.emit(`click`, 1, 2, 3, 4, 5);

// 取消事件监听
emitter.off(`click`, listener2);

// 发送事件
emitter.emit(`click`, 1, 2, 3, 4, 5);

// 获取注册的事件
console.log(emitter.eventNames()); // 获得所有事件的名称

console.log(emitter.listenerCount(`click`));    // 获取事件绑定的回调个数

console.log(emitter.listeners('click'));    // 获取所有回调函数对象