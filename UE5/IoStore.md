# IoStore

IoStore 是 UE4.25+ 引入的“新一代打包/IO 容器格式”，用 **Chunk + 容器 + 目录索引** 的方式替代/补充传统 PakFile，配合异步、IO/虚拟化等系统，实现更高效、更可控的运行时数据加载

## 为什么需要 IoStore

先解释 TOC，TOC 就是 Table Of Countents，即目录表

在 IoStore 中，TOC 描述了一个 IoStore 容器里所有 Chunk 的 目录文件/目录数据，在磁盘上就是 `.utoc` 文件，在代码里对应 `FIoStoreToHeader` + `FIoStoreTocResource` 这一套结构

在 IoStore 容器中，有两类东西

1. 数据本体，通常是大块二进制数据，放在 `.ucas` 文件里，该文件可以有多个分区
2. TOC（目录），记录有哪些 chunk，以及他们在 `.ucas` 里的偏移、长度、压缩块、目录索引 等元数据，放在 `.utoc` 文件中

引擎运行时

1. 先读取 `.utoc`，构造 `FIoStoreTocResource`
2. 再根据 TOC 里的信息去 `.ucas` 里读真实数据块

所以 `.utoc` 对应的是 TOC 文件，`FIoStoreTocResource` 对应的是 TOC 再内存中的表示

## IoStore 长啥样

### IoStore 容器 Toc 头 FIoStoreTocHeader

```cpp
// 摘要版，只挑关键字段
struct FIoStoreTocHeader
{
    static constexpr inline char TocMagicImg[] = "-==--==--==--==-";

    uint8   TocMagic[16];           // 魔数，用来识别这是 IoStore TOC 文件
    uint8   Version;                // 版本号 EIoStoreTocVersion
    uint32  TocHeaderSize;          // 头大小
    uint32  TocEntryCount;          // Chunk 条目数
    uint32  TocCompressedBlockEntryCount;
    uint32  TocCompressedBlockEntrySize;
    uint32  CompressionMethodNameCount;
    uint32  CompressionMethodNameLength;
    uint32  CompressionBlockSize;   // 压缩块大小
    uint32  DirectoryIndexSize;     // 目录索引数据大小
    uint32  PartitionCount;         // 分区数（容器可以被分成多个物理文件）
    FIoContainerId ContainerId;     // 容器 ID
    FGuid   EncryptionKeyGuid;      // 加密 key GUID
    EIoContainerFlags ContainerFlags;
    uint32  TocChunkPerfectHashSeedsCount;
    uint64  PartitionSize;
    uint32  TocChunksWithoutPerfectHashCount;
    uint64  Reserved8[5];

    void MakeMagic()    { FMemory::Memcpy(TocMagic, TocMagicImg, sizeof TocMagic); }
    bool CheckMagic() const { return FMemory::Memcmp(TocMagic, TocMagicImg, sizeof TocMagic) == 0; }
};
```

IoStore 容器有一个独立的 TOC 文件（就是 `.utoc`），里面存着

- 这个容器中有多少个 chunk(TocEntryCount)
- 每个 chunk 在大数据文件(`.ucas`) 里的偏移和长度
- 压缩块信息、支持的压缩算法、目录索引大小、分区数量等

### 压缩块描述 FIoStoreTocCompressedBlockEntry

`FIoStoreTocCompressedBlockEntry` 是 `IoStore` 的 `TOC` 里 **单个压缩块** 的描述结构

对应的压缩块就是 `.ucas` 容器文件里一段连续的压缩数据

`IoStore` 把很多 `Chunk` 的数据拼在这些压缩块里统一压缩/读取，通过这些 `entry` 来记录每个压缩块的位置、大小和压缩方式，从而实现高效的块级 IO 和解压

如果有 100 个 Chunk 自己独立压缩、独立存放，如果要同时加载这 100 个 Chunk，需要做 100 此随机 IO + 100 次解压

IoStore 的压缩方式，把多个 Chunk 数据合并到较大的 压缩快 中， IO 层按块读取，每个 Chunk 的读取可以共用同一个块的 IO，随机随小读变成较少次数的大顺序读取，磁盘利用率更高

```cpp
struct FIoStoreTocCompressedBlockEntry
{
    static constexpr uint32 OffsetBits = 40;
    static constexpr uint64 OffsetMask = (1ull << OffsetBits) - 1ull;
    static constexpr uint32 SizeBits = 24;
    static constexpr uint32 SizeMask = (1 << SizeBits) - 1;
    static constexpr uint32 SizeShift = 8;

    inline uint64 GetOffset() const { ... }
    inline void   SetOffset(uint64 InOffset) { ... }

    inline uint32 GetCompressedSize() const { ... }
    inline void   SetCompressedSize(uint32 InSize) { ... }

    inline uint32 GetUncompressedSize() const { ... }
    inline void   SetUncompressedSize(uint32 InSize) { ... }

    inline uint8  GetCompressionMethodIndex() const { ... }
    inline void   SetCompressionMethodIndex(uint8 InIndex) { ... }

private:
    // 5 字节偏移 + 3 字节压缩大小 + 3 字节未压缩大小 + 1 字节压缩方式
    uint8 Data[5 + 3 + 3 + 1];
};
```

IoStore 把容器数据按 **压缩块** 组织，而不是简单每个文件单独压缩

- 每个块有 40 位的 offset（支持大文件）
- 每个块有 24 位的压缩大小，24 位的未压缩大小
- 8 位的压缩算法索引

这些块在 TOC 中以数组形式存储，读取时可以快速定位某个 chunk 涉及哪些块

