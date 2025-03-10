#include "threadpool.hpp"
#include <iostream>
#include <unistd.h>
using namespace Tpool;
int task1(int a, int b) {
  int sum = 0;
  for (int i = 0; i < a; i++) {
    for (int j = 0; j < b; j++) {
      sum += i + j;
      std::cout << "sum: " << sum << std::endl;
    }
  }
}
void task2() {
  while (1) {
    std::cout << "task2 is running." << std::endl;
    sleep(2);
  }
}

int main(int argc, char *argv[]) {
  ThreadPool tpool(10, true);
  tpool.enqueue(ThreadPool::Priority::Normal, task1, 10, 10);
  tpool.enqueue(ThreadPool::Priority::Normal, task2);
  tpool.enqueue(ThreadPool::Priority::Normal, [] {
    while (1) {
      std::cout << "task3 is running." << std::endl;
      sleep(4);
    }
  });
  return 0;
}