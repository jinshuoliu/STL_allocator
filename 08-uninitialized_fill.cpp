template <class ForwardIterator, class T>
void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x){
  // 利用value_type()取出first的value type
  return __uninitialized_fill(first, last, x, value_type(first));
}

template <class ForwardIterator, class T, class T1>
inline void __uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x, T1*){
  typedef typename __type_traits<T1>::is_POD_type is_POD;
  return __uninitialized_fill_aux(first, last, x, is_POD());
}


template <class ForwardIterator, class T>
inline void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, __true_type) {
  return fill(first, last, x); // 交给高阶函数执行
}


template <class ForwardIterator, class T>
void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, __false_type) {
  ForwwardIterator cur = first;
  // 应有异常处理
  for(; n>0; --n, ++cur)
    construct(&*cur, x);
}