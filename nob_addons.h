#ifdef NOB_H_

void nob_breackpoint(void);

#ifdef NOB_IMPLEMENTATION
#define nob_clangpp(cmd) nob_cmd_append(cmd, "clang++")
#define nob_clangpp_flags(cmd)                                                 \
  nob_cmd_append(cmd, "-Wall", "-Wextra", "-std=c++26")

  void nob_breackpoint(void) {
    #if defined(__x86_64__) || defined(__i386__)
      __asm__ volatile("int3");
    #elif defined(__aarch64__) || defined(__arm__)
      __asm__ volatile("brk 1");
    #endif
  }
#endif

#ifndef NOB_UNSTRIP_PREFIX
#define clangpp nob_clangpp
#define clangpp_flags nob_clangpp_flags
#define breackpoint nob_breackpoint
#endif
#endif
