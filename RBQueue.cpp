// RBQueue.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "RBQueue.h"

struct Msg {
  enum {
    MSG_HEAD_LEN = 2
  };
	uint16_t len;
	uint8_t  data[0];
};

RBQueue::RBQueue(uint32_t capacity /*= 1024 * 1024*/) {
	_head = 0;
	_tail = 0;
	_queue = new uint8_t[capacity];
	_capacity = capacity;
  _size = 0;
	_count = 0;
}

RBQueue::~RBQueue() {
  if (_queue) {
    delete _queue;
    _queue = NULL;
  }
}

bool RBQueue::push(const void* data, uint16_t len) {
  if (data == NULL || len > (MAX_MSG_SIZE - Msg::MSG_HEAD_LEN) || len <= 0)
    return false;
  if (_capacity - _size < 2 * MAX_MSG_SIZE)
    return false;
  uint16_t msgTotalLen = Msg::MSG_HEAD_LEN + len;
  uint32_t startPos = _head & (_capacity - 1);
  Msg* msg = (Msg*)(_queue + startPos);
  if (_capacity - startPos - 1 < MAX_MSG_SIZE) {
    msg->len = 0;
    msg = (Msg*)_queue;
    _head = 0;
  }
  msg->len = len;
  memcpy(msg->data, data, len);
  _head += msgTotalLen;
  _size += Msg::MSG_HEAD_LEN + len;
  _count++;
	return true;
}

void* RBQueue::pop(uint16_t& len) {
  if (_count == 0)
    return NULL;
  Msg* msg = (Msg*)(_queue + (_tail & (_capacity - 1)));
  if (msg->len == 0) {
    _tail = 0;
    msg = (Msg*)_queue;
  }
  uint16_t msgTotalLen = Msg::MSG_HEAD_LEN + msg->len;
  _tail += msgTotalLen;
  _size -= msgTotalLen;
  _count--;
  len = msg->len;
  return msg->data;
}

#include <vector>
#include <string>
#include <stdlib.h>
#include <assert.h>
void test(int msgNum) {
  RBQueue* queue = new RBQueue(128 * 1024 * 1024);
  std::vector<std::string> vecOriMsg;
  char buf[2048];
  for (int msgIndex = 0; msgIndex < msgNum; msgIndex++) {
    std::string msg(buf, 30 + rand() % 1024);
    vecOriMsg.push_back(msg);
    queue->push(msg.c_str(), msg.size());
    queue->dump();
  }

  std::vector<std::string> vecOutMsg;
  uint16_t len = 0;
  void* msg = queue->pop(len);
  while (NULL != msg) {
    vecOutMsg.push_back(std::string((const char*)msg, len));
    msg = queue->pop(len);
  }
  printf("oriMsgNum[%d]outMsgNum[%d]\n", vecOriMsg.size(), vecOutMsg.size());
  for (int msgIndex = 0; msgIndex < msgNum; msgIndex++)
    assert(vecOriMsg[msgIndex] == vecOutMsg[msgIndex]);
  printf("test success!\n");
  delete queue;
  queue = NULL;
}

void test2() {
  RBQueue* queue = new RBQueue(4 * 1024);
  std::vector<std::string> vecOriMsg;
  char buf[2048];
  for (int msgIndex = 0; msgIndex < 1000; msgIndex++) {
    std::string msg(buf, 30 + rand() % 1024);
    vecOriMsg.push_back(msg);
    queue->push(msg.c_str(), msg.size());
    queue->dump();
    uint16_t outLen;
    void* outMsg = queue->pop(outLen);
    if (NULL == outMsg)
      outMsg = queue->pop(outLen);
    assert(0 == memcmp(msg.c_str(), outMsg, outLen) != 0);
  }
  delete queue;
  queue = NULL;
}

int main(int argc, char* argv[]) {
  int msgNum = 100;
  if (argc == 2)
    sscanf(argv[1], "%d", &msgNum);
  test(msgNum);
  test2();
  getchar();
	return 0;
}
