#include "threadpool.hpp"
#include <iostream>
#include <unistd.h>
using namespace Tpool;
int cnt = 0;
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
    if (cnt++ > 10) {
      std::cout << "task2 is exiting." << std::endl;
      break;
    }
    sleep(2);
  }
}

int main(int argc, char *argv[]) {
  ThreadPool tpool(10, true);
  int a = 0;
  tpool.enqueue(ThreadPool::Priority::Normal, task1, 10, 10);
  tpool.enqueue(ThreadPool::Priority::Normal, task2);
  tpool.enqueue(ThreadPool::Priority::Normal, [&a] {
    while (1) {
      std::cout << "task3 is running." << std::endl;
      if (a++ > 15) {
        std::cout << "task3 is exiting." << std::endl;
        break;
      }
      sleep(4);
    }
  });
  tpool.shutdown();
  std::cout << "ThreadPool shutdown." << std::endl;
  return 0;
}