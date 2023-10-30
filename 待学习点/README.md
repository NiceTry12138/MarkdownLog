1. 为什么ue的在ts里算的很慢

puerts计算 FVector 每次都会创建临时对象，导致巨大gc压力

比如cpp里Fvector一般当栈上对象用，但是要包装给ts的话必须要创建成堆上对象

有很多层面上的开销...比如：cpp里Fvector一般当栈上对象用，但是要包装给ts的话必须要创建成堆上对象

cpp里计算，编译器会帮你消除对象的创建，而且cpp里计算fvector这种都是在栈上，栈上直接清栈就可以了。

并且 FVector是个 pod 的对象

什么是 POD 对象? 

2. V8 引擎的内存管理机制：Orinoco

什么是 Orinoco? 其内部运行机制是什么?