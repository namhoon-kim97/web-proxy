#ifndef CACHE_H
#define CACHE_H

#include "csapp.h"
#include <stdlib.h>
#include <string.h>

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

typedef struct Node {
  char uri[MAXLINE];
  char *data;
  size_t data_length;
  struct Node *prev, *next;
} Node;

typedef struct {
  Node *head, *tail;
  int capacity, size;
} LRU_Cache;

static Node *createNode(char *uri, char *data, size_t data_length);

LRU_Cache *createCache(int capacity);
void moveToHead(LRU_Cache *cache, Node *node);
Node *find_cache(LRU_Cache *cache, char *uri);
void send_cache(int fd, Node *node);
void add_cache(LRU_Cache *cache, char *uri, char *data, size_t data_length);
void freeCache(LRU_Cache *cache);

#endif
