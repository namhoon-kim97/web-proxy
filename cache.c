#include "cache.h"
#include "csapp.h"

// 새로운 노드 생성
static Node *createNode(char *uri, char *data, size_t data_length) {
  Node *newNode = (Node *)malloc(sizeof(Node));
  if (!newNode)
    return NULL;
  strcpy(newNode->uri, uri);
  newNode->data = (char *)malloc(data_length);
  if (!newNode->data) {
    free(newNode);
    return NULL;
  }
  memcpy(newNode->data, data, data_length);
  newNode->data_length = data_length;
  newNode->prev = newNode->next = NULL;
  return newNode;
}

// 캐시 생성
LRU_Cache *createCache(int capacity) {
  LRU_Cache *cache = (LRU_Cache *)malloc(sizeof(LRU_Cache));
  if (!cache)
    return NULL;
  cache->capacity = capacity;
  cache->size = 0;
  cache->head = cache->tail = NULL;
  return cache;
}

// LRU 메커니즘에 따라 노드를 헤드로 이동
void moveToHead(LRU_Cache *cache, Node *node) {
  if (node == cache->head)
    return;
  if (node == cache->tail) {
    cache->tail = node->prev;
    cache->tail->next = NULL;
  } else if (node->prev) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
  }
  node->next = cache->head;
  node->prev = NULL;
  cache->head->prev = node;
  cache->head = node;
}

// URI를 기반으로 노드 찾기
Node *find_cache(LRU_Cache *cache, char *uri) {
  Node *node = cache->head;
  while (node) {
    if (strcmp(node->uri, uri) == 0) {
      return node;
    }
    node = node->next;
  }
  return NULL;
}

// 캐시된 데이터를 클라이언트에게 전송
void send_cache(int fd, Node *node) {
  Rio_writen(fd, node->data, node->data_length);
}

// 캐시에 새로운 데이터를 추가
void add_cache(LRU_Cache *cache, char *uri, char *data, size_t data_length) {
  if (cache->size == cache->capacity) { // 캐시가 꽉 찼으면
    Node *tail = cache->tail;
    if (cache->head == cache->tail) {
      cache->head = cache->tail = NULL;
    } else {
      cache->tail = tail->prev;
      cache->tail->next = NULL;
    }
    free(tail->data);
    free(tail);
    cache->size--;
  }
  Node *newNode = createNode(uri, data, data_length);
  if (!newNode)
    return;
  if (cache->head == NULL) {
    cache->head = cache->tail = newNode;
  } else {
    newNode->next = cache->head;
    cache->head->prev = newNode;
    cache->head = newNode;
  }
  cache->size++;
}

// 캐시 해제
void freeCache(LRU_Cache *cache) {
  Node *current = cache->head;
  while (current != NULL) {
    Node *next = current->next;
    free(current->data); // 데이터 메모리 해제
    free(current);       // 노드 메모리 해제
    current = next;
  }
  free(cache); // 캐시 구조체 메모리 해제
}