#pragma once
#define DEBUG 1

#if DEBUG
#define DEBUG_LOG(format, ...)                          \
do {                                                \
char buffer[128];                               \
sprintf(buffer, format, ##__VA_ARGS__);         \
Serial.println(buffer);                         \
} while (0)

#define DEBUG_LOG_NO_EOL(format, ...)                   \
do {                                                \
char buffer[128];                               \
sprintf(buffer, format, ##__VA_ARGS__);         \
Serial.print(buffer);                           \
} while (0)
#else
#define DEBUG_LOG(format, ...) do {} while (0)
#define DEBUG_LOG_NO_EOL(format, ...) do {} while (0)
#endif
