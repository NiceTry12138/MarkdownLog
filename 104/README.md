# 104

## 引擎架构分层

1. 工具层（引擎提供的各种编辑器）
2. 功能层（让游戏世界动起来、看得见）

![](./Image/104_01.png)

> 动画、相机、渲染、物理、脚本、UI等

3. 资源层（加载、管理资源）

> PSD、MAX、Mp3、XML、MPEG、JSON等文件

4. 核心代码层

> 容器、内存分配、垃圾回收、线程池等

5. 平台层

> Android、PC、Mac...  
> 手柄、鼠标、键盘、方向盘...

### 资源层

**以动画系统（Animation System）为例**

一套动画资源可能包括：骨骼信息、动作信息、蒙皮信息、数据文件等

在**源文件**中，这些文件可能包含额外的一些编辑时会用到的信息，这些信息在游戏中是无用的

> 比如同一篇文章，用word和txt两种格式存储的文件大小是完全不同的

又或者，原本的文件格式是不合适的，比如图片的PNG和JPEG是不能直接给GPU进行计算绘制的，需要将其转换成可以直接通过GPU绘制的文件格式

因此需要将这些离线文件转变成**引擎资产**

同样是上面动画资源的例子，一个文件可能关联其他多个文件，因此这些文件之间需要维系关联信息

![关联的文件](./Image/104_02.png)

```xml
<character name="robot" GUID="81-02985-0192835">
    <geometry>
        <mesh file_path="robot.mesh.ast"/>
        <texture>
            <abledo_texture file_path="robot_ambient.texture.ast"/>
            <roughness_texture file_path="robot_roughness.texture.ast"/>
        </texture>
        <material file_path="robot.material.ast" />
    </geometery>
    <animation>
        <animation_clip name="stand">
            <clip_file path="robot_stand.animation.ast" />
        </animation_clip>
        <animation_clip name="walk">
            <clip_file path="robot_walk.animation.ast" />
        </animation_clip>
    </animation>
</character>
```

对于上面图片中的一组关联文件，使用上面的XML配置可以记录关联信息

| key | 意义 |
| --- | --- |
| character | 表示角色 |
| geometry | 几何体信息 |
| mesh | 网格信息 |
| texture | 贴图信息 |
| material | 材质信息 |
| animation | 动作信息 |

注意上述XML中 `character` 标签中的 **GUID** 属性，表示唯一编号，通过该值可以唯一确定整个游戏项目中的一个文件，即该值全局唯一

通过记录**GUID**，即使文件位置被挪动，也可以快速定位，重新配置上述XML文件

综上所述，定义**引擎资产**的好处很多
1. 去除文件冗余数据，减少文件大小
2. 转换文件到合适的格式
3. 关联多文件之间的信息
4. 定义**GUID**确定唯一文件标识

一般游戏引擎都有**资源管理器**，属于一个虚拟的文件系统，通过路径加载、卸载各种资源；同时还要通过**hanle系统**管理资源的生命周期和各种引用

资源层一般就是管理资源的整个生命周期

![资源管理器支持的文件](./Image/104_03.png)

在整个游戏的过程中，资源会被不停的加载、卸载，如果某个瞬间加载、卸载的资源数目过多，会严重影响游戏的体验，因此资源管理器在游戏引擎中是十分重要的

### 功能层

![tick](./Image/104_04.png)

如何让游戏世界动起来，**tick**就是关键

```cpp

void tickLogic(float delta_time) {
    tickCamera(delta_time);
    tickMotor(delta_time);
    tickController(delta_time);
    tickAnimation(delta_time);
    tickPhysics(delta_time)l;
    /* ... */
}

void tickRender(float delta_time){
    tickRenderCamera();
    culling();
    rendering();
    postprocess();
    present();
}

void tickMain(float delta_time) {
    while(!exit_flag) {
        tickLogic(delta_time);
        tickRender(delta_time);
    }
}
```

一般会先tick计算Logic(逻辑)，然后再计算Render(渲染)

因此，如何让一个角色动起来：
1. 逻辑帧匹配角色的动画帧
2. 驱动角色骨骼和蒙皮
3. 在每一个渲染帧中完成角色的渲染作业

