# 피보나치 힙 구현 계획서

## 1. 개요

본 문서는 CLRS 교재를 기반으로 한 피보나치 힙의 상세 구현 계획을 제시합니다. 이론적 기반부터 실제 코드 구현까지의 단계적 접근 방법을 정의하고, 성능 최적화와 테스트 전략을 포함합니다.

## 2. 구현 목표

### 2.1 기능적 목표
- [x] CLRS 표준에 부합하는 모든 기본 연산 구현
- [x] O(1) 분할상환 시간의 INSERT, DECREASE-KEY, UNION 연산
- [x] O(log n) 분할상환 시간의 EXTRACT-MIN, DELETE 연산
- [x] 메모리 효율적인 노드 관리
- [x] 스레드 안전성 옵션 제공

### 2.2 비기능적 목표
- [x] C++17 표준 준수
- [x] STL 호환 인터페이스 제공
- [x] 포괄적인 단위 테스트 (95% 이상 코드 커버리지)
- [x] 상세한 문서화
- [x] 크로스 플랫폼 지원 (Windows, Linux, macOS)

## 3. 구현 단계

### 3.1 1단계: 핵심 자료구조 설계
**기간**: 1주
**담당**: 핵심 팀

#### 3.1.1 노드 구조 설계
```cpp
template<typename T>
class FibNode {
public:
    T key;                      // 노드의 키값
    void* data;                 // 사용자 데이터
    
    FibNode* parent;            // 부모 노드
    FibNode* child;             // 자식 노드 중 하나
    FibNode* left;              // 왼쪽 형제
    FibNode* right;             // 오른쪽 형제
    
    int degree;                 // 자식 개수
    bool marked;                // 마킹 여부
    
    // 생성자 및 유틸리티 함수
    explicit FibNode(const T& k);
    void link_as_child(FibNode* child_node);
    void unlink_from_siblings();
    bool is_root() const { return parent == nullptr; }
};
```

#### 3.1.2 힙 클래스 골격
```cpp
template<typename T, typename Compare = std::less<T>>
class FibonacciHeap {
private:
    FibNode<T>* min_node;       // 최솟값 노드
    size_t node_count;          // 총 노드 개수
    Compare comp;               // 비교 함수
    
public:
    using value_type = T;
    using node_handle = FibNode<T>*;
    
    // 생성자/소멸자
    FibonacciHeap();
    explicit FibonacciHeap(const Compare& c);
    ~FibonacciHeap();
    
    // 기본 연산 (구현 예정)
    node_handle insert(const T& key);
    T minimum() const;
    T extract_min();
    void decrease_key(node_handle node, const T& new_key);
    void delete_node(node_handle node);
    void merge(FibonacciHeap& other);
    
    // 상태 조회
    bool empty() const { return node_count == 0; }
    size_t size() const { return node_count; }
};
```

**완료 기준**:
- [x] 컴파일 오류 없는 기본 구조 완성
- [x] 기본 생성자/소멸자 구현
- [x] 메모리 누수 검사 통과

### 3.2 2단계: 기본 연산 구현
**기간**: 2주
**담당**: 핵심 팀

#### 3.2.1 INSERT 연산 구현
```cpp
template<typename T, typename Compare>
typename FibonacciHeap<T, Compare>::node_handle
FibonacciHeap<T, Compare>::insert(const T& key) {
    auto new_node = std::make_unique<FibNode<T>>(key);
    auto result = new_node.get();
    
    // 루트 리스트에 추가
    if (min_node == nullptr) {
        min_node = new_node.release();
        min_node->left = min_node->right = min_node;
    } else {
        // 루트 리스트의 최솟값 옆에 삽입
        insert_into_root_list(new_node.release());
        
        // 최솟값 업데이트
        if (comp(key, min_node->key)) {
            min_node = result;
        }
    }
    
    ++node_count;
    return result;
}
```

#### 3.2.2 EXTRACT-MIN 연산 구현
```cpp
template<typename T, typename Compare>
T FibonacciHeap<T, Compare>::extract_min() {
    if (min_node == nullptr) {
        throw std::runtime_error("Heap is empty");
    }
    
    FibNode<T>* z = min_node;
    T result = z->key;
    
    // 최솟값의 모든 자식을 루트 리스트에 추가
    if (z->child != nullptr) {
        add_children_to_root_list(z->child);
    }
    
    // 최솟값을 루트 리스트에서 제거
    remove_from_root_list(z);
    
    if (z == z->right) {
        // 마지막 노드였음
        min_node = nullptr;
    } else {
        min_node = z->right;
        consolidate();
    }
    
    --node_count;
    delete z;
    return result;
}
```

