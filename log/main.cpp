#include "easylogging++.hpp"

INITIALIZE_EASYLOGGINGPP

int main() {
  //   init logger
  el::Configurations conf;
  conf.setToDefault();
  conf.set(el::Level::Global, el::ConfigurationType::Format,
           "[%level | %datetime] | %msg");
  conf.set(el::Level::Global, el::ConfigurationType::ToStandardOutput, "true");
  conf.set(el::Level::Global, el::ConfigurationType::ToFile, "true");
  conf.set(el::Level::Global, el::ConfigurationType::Filename, "xxx.log");
  conf.set(el::Level::Global, el::ConfigurationType::MaxLogFileSize,
           "20971520"); // 20M
  el::Loggers::reconfigureLogger("default", conf);

  LOG(INFO) << "This is an info log message.";
  LOG(DEBUG) << "This is a debug log message.";
  LOG(WARNING) << "This is a warning log message.";
  LOG(ERROR) << "This is an error log message.";

  return 0;
}
