# Pak

在 UE 中，Pak 文件就是一个把 **很多普通文件打成一个大包** 的归档格式，类似于 Zip

专门为 UE 定制

- 按 文件路径 来索引内容，比如 `/Game/Content/Character/Hero.uasset`
- 支持
  - 按文件压缩(`FPakCompresssedBlcok`)
  - 可选加密
  - SHA1 校验、签名文件
  - 删除记录 用于 patch 覆盖
- 在运行时通过 `FPakFile` + `FPakPlatformFile` 实现统一挂载与查找

> `Pak` 的索引 Key 是文件名，`IoStore` 的 Key 是 `FIoChunkId`

典型的 `.pak` 文件布局大概是这样的

```
[ 文件1数据 ][ 文件2数据 ][ 文件3数据 ] ... [ 填充/其它数据 ][ Pak 索引区 ][ FPakInfo 头部 ]
```

- FPakInfo （Pak尾部的头信息）
- 索引区（Index），包含一个 **文件路径** -> `FPakEntry` 的索引（可能压缩、加密）
- 各个文件的实际数据块，每个文件用一个 `FPakEntry` 描述其起始 offset、长度、压缩块布局、加密标志

## 常见结构

### FPakInfo

在文件尾部存放的 Pak 文件头

```cpp
enum 
{
    /** Magic number to use in header */
    PakFile_Magic = 0x5A6F12E1,
    /** Size of cached data. */
    MaxChunkDataSize = 64*1024,
    /** Length of a compression format name */
    CompressionMethodNameLen = 32,
    /** Number of allowed different methods */
    MaxNumCompressionMethods=5, // when we remove patchcompatibilitymode421 we can reduce this to 4
};
```

| 枚举值 | 意义 | 作用 |
| --- | --- | --- |
| PakFile_Magic | Pak 文件的魔数（magic number），用于标识这是一个合法的 PakInfo 区块 | 读取 Pak 文件尾部时，通过读取该值判断是否真的是 UE 的 pak |
| MaxChunkDataSize | Pak 内部读写时，单次 IO 缓存的最大数据块大小，默认 64KB | 在若干 Pak 相关类里作为缓存块大小；读大文件时会按这个块大小切分，便于做签名校验 / 校验块边界 |
| CompressionMethodNameLen | 每种压缩算法名字在 Pak 文件里保存时的固定长度 | 因为 Pak 需要在头信息里存一张**压缩算法列表**，且要支持旧版本，所以用固定大小的 char 数组序列化：每种算法占 32 字节；不足补 0，多余截断 |
| MaxNumCompressionMethods | 一个 pak 文件中最多支持多少种不同的压缩算法 | 默认值：5，限制 CompressionMethods 中的元素数量，超过数量时会触发 checkf |

```cpp
enum
{
    PakFile_Version_Initial = 1,
    PakFile_Version_NoTimestamps = 2,
    PakFile_Version_CompressionEncryption = 3,
    PakFile_Version_IndexEncryption = 4,
    PakFile_Version_RelativeChunkOffsets = 5,
    PakFile_Version_DeleteRecords = 6,
    PakFile_Version_EncryptionKeyGuid = 7,
    PakFile_Version_FNameBasedCompressionMethod = 8,
    PakFile_Version_FrozenIndex = 9,
    PakFile_Version_PathHashIndex = 10,
    PakFile_Version_Fnv64BugFix = 11,


    PakFile_Version_Last,
    PakFile_Version_Invalid,
    PakFile_Version_Latest = PakFile_Version_Last - 1
};
```

这些是不同 Pak 格式演进的里程碑，用于兼容旧 Pak

| 枚举值 | 意义 |
| --- | --- |
| PakFile_Version_Initial | 最初版本，包含时间戳等字段 |
| PakFile_Version_NoTimestamps | 去掉了文件时间戳，减少 pak 记录中不必要的变动 |
| PakFile_Version_CompressionEncryption | 引入了压缩/加密相关的字段 |
| PakFile_Version_IndexEncryption | 支持索引区（Index）的加密 |
| PakFile_Version_RelativeChunkOffsets | 改用相对偏移方式记录压缩块起始偏移，节省空间并改善补丁情况。 |
| PakFile_Version_DeleteRecords | 支持 **删除记录**，用于 patch 标记某文件被删除 |
| PakFile_Version_EncryptionKeyGuid | 加入 `EncryptionKeyGuid`，允许根据 GUID 选择不同的加密密钥 |
| PakFile_Version_FNameBasedCompressionMethod | 压缩算法不再用旧的 COMPRESS_* bit 标志，而是用 FName 列表 + 索引形式 |
| PakFile_Version_FrozenIndex | 引入 冻结索引（Frozen Index） 机制，用于一些特殊打包/发布流程 |
| PakFile_Version_PathHashIndex | 引入路径哈希索引（FPathHashIndex），加快查找 |
| PakFile_Version_Fnv64BugFix | 修复了早期路径哈希算法里 FNV64 相关的 bug |
| PakFile_Version_Last | 用于计算 Latest |
| PakFile_Version_Invalid | 非法版本标志 |
| PakFile_Version_Latest | 当前引擎支持的最新版本号（= PakFile_Version_Last - 1） |

FPakInfo 的数据成员

```cpp
uint32 Magic;
int32 Version;
int64 IndexOffset;
int64 IndexSize;
FSHAHash IndexHash;
uint8 bEncryptedIndex;
FGuid EncryptionKeyGuid;
TArray<FName> CompressionMethods;
```

| 属性名 | 含义 | 作用 |
| --- | --- | --- | 
| Magic | Pak 文件魔数 | 用来校验当前读到的区域是不是合法的 Pak 头，一般为 `PakFile_Magic` |
| Version | 当前 pak 文件的版本号 | 对应上面版本值枚举 |
| IndexOffset | pak 文件中索引区（Index）的起始偏移 | 读取 pak 时，先读 FPakInfo，然后用 IndexOffset 定位索引区 |
| IndexSize | 索引区的字节大小 | 索引区的字节大小 |
| IndexHash | 索引区的 SHA1 哈希值 | 用于检测索引区是否被损坏或篡改 |
| bEncryptedIndex | 标记索引区是否被加密 | 若为 true，则读取索引区后需要用 AES 等方式解密 |
| EncryptionKeyGuid | 用于索引区加密的密钥 GUID | 若为空 GUID，表示使用**内置默认密钥**；否则，从某处密钥表中查找对应 GUID 的密钥 |
| CompressionMethods | 当前 pak 文件中使用的所有压缩算法名字列表 | 写 Pak 时，如果某个文件使用某种压缩方法，需要先通过 GetCompressionMethodIndex 注册/获取索引，再写入 FPakEntry::CompressionMethodIndex |

### FPakEntry

这是 Pak 文件中**每一个存文件记录**的描述


