/**
 * @brief 
 * 
 * @tparam ForwardIterator 
 * @tparam Size 
 * @tparam T 
 * @param first 迭代器first指向欲初始化空间的起点
 * @param n n表示欲初始化空间的大小
 * @param x x表示初值
 * @return ForwardIterator 
 */
template <class ForwardIterator, class Size, class T>
inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& x){
  // 利用value_type()取出first的value type
  return __uninitialized_fill_n(first, n, x, value_type(first));
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
 * @param n 
 * @param x 
 * @return ForwardIterator 
 */
template <class ForwardIterator, class Size, class T, class T1>
inline ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n, const T& x, T1*){
  typedef typename __type_traits<T1>::is_POD_type is_POD;
  return __uninitialized_fill_n_aux(first, n, x, is_POD());
}

// 如果copy construction等同于assignment，而且destructor是trivial，以下就有效
/**
 * @brief 如果是POD型别
 * 这个是由function template的参数推导机制而得
 * @tparam ForwardIterator 
 * @tparam Size 
 * @tparam T 
 * @param first 
 * @param n 
 * @param x 
 * @return ForwardIterator 
 */
template <class ForwardIterator, class Size, class T>
inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __true_type) {
  return fill_n(first, n, x); // 交给高阶函数执行
}

/**
 * @brief 不是POD型别
 * 
 * @tparam ForwardIterator 
 * @tparam Size 
 * @tparam T 
 * @param first 
 * @param n 
 * @param x 
 * @return ForwardIterator 
 */
template <class ForwardIterator, class Size, class T>
ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __false_type) {
  ForwwardIterator cur = first;
  // 应有异常处理
  for(; n>0; --n, ++cur)
    construct(&*cur, x);
  return cur;
}