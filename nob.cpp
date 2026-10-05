#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NOB_IMPLEMENTATION
#include "nob_addons.h"
#define FLAG_IMPLEMENTATION
#include "flag.h"

#define BUILD_FOLDER "build/"
#define SOURCE_FOLDER "src/"
#define THIRDPARTY_FOLDER "thirdparty/"
#define THIRDPARTY_INCLUDE_FOLDER THIRDPARTY_FOLDER "include/"

#define EXECUTABLE BUILD_FOLDER "chess-engine"

Cmd         cmd = {0};

const char *cwd = get_current_dir_temp();
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

bool build_ixwebsocket(bool debug) {
  if (!mkdir_if_not_exists(BUILD_FOLDER "ixwebsocket")) { return false; }
  cmd_append(&cmd, "cmake");
  cmd_append(&cmd, "-DUSE_TLS=1");
  cmd_append(&cmd, "-DUSE_OPEN_SSL=1");
  cmd_append(&cmd, "-DUSE_ZLIB = 1");
  cmd_append(&cmd, "--install-prefix",
             temp_sprintf("%s%s", cwd, BUILD_FOLDER "ixwebsocket"));
  cmd_append(&cmd, "-B", BUILD_FOLDER "ixwebsocket");
  cmd_append(&cmd, "-S", THIRDPARTY_FOLDER "IXWebSocket");

  if (!cmd_run(&cmd)) { return false; }

  cmd_append(&cmd, "make");
  cmd_append(&cmd, "-C", BUILD_FOLDER "ixwebsocket");
  if (!cmd_run(&cmd)) { return false; }

  return true;
}

int main(int argc, char **argv) {
  addon_init_logging();
  F_Args f_args = {0};

  for (int i = 0; i < argc; ++i) { da_append(&f_args, strdup(argv[i])); }
  NOB_GO_REBUILD_URSELF(argc, argv);

  bool  help = false;
  bool  compile = false;
  bool  debug = false;
  char *debugger = (char *)"lldb";
  bool  run = false;
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
    if (!mkdir_if_not_exists(BUILD_FOLDER)) { return 1; }
    if (!build_ixwebsocket(debug)) { return 1; }
    clangpp(&cmd);
    cmd_append(&cmd, SOURCE_FOLDER "main.cpp");
    if (debug) {
      cmd_append(&cmd, "-g");
      // NEU: Zwingt Clang, die .o Datei zu behalten, damit dsymutil sie lesen
      // kann
      cmd_append(&cmd, "-save-temps=obj");
      // NEU: Legt die .o Datei sauber im "build/"-Ordner ab anstatt im
      // Hauptverzeichnis
      cmd_append(&cmd, "-dumpdir", BUILD_FOLDER);
    } else {
      cmd_append(&cmd, "-O3");
    }
    clangpp_flags(&cmd);
    cmd_append(&cmd, "-I", THIRDPARTY_INCLUDE_FOLDER);
    cmd_append(&cmd, "-I", SOURCE_FOLDER);
    cmd_append(&cmd, "-I", BUILD_FOLDER "assets");
    cmd_append(&cmd, "-I", ".");
    cmd_append(&cmd, "-L", BUILD_FOLDER "ixwebsocket");
    cmd_append(&cmd, "-lixwebsocket");
    cmd_append(&cmd, "-lm");
    cmd_append(&cmd, "-lz");

#ifdef __APPLE__

    cmd_append(&cmd, "-mmacos-version-min=26.0");

    cmd_append(&cmd, "-framework", "Security");
    cmd_append(&cmd, "-framework", "Foundation");
#elif defined(__linux__)
    cmd_append(&cmd, "-lssl");
    cmd_append(&cmd, "-lcrypto");
// linux specific flags can be added here
#elif defined(_WIN32)
    cmd_append(&cmd, "-lssl");
    cmd_append(&cmd, "-lcrypto");
// windows specific flags can be added here
//----------------------------------
// more platforms can be added here
#endif

    cmd_append(&cmd, "-Wno-unused-function");
    cmd_append(&cmd, "-Wno-unused-variable");
    cmd_append(&cmd, "-Wno-unused-parameter");
    cmd_append(&cmd, "-Wno-missing-field-initializers");
    cmd_append(&cmd, "-Wno-unused-value");
    cmd_append(&cmd, "-Wno-writable-strings");
    cmd_append(&cmd, "-Wno-unused-command-line-argument");

    cmd_append(&cmd, "-static-libclosure", "-static-libsan", "-static-openmp");

    cmd_append(&cmd, "-o", EXECUTABLE);
    if (!cmd_run(&cmd)) { return 1; }
  }

  if (debug && run) { cmd_append(&cmd, debugger); }

  if (run) {
    cmd_append(&cmd, EXECUTABLE);
    if (!cmd_run(&cmd)) { return 1; }
  }
  return 0;
}
