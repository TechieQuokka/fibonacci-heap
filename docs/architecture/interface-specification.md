# 피보나치 힙 인터페이스 명세서

## 1. 개요

이 문서는 피보나치 힙의 공개 인터페이스를 상세히 정의합니다. 다양한 프로그래밍 언어와 사용 사례를 지원하기 위해 여러 레벨의 인터페이스를 제공합니다.

## 2. C++ 인터페이스

### 2.1 기본 클래스 인터페이스

```cpp
#ifndef FIBONACCI_HEAP_H
#define FIBONACCI_HEAP_H

#include <memory>
#include <functional>
#include <iterator>
#include <stdexcept>

namespace fib_heap {

// 전방 선언
template<typename T, typename Compare>
class fibonacci_heap;

template<typename T>
class fib_node;

// 핸들 클래스 - 노드에 대한 안전한 참조
template<typename T>
class node_handle {
private:
    friend class fibonacci_heap<T, std::less<T>>;
    fib_node<T>* node_ptr;
    std::uint64_t generation;  // ABA 문제 방지용
    
public:
    node_handle() : node_ptr(nullptr), generation(0) {}
    
    bool is_valid() const noexcept;
    bool operator==(const node_handle& other) const noexcept;
    bool operator!=(const node_handle& other) const noexcept;
};

// 메인 피보나치 힙 클래스
template<typename T, typename Compare = std::less<T>>
class fibonacci_heap {
public:
    // 타입 정의
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using key_compare = Compare;
    using handle_type = node_handle<T>;
    
    // 생성자 및 소멸자
    fibonacci_heap();
    explicit fibonacci_heap(const Compare& comp);
    fibonacci_heap(const fibonacci_heap& other);
    fibonacci_heap(fibonacci_heap&& other) noexcept;
    ~fibonacci_heap();
    
    // 대입 연산자
    fibonacci_heap& operator=(const fibonacci_heap& other);
    fibonacci_heap& operator=(fibonacci_heap&& other) noexcept;
    
    // 용량 관련 함수
    bool empty() const noexcept;
    size_type size() const noexcept;
    size_type max_size() const noexcept;
    
    // 원소 접근
    const_reference top() const;
    
    // 수정 관련 함수
    handle_type push(const value_type& value);
    handle_type push(value_type&& value);
    
    template<typename... Args>
    handle_type emplace(Args&&... args);
    
    void pop();
    
    void decrease_key(handle_type handle, const value_type& new_value);
    void increase_key(handle_type handle, const value_type& new_value);
    
    void erase(handle_type handle);
    
    void merge(fibonacci_heap& other);
    void swap(fibonacci_heap& other) noexcept;
    void clear() noexcept;
    
    // 비교 함수 접근
    key_compare key_comp() const;
    
    // 디버깅 및 통계
    bool validate() const;
    void print_structure(std::ostream& os) const;
    
    struct statistics {
        size_type total_nodes;
        size_type marked_nodes;
        size_type root_trees;
        int max_degree;
        double avg_degree;
    };
    
    statistics get_statistics() const;

private:
    // 구현부 (private 멤버들)
    class impl;
    std::unique_ptr<impl> pimpl;
};

// 비교 연산자
template<typename T, typename Compare>
bool operator==(const fibonacci_heap<T, Compare>& lhs,
                const fibonacci_heap<T, Compare>& rhs);

template<typename T, typename Compare>
bool operator!=(const fibonacci_heap<T, Compare>& lhs,
                const fibonacci_heap<T, Compare>& rhs);

// 특수화된 swap
template<typename T, typename Compare>
void swap(fibonacci_heap<T, Compare>& lhs,
          fibonacci_heap<T, Compare>& rhs) noexcept;

} // namespace fib_heap

#endif // FIBONACCI_HEAP_H
```

### 2.2 예외 클래스

```cpp
namespace fib_heap {

class fibonacci_heap_error : public std::runtime_error {
public:
    explicit fibonacci_heap_error(const std::string& message)
        : std::runtime_error(message) {}
};

class invalid_handle : public fibonacci_heap_error {
public:
    invalid_handle() 
        : fibonacci_heap_error("Invalid or expired node handle") {}
};

class empty_heap : public fibonacci_heap_error {
public:
    empty_heap() 
        : fibonacci_heap_error("Operation on empty heap") {}
};

class invalid_key_change : public fibonacci_heap_error {
public:
    explicit invalid_key_change(const std::string& details)
        : fibonacci_heap_error("Invalid key change: " + details) {}
};

} // namespace fib_heap
```