除此了tick外，功能层还有多线程任务分配等操作

![目前游戏引擎的多线程任务分配](./Image/104_05.png)

> 目前游戏引擎的多线程任务分配

![理想的满线程使用的任务分配](./Image/104_06.png)

> 理想的满线程使用的任务分配，但是因为任务之间的关联性，很多信息的计算必须在某个任务执行完毕之后，所以目前这种线程的分配方式不太好实现

### 核心层

核心层首当其冲就是**数学库**，各种值的计算、转换等操作都依赖数学库

> 游戏逻辑、绘制、物理等

**SIMD**全称Single Instruction Multiple Data，单指令多数据流，能够通过一条指令执行多次计算

> 以加法指令为例，单指令单数据（SISD）的CPU对加法指令译码后，执行部件先访问内存，取得第一个操作数；之后再一次访问内存，取得第二个操作数；随后才能进行求和运算。而在SIMD型的CPU中，指令译码后几个执行部件同时访问内存，一次性获得所有操作数进行运算。这个特点使SIMD特别适合于多媒体应用等数据密集型运算

除了**数学库**，核心层还提供各种常用的**数据结构**
- Array
- LinkList
- Queue
- Heap
- Tree
- Graph
- Stack
- Hashing
- Skeleton Tree
- Animation Frame Sequence 
- ...

当然还有最重要的**内存管理**
游戏引擎会一次性申请一大堆内存自行管理（**内存池**），以此来追求最大的效率

> 因为内存都在一起，所以**Cache的命中率**会更高

当然还要注意的就是**内存对齐**

### 平台层

平台层就是为了使开发者不用关注不同平台之间差异，从而提高开发效率

1. 系统不同

不同平台的文件路径、正反斜线都有区别

2. 图形渲染不同

不同平台的图形API也不同：OpenGLES、DirectX11、DirectX12等等

> RHI 硬件渲染接口，重新定义一层渲染API，将不同硬件的SDK的区别封装起来

3. CPU不同

![不同的CPU](./Image/104_07.png)

需要考虑哪个算法在哪个核心中计算更合理（部分CPU特别提供计算核心）

### 工具层

给用户提供编辑各种文件的界面，游戏场景的可视化、动画资源预览、可编程的shader等

工具层是依赖游戏引擎的，它具有创建、编辑、交换游戏资源的能力

工具层的开发方式比较灵活，游戏引擎最佳是C++(效率)，但是工具层任何语言都是可以的考虑的

除了引擎工具层提供的资产生产工具外，还有其他厂商提供的开发工具

**DCC** => Digital Content Creation，数字资产

无论是自己通过引擎创建的，或者通过其他厂商工具创建的**DCC**，全部都可以通过游戏引擎的**资源层**导入到游戏引擎中，通过**Asset Conditioning Pipeline**，即各种导入、导出工具

#### 总结

与网络分层类似，每个层次只关注自己层次的功能实现，低层次为高层次提供服务，高层次不需要知道低层次的存在

一般来说高层次的发展会更加快速，低层次发展会较慢

## 构建游戏世界

### GameObject和Component

1. 动态物：游戏中会运动的物体，例如：人
2. 静态物：游戏中不会移动的物体，例如：房子
3. 地形系统
4. 植被
5. 天空

游戏中所有的物体都可以抽象成GO->**GameObject**，游戏最重要的就是管理这些GameObject

那么如何定义游戏中的对象？

最经典的做法就是**继承**，鸭子=>会叫的鸭子=>会飞的鸭子=>...

但是随着游戏越做越复杂，很多东西并没有清晰的父子关系：水陆两栖坦克(坦克+巡逻艇， 坦克派生自汽车，巡逻艇派生自船)

为了应对**继承**带来的一些问题，现代游戏引擎提供了**组件化**的开发方法，将行为/能力拆分到组件中，然后添加到对象上

对于**组件化**开发，最容易想到的就是枪械了，比如吃鸡中的1、2、3、4倍镜，各种握把等，都可以理解为组件，为枪械提供了某些能力

