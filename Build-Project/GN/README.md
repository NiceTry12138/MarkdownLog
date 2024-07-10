# gn

[gn快速开始](https://github.com/chinanf-boy/gn-zh/blob/master/docs/quick_start.zh.md)

> 该仓库基于 google 的 gn 仓库进行的翻译

## 语法

GN 使用非常简单、动态类型的语言

- 布尔类型 (`true`、`false`)
- 64有符号整数
- string
- 列表
- 范围(类似字典，只用于内置的东西)

### 字符串

**字符串**以双引号括起来，并使用反斜杠作为转义字符

但是反斜杠只支持 `\"`、 `\$`、 `\\`，反斜杠任何其他的用途都会被视为**字面反斜杠**

> `"C:\foo\bar.h"` 这里的 `\b` 没有被转义

**字符串**支持跟随 `$` 符号的词被变量的值替换，也可以使用 `{}` 来替换变量值

以 `a = "my_path"` 为例

| 写法 | 实际值 |
| --- | --- |
| `b = "$a/foo.cc"` | `b = "my_path/foo.cc"` |
| `c = "$foo_{a}_bar.cc"` | `c = "foo_my_path_bar.cc"` |

### 列表

**列表**支持增加、删除、索引操作，但是**不支持获取列表长度**

```gn
# 增加
a = [ "first" ]
a += [ "second" ]  # [ "first", "second" ]
a += [ "third", "fourth" ]  # [ "first", "second", "third", "fourth" ]
b = a + [ "fifth" ]  # [ "first", "second", "third", "fourth", "fifth" ]

# 删除
a = [ "first", "second", "third", "first" ]
b = a - [ "first" ]  # [ "second", "third" ]
a -= [ "second" ]  # [ "first", "third", "fourth" ]

# 索引
a = [ "first", "second", "third" ]
b = a[1]  # -> "second"
```

`[]` 运算符是只读的，不能用于对列表进行改变，这种i情况的主要用途是当外部脚本返回几个已知值时，可以提取

```gn
a = [ "one" ]
a = [ "two" ]  # Error: overwriting nonempty list with a nonempty list.
a = []         # OK
a = [ "two" ]  # OK
```

### 条件

```c
if (is_linux || (is_win && target_cpu == "x86")) {
        sources -= [ "something.cc" ]
    } else if (...) {
        ...
    } else {
    ...
}
```


### 循环

```c
foreach(i, mylist) {
    print(i)  # Note: i is a copy of each element, not a reference to it.
}
```

### 文件名

- 相对名称

```c
"foo.cc"
"src/foo.cc"
"../src/foo.cc"
```

- 源数绝对名称

```c
"//net/foo.cc"
"//base/test/foo.cc"
```

- 系统绝对名称

```c
"/usr/local/include/"
"/C:/Program Files/Windows Kits/Include"
```

### 构建配置-目标

| 命令 | 作用 |
| --- | --- |
| action | 运行脚本生成文件. |
| action_foreach | 为每个源文件运行一次脚本. |
| bundle_dat | 一个声明数据进入 MAC/iOS 包. |
| create_bundle | 创建一个 Mac/IOS 包. |
| executable | 生成一个可执行文件. |
| group | 是指一个或多个其他目标的虚拟依赖节点. |
| shared_library | 一个 . DLL 或.SO. |
| loadable_module | 一个 .DLL 或.SO ,只能在运行时加载. |
| source_set | 一个轻量级的虚拟静态库(通常比实际静态库更可取,因为它将更快地构建). |
| static_library | 是.LIB 或.file(通常你需要一个source_set取而代之) |

### 构建配置-Config

```gn
# 定义 config
config("my_config") {
  # 预处理器宏定义
  defines = [
    "USE_FEATURE_X",
    "ENABLE_LOGGING"
  ]

  # 包含目录
  include_dirs = [
    "//include",
    "//third_party/lib/include"
  ]

  # 编译选项
  cflags = [
    "-Wall",
    "-O2"
  ]

  # 链接选项
  ldflags = [
    "-lmylib"
  ]
}

```

```gn
# 将 config 应用到目标
# 引用自定义配置文件
import("//configs/my_config.gni")

executable("my_app") {
  sources = [
    "main.cc",
    "app.cc"
  ]

  # 使用定义的 config
  configs += [ ":my_config" ]
}

```

为什么上面例子应用 config 时要用 `configs += [":my_config"]` ？

这里 `+=` 表示向现有的 configs 属性追加 :my_config，保留之前的所有设置。如果直接使用 `=` 则表示直接覆盖掉之前的配置 

为什么应用 config 时要写 `:my_config` ?

- `:config_name` 表示当前文件中的配置
- `//path/to:config_name` 表示绝对路劲的配置
- `//path/to/configs:config_name` 表示绝对路径下特定目录中的配置

以下面的路劲为例子

```bash
my_project/
├── BUILD.gn
├── config/
│   └── my_config.gni
└── src/
    └── main.cc
```

在 `config/my_config.gni` 中定义

```gn
config("my_config") {
  cflags = [
    "-Wall",
    "-O2"
  ]
}
```

在 `BUILD.gn` 中

```gn
# 导入配置文件
import("//config/my_config.gni")

executable("my_app") {
  sources = [
    "src/main.cc"
  ]

  # 正确的使用方式：使用绝对路径或当前作用域
  configs = [ "//config:my_config" ]  # 绝对路径
  # 或
  configs += [ ":my_config" ]  # 当前作用域，假设 my_config 在当前文件中定义
}
```

假设模块A依赖于某个配置B，希望所有依赖于 A 模块的其他模块都依赖配置 B，这个时候应该使用 **公共配置**

```gn
# 配置B
config("my_external_library_config") {
  includes = "."
  defines = [ "DISABLE_JANK" ]
}

# 模块A
shared_library("my_external_library") {
  ...
  # 所有依赖于这个库的目标都拿到了公共配置.
  public_configs = [ ":my_external_library_config" ]
}

# 其他模块
static_library("intermediate_library") {
  ...
  # Targets that depend on this one also get the configs from "my external library".
  public_deps = [ ":my_external_library" ]
}
```

### 构建配置-模板

`template` 㻾创建自定义的符合构建目标。通过定义模板，可以将一组常用的构建配置封装起来，然后再多个地方复用，减少重复代码并提高构建文件的可维护性

```gn
# 定义 template
# my_templates.gni

template("my_executable_with_tests") {
  # 定义模板参数
  declare_args() {
    sources = []
    test_sources = []
  }

  # 创建可执行文件目标
  executable(target_name) {
    sources = sources
  }

  # 创建测试可执行文件目标
  executable(target_name + "_test") {
    sources = test_sources
    deps = [ ":$target_name" ]
  }
}
```

使用 `template`

```gn
# 引用模板文件
import("//path/to/my_templates.gni")

# 使用模板来创建构建目标
my_executable_with_tests("my_app") {
  sources = [ "main.cc", "app.cc" ]
  test_sources = [ "app_test.cc" ]
}
```

模板的第一个参数会被用作 target_name，即目标名称。所以 `template` 中定义的 `target_name` 对应的就是 `my_app`

除了定义之外，`template` 中还能定义 条件判断、循环、嵌套模板

```gn
# templates/my_templates.gni

template("my_executable_with_tests") {
  declare_args() {
    sources = []
    test_sources = []
    use_optimization = false
  }

  if (use_optimization) {
    config("optimization_config") {
      cflags = [ "-O2" ]
    }
  }

  executable(target_name) {
    sources = sources
    if (use_optimization) {
      configs += [ ":optimization_config" ]
    }
  }

  executable(target_name + "_test") {
    sources = test_sources
    deps = [ ":$target_name" ]
  }
}

```

## 交叉编译

[GN如何处理跨平台编译](https://github.com/chinanf-boy/gn-zh/blob/master/docs/cross_compiles.zh.md)

## 注意

注意路径中不要出现中文，否则可能会出现莫名其妙的错误