### 2.3 이터레이터 인터페이스

```cpp
namespace fib_heap {

// 읽기 전용 이터레이터
template<typename T, typename Compare>
class const_iterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = const T*;
    using reference = const T&;
    
    const_iterator();
    const_iterator(const const_iterator& other);
    ~const_iterator();
    
    const_iterator& operator=(const const_iterator& other);
    
    reference operator*() const;
    pointer operator->() const;
    
    const_iterator& operator++();
    const_iterator operator++(int);
    
    bool operator==(const const_iterator& other) const;
    bool operator!=(const const_iterator& other) const;

private:
    friend class fibonacci_heap<T, Compare>;
    // 내부 구현
};

// 힙에 이터레이터 지원 추가
template<typename T, typename Compare>
class fibonacci_heap {
    // ... 기존 멤버들 ...
    
public:
    using const_iterator = fib_heap::const_iterator<T, Compare>;
    
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;
};

} // namespace fib_heap
```

## 3. C 인터페이스

### 3.1 기본 C API

```c
#ifndef FIBONACCI_HEAP_C_H
#define FIBONACCI_HEAP_C_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// 불투명 타입 선언
typedef struct fib_heap fib_heap_t;
typedef struct fib_node_handle fib_node_handle_t;

// 에러 코드
typedef enum {
    FIB_HEAP_SUCCESS = 0,
    FIB_HEAP_ERROR_NULL_POINTER,
    FIB_HEAP_ERROR_INVALID_HANDLE,
    FIB_HEAP_ERROR_OUT_OF_MEMORY,
    FIB_HEAP_ERROR_EMPTY_HEAP,
    FIB_HEAP_ERROR_INVALID_KEY,
    FIB_HEAP_ERROR_HEAP_CORRUPTION
} fib_heap_error_t;

// 비교 함수 타입
typedef int (*fib_heap_compare_func_t)(const void* a, const void* b);

// 메모리 할당 함수 타입
typedef void* (*fib_heap_malloc_func_t)(size_t size);
typedef void (*fib_heap_free_func_t)(void* ptr);

// 힙 생성 및 삭제
fib_heap_t* fib_heap_create(fib_heap_compare_func_t compare_func);
fib_heap_t* fib_heap_create_with_allocator(
    fib_heap_compare_func_t compare_func,
    fib_heap_malloc_func_t malloc_func,
    fib_heap_free_func_t free_func
);
void fib_heap_destroy(fib_heap_t* heap);

// 기본 연산
fib_heap_error_t fib_heap_insert(fib_heap_t* heap, 
                                 const void* key, 
                                 void* data,
                                 fib_node_handle_t* handle);

fib_heap_error_t fib_heap_minimum(fib_heap_t* heap, 
                                  void** key, 
                                  void** data);

fib_heap_error_t fib_heap_extract_min(fib_heap_t* heap, 
                                      void** key, 
                                      void** data);

fib_heap_error_t fib_heap_decrease_key(fib_heap_t* heap,
                                       fib_node_handle_t handle,
                                       const void* new_key);

fib_heap_error_t fib_heap_delete_node(fib_heap_t* heap,
                                      fib_node_handle_t handle);

fib_heap_error_t fib_heap_merge(fib_heap_t* heap1, fib_heap_t* heap2);

// 상태 조회
bool fib_heap_empty(fib_heap_t* heap);
size_t fib_heap_size(fib_heap_t* heap);

// 핸들 관련
bool fib_heap_handle_valid(fib_heap_t* heap, fib_node_handle_t handle);
fib_heap_error_t fib_heap_handle_get_data(fib_heap_t* heap,
                                          fib_node_handle_t handle,
                                          void** key,
                                          void** data);

// 유틸리티
const char* fib_heap_error_string(fib_heap_error_t error);
fib_heap_error_t fib_heap_validate(fib_heap_t* heap);

#ifdef __cplusplus
}
#endif

#endif // FIBONACCI_HEAP_C_H
```

### 3.2 타입별 특화 API