| 函数名 | 作用 |
| --- | --- |
| GetOffset | 该压缩块在 .ucas 容器文件里的起始偏移 |
| GetCompressedSize | 这个块压缩后的大小（字节） |
| GetUncompressedSize | 这个块解压后的大小（字节） |
| GetCompressionMethodIndex | 使用哪种压缩算法（通过索引到 CompressionMethods 数组） | 

> 通过  FFileIoStore::ReadBlocks 可以看到对 FFileIoStoreCompressedBlock 的使用

### 整个 TOC 资源 FIoStoreTocResource

```cpp
struct FIoStoreTocResource
{
    enum { CompressionMethodNameLen = 32 };

    FIoStoreTocHeader Header;

    TArray<FIoChunkId>                ChunkIds;
    TArray<FIoOffsetAndLength>        ChunkOffsetLengths;
    TArray<int32>                     ChunkPerfectHashSeeds;
    TArray<int32>                     ChunkIndicesWithoutPerfectHash;
    TArray<FIoStoreTocCompressedBlockEntry> CompressionBlocks;
    TArray<FName>                     CompressionMethods;
    FSHAHash                          SignatureHash;
    TArray<FSHAHash>                  ChunkBlockSignatures;
    TArray<FIoStoreTocEntryMeta>      ChunkMetas;
    TArray<uint8>                     DirectoryIndexBuffer;

    static FIoStatus Read(...);
    static TIoStatusOr<uint64> Write(...);
    static uint64 HashChunkIdWithSeed(int32 Seed, const FIoChunkId& ChunkId);
};
```

| 属性 | 作用 |
| --- | --- |
| ChunkIds | 这个容器里有哪些 `Chunk`（`FIoChunkId` 可以表示 任意类型的资源块 ，比如某个 Package、ShaderGroup 等） |
| ChunkOffsetLengths | 每个 Chunk 在 `.ucas` 里的偏移和长度 |
| ChunkPerfectHashSeeds + ChunkIndicesWithoutPerfectHash | 用于构建 完美哈希表，可以 O(1) 查到某个 Chunk 在 TOC 中的条目——也就是 IoStore 对查找性能的优化 |
| CompressionBlocks | 所有压缩块及其算法 |
| DirectoryIndexBuffer | 文件路径 -> ChunkId 的 目录索引 二进制数据 |
| Read/Write | 负责把这些数据读写到 *.utoc 文件 |

注意，这里 `ChunkOffsetLengths` 和 `FIoStoreTocCompressedBlockEntry` 的 `Offset` 和 `Size` 并不相同

假设：压缩快解压后大小上限 256 字节，容器中有 ChunkA 和 ChunkB，分别是 300 字节和 100 字节

Write 之后的布局可能是

- Block0 存放 ChunkA 的 前 256 字节
- Block1
  - 0 ~ 43 存放 ChunkA 的后 44 字节
  - 44 ~ 143 存档 ChunkB 的 100 字节

此时 FIoStoreTocResource::CompressionBlocks 中的数据大概是

- CompressionBlocks[0]
  - Offset = 0，在 .ucas 内的文件偏移
  - CompressedSize = 120， 压缩后 120 字节
  - UncompressedSize = 256
- CompressionBlocks[1]
  - Offset = 120，在 .ucas 内的文件偏移
  - CompressedSize = 90， 压缩后 100 字节
  - UncompressedSize = 144

> 这里的 Offset 和 Size 都是针对 .ucas 文件的物理坐标偏移和物理大小

此時 FIoStoreTocResource::ChunkOffsetLengths 存放的 Chunk 逻辑表数据是

- 把 Block 解压后的数据顺序排起来
  - Block0 解压后负压逻辑区间 `[0, 256)`
  - Block1 解压后正压逻辑区间 `[256, 400)`

在这个逻辑区间上

- ChunkA 从逻辑偏移 0 开始，占 300 字节
  - Offset = 0
  - Length = 300
- ChunkB 紧接着 A 的300 字节之后，占 100 字节
  - Offset = 30
  - Length = 100

从 `FFileIoStore::ReadBlocks` 函数可以看到数据的获取方式

```cpp
FFileIoStoreContainerFile* ContainerFile = ResolvedRequest.GetContainerFile();
const uint64 CompressionBlockSize = ContainerFile->CompressionBlockSize;
const uint64 RequestEndOffset = ResolvedRequest.ResolvedOffset + ResolvedRequest.ResolvedSize;
int32 RequestBeginBlockIndex = int32(ResolvedRequest.ResolvedOffset / CompressionBlockSize);
int32 RequestEndBlockIndex = int32((RequestEndOffset - 1) / CompressionBlockSize);

FFileIoStoreReadRequestList NewBlocks;

uint64 RequestStartOffsetInBlock = ResolvedRequest.ResolvedOffset - RequestBeginBlockIndex * CompressionBlockSize;
uint64 RequestRemainingBytes = ResolvedRequest.ResolvedSize;
uint64 OffsetInRequest = 0;
```

| 变量名 | 作用 |
| --- | --- |
| ContainerFile | 这个请求所在的 IoStore 容器（一个 .utoc+.ucas 对） |
| CompressionBlockSize | 容器中每个 **逻辑压缩块** 的解压后的最大大小 |
| ResolvedOffset / ResolvedSize | 这个请求的逻辑起始偏移和长度 |
| RequestBeginBlockIndex / RequestEndBlockIndex | 计算这个请求覆盖了哪些压缩块 |
| RequestStartOffsetInBlock | 在起始块中的 **块内偏移** ，如果请求不是从块的起点开始 |
| RequestRemainingBytes | 剩余还没覆盖的请求字节数 |

然后通过 RequestBeginBlockIndex 和 RequestEndBlockIndex 遍历涉及的所有压缩快
