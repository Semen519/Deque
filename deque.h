#include <iostream>
#include <iterator>
#include <type_traits>

template<typename T>
class Deque {
 private:

  static const int block = 32;

 public:
  template<bool is_const>
  class base_iterator {
   public:
    using Memory = typename std::conditional<is_const, const T **, T **>::type;
    using Pointer = typename std::conditional<is_const, const T *, T *>::type;
    using Reference = typename std::conditional<is_const, const T &, T &>::type;
    using value_type = typename std::conditional<is_const, const T, T>::type;
    using iterator_category = std::random_access_iterator_tag;

   public:

    Memory mem;
    int row;
    int col;

   public:
    base_iterator() = default;

    base_iterator(int row, int col) : mem(nullptr), row(row), col(col) {}

    base_iterator(Memory mem, int row, int col) : mem(mem), row(row), col(col) {}

    base_iterator(const base_iterator &other) : mem(other.mem), row(other.row), col(other.col) {}

    base_iterator &operator=(const base_iterator &other) = default;

    ~base_iterator() = default;

    operator base_iterator<true>() {
      return {const_cast<const T **>(mem), row, col};
    }

    base_iterator &operator++() {
      if (col != block - 1) {
        ++col;
      } else {
        ++row;
        col = 0;
      }
      return *this;
    }

    base_iterator operator++(int) {
      base_iterator copy = *this;
      ++(*this);
      return copy;
    }

    base_iterator &operator--() {
      if (col != 0) {
        --col;
      } else {
        --row;
        col = block - 1;
      }
      return *this;
    }

    base_iterator operator--(int) {
      base_iterator copy = *this;
      --(*this);
      return copy;
    }

    base_iterator &operator+=(const int term) {
      int a = term / block;
      int b = term % block;
      row += a;
      col += b;
      if (col >= block) {
        ++row;
        col -= block;
      } else if (col < 0) {
        --row;
        col += block;
      }
      return *this;
    }

    base_iterator &operator-=(const int term) {
      int term1 = -term;
      *this += term1;
      return *this;
    }

    base_iterator operator+(const int term) const {
      base_iterator copy = *this;
      copy += term;
      return copy;
    }

    base_iterator operator-(const int term) const {
      base_iterator copy = *this;
      copy -= term;
      return copy;
    }

    int operator-(const base_iterator &other) const {
      int a = row - other.row;
      int b = col - other.col;
      int ans = a * block + b;
      return ans;
    }

    bool operator==(const base_iterator &other) const {
      return (*this - other) == 0;
    }

    bool operator!=(const base_iterator &other) const {
      return !((*this) == other);
    }

    bool operator<(const base_iterator &other) const {
      return (*this - other) < 0;
    }

    bool operator>(const base_iterator &other) const {
      return other < *this;
    }

    bool operator<=(const base_iterator &other) const {
      return !(other < *this);
    }

    bool operator>=(const base_iterator &other) const {
      return !(*this < other);
    }

    Reference operator*() const {
      return mem[row][col];
    }

    Pointer operator->() const {
      return &mem[row][col];
    }
  };

  typedef base_iterator<false> iterator;
  typedef base_iterator<true> const_iterator;

  size_t sz;
  size_t cap;
  T** out;
  iterator b_ind;
  iterator f_ind;
  iterator e_ind;
  iterator l_ind;

  void deallocate_out(size_t i, T** place) {
    for (size_t k = 0; k != i; ++k) {
      delete[] reinterpret_cast<char *>(place[k]);
    }
    delete[] place;
  }

  void safety_alloc_out(T** place, size_t alloc_cap) {
    size_t amount = 0;
    try {
      for (; amount < alloc_cap; ++amount) {
        place[amount] = reinterpret_cast<T *>(new char[sizeof(T) * block]);
      }
    } catch (...) {
      deallocate_out(amount, place);
      throw;
    }
  }

  void deallocate_elems(iterator end) {
    for (iterator j = b_ind; j != end; ++j) {
      out[end.row][end.col].~T();
    }
    deallocate_out(cap, out);
  }

 public:
  Deque() : sz(0), cap(3), out(new T* [3]), b_ind(cap / 3, 0), f_ind(b_ind - 1), e_ind(b_ind + sz), l_ind(e_ind - 1) {
    safety_alloc_out(out, cap);
  }

  Deque(const size_t new_sz, const T &T_val) : sz(new_sz), cap(3 * (sz / block + 1)), out(new T* [cap]),
        b_ind(cap / 3, 0), f_ind(b_ind - 1), e_ind(b_ind + sz), l_ind(e_ind - 1) {
    safety_alloc_out(out, cap);
    iterator i = b_ind;
    try {
      while (i != e_ind) {
        new(out[i.row] + i.col) T(T_val);
        ++i;
      }
    } catch (...) {
      deallocate_elems(i);
      throw;
    }
  }

  explicit Deque(const size_t new_sz) : Deque(new_sz, T()) {}

  Deque(const Deque<T> &other) : sz(other.sz), cap(other.cap), out(new T* [cap]), b_ind(other.b_ind),
                                 f_ind(other.f_ind), e_ind(other.e_ind), l_ind(other.l_ind) {
    safety_alloc_out(out, cap);
    iterator i = b_ind;
    try {
      for (; i != e_ind; ++i) {
        new(out[i.row] + i.col) T(other.out[i.row][i.col]);
      }
    }
    catch (...) {
      deallocate_elems(i);
      throw;
    }
  }