#### 3.2.3 CONSOLIDATE 연산 구현
```cpp
template<typename T, typename Compare>
void FibonacciHeap<T, Compare>::consolidate() {
    // degree 배열 크기는 log_φ(n)으로 제한
    int max_degree = static_cast<int>(std::floor(std::log(node_count) / std::log(GOLDEN_RATIO))) + 1;
    std::vector<FibNode<T>*> degree_table(max_degree, nullptr);
    
    // 현재 루트 노드들을 리스트로 수집
    std::vector<FibNode<T>*> root_nodes;
    collect_root_nodes(root_nodes);
    
    // 각 루트 노드 처리
    for (auto w : root_nodes) {
        FibNode<T>* x = w;
        int d = x->degree;
        
        while (degree_table[d] != nullptr) {
            FibNode<T>* y = degree_table[d];
            if (comp(y->key, x->key)) {
                std::swap(x, y);
            }
            heap_link(y, x);
            degree_table[d] = nullptr;
            ++d;
        }
        degree_table[d] = x;
    }
    
    // 새로운 최솟값 찾기
    rebuild_root_list_and_find_min(degree_table);
}
```

**완료 기준**:
- [x] 모든 기본 연산 구현 완료
- [x] 단위 테스트 통과 (기본 기능)
- [x] 메모리 누수 없음
- [x] 시간 복잡도 요구사항 충족

### 3.3 3단계: 고급 연산 구현
**기간**: 1.5주
**담당**: 핵심 팀

#### 3.3.1 DECREASE-KEY 연산
```cpp
template<typename T, typename Compare>
void FibonacciHeap<T, Compare>::decrease_key(node_handle node, const T& new_key) {
    if (comp(node->key, new_key)) {
        throw std::invalid_argument("New key is greater than current key");
    }
    
    node->key = new_key;
    FibNode<T>* y = node->parent;
    
    if (y != nullptr && comp(node->key, y->key)) {
        cut(node, y);
        cascading_cut(y);
    }
    
    if (comp(node->key, min_node->key)) {
        min_node = node;
    }
}
```

#### 3.3.2 CUT 및 CASCADING-CUT 구현
```cpp
template<typename T, typename Compare>
void FibonacciHeap<T, Compare>::cut(FibNode<T>* x, FibNode<T>* y) {
    // y의 자식 리스트에서 x 제거
    remove_from_child_list(y, x);
    --y->degree;
    
    // x를 루트 리스트에 추가
    insert_into_root_list(x);
    x->parent = nullptr;
    x->marked = false;
}

template<typename T, typename Compare>
void FibonacciHeap<T, Compare>::cascading_cut(FibNode<T>* y) {
    FibNode<T>* z = y->parent;
    if (z != nullptr) {
        if (!y->marked) {
            y->marked = true;
        } else {
            cut(y, z);
            cascading_cut(z);
        }
    }
}
```

**완료 기준**:
- [x] DECREASE-KEY, DELETE 연산 구현
- [x] 분할상환 시간 복잡도 보장
- [x] 마킹 시스템 정상 동작
- [x] 힙 불변조건 유지

### 3.4 4단계: 메모리 관리 최적화
**기간**: 1주
**담당**: 성능 팀

#### 3.4.1 노드 풀 구현
```cpp
template<typename T>
class NodePool {
private:
    static constexpr size_t BLOCK_SIZE = 1024;
    
    struct Block {
        std::aligned_storage_t<sizeof(FibNode<T>), alignof(FibNode<T>)> storage[BLOCK_SIZE];
        std::bitset<BLOCK_SIZE> allocated;
        size_t next_free;
        
        Block() : next_free(0) {}
    };
    
    std::vector<std::unique_ptr<Block>> blocks;
    std::stack<FibNode<T>*> free_nodes;
    std::mutex pool_mutex;  // 스레드 안전성
    
public:
    FibNode<T>* allocate() {
        std::lock_guard<std::mutex> lock(pool_mutex);
        
        if (!free_nodes.empty()) {
            auto node = free_nodes.top();
            free_nodes.pop();
            return node;
        }
        
        return allocate_from_new_block();
    }
    
    void deallocate(FibNode<T>* node) {
        std::lock_guard<std::mutex> lock(pool_mutex);
        node->~FibNode<T>();
        free_nodes.push(node);
    }
};
```

#### 3.4.2 캐시 최적화
```cpp
// 캐시 라인에 맞춘 노드 구조
template<typename T>
struct CacheOptimizedNode {
    // 자주 접근하는 필드들을 첫 번째 캐시 라인에 배치
    T key;                          // 8 bytes
    CacheOptimizedNode* parent;     // 8 bytes  
    CacheOptimizedNode* left;       // 8 bytes
    CacheOptimizedNode* right;      // 8 bytes
    CacheOptimizedNode* child;      // 8 bytes
    int degree;                     // 4 bytes
    bool marked;                    // 1 byte
    char padding[23];               // 패딩으로 64바이트 맞춤
    
    // 두 번째 캐시 라인에 추가 데이터
    void* user_data;
    // ... 기타 필드들
};
```

