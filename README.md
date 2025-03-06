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

### utils-log

+ 添加easyloging++.cpp .hpp

### utils-jsoncpp

+ 添加jsoncpp