```c
// 정수 키용 특화 API
fib_heap_t* fib_heap_create_int(void);
fib_heap_error_t fib_heap_insert_int(fib_heap_t* heap, 
                                     int key, 
                                     void* data,
                                     fib_node_handle_t* handle);
fib_heap_error_t fib_heap_decrease_key_int(fib_heap_t* heap,
                                          fib_node_handle_t handle,
                                          int new_key);

// 문자열 키용 특화 API
fib_heap_t* fib_heap_create_string(void);
fib_heap_error_t fib_heap_insert_string(fib_heap_t* heap,
                                        const char* key,
                                        void* data,
                                        fib_node_handle_t* handle);

// 실수 키용 특화 API
fib_heap_t* fib_heap_create_double(void);
fib_heap_error_t fib_heap_insert_double(fib_heap_t* heap,
                                        double key,
                                        void* data,
                                        fib_node_handle_t* handle);
```

## 4. Python 바인딩 인터페이스

### 4.1 Python 클래스 인터페이스

```python
# fibonacci_heap.py
from typing import TypeVar, Generic, Optional, Iterator, Callable
from abc import ABC, abstractmethod

T = TypeVar('T')

class NodeHandle:
    """피보나치 힙 노드에 대한 핸들"""
    
    def __init__(self, internal_handle):
        self._handle = internal_handle
    
    def is_valid(self) -> bool:
        """핸들이 유효한지 확인"""
        pass
    
    def __eq__(self, other) -> bool:
        pass
    
    def __hash__(self) -> int:
        pass

class FibonacciHeap(Generic[T]):
    """피보나치 힙 구현"""
    
    def __init__(self, key_function: Optional[Callable[[T], float]] = None):
        """
        피보나치 힙 생성
        
        Args:
            key_function: 원소에서 키를 추출하는 함수. None이면 원소 자체를 키로 사용
        """
        pass
    
    def __len__(self) -> int:
        """힙의 크기 반환"""
        pass
    
    def __bool__(self) -> bool:
        """힙이 비어있지 않은지 확인"""
        pass
    
    def __iter__(self) -> Iterator[T]:
        """힙의 모든 원소를 순회"""
        pass
    
    def push(self, item: T) -> NodeHandle:
        """
        원소를 힙에 추가
        
        Args:
            item: 추가할 원소
            
        Returns:
            추가된 노드의 핸들
        """
        pass
    
    def peek(self) -> T:
        """
        최솟값 반환 (제거하지 않음)
        
        Returns:
            최솟값
            
        Raises:
            IndexError: 힙이 비어있는 경우
        """
        pass
    
    def pop(self) -> T:
        """
        최솟값 제거 및 반환
        
        Returns:
            최솟값
            
        Raises:
            IndexError: 힙이 비어있는 경우
        """
        pass
    
    def decrease_key(self, handle: NodeHandle, new_item: T) -> None:
        """
        노드의 키 감소
        
        Args:
            handle: 대상 노드의 핸들
            new_item: 새로운 값 (키가 더 작아야 함)
            
        Raises:
            ValueError: 새로운 키가 더 큰 경우
            KeyError: 핸들이 유효하지 않은 경우
        """
        pass
    
    def delete(self, handle: NodeHandle) -> T:
        """
        노드 삭제
        
        Args:
            handle: 삭제할 노드의 핸들
            
        Returns:
            삭제된 값
            
        Raises:
            KeyError: 핸들이 유효하지 않은 경우
        """
        pass
    
    def merge(self, other: 'FibonacciHeap[T]') -> None:
        """
        다른 힙과 병합
        
        Args:
            other: 병합할 힙 (병합 후 비워짐)
        """
        pass
    
    def clear(self) -> None:
        """힙의 모든 원소 제거"""
        pass
    
    def copy(self) -> 'FibonacciHeap[T]':
        """힙의 복사본 생성"""
        pass
    
    def validate(self) -> bool:
        """힙의 불변 조건 검증"""
        pass
    
    @property
    def statistics(self) -> dict:
        """힙의 통계 정보 반환"""
        pass

# 유틸리티 함수들
def heapify(iterable: Iterator[T], 
           key_function: Optional[Callable[[T], float]] = None) -> FibonacciHeap[T]:
    """반복 가능한 객체로부터 힙 생성"""
    pass

def merge_heaps(*heaps: FibonacciHeap[T]) -> FibonacciHeap[T]:
    """여러 힙을 병합"""
    pass
```

