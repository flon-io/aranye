
//
// Copyright (c) 2014-2015, John Mettraux, jmettraux+flon@gmail.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// Made in Japan.
//

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "flutil.h"
#include "aabro.h"
#include "fara_svar.h"


static void print_usage()
{
  fprintf(stderr, "\n");
  fprintf(stderr, "fara-svar {source css}+\n");
  fprintf(stderr, "\n");
}

static int process(const char *path)
{
  printf("\n/* %s */\n\n", path);

  flu_dict *vars = flu_list_malloc();

  FILE *f = fopen(path, "r");
  if (f == NULL) goto _over;

  char *line = NULL;
  size_t n = 0;

  while (1)
  {
    if (getline(&line, &n, f) == -1) break;

    char *s = fara_extrapolate(line, vars);
    if (s) printf(s);
    free(s);
  }
  free(line);

  fclose(f);

_over:

  flu_list_free_all(vars);

  if (errno)
  {
    char *s = flu_sprintf("error processing %s", path);
    perror(s);
    free(s);
  }

  return errno;
}

int main(int argc, char *argv[])
{
  if (argc < 2) { print_usage(); return 1; }

  int r = 0;

  for (int i = 1; i < argc; ++i)
  {
    int rr = process(argv[i]);
    if (r == 0) r = rr;
  }

  return r;
}

