/**
 * @file 04-std_alloc_test.cpp
 * @author 951395171@qq.com
 * @brief 第一部分：构造和析构的讲解
 * @version 0.1
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */
// <stl_construct.h>的部分内容
#include <new.h> // 欲使用placement new 需要先包含此文件

// 接受一个指针和一个初值value，它的作用是将初值设定到指针所指的空间上。
template <class T1, class T2>
inline void construct(T1* p, const T2& value) {
  new (p) T1 (value); // placement new; 调用T1::T1(value);
}

// 以下是 destory() 第一版本，接受一个指针，就直接调用该行对象的析构函数即可。
template <class T>
inline void destory(T* pointer) {
  pointer->~T(); // 调用 dtor ~T()
}

// 以下是 destory() 第二版本，接受两个迭代器。此函数设法找出元素的数值型别
// 这个版本是将 [first,last) 范围内的所有对象析构掉。
// 这里为了不一次次的调用析构函数，提升效率，就先获取迭代器所指向对象的型别，若是(__true_type)就直接结束，若是(__false_type),就循环的调用destory()
// 进而利用 __type_traits<> 求取最适当措施
template <class ForwardIterator>
inline void destory(ForwardIterator first, ForwardIterator last) {
  __destory(first, last, value_type(first));
}

// 判断元素的沪指型别(value type) 是否有 trivial destructor
template <class ForwardIterator, class T>
inline void __destory(ForwardIterator first, ForwardIterator last, T*) {
  typedef typename __type_traits<T>::hash_trivial_destructor trivial_destructor;
  __destoory_aux(first, last, trivial_destructor());
}

// 如果元素的数值型别(value type) 有 non-trivial destructor...
template <class ForwardIterator>
inline void __destory_aux(ForwardIterator first, ForwardIterator last, __false_type) {
  for( ; first < last; ++first)
    destory(&*first);
}

// 如果元素的数值型别有 trivial destructor...
template <class ForwardIterator>
inline void __destory_aux(ForwardIterator, ForwardIterator, __true_type) {}

// 以下是destory() 第二版本针对迭代器为 char* 和 wchar_t* 的特化版本
inline void destory(char*, char*) {}
inline void destory(wchar_t*, wchar_t*) {}

/**
 * @brief 第二部分：空间配置和释放的讲解
 * 其中：
 *    __malloc_alloc_template就是第一级配置器
 *    __default_alloc_template就是第二级配置器
 */
// 只开放第一级配置器还是同时开放出第二级配置器，取决于 __USE_MALLOC是否被定义
#ifdef __USE_MALLOC

typedef __malloc_alloc_template<0> malloc_alloc;
typedef malloc_alloc alloc; // 令alloc为第一级配置器

#else

// 令alloc为第二级配置器
typedef __default_alloc_template<__NODE_ALLOCATOR_THERADS, 0> alloc;
#endif /* ! __USE_MALLOC */

// 无论alloc是第一级还是第二级配置器，SGI都为它包装一个接口，使配置器的接口能符合STL规格：
/**
 * @brief 
 * 其内部的四个成员函数都是淡出的转调用，调用传递给适配器的成员函数。
 * 这个接口是使适配器的配置单位从bytes转为其他元素的大小。
 * @tparam T 
 * @tparam Alloc 
 */
template<class T, class Alloc>
class simple_alloc {
public:
  static T *allocate(size_t n) {
    return 0 == n ? 0 : (T*)Alloc::allocate(n*sizeof(T));
  }
  static T *allocate(void) {
    return (T*)Alloc::allocate(sizeof(T));
  }
  static void deallocate(T *p, size_t n) {
    if(0 != n)
      Alloc::deallocate(p, n*sizeof(T));
  }
  static void deallocate(T *p) {
    Alloc::deallocate(p, sizeof(T));
  } 
};

// 所有的SGI STL容器全部都使用这个simple_alloc接口。
// 举个例子
template<class T, class Alloc = alloc>
class vector {
  protected:
  // 专属空间配置器，每次配置一个元素的大小
  typedef simple_alloc<value_type, Alloc> data_allocator;

  void deallocate() {
    if(...)
      data_allocator::deallocate(start, end_of_storage -start);
  }
  ...
};


// 第一级配置器与第二级配置器的包装接口和运用方式
// 由__USE_MALLOC来决定使用哪一级配置器
// 实际运用方式
// 这里的alloc就是定义一二级配置器得到的alloc
template<class T, class Alloc=alloc>
class vector {
  // 1
  typedef simple_alloc<T, Alloc> data_allocator;
  data_alloctor:;allocate(n); // 配置n个元素
  // 配置完成后，接下来必须设定初值
};

// 这里的alloc就是定义一二级配置器得到的alloc
template<class T, class Alloc=alloc, size_t BufSiz=0>
class deque {
  // 1
  typedef simple_alloc<T, Alloc> data_allocator;
  typedef simple_alloc<T*, Alloc> map_allocator;
  data_allocator::allocate(n); // 配置n个元素
  map_allocator::allocate(n); // 配置n个节点
  // 配置完成后，接下来必须设定初值
};

// 标 1 都是从这里调用的simple_alloc
template<class T,class Alloc>
class simple_alloc {
public:
  static T *allocate(size_t);
  static T *allocate(void);
  static void deallocate(T*, size_t);
  static void deallocate(T*);
}
