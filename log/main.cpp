#include "easylogging++.hpp"

INITIALIZE_EASYLOGGINGPP
#define ANSI_COLOR(code) "\033[" #code "m"
#define COLOR_RESET ANSI_COLOR(0)
#define COLOR_RED ANSI_COLOR(31)
#define COLOR_GREEN ANSI_COLOR(32)
#define COLOR_YELLOW ANSI_COLOR(33)
#define COLOR_BLUE ANSI_COLOR(34)
int main() {
  //   init logger
  el::Configurations conf;
  conf.setToDefault();
  conf.set(
      el::Level::Global, el::ConfigurationType::Format,
      "%datetime{%Y-%M-%d %H:%m:%s.%g} [%level] [%thread] %fbase:%line | %msg");
  conf.set(el::Level::Global, el::ConfigurationType::ToStandardOutput, "true");
  conf.set(el::Level::Global, el::ConfigurationType::ToFile, "true");
  conf.set(el::Level::Global, el::ConfigurationType::Filename, "xxx.log");
  conf.set(el::Level::Global, el::ConfigurationType::MaxLogFileSize,
           "20971520"); // 20M

  conf.set(el::Level::Info, el::ConfigurationType::Format,
           "%datetime{%Y-%M-%d %H:%m:%s.%g}" COLOR_BLUE " INF " COLOR_RESET
           "[%fbase:%line] | %msg");
  conf.set(el::Level::Warning, el::ConfigurationType::Format,
           "%datetime{%Y-%M-%d %H:%m:%s.%g}" COLOR_YELLOW " WAR " COLOR_RESET
           "[%fbase:%line] | %msg");
  conf.set(el::Level::Error, el::ConfigurationType::Format,
           "%datetime{%Y-%M-%d %H:%m:%s.%g}" COLOR_RED " ERR " COLOR_RESET
           "[%fbase:%line] | %msg");
  el::Loggers::reconfigureLogger("default", conf);

  LOG(INFO) << "This is an info log message.";
  LOG(DEBUG) << "This is a debug log message.";
  LOG(WARNING) << "This is a warning log message.";
  LOG(ERROR) << "This is an error log message.";
  return 0;
}
