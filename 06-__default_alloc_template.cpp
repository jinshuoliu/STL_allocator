enum {__ALIGN = 8}; // 小型区块的上调边界
// 就是将小额区块的内存需求调到8的倍数
enum {__MAX_BYTES = 128}; // 小型区块的上限
// 维护了8、16、24、32、40、48、56、64、72、80、88、96、104、112、120、128这16个free-list的小额区块
enum {__NFREELISTS = __MAX_BYTES/__ALIGN}; // free-lists 个数

// 以下是第二级配置器
// 第一参数用于多线程环境下，不在此讨论
template <bool threads, int inst>
class __default_alloc_template {
private:
  // ROUND_UP() 将 bytes 上调至 8 的倍数
  static size_t ROUND_UP(size_t bytes) {
    return (((bytes) + __ALIGN - 1) & ~(__ALIGN - 1));
  }

private:
  // 注意这个被union(联合)关键字定义的类型
  // 被装入该类型定义的任何一种数据，共享一段内存。
  // 而 SGI 为了解决维护链表造成的额外负担使用它
  // 从第一个字段来看 obj可以被看做一个指针，
  union obj { // free-lists的节点构造
    union obj * free_list_link;
    char client_data[1]; /* The client sees this. */
  };

private:
  // 16个free-lists
  // volatile：它声明的类型变量表示可以被某些编译器未知的因素更改
  static obj * volatile free_list[__NFREELISTS];
  // 以下函数根据区块大小，决定使用第n号free-list. n从0开始
  static size_t FREELIST_INDEX(size_t bytes) {
    // bytes + __ALIGN - 1: 为了使区块大小满足8的整数倍(虽然加多了，但是除了8小数也就省略了)
    // 后面-1是为了从0开始
    return (((bytes) + __ALIGN-1) / __ALIGN -1);
  }

  // 返回一个大小为n的对象，并可能加入大小为n的其它区块到free list
  static void *refill(size_t n);
  // 配置一大块空间，可容纳nobjs个大小为"size"的区块
  // 如果配置nbjs个区块有所不便，nobjs可能会降低
  static char *chunk_alloc(size_T size, int &nobjs);

  // Chunk allocation state
  static char *start_free; // 内存池起始位置，只在chunk_alloc()中变化
  static char *end_free; // 内存池结束位置，只在chunk_alloc()中变化
  static size_t heap_size;

public:
  /* 空间配置函数 */
  // n must be >0
  static void * allocate(size_t n )
  {
    obj * volatile * my_free_list;
    obj * result;

    // 大于128就调节第一级配置器
    if (n>(size_t) __MAX_BYTES) {
      return (malloc_alloc::allocate(n));
    }

    // 寻找16个 free lists 中适合的一个
    my_free_list = free_list + FREELIST_INDEX(n);
    result = *my_free_list;
    if (result == 0) {
      // 没找到可用的 free list 准备填充free list
      void *r = refill(ROUND_UP(n));
      return r;
    }

    // 调整freee list
    *my_free_list = result -> free_list_link;
    return (result);
  }

  /* 空间释放函数 */
  static void deallocate(void *p, size_t n) {
    obj *q = (obj *)p;
    obj * volatile * my_free_list;

    // 大于128就调用第一级配置器
    if (n>(size_t)__MAX_BYTES) {
      malloc_alloc::deallocate(p, n);
      return;
    }

    // 寻找对应的free list
    my_free_list = free_list + FREELIST_INDEX(n);
    // 调整free list 回收区块
    q -> free_list_link = *my_free_list;
    *my_free_list = q;
  }
  static void *reallocate(void *p, size_t old_sz, size_t new_sz);

  
};

// 以下是 static data member 的定义与初值设定
template <bool threads, int inst>
char *__default_alloc_template<threads, inst>::start_free = 0;

template <bool threads, int inst>
char *__default_alloc_template<threads, inst>::end_free = 0;

template <bool threads, int inst>
size_t __default_alloc_template<threads, inst>::heap_size = 0;

template <bool threads, int inst>
__default_alloc_template<threads, inst>::obj * volatile
__default_alloc_template<threads, inst>::free_list[__NFREELISTS] = 
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };

