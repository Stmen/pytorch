#pragma once

#include <c10/macros/Macros.h>
#include <c10/util/Exception.h>

namespace at { namespace native {

template <typename T>
struct DefaultPtrTraits {
  using PtrType = T*;
};

#if (defined(_WIN32) || defined(_WIN64))
#define RESTRICT __restrict
#else
#define RESTRICT __restrict__
#endif

template <typename T>
struct RestrictPtrTraits {
  using PtrType = T* RESTRICT;
};

template <
  typename T,
  typename index_t = int64_t,
  template <typename U> class PtrTraits = DefaultPtrTraits
>
class ConstStridedRandomAccessor {
public:
  using difference_type = ptrdiff_t;
  using value_type = const T;
  using pointer = const typename PtrTraits<T>::PtrType;
  using reference = const value_type&;
  using iterator_category = std::random_access_iterator_tag;

  using PtrType = typename PtrTraits<T>::PtrType;

  // Constructors {
  C10_HOST_DEVICE
  ConstStridedRandomAccessor(PtrType ptr, index_t stride)
    : ptr{ptr}, stride{stride}
  {}

  C10_HOST_DEVICE
  explicit ConstStridedRandomAccessor(PtrType ptr)
    : ptr{ptr}, stride{static_cast<index_t>(1)}
  {}

  C10_HOST_DEVICE
  ConstStridedRandomAccessor()
    : ptr{nullptr}, stride{static_cast<index_t>(1)}
  {}
  // }

  // Pointer-like operations {
  C10_HOST_DEVICE
  reference operator*() const {
    return *ptr;
  }

  C10_HOST_DEVICE
  const value_type* operator->() const {
    return reinterpret_cast<const value_type*>(ptr);
  }

  C10_HOST_DEVICE
  reference operator[](index_t idx) const {
    return ptr[idx * stride];
  }
  // }

  // Prefix/postfix increment/decrement {
  C10_HOST_DEVICE
  ConstStridedRandomAccessor& operator++() {
    ptr += stride;
    return *this;
  }

  C10_HOST_DEVICE
  ConstStridedRandomAccessor operator++(int) {
    auto copy = *this;
    ++*this;
    return copy;
  }

  C10_HOST_DEVICE
  ConstStridedRandomAccessor& operator--() {
    ptr -= stride;
    return *this;
  }

  C10_HOST_DEVICE
  ConstStridedRandomAccessor operator--(int) {
    auto copy = *this;
    --*this;
    return copy;
  }
  // }

  // Arithmetic operations {
  C10_HOST_DEVICE
  ConstStridedRandomAccessor& operator+=(index_t offset) {
    ptr += offset * stride;
    return *this;
  }

  C10_HOST_DEVICE
  ConstStridedRandomAccessor operator+(index_t offset) const {
    return ConstStridedRandomAccessor(ptr + offset * stride, stride);
  }

  C10_HOST_DEVICE
  friend ConstStridedRandomAccessor operator+(
    index_t offset,
    const ConstStridedRandomAccessor& accessor
  ) {
    return accessor + offset;
  }

  C10_HOST_DEVICE
  ConstStridedRandomAccessor& operator-=(index_t offset) {
    ptr -= offset * stride;
    return *this;
  }

  C10_HOST_DEVICE
  ConstStridedRandomAccessor operator-(index_t offset) const {
    return ConstStridedRandomAccessor(ptr - offset * stride, stride);
  }

  C10_HOST_DEVICE
  difference_type operator-(const ConstStridedRandomAccessor& other) const {
    return ptr - other.ptr;
  }
  // }
  
  // Comparison operators {
  C10_HOST_DEVICE
  bool operator==(const ConstStridedRandomAccessor& other) const {
    return (ptr == other.ptr) && (stride == other.stride);
  }

  C10_HOST_DEVICE
  bool operator!=(const ConstStridedRandomAccessor& other) const {
    return !(*this == other);
  }

  C10_HOST_DEVICE
  bool operator<(const ConstStridedRandomAccessor& other) const {
    return ptr < other.ptr;
  }

  C10_HOST_DEVICE
  bool operator<=(const ConstStridedRandomAccessor& other) const {
    return (*this < other) || (*this == other);
  }

  C10_HOST_DEVICE
  bool operator>(const ConstStridedRandomAccessor& other) const {
    return !(*this <= other);
  }

