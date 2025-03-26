# 项目介绍

***维护一个日常开发经常用到的多功能组件库、根据开发需要自行获取***  
***拒绝重复冗余的工作，减少代码工作量***

---

## 维护项目列表

### utils-mqtt

+ 使用c++封装libmosquitto
+ 使用互斥锁、增加线程安全
+ 在publish增强、添加重连机制
+ 使用函数指针、lambda表达式、增加代码可读性
+ 使用mosquitto_loop_start()替代mosquitto_loop()
+ 在析构函数中条件mosiquitto_loop_stop()
+ 引入m_utils.hpp,优化打印调式信息

### utils-log

+ 添加easyloging++.cpp .hpp

### utils-jsoncpp

+ 添加jsoncpp

### utils-Tpool

+ 增加c++线程池 -std=c++11
+ 支持cpu亲和性
+ 支持优先级队列
+ 添加inline关键字，避免函数被翻译成多个单元导致重复定义；
+ 使用inline关键字修饰函数，可以使其定义唯一

### utils-m_utls

+ 添加m_utils.hpp
+ 使用inline关键字，保证mtx全局唯一
+ 增加线程安全