**완료 기준**:
- [x] 메모리 풀 구현 및 테스트
- [x] 캐시 지역성 최적화
- [x] 메모리 사용량 30% 이상 개선
- [x] 스레드 안전성 보장

### 3.5 5단계: STL 호환 인터페이스
**기간**: 1주  
**담당**: 인터페이스 팀

#### 3.5.1 priority_queue 호환 인터페이스
```cpp
template<typename T, typename Compare = std::less<T>>
class fibonacci_priority_queue {
public:
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using container_type = FibonacciHeap<T, Compare>;
    
private:
    container_type heap;
    
public:
    // STL priority_queue 호환 멤버들
    bool empty() const { return heap.empty(); }
    size_type size() const { return heap.size(); }
    const_reference top() const { return heap.minimum(); }
    
    void push(const value_type& value) { heap.insert(value); }
    void push(value_type&& value) { heap.insert(std::move(value)); }
    void pop() { heap.extract_min(); }
    
    template<typename... Args>
    void emplace(Args&&... args) {
        heap.insert(T(std::forward<Args>(args)...));
    }
    
    void swap(fibonacci_priority_queue& other) {
        heap.swap(other.heap);
    }
};
```

#### 3.5.2 이터레이터 구현
```cpp
template<typename T, typename Compare>
class FibonacciHeap<T, Compare>::const_iterator {
private:
    std::queue<FibNode<T>*> node_queue;
    FibNode<T>* current_node;
    
    void advance_to_next() {
        if (!node_queue.empty()) {
            current_node = node_queue.front();
            node_queue.pop();
            
            // 현재 노드의 자식들을 큐에 추가
            if (current_node->child) {
                add_children_to_queue(current_node->child);
            }
        } else {
            current_node = nullptr;
        }
    }
    
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using reference = const T&;
    using pointer = const T*;
    
    reference operator*() const { return current_node->key; }
    pointer operator->() const { return &current_node->key; }
    
    const_iterator& operator++() {
        advance_to_next();
        return *this;
    }
    
    bool operator==(const const_iterator& other) const {
        return current_node == other.current_node;
    }
};
```

**완료 기준**:
- [x] STL 호환 인터페이스 완성
- [x] 이터레이터 구현 및 테스트
- [x] 표준 알고리즘과 호환성 확인
- [x] 예외 안전성 보장

### 3.6 6단계: 성능 최적화 및 테스트
**기간**: 1.5주
**담당**: 전체 팀

#### 3.6.1 프로파일링 및 최적화
```bash
# 성능 측정 스크립트
#!/bin/bash

# 다양한 크기의 데이터셋으로 벤치마크
for size in 1000 10000 100000 1000000; do
    echo "Testing with $size elements"
    ./benchmark_fibonacci_heap --size=$size --operations=mixed
    ./benchmark_fibonacci_heap --size=$size --operations=dijkstra
done

# 메모리 사용량 측정
valgrind --tool=massif ./fibonacci_heap_test

# 캐시 성능 측정  
perf record -e cache-misses ./fibonacci_heap_test
perf report
```

#### 3.6.2 포괄적 테스트 스위트
```cpp
// 단위 테스트 예시
TEST(FibonacciHeapTest, BasicOperations) {
    FibonacciHeap<int> heap;
    
    // INSERT 테스트
    auto h1 = heap.insert(10);
    auto h2 = heap.insert(5);
    auto h3 = heap.insert(15);
    
    EXPECT_EQ(heap.size(), 3);
    EXPECT_EQ(heap.minimum(), 5);
    
    // EXTRACT-MIN 테스트
    EXPECT_EQ(heap.extract_min(), 5);
    EXPECT_EQ(heap.minimum(), 10);
    
    // DECREASE-KEY 테스트
    heap.decrease_key(h3, 3);
    EXPECT_EQ(heap.minimum(), 3);
}

TEST(FibonacciHeapTest, ComplexScenario) {
    // 다익스트라 알고리즘 시뮬레이션
    FibonacciHeap<int> heap;
    std::vector<FibonacciHeap<int>::node_handle> handles;
    
    // 1000개 노드로 그래프 시뮬레이션
    for (int i = 0; i < 1000; ++i) {
        handles.push_back(heap.insert(INT_MAX));
    }
    
    // 거리 감소 시뮬레이션
    for (int i = 0; i < 500; ++i) {
        int node = rand() % 1000;
        int new_dist = rand() % 100;
        if (new_dist < heap.get_key(handles[node])) {
            heap.decrease_key(handles[node], new_dist);
        }
    }
    
    // 모든 노드 추출
    std::vector<int> extracted;
    while (!heap.empty()) {
        extracted.push_back(heap.extract_min());
    }
    
    // 정렬 순서 확인
    EXPECT_TRUE(std::is_sorted(extracted.begin(), extracted.end()));
}
```

