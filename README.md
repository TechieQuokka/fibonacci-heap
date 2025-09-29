# 피보나치 힙 (Fibonacci Heap) 연구 프로젝트

## 프로젝트 개요
이 프로젝트는 Cormen, Leiserson, Rivest, Stein의 "Introduction to Algorithms" (CLRS) 교재에서 다루는 피보나치 힙 자료구조를 깊이 있게 연구하고 구현하는 것을 목표로 합니다.

## 프로젝트 구조
```
Fibonacci-heap/
├── docs/                          # 문서 디렉토리
│   ├── theory/                     # 이론적 배경
│   │   ├── fibonacci-heap-theory.md
│   │   ├── complexity-analysis.md
│   │   └── comparison.md
│   ├── architecture/               # 아키텍처 설계
│   │   ├── system-design.md
│   │   ├── interface-specification.md
│   │   └── implementation-plan.md
│   └── paper/                      # 기술 논문
│       ├── fibonacci-heap-paper.md
│       └── references.md
├── src/                           # 구현 코드
├── tests/                         # 테스트 코드
├── examples/                      # 예제 및 데모
└── README.md                      # 프로젝트 소개
```

## 연구 목표

### 1. 이론적 이해
- CLRS 교재의 피보나치 힙 이론 완전 분석
- 시간 복잡도 증명 및 분할상환 분석
- 다른 우선순위 큐 자료구조와의 비교 연구

### 2. 아키텍처 설계
- 모듈화된 피보나치 힙 시스템 아키텍처
- 확장 가능한 인터페이스 설계
- 성능 최적화 전략

### 3. 구현 및 검증
- 완전한 피보나치 힙 구현
- 포괄적 테스트 슈트
- 성능 벤치마크

### 4. 응용 연구
- 그래프 알고리즘에서의 활용
- 다익스트라 알고리즘 최적화
- 최소 신장 트리 알고리즘 개선

## 핵심 특징

### 피보나치 힙의 주요 연산
- **INSERT**: O(1) 분할상환 시간
- **MINIMUM**: O(1) 최악 시간
- **EXTRACT-MIN**: O(log n) 분할상환 시간
- **UNION**: O(1) 실제 시간
- **DECREASE-KEY**: O(1) 분할상환 시간
- **DELETE**: O(log n) 분할상환 시간

### 구조적 특성
- 힙-정렬된 트리들의 집합
- 원형 이중 연결 리스트로 구현된 루트 리스트
- 마킹 시스템을 통한 구조 유지
- 지연된 합병 (lazy consolidation)

## 연구 일정
1. **1주차**: 이론 연구 및 문서화
2. **2주차**: 아키텍처 설계 및 명세
3. **3주차**: 핵심 구현
4. **4주차**: 테스트 및 최적화
5. **5주차**: 기술 논문 작성

## 참고문헌
- Cormen, T. H., Leiserson, C. E., Rivest, R. L., & Stein, C. (2009). Introduction to Algorithms (3rd ed.)
- Fredman, M. L., & Tarjan, R. E. (1987). Fibonacci heaps and their uses in improved network optimization algorithms

## 라이센스
이 프로젝트는 학술 연구 목적으로 제작되었습니다.