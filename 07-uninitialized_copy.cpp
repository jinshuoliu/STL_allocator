/**
 * @brief 
 * 
 * @tparam InputIterator 
 * @tparam ForwardIterator 
 * @param first 指向输入端的起始位置
 * @param last 指向结束位置，前闭后开
 * @param result 输入端的起始处(欲初始化空间)
 * @return ForwardIterator 
 */
template <class InputIterator, class ForwardIterator>
ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result){
  return __uninitialized_copy(first, last, result, value_type(result)); // 利用value_type()取出first的value type
}

/**
 * @brief 
 * 这个函数的逻辑就是：先萃取出迭代器first的value type，然后判断该型别是否为POD型别
 * POD：Plain Old Data(标量型别、传统的C struct型别)
 * 必然拥有trivial ctor/dtor/copy/assignment函数。
 * 可以对POD型别采用最有效的初值填写手法，而对non-POD型别采用保险安全的做法
 * @tparam ForwardIterator 
 * @tparam Size 
 * @tparam T 
 * @tparam T1 
 * @param first 
 * @param last 
 * @param result 
 * @return ForwardIterator 
 */
template <class InputIterator, class ForwardIterator, class T>
inline ForwardIterator __uninitialized_copy(ForwardIterator first, ForwardIterator last, ForwardIterator result, T*){
  typedef typename __type_traits<T1>::is_POD_type is_POD;
  return __uninitialized_copy_aux(first, last, result, is_POD());
}

/**
 * @brief 是POD型别
 * 
 * @tparam InputIterator 
 * @tparam ForwardIterator 
 * @param first 
 * @param last 
 * @param result 
 * @return ForwardIterator 
 */
template <class InputIterator, class ForwardIterator>
inline ForwardIterator __uninitialized_copy_aux(ForwardIterator first, ForwardIterator last, ForwardIterator result, __true_type) {
  return copy(first, last, result); // 交给高阶函数执行
}

/**
 * @brief 不是POD型别
 * 
 * @tparam InputIterator 
 * @tparam ForwardIterator 
 * @param first 
 * @param last 
 * @param result 
 * @return ForwardIterator 
 */
template <class InputIterator, class ForwardIterator>
ForwardIterator __uninitialized_copy_aux(ForwardIterator first, ForwardIterator last, ForwardIterator result, __false_type) {
  ForwwardIterator cur = result;
  // 应有异常处理
  for(; n>0; --n, ++cur)
    construct(&*cur, *first);
  return cur;
}

// 针对char*和wchar_t*两种型别，可以采用最具效率的memmove来执行复制行为
// memmove：直接移动内存内容
/**
 * @brief 针对const char*的特化版本
 * 
 * @param first 
 * @param last 
 * @param result 
 * @return char* 
 */
inline char* uninitialized_copy(const char* first, const char* last, char* result){
  memmove(result, first, last-first);
  return result + (last-first);
}

/**
 * @brief 针对const wchar_t*的特化版本
 * 
 * @param first 
 * @param last 
 * @param result 
 * @return wchar_t* 
 */
inline wchar_t* uninitialized_copy(const wchar_t* first, const wchar_t* last, wchar_t* result) {
  memove(result, first, sizeof(wchar_t)*(last-first));
  return result+(last-first);
}