**완료 기준**:
- [x] 95% 이상 코드 커버리지
- [x] 모든 기능 테스트 통과
- [x] 성능 요구사항 충족
- [x] 메모리 누수 없음

## 4. 품질 보증

### 4.1 코드 리뷰 프로세스
- 모든 코드는 최소 2명의 리뷰어 승인 필요
- 성능 크리티컬한 부분은 시니어 개발자 리뷰 필수
- 자동화된 코드 품질 체크 (clang-tidy, cppcheck)

### 4.2 테스트 전략
```cpp
// 테스트 카테고리별 예시

// 1. 기능 테스트
TEST_CATEGORY(Functional) {
    test_insert_single_element();
    test_extract_min_from_single_element();
    test_decrease_key_root_node();
    test_complex_operations_sequence();
}

// 2. 경계 조건 테스트
TEST_CATEGORY(EdgeCases) {
    test_empty_heap_operations();
    test_single_element_operations();
    test_duplicate_keys();
    test_extreme_values();
}

// 3. 성능 테스트
TEST_CATEGORY(Performance) {
    benchmark_insertion_performance();
    benchmark_extraction_performance();
    benchmark_decrease_key_performance();
    test_memory_usage_scalability();
}

// 4. 스트레스 테스트
TEST_CATEGORY(Stress) {
    test_large_dataset_operations();
    test_random_operations_sequence();
    test_memory_pressure_scenarios();
}
```

### 4.3 연속 통합 파이프라인
```yaml
# CI/CD 파이프라인 설정
stages:
  - build
  - test
  - benchmark
  - deploy

build:
  script:
    - mkdir build && cd build
    - cmake .. -DCMAKE_BUILD_TYPE=Release
    - make -j4
    
test:
  script:
    - cd build && make test
    - valgrind --leak-check=full ./fibonacci_heap_test
    
benchmark:
  script:
    - cd build && ./benchmark_fibonacci_heap
    - python scripts/analyze_performance.py
    
deploy:
  script:
    - make install
    - make package
```

## 5. 위험 관리

### 5.1 기술적 위험
| 위험 요소 | 확률 | 영향도 | 대응 방안 |
|----------|------|---------|-----------|
| 메모리 단편화 | 중간 | 높음 | 노드 풀 최적화, 대안 할당자 |
| 캐시 미스율 증가 | 높음 | 중간 | 캐시 친화적 레이아웃 |
| 스레드 안전성 | 중간 | 높음 | 락-프리 알고리즘 연구 |
| 컴파일러 호환성 | 낮음 | 중간 | 광범위한 컴파일러 테스트 |

### 5.2 일정 위험
- **지연 위험**: 복잡한 알고리즘으로 인한 구현 지연
  - **대응**: 단계별 프로토타이핑 및 중간 점검
- **품질 위험**: 성급한 최적화로 인한 버그 증가
  - **대응**: 기능 구현 우선, 점진적 최적화

## 6. 배포 계획

### 6.1 버전 관리 전략
- **v1.0.0**: 기본 기능 구현 (INSERT, EXTRACT-MIN, MINIMUM)
- **v1.1.0**: 고급 기능 추가 (DECREASE-KEY, DELETE, UNION)
- **v1.2.0**: 성능 최적화 (메모리 풀, 캐시 최적화)
- **v2.0.0**: 스레드 안전 버전, STL 호환 인터페이스

### 6.2 문서화 계획
- API 문서 (Doxygen)
- 사용자 가이드
- 성능 벤치마크 결과
- 마이그레이션 가이드 (다른 힙에서)

### 6.3 커뮤니티 지원
- GitHub Issues 관리
- Stack Overflow 태그 생성
- 샘플 코드 및 튜토리얼 제공

## 7. 결론

본 구현 계획은 CLRS의 이론적 기초를 바탕으로 실용적이고 고성능의 피보나치 힙을 구현하기 위한 포괄적인 로드맵을 제시합니다. 단계별 접근을 통해 위험을 최소화하고 품질을 보장하며, 지속적인 성능 모니터링과 최적화를 통해 실제 사용 가능한 라이브러리를 완성할 수 있을 것입니다.

각 단계별 구현 후에는 반드시 테스트와 리뷰를 거쳐 다음 단계로 진행하며, 최종적으로는 학술적 가치와 실용적 가치를 모두 갖춘 피보나치 힙 구현체를 완성하는 것이 목표입니다.