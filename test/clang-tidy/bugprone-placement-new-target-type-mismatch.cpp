// RUN: %check_clang_tidy %s bugprone-placement-new-target-type-mismatch %t

// definitions
	
namespace std {
struct nothrow_t { explicit nothrow_t() = default; } nothrow;
template<class T> T* addressof(T& arg) noexcept;
template< class T > struct remove_reference      {typedef T type;};
template< class T > struct remove_reference<T&>  {typedef T type;};
template< class T > struct remove_reference<T&&> {typedef T type;};
template< class T >
T&& forward( typename std::remove_reference<T>::type& t ) noexcept;
} // namespace std

using size_type = unsigned long;
void *operator new(size_type, void *);
void *operator new[](size_type, void *);
void* operator new(size_type size, const std::nothrow_t&) noexcept;
void* operator new(size_type size, const std::nothrow_t&) noexcept;
void* operator new[](size_type size, const std::nothrow_t&) noexcept;

struct Foo {
  int a;
  int b;
  int c;
  int d;
};

template<typename T>
T& getT() {
  static T f;
  return f;
}

// instances emitting warnings

void f1() {
  struct Dummy {
    int a;
    int b;
  };
  int *ptr = new int;
  new (ptr) Dummy;
  // CHECK-MESSAGES: :[[@LINE-1]]:8: warning: placement new parameter and allocated type mismatch [bugprone-placement-new-target-type-mismatch]
}

void f2() {
  int * ptr = new int;
  new (ptr) Foo;
  // CHECK-MESSAGES: :[[@LINE-1]]:8: warning: placement new parameter and allocated type mismatch [bugprone-placement-new-target-type-mismatch]
}

void f3() {
  char *ptr = new char[17*sizeof(char)];
  new (ptr) float[13];
  // CHECK-MESSAGES: :[[@LINE-1]]:8: warning: placement new parameter and allocated type mismatch [bugprone-placement-new-target-type-mismatch]
}

void f4() {
  new (std::addressof(getT<int>())) Foo;
  // CHECK-MESSAGES: :[[@LINE-1]]:8: warning: placement new parameter and allocated type mismatch [bugprone-placement-new-target-type-mismatch]
}

void f5() {
  char *ptr = new char[17*sizeof(char)];
  new (ptr) float{13.f};
  // CHECK-MESSAGES: :[[@LINE-1]]:8: warning: placement new parameter and allocated type mismatch [bugprone-placement-new-target-type-mismatch]
}

void f6() {
  char array[17*sizeof(char)];
  new (array) float{13.f};
  // CHECK-MESSAGES: :[[@LINE-1]]:8: warning: placement new parameter and allocated type mismatch [bugprone-placement-new-target-type-mismatch]
}

// instances not emitting a warning

void g1() {
  Foo * ptr = new Foo;
  new (ptr) Foo;
}

void g2() {
  char *ptr = new char[17*sizeof(char)];
  new ((float *)ptr) float{13.f};
}

void g3() {
  char array[17*sizeof(char)];
  new (array) char('A');
}

void g4() {
  new ((void *)std::addressof(getT<Foo>())) Foo;
}

union
{
  char * buffer;
} Union;

template <typename T, typename... U>
void g5(U &&... V) {
  new ((Union.buffer)) T(std::forward<U>(V)...);
}

template <typename T, typename... U>
void g6(U &&... V) {
  new (std::nothrow) T(std::forward<U>(V)...);
}