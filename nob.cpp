#include <stdio.h>
#include <stdlib.h>
#define NOB_IMPLEMENTATION
#include "nob.h"
#include "nob_addons.h"
#define FLAG_IMPLEMENTATION
#include "flag.h"

#define BUILD_FOLDER "build/"
#define SOURCE_FOLDER "src/"
#define THIRDPARTY_FOLDER "thirdparty/"

#define EXECUTABLE (BUILD_FOLDER "chess-engine")
typedef struct {
    char **items;
    size_t count;
    size_t capacity;

} F_Args;



void usage(FILE *stream) {
  fprintf(stream, "Usage: ./nob [OPTIONS]\n");
  fprintf(stream, "OPTIONS:\n");
  flag_print_options(stream);
}

void print_list(const char **items, size_t count) {
  printf("[");
  for (size_t i = 0; i < count; ++i) {
    if (i > 0) { printf(", "); }
    printf("%s", items[i]);
  }
  printf("]\n");
}

int main(int argc, char **argv) {
  F_Args f_args = {0};
  Cmd cmd = {0};
  for (int i = 0; i < argc; ++i) { da_append(&f_args, strdup(argv[i])); }
  NOB_GO_REBUILD_URSELF(argc, argv);

  nob_set_log_handler(nob_cancer_log_handler);

  bool help = false;
  bool compile = false;
  bool debug = false;
  char *debugger = "lldb";
  bool   run = false;
  flag_bool_var(&help, "-help", false,
                "Print this help to stdout and exit with 0");
  flag_bool_var(&help, "h", false, "Print this help to stdout and exit with 0");
  flag_bool_var(&compile, "-compile", false, "Compile the project");
  flag_bool_var(&compile, "c", false, "Compile the project");

  flag_bool_var(&debug, "-debug", false, "Build in debug mode");
  flag_bool_var(&debug, "d", false, "Build in debug mode");

  flag_bool_var(&run, "-run", false, "Run the project");
  flag_bool_var(&run, "r", false, "Run the project");

  flag_str_var(&debugger, "-debugger", "lldb", "The debugger to use");


  if (f_args.count == 1) {
    usage(stderr);
    exit(1);
  }

  if (!flag_parse(f_args.count, f_args.items)) {
    usage(stderr);
    flag_print_error(stderr);
    exit(1);
  }

  f_args.count = flag_rest_argc();
  f_args.items = flag_rest_argv();

  if (help) {
    usage(stdout);
    exit(0);
  }

  if (compile) {
    if (!mkdir_if_not_exists(BUILD_FOLDER)) return 1;
    clangpp(&cmd);
    clangpp_flags(&cmd);
    cmd_append(&cmd, "-I", THIRDPARTY_FOLDER "nob.h");
    cmd_append(&cmd, "-I", THIRDPARTY_FOLDER "flag.h");
    cmd_append(&cmd, "-I", THIRDPARTY_FOLDER "ht.h");
    cmd_append(&cmd, "-I", THIRDPARTY_FOLDER );
    cmd_append(&cmd, "-I", ".");
    cmd_append(&cmd, "-Wno-unused-function");
    cmd_append(&cmd, "-Wno-unused-variable");
    cmd_append(&cmd, "-Wno-unused-parameter");
    cmd_append(&cmd, "-Wno-missing-field-initializers");
    cmd_append(&cmd, "-Wno-unused-value");
    cmd_append(&cmd, "-Wno-writable-strings");

    cmd_append(&cmd, "-o", EXECUTABLE, SOURCE_FOLDER "main.cpp");
    if (!cmd_run(&cmd)) return 1;
  }

  if (debug) {
    cmd_append(&cmd, debugger);
  }

   if (run) {
    cmd_append(&cmd, EXECUTABLE);
    if (!cmd_run(&cmd)) return 1;
  }
  return 0;
}
