#ifdef NOB_H_
#define nob_clangpp(cmd) nob_cmd_append(cmd, "clang++")
#define nob_clangpp_flags(cmd)                                                 \
  nob_cmd_append(cmd, "-Wall", "-Wextra", "-std=c++26")

#ifdef NOB_IMPLEMENTATION
#endif

#ifndef NOB_UNSTRIP_PREFIX
#define clangpp nob_clangpp
#define clangpp_flags nob_clangpp_flags
#endif
#endif
