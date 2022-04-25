#include <iostream>
using namespace std;

/**
 * @brief 
 * 分配器的效率影响内存
 * 他是为容器服务的
 * 
 */
/*
// namespace leeknew{
  void *operator new(size_t size, const std::nothrow _t&)
    _THROW0()
    {
      // try to allocate size bytes
      void *p;
      while((p = malloc(size) == 0))
      {
        // buy more memory or return null pointer
        _TRY_BEGIN
        if(_callnewh(size) ==0) break;
        _CATCH(std::bad_alloc) return (0);
        _CATCH_END
      }
      return (p);
    }

  // inline versions of the nothrow_t versions of new & delete operators
  inline void * _RTLENTRY operator new (size_t size, const std::nethrow_t &)
  {
    size = size?size:1;
    return malloc(size);
  }
// }
*/
/**/
// #ifdef _FARQ
#define _FARQ
#define _PDET ptrdiff_t
#define _SIZT size_t
// #endif
#define _POINIER_X(T,A) T _FARQ*
#define _PEFERENCE_X(T,A) T _FARQ&

namespace leekalloc{
  template<class _Ty>
  class allocator{
    public:
    typedef _SIZE size_type;
    typedef _PDFT difference_type;
    typedef _Ty _FARQ *pointer;
    typedef _Ty value_type;
    pointer allocate(size_type _N, const void *)
    {
      return (_Allocate((difference_type) _N, (pointer)0));
    }
    void deallocate(void _FARQ * _P, size_type){
      operator delete(_P);
    }
  };

  template<class _Ty> inline
  _Ty _FARQ *_Allocate(_PDFT _N, _Ty _FARQ *)
  {
    if(_N < 0) _N = 0;
    return ((_Ty _FARQ *) operator new ((_SIZE)_N * sizeof (_Ty)));
  }

}
/**/

// #ifdef _JJALLOC_
// #define _JJALLOC_
#include <new>
#include <cstddef>
#include <cstdlib>
#include <climits>
#include <iostream>

namespace leekJJ{
  template <class T>
  inline T* _allocate(ptrdiff_t size, T*){
    set_new_handler(0);
    T* tmp = (T*)(::operator new((size_t)(size * sizeof(T))));
    if(tmp == 0){
      cerr << "out of memory" << endl;
      exit(1);
    }
    return tmp;
  }

  template <class T>
  inline void _deallocate(T* buffer){
    ::operator delete(buffer);
  }

  template <class T1, class T2>
  inline void _construct(T1* p, const T2& value){
    new(p) T1(value); // placement new. invoke ctor of T1
  }

  template <class T>
  inline void _destory(T* ptr){
    ptr->~T();
  }

  template <class T>
  class allocator {
    public:
      typedef T value_type;
      typedef T* pointer;
      typedef const T* const_pointer;
      typedef T& reference;
      typedef const T& const_reference;
      typedef size_t size_type;
      typedef ptrdiff_t difference_type;

      //rebind allocator of type U
      template <class U>
      struct rebind{
        typedef allocator<U> other;
      };

      //hint used for locality. ref.[Austern],p189
      pointer allocate(size_type n, const void* hint=0){
        return _allocate((difference_type)n, (pointer)0);
      }

      void deallocate(pointer p, size_type n){
        _deallocate(p);
      }

      void construct(pointer p, const T& value){
        _construct(p, value);
      }

      void destory(pointer p){
        _destory(p);
      }

      pointer address(reference x){
        return (pointer)&x;
      }

      const_pointer const_address(const_reference x){
        return (const_pointer)&x;
      }

      size_type max_size() const{
        return size_type(UINT_MAX/sizeof(T));
      }
  };

} // end of namespace leekJJ

// #endif // _JJALLOC_

#include <vector>


// int main()
// {
//   // 分配512个int,但是出错了
//   int* p = leekalloc::allocator<int>().allocate(512,(int*)0);
//   leekalloc::allocator<int>().deallocate(p, 512);

//   // 运行leekJJ的内容
//   int ia[5] = {0,1,2,3,4};
//   unsigned int i;

//   vector<int,leekJJ::allocator<int>> iv(ia, ia+5);
//   for(i=0; i<iv.size(); i++)
//     cout << iv[i] << ' ';
//   cout << endl;

//   system("pause");
//   return 0;
// }


