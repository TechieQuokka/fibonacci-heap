# 피보나치 힙 이론적 배경

## 1. 개요

피보나치 힙(Fibonacci Heap)은 Michael L. Fredman과 Robert E. Tarjan이 1984년에 개발한 우선순위 큐 자료구조입니다. 이 자료구조는 특히 그래프 알고리즘에서 뛰어난 성능을 보이며, 다익스트라의 최단 경로 알고리즘과 프림의 최소 신장 트리 알고리즘의 시간 복잡도를 개선하는 데 사용됩니다.

## 2. 기본 구조

### 2.1 노드 구조
피보나치 힙의 각 노드는 다음과 같은 필드를 가집니다:

```
Node:
├── key: 노드의 키 값
├── degree: 자식 노드의 개수
├── parent: 부모 노드에 대한 포인터
├── child: 자식 노드 중 하나에 대한 포인터
├── left: 형제 노드 중 왼쪽 노드에 대한 포인터
├── right: 형제 노드 중 오른쪽 노드에 대한 포인터
└── mark: 마킹 여부 (boolean)
```

### 2.2 힙 구조
피보나치 힙은 다음과 같이 구성됩니다:

```
Fibonacci Heap:
├── min: 최솟값 노드에 대한 포인터
├── n: 힙에 포함된 노드의 총 개수
└── root_list: 루트 노드들의 원형 이중 연결 리스트
```

## 3. 구조적 특성

### 3.1 힙 특성 (Heap Property)
- **최소 힙 특성**: 모든 노드에서 부모의 키는 자식의 키보다 작거나 같습니다.
- **루트 리스트**: 힙의 루트 노드들은 원형 이중 연결 리스트로 연결됩니다.

### 3.2 피보나치 특성
- **Degree 제한**: degree가 k인 노드를 루트로 하는 서브트리의 크기는 최소 F_{k+2}입니다. (F_i는 i번째 피보나치 수)
- **마킹 시스템**: 루트가 아닌 노드는 최대 하나의 자식을 잃을 수 있으며, 두 번째 자식을 잃으면 부모로부터 절단됩니다.

### 3.3 피보나치 수열과의 관계
피보나치 힙의 이름은 다음 정리에서 유래됩니다:

**정리**: degree가 k인 임의의 노드 x에 대해, size(x) ≥ F_{k+2}
여기서 size(x)는 x를 루트로 하는 서브트리의 노드 개수입니다.

**증명 아이디어**:
- x의 자식들을 degree 순으로 정렬했을 때, i번째 자식의 degree는 최소 i-2입니다.
- 귀납법을 통해 S(k) ≥ F_{k+2}임을 증명할 수 있습니다.

## 4. 주요 연산

### 4.1 INSERT 연산
```
FIB-HEAP-INSERT(H, x):
1. x.degree = 0
2. x.parent = NIL
3. x.child = NIL
4. x.mark = FALSE
5. H의 루트 리스트에 x를 추가
6. if H.min = NIL or x.key < H.min.key:
7.    H.min = x
8. H.n = H.n + 1
```

### 4.2 EXTRACT-MIN 연산
```
FIB-HEAP-EXTRACT-MIN(H):
1. z = H.min
2. if z ≠ NIL:
3.    z의 모든 자식을 H의 루트 리스트에 추가
4.    H의 루트 리스트에서 z를 제거
5.    if z = z.right:  # z가 유일한 노드였다면
6.       H.min = NIL
7.    else:
8.       H.min = z.right
9.       CONSOLIDATE(H)
10.   H.n = H.n - 1
11. return z
```

### 4.3 DECREASE-KEY 연산
```
FIB-HEAP-DECREASE-KEY(H, x, k):
1. if k > x.key:
2.    error "new key is greater than current key"
3. x.key = k
4. y = x.parent
5. if y ≠ NIL and x.key < y.key:
6.    CUT(H, x, y)
7.    CASCADING-CUT(H, y)
8. if x.key < H.min.key:
9.    H.min = x
```

## 5. 분할상환 분석 (Amortized Analysis)

### 5.1 포텐셜 함수
피보나치 힙의 포텐셜 함수는 다음과 같이 정의됩니다:
```
Φ(H) = t(H) + 2m(H)
```
여기서:
- t(H): 힙 H의 루트 리스트에 있는 트리의 개수
- m(H): 힙 H에서 마킹된 노드의 개수

### 5.2 연산별 분할상환 시간
- **INSERT**: O(1)
- **MINIMUM**: O(1)
- **EXTRACT-MIN**: O(log n)
- **UNION**: O(1)
- **DECREASE-KEY**: O(1)
- **DELETE**: O(log n)

## 6. 정리와 증명

### 6.1 주요 정리
**정리 19.1**: n개의 노드를 가진 피보나치 힙에서 임의의 노드의 degree의 최댓값은 O(log n)입니다.

**증명**: degree가 k인 노드의 서브트리 크기가 최소 F_{k+2}이고, F_{k+2} ≥ φ^k (φ = (1+√5)/2)이므로, k ≤ log_φ n = O(log n)입니다.

### 6.2 분할상환 분석의 핵심
CONSOLIDATE 과정에서 degree가 같은 트리들을 합병하여 트리의 개수를 줄이고, CASCADING-CUT을 통해 마킹된 노드의 수를 제한하는 것이 효율성의 핵심입니다.

## 7. 응용 분야

### 7.1 그래프 알고리즘
- **다익스트라 알고리즘**: O((V + E) log V) → O(E + V log V)
- **프림 알고리즘**: O(E log V) → O(E + V log V)

### 7.2 기타 응용
- 네트워크 최적화 문제
- 최단 경로 문제의 변형들
- 동적 최적화 문제들

## 8. 다른 힙과의 비교

| 연산 | 이진 힙 | 이항 힙 | 피보나치 힙 |
|------|---------|---------|-------------|
| INSERT | O(log n) | O(log n) | O(1) |
| MINIMUM | O(1) | O(log n) | O(1) |
| EXTRACT-MIN | O(log n) | O(log n) | O(log n) |
| UNION | O(n) | O(log n) | O(1) |
| DECREASE-KEY | O(log n) | O(log n) | O(1) |
| DELETE | O(log n) | O(log n) | O(log n) |

피보나치 힙의 주요 장점은 INSERT, UNION, DECREASE-KEY 연산이 상수 시간에 수행된다는 점입니다.