  void swap(Deque<T> &d1, Deque<T> &d2) {
    std::swap(d1.out, d2.out);
    std::swap(d1.sz, d2.sz);
    std::swap(d1.b_ind, d2.b_ind);
    std::swap(d1.l_ind, d2.l_ind);
    std::swap(d1.e_ind, d2.e_ind);
    std::swap(d1.f_ind, d2.f_ind);
  }

  Deque<T> &operator=(Deque<T> other) {
    swap(*this, other);
    return *this;
  }

  size_t size() const {
    return sz;
  }

  T &operator[](const size_t term) {
    iterator ans = b_ind + term;
    return out[ans.row][ans.col];
  }

  const T &operator[](const size_t term) const {
    iterator ans = b_ind + term;
    return out[ans.row][ans.col];
  }

  T &at(const size_t term) {
    if (term < 0 || term >= sz) {
      throw std::out_of_range("");
    }
    return (*this)[term];
  }

  const T &at(const size_t term) const {
    if (term < 0 || term >= sz) {
      throw std::out_of_range("");
    }
    return (*this)[term];
  }

  void pop_back() {
    out[l_ind.row][l_ind.col].~T();
    --sz;
    --l_ind;
    --e_ind;
  }

  void pop_front() {
    out[b_ind.row][b_ind.col].~T();
    --sz;
    ++f_ind;
    ++b_ind;
  }

  void push_back(const T &T_val) {
    if (static_cast<size_t>(e_ind.row) < cap) {
      new(out[e_ind.row] + e_ind.col) T(T_val);
    } else {
      size_t new_cap = 3 * cap + 1;
      T **new_out = new T *[new_cap];
      size_t l = b_ind.row;
      size_t r = l_ind.row;
      for (size_t i = 0; i < new_cap; ++i) {
        new_out[i] = reinterpret_cast<T *>(new char[sizeof(T) * block]);
      }
      for (size_t i = l; i <= r; ++i) {
        new_out[cap + i - l] = out[i];
      }
      iterator l_ind_new = l_ind;
      l_ind_new.row = cap + r - l;
      iterator e_ind_new = l_ind_new + 1;
      try {
        new(new_out[e_ind_new.row] + e_ind_new.col) T(T_val);
      } catch (...) {
        delete[] new_out;
        throw;
      }
      std::swap(out, new_out);
      delete[] new_out;
      l_ind = l_ind_new;
      e_ind = e_ind_new;
      b_ind.row = cap;
      f_ind = b_ind - 1;
      cap = new_cap;
    }
    ++sz;
    ++e_ind;
    ++l_ind;
  }

  void push_front(const T &T_val) {
    if (f_ind.row >= 0) {
      new(out[f_ind.row] + f_ind.col) T(T_val);
    } else {
      size_t new_cap = 3 * cap + 1;
      T **new_out = new T *[new_cap];
      size_t l = b_ind.row;
      size_t r = l_ind.row;
      safety_alloc_out(new_out, new_cap);
      for (size_t i = l; i <= r; ++i) {
        new_out[cap + i - l] = out[i];
      }
      iterator b_ind_new = b_ind;
      b_ind_new.row = cap;
      iterator f_ind_new = b_ind_new - 1;
      try {
        new(new_out[f_ind_new.row] + f_ind_new.col) T(T_val);
      } catch (...) {
        delete[] new_out;
        throw;
      }
      std::swap(out, new_out);
      delete[] new_out;
      b_ind = b_ind_new;
      f_ind = f_ind_new;
      l_ind.row = cap + r - l;
      e_ind = l_ind + 1;
      cap = new_cap;
    }
    ++sz;
    --b_ind;
    --f_ind;
  }

  iterator insert(const_iterator cit, const T &T_val) {
    T temp = T_val;
    int i = cit - begin();
    iterator it = begin() + i;
    for (; it != end(); ++it) {
      std::swap(*it, temp);
    }
    push_back(temp);
    it = begin() + i;
    return it;
  }

  iterator erase(const_iterator cit) {
    int i = cit - begin();
    iterator it = begin() + i;
    for (; it != end() - 1; ++it) {
      std::swap(*it, *(it + 1));
    }
    pop_back();
    it = begin() + i;
    return it;
  }

  iterator begin() {
    return iterator(out, b_ind.row, b_ind.col);
  }

  const_iterator begin() const {
    return iterator(out, b_ind.row, b_ind.col);
  }

  const_iterator cbegin() const {
    return iterator(out, b_ind.row, b_ind.col);
  }

  iterator end() {
    return iterator(out, e_ind.row, e_ind.col);
  }

  const_iterator end() const {
    return iterator(out, e_ind.row, e_ind.col);
  }

  const_iterator cend() const {
    return iterator(out, e_ind.row, e_ind.col);
  }

  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  const_reverse_iterator rbegin() const {
    return reverse_iterator(end());
  }

  reverse_iterator rbegin() {
    return reverse_iterator(end());
  }

  const_reverse_iterator rend() const {
    return reverse_iterator(end());
  }

  reverse_iterator rend() {
    return reverse_iterator(begin());
  }

  const_reverse_iterator crbegin() const {
    return reverse_iterator(cend());
  }

  const_reverse_iterator crend() const {
    return reverse_iterator(cbegin());
  }

  ~Deque() noexcept {
    deallocate_elems(e_ind);
  }
};