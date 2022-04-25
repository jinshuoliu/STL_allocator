/**
 * @brief 
 * 分配器（配置器）allocator
 * 
 */

/* 
// 默认的
template<typename _Tp, typename _Alloc=std::allocator<_Tp>>
class vector : protected _Vector_base<_Tp, _Alloc>

template<typename _Tp, typename _Alloc=std::allocator<_Tp>>
class list : protected _List_base<_Tp, _Alloc>

template<typename _Tp, typename _Alloc=std::allocator<_Tp>>
class Deque : protected _Deque_base<_Tp, _Alloc>

template<typename _Key, typename _Compare=std::less<_Key>,
          typename _Alloc=std::allocator<_Key>>
class set

template<typename _Key, typename_Tp, typename _Compare=std::less<_Key>,
          typename _Alloc=std::allocator<std::pair<const _Key, _Tp>>
class map

template<class _Value,
          class _Hash=hash<_Value>,
          class _Pred=std::equal_to<_Value>,
          class _Alloc=std::allocator<_Value>>
class unordered_set

template<class _Key, class _Tp,
          class _Hash=hash<_Key>,
          class _Pred=std::equal_to<_Value>,
          class _Alloc=std::allocator<std::pair<const _Key, _Tp>>
class unordered_map
*/

#include <list>
#include <stdexcept>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <iostream>
#include <ctime>

#include <cstddef>
#include <memory>
// 使用std::allocator以外的allocator，需要自行导入
#include <ext/array_allocator.h>
#include <ext/mt_allocator.h>
#include <ext/debug_allocator.h>
#include <ext/pool_allocator.h>
#include <ext/bitmap_allocator.h>
#include <ext/malloc_allocator.h>
#include <ext/new_allocator.h>
using namespace std;

namespace leeklist_alloc{
  void test_list_with_special_allocator(){
    cout << "\ntest_list_with_special_allocator()............\n";

    list <string, allocator<string>> c1;
    list <string, __gnu_cxx::malloc_allocator<string>> c2;
    list <string, __gnu_cxx::new_allocator<string>> c3;
    list <string, __gnu_cxx::__pool_alloc<string>> c4;
    list <string, __gnu_cxx::__mt_alloc<string>> c5;
    list <string, __gnu_cxx::bitmap_allocator<string>> c6;

    int choice;
    long value;

    cout << "select:";
    cin >> choice;
    if(choice != 0) {
      cout << "how many elements: ";
      cin >> value;
    }

    char buf[10];
    clock_t timeStart = clock();
    for(long i=0;i<value;++i){
      try{
        snprintf(buf, 10, "%ld", i);
        switch(choice){
          case 1:
            c1.push_back(string(buf));
            break;
          case 2:
            c2.push_back(string(buf));
            break;
          case 3:
            c3.push_back(string(buf));
            break;
          case 4:
            c4.push_back(string(buf));
            break;
          case 5:
            c5.push_back(string(buf));
            break;
          case 6:
            c6.push_back(string(buf));
            break;
          default:
            break;
        }
      }
      catch(exception& p) {
        cout << "i=" << i << " " << p.what() << endl;
        abort();
      }
    }
    // 查看使用双向链表存放一定量的数据的时候，各个分配器的耗时
    cout << "a lot of push_back(),milli-seconds : " << (clock()-timeStart) << endl;

    int* p;
    allocator<int> alloc1;
    p = alloc1.allocate(1);
    alloc1.deallocate(p, 1);

    __gnu_cxx::malloc_allocator<int> alloc2;
    p = alloc2.allocate(1);
    alloc2.deallocate(p, 1);

    __gnu_cxx::new_allocator<int> alloc3;
    p = alloc3.allocate(1);
    alloc3.deallocate(p, 1);

    __gnu_cxx::__pool_alloc<int> alloc4;
    p = alloc4.allocate(2);
    alloc4.deallocate(p, 2);

    __gnu_cxx::__mt_alloc<int> alloc5;
    p = alloc5.allocate(1);
    alloc5.deallocate(p, 1);

    __gnu_cxx::bitmap_allocator<int> alloc6;
    p = alloc6.allocate(3);
    alloc6.deallocate(p, 3);

  }
}

// int main()
// {
//   leeklist_alloc::test_list_with_special_allocator();


//   system("pause");
//   return 0;
// }
