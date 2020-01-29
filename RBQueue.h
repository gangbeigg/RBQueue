#ifndef RBQUEUE_H_
#define RBQUEUE_H_

#include <stdint.h>

class RBQueue {
public:
  enum {
                 MAX_MSG_SIZE = 2048
  };
	               RBQueue(uint32_t capacity = 1024 * 1024);
	               ~RBQueue();
	bool           push(const void* data, uint16_t len);
	void*          pop(uint16_t& len);
  const uint32_t capacity() { return _capacity; };
  const uint32_t size() { return _count; }
  const uint32_t count() { return _count; }
  void           dump() { printf("head[%u]tail[%u]capacity[%u]size[%u]count[%u]\n", _head, _tail, _capacity, _size, _count); }

	uint32_t       _head;
	uint32_t       _tail;
	uint8_t*       _queue;
	uint32_t       _capacity;
  uint32_t       _size;
	uint32_t       _count;
};

#endif
