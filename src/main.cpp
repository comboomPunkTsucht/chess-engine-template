#include <stdio.h>
#include <stdlib.h>

#define NOB_IMPLEMENTATION
#include "nob_addons.h"

#define FLAG_IMPLEMENTATION
#include "flag.h"

#define HT_IMPLEMENTATION
#include "ht.h"

int main(int argc, char **argv) {
  addon_init_logging();
  UNUSED(argc);
  UNUSED(argv);
  nob_log(INFO, "Hello, World!");
  return 0;
}
