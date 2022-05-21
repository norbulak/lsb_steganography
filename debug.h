#ifndef DEBUG_H
#define DEBUG_H

#define DBUG  1
#define INFO(fmt, ...) \
    printf("INFO:" fmt,##__VA_ARGS__);

#define ERROR(fmt, ...) \
    fprintf(stderr, "ERROR:%s:%d:%s(): " fmt, __FILE__, __LINE__,\
                                            __func__, ##__VA_ARGS__);

#define DEBUG(fmt, ...) \
        do { if (DBUG) fprintf(stderr, "DEBUG:%s:%d:%s(): " fmt, __FILE__, \
                                __LINE__, __func__, ##__VA_ARGS__); } while (0)
#endif