/**
 * @brief 重新填充free lists
 * 功能是在free lists有些需要的区块没有空闲时重新获得一些区块
 * @tparam threads 
 * @tparam inst 
 * @param n 
 * @return void* 
 */
template <bool threads, int inst>
void *__default_alloc_template<threads, inst>::refill(size_t n) {
  int nobjs = 20;
  // 调用chunk_alloc()，尝试取得nobjs个区块作为free list 的新节点
  // 注意参数nobjs是pass by reference
  char * chunk = chunk_alloc(n, nobjs);
  obj * voltaile * my_free_list;
  obj * result;
  obj * current_obj, * next_obj;
  int i;

  // 如果只获得一个区块，这个区块就分配给调用者使用，free lists 无新节点
  if(1 == nobjs)
    return (chunk);
  
  // 否则准备调整free lists，纳入新节点
  my_free_list = free_list + FREELIST_INDEX(n);

  // 以下在chunk空间内建立free lists
  result = (obj *)chunk; // 这一块准备返回给客端
  // 以下导引free list指向新配置的空间(取自内存池)
  *my_free_list = next_obj = (obj *)(chunk+n);
  // 以下导引free list的个节点串联起来
  for(i = 1; ; i++) { // 从1开始，因为第0个将返回给客端
    current_obj = next_obj;
    next_obj = (obj *)((char *)next_obj + n);
    if(nobjs - 1 == i) {
      current_obj -> free_list_link = 0;
      break;
    } else {
      current_obj -> free_list_link = next_obj;
    }
  }
  return (result);
}

template <bool threads, int inst>
char * __default_alloc_template<threads, inst>::chunk_alloc(size_t size, int& nobjs) {
  char * result;
  size_t total_bytes = size * nobjsl
  size_t bytes_left = end_free - start_free; // 内存池剩余空间

  if (bytes_left >= total_bytes) {
    // 内存池剩余空间充裕
    result = start_free;
    start_free += total_bytes;
    return (result);
  } else if (bytes_left >= size) {
    // 内存池剩余空间不足，但是还可以提供至少一个区块
    nobjs = bytes_left/size;
    total_bytes = size * nobjs;
    result = start_free;
    start_free += total_bytesl
    return (result);
  } else {
    // 内存池空间一个区块都没了
    size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
    // 尝试让内存池中剩下的那一丝空间有利用价值
    if (bytes_left > 0) {
      // 内存池里面还有点，先分配给合适的free list
      // 首先寻找适当的free list
      obj * volatile * my_free_list = 
      free_list + FREELIST_INDEX(bytes_left);
      // 调整free list，将内存池中的剩余空间编进来
      ((obj *)start_free) -> free_list_link = *my_free_list;
      *my_free_list = (obj *)start_free;
    }

    // 配置heap空间，用来补充内存池
    start_free = (char *)malloc(bytes_to_get);
    if(0 == start_free) {
      // heap空间不足，malloc()失败
      int i;
      obj * volatile * my_free_list, *p;
      // 试着检查我们手上拥有的东西
      // 不打算尝试配置较小的区块，因为那在多进程机器上容易导致灾难
      // 搜索适当的free list(未用的区块，而且空间足够大)
      for (i=size; i<=__MAX_BYTES; i+=__ALIGN) {
        my_free_list = free_list + FREELIST_INDEX(i);
        p = *my_free_list;
        if(0 != p) { // 操作free list内未使用的区块
          // 调整free list 以释放出未使用的区块
          *my_free_list = p -> free_list_link;
          start_free = (char *)p;
           end_free = start_free + i;
           // 递归调用自己(为了修正nobjs)
           return (chunk_alloc(size,nobjs));
           // 注意：任何参与零头都将被编入free list中备用
        }
      }
      end_free = 0; // 山穷水尽了，哪里都没内存了
      // 调用第一级配置器，看看out-of-memory机制能否帮下忙
      start_free = (char *)malloc_alloc::allocate(bytes_to_get);
      // 这回导致抛出异常，或内存不足的情况获得改善
      heap_size += bytes_to_get;
      end_free = start_free + bytes_to_get;
      // 递归调用自己，修正nobjs
      return (chunk_alloc(size, nobjs));
    }
  }
  
}