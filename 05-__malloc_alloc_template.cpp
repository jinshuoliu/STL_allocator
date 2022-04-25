#if 0
# include <new>
# define __THROW_BAD_ALLOC throw bad_alloc // 定义的要丢出的异常信息，会在oom_malloc()和oom_realloc()中被用到 
#elif !defined(__THROW_BAD_ALLOC)
# include <iostream> // 原文为# include <iostream.h> 但会报错
# define __THROW_BAD_ALLOC cerr << "out of memory" << endl; exit(1)
#endif

// 第一级配置器通常会比第二级配置器的速度慢
// 以下是第一级配置器
template<int inst>
class __malloc_alloc_template {
private:
  // 以下函数用于处理内存不足的情况
  // oom : out of memory
  static void *oom_malloc(size_t);
  static void *oom_realloc(void *, size_t);
  static void (* __malloc_alloc_oom_handler) ();

public:
  // 以及配置器的主要操作

  // 内存配置
  static void * allocate(size_t n) {
    void *result ::malloc(n); // 第一级配置器直接使用malloc()
    // 当无法满足需求的时候，改为oom_malloc()
    if (0 == result)
      result = oom_malloc(n);
    return result;
  }

  // 内存释放
  static void deallocate(void *p, size_t /* n */){
    // 直接使用free()
    free(p);
  }

  // 内存重配置
  static void *reallocate(void *p, size_t /* old_sz */, size_t new_sz) {
    // 直接使用realloc(), realloc()用于重新分配内存空间
    void * result = realloc(p, new_sz); 
    // 如果无法满足需求，改为oom_realloc()
    if (0 == result)
      result = oom_realloc(p, new_sz);
    return result;
  }

  // 以下仿真C++的 set_New_handler().
  // 就是说可以通过它指定自己的 out-of-memory handler
  static void (* set_malloc_handler(void (*f)()))(){
    void (* old)() = __malloc_alloc_oom_handler;
    __malloc_alloc_oom_handler = f;
    return (old);
  }

};

/**
 * @brief 无法直接malloc()、free()等操作后所执行的操作
 * 这里面还运用了 C++ new-handler 机制：
 *  可以要求系统再内存配置需求无法被满足的时候，调用一个你所指定的函数。
 */

// malloc_alloc out-of-memory handling
// 初值为0. 有待客户端设定
template <int inst>
void (* __malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;

// oom_malloc内存配置的方法
template <int inst>
void * __malloc_alloc_template<inst>::oom_malloc(size_t n) {
  void (* my_malloc_handler)();
  void *result;

  for(;;) { // 不断尝试释放、配置、再释放、再配置...
    my_malloc_handler = __malloc_alloc_oom_handler;
    if (0 == my_malloc_handler) { // 内存不足处理例程，只能丢出异常信息
      __THROW_BAD_ALLOC;
    }
    (*my_malloc_handler)(); // 调用处理历程，企图释放内存
    result = malloc(n); // 再次尝试配置内存
    if(result)
      return (result);
  }
}

// oom_realloc内存重配置的方法
template <int inst>
void * __malloc_alloc_template<inst>::oom_realloc(void *p, size_t n) {
  void (* my_malloc_handler)();
  void *result;

  for(;;) { // 不断尝试释放、配置、再释放、再配置...
    my_malloc_handler = __malloc_alloc_oom_handler;
    if (0 == my_malloc_handler) {
      __THROW_BAD_ALLOC;
    }
    (*my_malloc_handler)(); //调用处理历程，企图释放内存
    result = realloc(p, n); // 再次尝试配置内存
    if(result)
      return (result);
  }
}

// 注意，以下直接将参数inst指定为0
typedef __malloc_alloc_template<0> malloc_alloc;  