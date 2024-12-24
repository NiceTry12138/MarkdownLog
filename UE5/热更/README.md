# 网络热更

1. 解压解密和压缩加密数据
2. OSS 数据上传和下载
3. Pak打包
4. Pak包安装和卸载

## 命令行 Pak

点击按钮， Cook 烘焙资源，在 `项目路径/Saved/Cooked/Android_ASTC/` 中会存储本次烘焙的结果，并按照引擎和项目资产进行区分

| 烘焙 | 烘焙结果 |
| --- | --- |
|  ![](Image/001.png) | ![](Image/002.png) |

随便找个文件夹将烘焙的一些路径拷贝出来，后续测试可用

![](Image/003.png)

在上级目录，也就是 `E:\UEProj\Empty53\Cooked` 中新建文本文件，用于标记那些文件需要被打入到 `pak` 中

![](Image/004.png)

`E:\UEProj\Empty53\Cooked\Cooked\*.*` 使用 `*.*` 表示文件夹内所有文件

然后就可以直接使用 `UnrealPak.exe` 这个 UE 提供的 Pak 工具进行文件打包

`E:\UEEngine\UE_5.3\Engine\Binaries\Win64\UnrealPak.exe E:\UEProj\Empty53\Cooked\EmptyPak.pak -create=E:\UEProj\Empty53\Cooked\CookedFilePath.txt`

> 上述命令格式就是： `UnrealPak.exe` + `Pak` 文件输出路径 + `Cooked` 文件路径索引文件

![](Image/005.png)

使用 `E:\UEEngine\UE_5.3\Engine\Binaries\Win64\UnrealPak.exe E:\UEProj\Empty53\Saved\Pak\Characters.pak -list` 可以解析出 `Pak` 中文件列表

![](Image/006.png)

