# Git

## 使用Git的理由

1. 每日工作备份(其实也不是每日，可以每小时每分钟)
2. 异地协同工作
3. 现场版本控制，产品部署线程进行源代码修改，并在修改过程中进行版本控制，以便在完成修改后能够将修改结果甚至修改过程一并带走，并能够将修改结果合并至项目对应的代码库中
4. 避免引入辅助目录，SVN会引入.svn目录，Git虽然也会引入.git目录，但是Git可以指定外部.git目录
5. 重写提交说明
6. 想吃后悔药，错误提交了1G的文件
    - 对SVN来说可能不太好解决
    - 对Git来说`git rm--cached xxx.img` 和 `git commit--amend`，然后使用变基操作`git rebast-i<comit-id>`
7. 更好的提交列表
    - 一般而言，一次提交只干一件事情，要么修改bug，要么优化代码，切不可修改bug和优化代码一起提交
    - Git的解决方案：执行`git add`命令将修改内容加入提交残存去，执行`git add-u`命令可以将所有修改过的文件加入暂存区，执行`git add -A`命令可以将本地删除文件和新增添加都提交到暂存区，执行`git add-p`命令甚至可以对一个文件内容的修改进行有选择性的添加
8. 更好的差异比价
    - 支持二进制文件的比较
    - 支持一行内逐字比较
    - 修改的文件执行`git diff`查看修改造成的差异
9. 工作进度保存

## Git初始化

```bash
$ git --version
git version 2.32.0.windows.2
```

1. 配置当前用户的姓名和邮箱，在提交版本库时会用到

设置一下Git的配置变量，这是一次性工作，这些设置会在全局文件(用户主目录下的.gitconfig)或系统文件中做永久的记录

```bash
git config --global user.name "用户名"
git config --global user.emial "邮箱"
```

2. 设置Git变量，使命令变得简洁

```bash
git config --global alias.st status      # git status    => git st
git config --global alias.ci commit      # git commit    => git ci
git config --global alias.co checkout    # git checkout  => git co
git config --global alias.br branch      # git branch    => git br
```

3. git命令输出中开启颜色 显示

```bash
git config --global colour.ui true
```

------

从零开始创建版本库

1. 创建新的工作目录，使用`git init`创建版本库

```bash
mkdir demo
cd demo
git init
```

在1.6.5版本之后，可以直接通过`git init demo`完成文件夹的创建和版本库的初始化

> 上述命令基于linux，在windows中可以使用Git Bash

在创建出来的demo文件夹中可以看到`.git`文件夹，这个.git文件夹就是Git版本库，又叫仓库,repository

2. 管理文件

在demo文件夹中添加welcome.txt，内容为"Hello World"

```bash
git add welcome.txt
```

通过上述命令，将新建立的文件添加到版本库，但是跟其他版本管理系统一样，需要执行员一次提交操作，对git来说就是`git commit`，此时git会弹出一个文件要求填写提交说明，使用`git commit -m "提交说明"`来省去弹出编辑器的操作

```bash
$ git commit -m "initliaze"
[master (root-commit) 4eb8b92] initliaze
 1 file changed, 1 insertion(+)
 create mode 100644 welcome.txt
```

- 第一行：提交到master分支，并且是该分支的第一个提交(root-commit)，提交ID4eb8b92
- 第二行：此次提交修改了一个文件，包含一行插入
- 第三行：此次提交创建了welcome.txt文件

> 提交ID一般来说是不一样的，如果你我一样建议结婚

-----

### 为什么需要一个.git目录

Git及其它分布式版本控制系统的一个共同的显著特点就是，版本库位于工作区的根目录下，对git来说版本库位于工作区根目录下的.git目录中

git将版本库(.git目录)放在工作区跟目录下，在工作区的子目录中执行Git命令时，git会**向上递归查找**.git目录，找到.git目录就是工作区对应的版本库，.git所在的目录就是工作区的根目录，文件.git/index记录了工作区文件的状态(就是**暂存区**的状态)

- 使用`git rev-parse --git-dir`查看git所在路径
- 使用`git rev-parse --show-toplevel`查看工作区根目录
- 使用`git rev-parse --show-prefix`查看当前目录相对根目录的相对路径
- 使用`git rev-parse --show-cdup`显示当前目录后退到工作区的根的深度

比如，我们通过`mkdir -p a/b/c`，在新创建的`c`文件夹中调用`git rev-parse --git-dir`命令测试一下就行

![](./Image/1.png)

### git config命令的各参数有何区别

- `git config -e`将打开.git/config文件进行编辑
- `git config -e --global`将打开用户目录下的.gitconfig文件进行编辑
  - win:C:/Users/用户名/.gitconfig 
  - linux:/home/用户名/.gitconfig
- `git config -e --system`将打开系统级配置文件进行编辑
  - win:C:/Program Files/Git/etc/gitconfig
  - linux:/etc/gitconfig

git的三个配置文件分别是**版本库级**，**全局配置文件**(用户主目录下)，**系统级配置文件**，其中版本库级别的配置文件的优先级最高，全局配置次之，系统级配置优先级最低

这样的优先级设置可以让版本库.git目录下的config文件中的配置覆盖用户主目录下的git环境配置，而用户主目录下的配置也可以覆盖系统的

查看config文件的内容

```ini
[core]
        repositoryformatversion = 0
        filemode = false
        bare = false
        logallrefupdates = true
        symlinks = false
        ignorecase = true
```

可以从内容发现config是INI文件格式，对应的可以使用`git config <section>.<key>`命令来读取INI文件文件，比如`git config core.bare`就会输出false

相对的，使用`git config <section>.<key> <value>`来设置属性

### 是谁完成的提交

最开始执行了user.name和user.email的配置可以通过下面的代码删除

```bash
git config --unset --global user.name
git config --unset --global user.email
```

在通过上述命令删除name和email之后，再执行commit命令

```bash
git commit --allow-empty -m "commit"
```

> --allow-empty 允许没有任何修改文件的情况下提交

```bash
git commit --allow-empty -m "commit"
*** Please tell me who you are.

Run

  git config --global user.email "you@example.com"
  git config --global user.name "Your Name"

to set your account's default identity.
Omit --global to set the identity only in this repository.
```

通过`git log --pretty=fuller`来查看版本库的提交日志

### 随意设置提交者姓名是否不太安全

例如CVS和Subversion等集中式版本控制系统，登录ID就作为提交者ID出现在版本库的提交日志中

像Git这样的分布式版本控制系统，可以随心所欲地设定提交者，这似乎不太安全。当版本库属于个人的时候，很难也没有必要进行身份认证；但是团队合作时授权就成为必需了

通常团队协作时会设置一个共享版本库，再团队成员向共享版本库传送(推送)新提交时，会进行用户身份认证并检查授权。一旦用户通过身份认证，一般来说不会对提交中的包含的提交者ID做进一步检查，但Android项目是个例外

Android项目为了更好地使用Git实现对代码的集中管理，开发了一套叫做Gerrit的审核服务器来管理Git提交，对提交者的邮件地址进行审核

但是即使没有Gerrit的服务，作为提交者也不应该随意改变配置变量 user.name和user.email的设置，因为当多人写作时会给他人造成迷惑，也会给一些项目管理软件带来麻烦

### 命令别名是干什么

```bash
git config --global alias.ci "commit -s"
```

通过上述命令会自动带上-s参数，这样会在提交说明时自动添加上包含提交者姓名和邮件地址的签名标识

不过别名命令会带来一些困扰，尤其是写文档或者演示的时候

## Git暂存区

