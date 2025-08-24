# TinyRenderer

[参考项目 tinyrenderer](https://github.com/ssloy/tinyrenderer)

## 光栅化

### 求重心

重心坐标 (α, β, γ) 表示点 P 可以表示为三角形三个顶点A、B、C的加权和：P = αA + βB + γC，其中 α + β + γ = 1

如果点P在三角形内部，则满足：0 ≤ α, β, γ ≤ 1

通过重心坐标，即可知道某个点是否在三角形内部

```
α·A_x + β·B_x + γ·C_x = P_x
α·A_y + β·B_y + γ·C_y = P_y
α + β + γ = 1
```

对应的矩阵表现形式是 

```
| A_x  B_x  C_x |   | α |   | P_x |
| A_y  B_y  C_y | × | β | = | P_y |
|  1    1    1  |   | γ |   |  1  |
```

设矩阵 M 为

```
    | A_x  B_x  C_x |
M = | A_y  B_y  C_y |
    |  1    1    1  |
```

需要求解的是向量 [α, β, γ]ᵀ ，使得： M × [α, β, γ]ᵀ = [P_x, P_y, 1]ᵀ

解为：[α, β, γ]ᵀ = M⁻¹ × [P_x, P_y, 1]ᵀ

```cpp
vec3 barycentric(const vec2 tri[3], const vec2 P) {
    mat<3,3> ABC = {{ {tri[0].x, tri[0].y, 1.}, {tri[1].x, tri[1].y, 1.}, {tri[2].x, tri[2].y, 1.} }};
    if (ABC.det()<1) return {-1,1,1}; 
    return ABC.invert_transpose() * vec3{P.x, P.y, 1.};
}
```

### 透视矫正

| 参考文章 | 链接 |
| --- | --- |
| 透视矫正插值 | https://zhuanlan.zhihu.com/p/1929932158643996005
 |
| Perspective Correct Interpolation | https://www.cs.ucr.edu/~craigs/courses/2018-fall-cs-130/lectures/perspective-correct-interpolation.pdf |

| 贴图 | 渲染效果 |
| --- | --- |
| ![](Image/001.jpg) | ![](Image/002.jpg) |

示意图

![](Image/003.jpg)

![](Image/004.jpg)

由于透视的问题，明明 Q 点是 AC 中点；但是由于透视的存在，看起来 P 点才像是 AC 中点

在坐标点通过 MVP 矩阵变化到屏幕空间中，会将 P 点的 UV 坐标设置为 `(1/2, 1/2)`，最终导致显示问题

可以通过 **顶点的世界坐标** 和 **目标点的屏幕坐标**，计算得到目标点对应平面上的 **世界坐标**
三角形 ABC 中的某个点 P 在透视分割后会变换为点 P'

$$
p = \begin{bmatrix}
x \\
y \\
z
\end{bmatrix} p' = \frac{1}{z * r + 1} * \begin{bmatrix}
x \\
y \\
z
\end{bmatrix}
$$

r 是一个固定参数，决定 w 对 z 的依赖程度，本质上与相机的几何参数（焦距/投影平面位置）有关

点 P' 相对于三角形 A'B'C' 的重心坐标

$$
P' = \left [ A' B' C' \right ]  * \begin{bmatrix}
\alpha' \\
\beta' \\
\gamma'
\end{bmatrix}
$$

已知屏幕空间坐标 A'B'C' 和 P' 相对于 A'B'C' 的重心坐标，接下来需要找到 P 相对于原始三角形 ABC 的重心坐标

$$
P = \left [ A B C \right ]  * \begin{bmatrix}
\alpha \\
\beta \\
\gamma
\end{bmatrix}
$$

推理 P' 的表达方式

$$
P' = \left [ A' B' C' \right ]  * \begin{bmatrix}
\alpha' \\
\beta' \\
\gamma'
\end{bmatrix} = P * \frac{1}{r P.z + 1} = \left [ A * \frac{1}{r A.z + 1} B* \frac{1}{r B.z + 1} C* \frac{1}{r C.z + 1} \right ]  * \begin{bmatrix}
\alpha' \\
\beta' \\
\gamma'
\end{bmatrix}
$$

根据

$$
P * \frac{1}{r P.z + 1} = \left [ A * \frac{1}{r A.z + 1} B* \frac{1}{r B.z + 1} C* \frac{1}{r C.z + 1} \right ]  * \begin{bmatrix}
\alpha' \\
\beta' \\
\gamma'
\end{bmatrix}
$$

两边乘以 `rP.z + 1` 

$$
P = \left [ A B C \right ] * \begin{bmatrix}
\alpha' / (r A.z + 1) \\
\beta'  / (r B.z + 1) \\
\gamma' / (r C.z + 1)
\end{bmatrix} * (r P.z + 1)
$$