  C10_HOST_DEVICE
  bool operator>=(const ConstStridedRandomAccessor& other) const {
    return !(*this < other);
  }
  // }
  
protected:
  PtrType ptr;
  index_t stride;
};

template <
  typename T,
  typename index_t = int64_t,
  template <typename U> class PtrTraits = DefaultPtrTraits
>
class StridedRandomAccessor 
  : public ConstStridedRandomAccessor<T, index_t, PtrTraits> {
public:
  using difference_type = ptrdiff_t;
  using value_type = T;
  using pointer = typename PtrTraits<T>::PtrType;
  using reference = value_type&;

  using BaseType = ConstStridedRandomAccessor<T, index_t, PtrTraits>;
  using PtrType = typename PtrTraits<T>::PtrType;

  // Constructors {
  C10_HOST_DEVICE
  StridedRandomAccessor(PtrType ptr, index_t stride)
    : BaseType(ptr, stride)
  {}

  C10_HOST_DEVICE
  explicit StridedRandomAccessor(PtrType ptr)
    : BaseType(ptr)
  {}

  C10_HOST_DEVICE
  StridedRandomAccessor()
    : BaseType()
  {}
  // }

  // Pointer-like operations {
  C10_HOST_DEVICE
  reference operator*() const {
    return *this->ptr;
  }

  C10_HOST_DEVICE
  value_type* operator->() const {
    return reinterpret_cast<value_type*>(this->ptr);
  }

  C10_HOST_DEVICE
  reference operator[](index_t idx) const {
    return this->ptr[idx * this->stride];
  }

  C10_HOST_DEVICE
  const reference operator[](index_t idx) {
    return this->ptr[idx * this->stride];
  }
  // }

  // Prefix/postfix increment/decrement {
  C10_HOST_DEVICE
  StridedRandomAccessor& operator++() {
    this->ptr += this->stride;
    return *this;
  }

  C10_HOST_DEVICE
  StridedRandomAccessor operator++(int) {
    auto copy = *this;
    ++*this;
    return copy;
  }

  C10_HOST_DEVICE
  StridedRandomAccessor& operator--() {
    this->ptr -= this->stride;
    return *this;
  }

  C10_HOST_DEVICE
  StridedRandomAccessor operator--(int) {
    auto copy = *this;
    --*this;
    return copy;
  }
  // }

  // Arithmetic operations {
  C10_HOST_DEVICE
  StridedRandomAccessor& operator+=(index_t offset) {
    this->ptr += offset * this->stride;
    return *this;
  }

  C10_HOST_DEVICE
  StridedRandomAccessor operator+(index_t offset) const {
    return StridedRandomAccessor(this->ptr + offset * this->stride, this->stride);
  }

  C10_HOST_DEVICE
  friend StridedRandomAccessor operator+(
    index_t offset,
    const StridedRandomAccessor& accessor
  ) {
    return accessor + offset;
  }

  C10_HOST_DEVICE
  StridedRandomAccessor& operator-=(index_t offset) {
    this->ptr -= offset * this->stride;
    return *this;
  }

  C10_HOST_DEVICE
  StridedRandomAccessor operator-(index_t offset) const {
    return StridedRandomAccessor(this->ptr - offset * this->stride, this->stride);
  }

  C10_HOST_DEVICE
  difference_type operator-(const BaseType& other) const {
    return static_cast<const BaseType&>(*this) - other;
  }
  // }
};

template <typename ValueAccessor, typename IndexAccessor>
class IndexedRandomAccessor {
public:
  using value_type = std::pair<
    typename std::iterator_traits<ValueAccessor>::value_type,
    typename std::iterator_traits<IndexAccessor>::value_type>;
  using reference_type = std::pair<
    typename std::iterator_traits<ValueAccessor>::value_type&,
    typename std::iterator_traits<IndexAccessor>::value_type&>;
  using pointer = typename std::iterator_traits<ValueAccessor>::pointer;
  using difference_type = typename std::iterator_traits<ValueAccessor>::difference_type;
  using iterator_category = std::random_access_iterator_tag;

  using ValuePtrType = typename std::iterator_traits<ValueAccessor>::pointer;
  using IndexPtrType = typename std::iterator_traits<IndexAccessor>::pointer;

  IndexedRandomAccessor(ValueAccessor va, IndexAccessor ia)
    : va{va}, ia{ia}
  {}

protected:
  ValueAccessor va;
  IndexAccessor ia;
};

}} // namespace at::native
