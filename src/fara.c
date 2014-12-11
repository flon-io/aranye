
//
// Copyright (c) 2014-2014, John Mettraux, jmettraux+flon@gmail.com
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

#include <unistd.h>

#include "flutil.h"
#include "fara_haml.h"


static void print_usage()
{
  fprintf(stderr, "\n");
  fprintf(stderr, "fara {source haml} {target html}\n");
  fprintf(stderr, "\n");
}

int main(int argc, char *argv[])
{
  if (argc != 3) { print_usage(); return 1; }

  char *in = argv[1];
  char *out = argv[2];

  int r;

  fara_node *n = fara_haml_parse_f(in, NULL, NULL, NULL);

  if (n == NULL) { perror("couldn't generate html"); return 1; }

  char *dir = flu_dirname(out);
  r = flu_mkdir_p(dir, 0755);

  if (r != 0) { perror("couldn't make the dirs to the target file"); return 1; }

  free(dir);

  char *s = fara_node_to_html(n, 1);
  r = flu_writeall(out, s);
  free(s);

  if (r != 1) { perror("failed to write target file"); return 1; }

  fara_node_free(n);

  return 0; // success
}