### 4.2 Python 예외 클래스

```python
class FibonacciHeapError(Exception):
    """피보나치 힙 관련 기본 예외"""
    pass

class EmptyHeapError(FibonacciHeapError, IndexError):
    """빈 힙에 대한 연산 시 발생"""
    pass

class InvalidHandleError(FibonacciHeapError, KeyError):
    """유효하지 않은 핸들 사용 시 발생"""
    pass

class InvalidKeyError(FibonacciHeapError, ValueError):
    """잘못된 키 변경 시 발생"""
    pass
```

## 5. JavaScript/TypeScript 인터페이스

### 5.1 TypeScript 인터페이스

```typescript
// fibonacci-heap.d.ts
export interface NodeHandle<T> {
    readonly valid: boolean;
    readonly value: T;
}

export interface HeapStatistics {
    totalNodes: number;
    markedNodes: number;
    rootTrees: number;
    maxDegree: number;
    averageDegree: number;
}

export class FibonacciHeap<T> {
    constructor(compareFunction?: (a: T, b: T) => number);
    
    readonly size: number;
    readonly empty: boolean;
    
    push(item: T): NodeHandle<T>;
    peek(): T | undefined;
    pop(): T | undefined;
    
    decreaseKey(handle: NodeHandle<T>, newItem: T): void;
    delete(handle: NodeHandle<T>): T;
    
    merge(other: FibonacciHeap<T>): void;
    clear(): void;
    
    [Symbol.iterator](): Iterator<T>;
    
    validate(): boolean;
    getStatistics(): HeapStatistics;
}

export function heapify<T>(
    items: Iterable<T>,
    compareFunction?: (a: T, b: T) => number
): FibonacciHeap<T>;

export class FibonacciHeapError extends Error {}
export class EmptyHeapError extends FibonacciHeapError {}
export class InvalidHandleError extends FibonacciHeapError {}
export class InvalidKeyError extends FibonacciHeapError {}
```

## 6. 성능 보장사항

### 6.1 시간 복잡도 보장
```cpp
namespace fib_heap {
namespace performance {

// 시간 복잡도 보장사항을 문서화하는 타입
template<typename Operation>
struct time_complexity;

template<>
struct time_complexity<void(fibonacci_heap::*)(const value_type&)> {
    static constexpr const char* amortized = "O(1)";
    static constexpr const char* worst_case = "O(log n)";
};

// 연산별 성능 보장
enum class operation_type {
    INSERT,      // O(1) amortized
    MINIMUM,     // O(1) worst-case
    EXTRACT_MIN, // O(log n) amortized
    DECREASE_KEY,// O(1) amortized
    DELETE_NODE, // O(log n) amortized
    MERGE        // O(1) worst-case
};

const char* get_complexity(operation_type op, bool amortized = true);

} // namespace performance
} // namespace fib_heap
```

### 6.2 메모리 사용량 보장
```cpp
namespace fib_heap {
namespace memory {

struct memory_profile {
    size_t bytes_per_node;      // 노드당 메모리 사용량
    size_t overhead_bytes;      // 고정 오버헤드
    double fragmentation_ratio; // 메모리 단편화 비율
};

memory_profile get_memory_profile();

// 메모리 사용량 예측
size_t estimate_memory_usage(size_t node_count);

} // namespace memory
} // namespace fib_heap
```

## 7. 호환성 매트릭스

### 7.1 플랫폼 지원

| 플랫폼 | C++ API | C API | Python | JavaScript |
|--------|---------|-------|--------|------------|
| Linux  | ✓       | ✓     | ✓      | ✓          |
| Windows| ✓       | ✓     | ✓      | ✓          |
| macOS  | ✓       | ✓     | ✓      | ✓          |
| WebAssembly | ✗  | ✗     | ✗      | ✓          |

### 7.2 컴파일러 지원

| 컴파일러 | 최소 버전 | C++17 | C++20 |
|----------|-----------|-------|-------|
| GCC      | 7.0       | ✓     | ✓     |
| Clang    | 6.0       | ✓     | ✓     |
| MSVC     | 19.14     | ✓     | ✓     |

이 인터페이스 명세서는 다양한 환경과 언어에서 피보나치 힙을 일관되고 안전하게 사용할 수 있도록 설계되었습니다.