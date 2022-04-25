# SGI空间配置器

## 1.代码文件解析

```txt
01-alloctest.cpp:STL源码解析书上p44页设计的简单空间配置器
02-alloc_book.cpp:跟着侯捷老师敲的配置器测试代码
03-std_allocator_test.cpp:SGI标准空间配置器
04-std_alloc_test.cpp:<stl_construct.h>的部分内容
05-__malloc_alloc_template.cpp:第一级配置器的剖析
06-__default_alloc_template.cpp:第二级配置器的剖析
```

## 2.具备配置力的SGI空间配置器解析

**普通的new操作：**

1. 调用 ::operator new 配置内存
2. 调用构造函数

**普通的delete操作：**

1. 调用析构函数
2. 调用 ::operator delete 释放内存

**而STL allocator 因为精密分工，把这两段操作区分开：**

- 由 alloc::allocate() 配置内存
- 由 alloc::deallocate() 释放内存
- 由 ::construct() 对象构造
- 由 ::destory() 对象析构

### 2.1 SGI配置器定义在\<memory>中，它其中的文件

- #include <stl_alloc.h> 负责内存空间的配置与释放，定义了一、二级配置器，彼此合作。
- #include <stl_construct.h> 定义全局函数，负责对象内容的构造与析构。
- #include <stl_uninitlized.h> 定义全局函数用来填充或复制大块内存数据。

### 2.2 构造和析构基本工具：construct()和destory()

- 在04-std_alloc_test.cpp中对这两个函数进行了详细的讲解。

### 2.3 空间的配置与释放

- C++的内存配置基础操作是::operator new()，内存的释放基本操作是::operator delete()。
- 而在C语言中，这两个操作是：malloc() 和 free()
- 对于空间配置与释放会有几个设计哲学：
  - 向system heap要求空间
  - 考虑多线程(multi-threads)状态(在此时先暂时排除这个状态)
  - 考虑内存不足时的应变措施
  - 考虑内存碎片问题

**关于一二级配置器的关系：**

- 对于内存碎片问题，SGI设计了双层级配置器：
  - 第一级配置器直接使用malloc()和free()
  - 第二级配置器则视情况采用不同的策略：
    - 配置区块超过128bytes时，视为足够大，采用第一级配置器。
    - 配置区域小于128bytes时，视为过小，为了降低额外负担，便采用复杂的memory pool 整理方式(会在2.5讲解)。
  - 只开放第一级配置器还是同时开放出第二级配置器会在04-std_alloc_test.cpp中讲解

- 一级配置器：
  - allocate()直接使用malloc()
  - deallocate()直接使用free()
  - 模拟C++的set_new_hadler()以处理内存不足的情况

- 二级配置器：
  - 维护16个自由链表(free lists)
  - 负责16种小型区块的次配置能力。
  - 内存池(memory pool)以malloc()配置而得到
  - 如果内存不足，则转调为一级配置器
  - 如果需求区块大于128bytes，就转调一级配置器。

### 2.4 第一级配置器剖析

主要的代码在:05-__malloc_alloc_template.cpp中

### 2.5 第二级配置器剖析

主要代码在:06-__default_alloc_template.cpp中

**注意：**

- 第二级配置器有一些机制，避免太多小额区块造成内存的碎块。
- 小于128bytes就以内存池来管理。
  - 每次配置一大块内存，维护一个free-list(自由链表)，有内存需求就由它给配置。
- 配置器负责配置和回收这些区块。

### 2.6 空间配置函数allocate()

- 将在06-__default_alloc_template.cpp中详细解释
- 此函数要判断区块的大小，决定是否调整到第一级配置器
- 对于小于128bytes 的检查对应的 free list 查看是否有可用区块(有就用，没有继续)
- 没有的话就把区块大小调整到8倍数的边界，调用refill()，为free list重新填充空间

### 2.7 空间释放函数deallocate()

回收空间，在06-__default_alloc_template.cpp中详细解释

### 2.8 重新填充 free lists

就是当需要的 free lists 中的块没有可用的了，这个时候就调用refill(),为free lists 重新填充空间(取自内存池，由chunk_alloc()完成)

- 将在06-__default_alloc_template.cpp中详细解释

### 2.9 内存池

- 将在06-__default_alloc_template.cpp中详细解释

**找内存的步骤：**

- 1.内存池里面剩余空间充足，要多少直接给多少
- 2.内存池里面不够，但是还有那么几个的，就先给最多能给的
- 3.内存池里面一个都没有了
  - 先把零头给了
  - 零头也没了，配置heap补充一下(这里应该是找free list里面没用的那些给它匀回来)
  - 调用第一级配置器，通过out-of-memory机制改善一下