```cpp
class BaseComponent {

}

class TransformComponnet : public BaseComponent{

};

class AIComponent : public BaseComponnet {

};

class FlyComponent : public BaseComponent {

};

class GameobjectBase {
    vector<BaseComponent*> Components;
}

class FlyRobot : public GameobjectBase {
    Components = [FlyComponent, AIComponent, TransformComponnet];
}
```

角色能力统一封装到组件中，在角色中存储这些组件，即代表角色存在这些能力，此时想要添加或者修改能力只需要添加或者替换组件即可实现

![U3D和UE对组件的支持](./Image/104_08.png)

> U3D和UE中的Component

**综上**：游戏中的任何物体都是一个GameObject，每个GameObject由各种不同的Component组成

### GameObject之间的交互

最简单的写法就是

```cpp
void Bomb::OnExpode(){
    // 炸弹要自己判断各种类型的处理方式
    // ...
    switch(touch_type){
        case TouchType.Humen:
            // 扣血
            break;
        case TouchType.Tank:
            // 扣血
            break;
        case TouchType.Stone:
            // 不做处理
            break;
    }
    // ...
}
```

上面就是最暴力的写法，爆炸的时候获得爆炸影响的对象，用switch判断其类型，走不同的逻辑

这种写法最大的问题就是，**不好维护**，当后续对象类型添加到几百种，总不能写几百种Switch-case

此时，比较好的解决方案其实是 **事件**（Event） 机制

参考设计模式：观察者模式，使用事件的好处就是只是通知被影响的物体发生了何种事情，由被通知的对象自行做后续处理

```cpp
void Bomb::OnExpode(){
    // ...
    sendExploadEvent(go_actor); // 炸弹只做事件分发，不做数据处理
    // ...
}
```

```cpp
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_TwoParams(FActorComponentActivatedSignature, UActorComponent, OnComponentActivated, UActorComponent*, Component, bool, bReset);
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FActorComponentDeactivateSignature, UActorComponent, OnComponentDeactivated, UActorComponent*, Component);

DECLARE_MULTICAST_DELEGATE_OneParam(FActorComponentGlobalCreatePhysicsSignature, UActorComponent*);
DECLARE_MULTICAST_DELEGATE_OneParam(FActorComponentGlobalDestroyPhysicsSignature, UActorComponent*);
```

> 虚幻中的事件声明

### GameObject的管理

每个GameObject都需要一个唯一ID 作为唯一标识  
每个GameObject都需要一个坐标

> 这里GameObject是场景中的物体，并非UE中的UObject这种，更像是UE中的AActor

![八叉树](./Image/104_09.png)

上图中左边为普通网格管理，右边为八叉树管理

因为地图中的物体并不是均匀分布的，导致如果采用网格管理的方式，会出现一些网格中的对象过多，另一部分网格中却没有对象


当然除了**八叉树**外，还有其他的算法去做场景对象管理

![场景对象管理](./Image/104_10.png)

当时游戏发开并没有完全通用的解决方案，还是不同情况采用更为合适的方案为宜

### 其他

当子GameObject绑定到父GameObject后，tick时要先计算父GameObject，再计算子GameObject

再比如消息的传递，A给B发送信息，同时B也给A发送了信息，此时微观上其实是由先后顺序的，但是如果信息的处理是交给两个核心处理，那么可能这次是A先收到信息，下次是B先收到信息，这样程序的运行结果可能不同，导致严重的问题出现

所以，很多时候会有信息管理器，将信息统一发送到一个管理器中，再由管理器去根据顺序统一发送信息

因此游戏引擎中，**时序**是一个很重要的问题，需要着重考虑

## 游戏引擎的渲染

游戏绘制系统会遇到的问题
1. 成千上万的渲染对象和千奇百怪的渲染类型(水体、植被、角色、云等绘制算法各不相同)
2. 深度适配当代硬件(CPU、GPU)
3. 如何把计算时间维持在固定的时间内，以此来维持帧率
4. 游戏渲染不能占用100%的CPU资源，否则其他模块无法计算

 ### 基础游戏渲染

1. 传入顶点的3D空间坐标
2. 映射顶点的3D空间坐标到屏幕空间上
3. 三角面片
4. 栅格化
5. 贴图
6. 输出

 ### 材质、Shader和光照

 ### 特殊的渲染（后处理）

 ### 管道
