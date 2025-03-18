#ifndef M_UTILS_HPP
#define M_UTILS_HPP
#include <cstdio> // 使用标准库函数
#include <ctime>  // 时间处理
#include <mutex>

#define DEBUG // 开启调式
#define ANSI_COLOR(code) "\033[" #code "m"
#define COLOR_RESET ANSI_COLOR(0)
#define COLOR_RED ANSI_COLOR(31)
#define COLOR_GREEN ANSI_COLOR(32)
#define COLOR_YELLOW ANSI_COLOR(33)

#ifdef DEBUG
inline std::mutex &get_dbg_mtx() {
  static std::mutex dbg_mtx_;
  return dbg_mtx_;
}
inline void format_time(char *buffer, size_t size) {
  time_t rawtime;
  time(&rawtime);
  struct tm timeinfo;
#ifdef _WIN32
  localtime_s(&timeinfo, &rawtime);
#else
  localtime_r(&rawtime, &timeinfo);
#endif
  strftime(buffer, size, "%Y-%m-%d %H:%M:%S", &timeinfo);
}

#define LOG(level, color, stream, format, ...)                                 \
  do {                                                                         \
    std::lock_guard<std::mutex> lock(get_dbg_mtx());                           \
    char timestamp[20];                                                        \
    format_time(timestamp, sizeof(timestamp));                                 \
    std::fprintf(stream, "%s %s%-3s%s [%s:%d] %s: " format "\n", timestamp,    \
                 color, level, COLOR_RESET, __FILE__, __LINE__, __func__,      \
                 ##__VA_ARGS__);                                               \
  } while (0)

#define APP_DBG(format, ...)                                                   \
  LOG("DBG", COLOR_GREEN, stdout, format, ##__VA_ARGS__)
#define APP_WAR(format, ...)                                                   \
  LOG("WAR", COLOR_YELLOW, stdout, format, ##__VA_ARGS__)
#define APP_ERR(format, ...)                                                   \
  LOG("ERR", COLOR_RED, stderr, format, ##__VA_ARGS__)
#else
#define APP_DBG(format, ...) ((void)0)
#define APP_WAR(format, ...) ((void)0)
#define APP_ERR(format, ...) ((void)0)
#endif

#endif
