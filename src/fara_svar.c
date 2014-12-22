
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

// https://github.com/flon-io/aranye

#define _POSIX_C_SOURCE 200809L

//#include <stdlib.h>

#include "aabro.h"
#include "fara_svar.h"


fabr_parser *parser = NULL;

void init_parser()
{
  fabr_parser *definition =
    fabr_n_seq(
      "d",
      fabr_rex("[ \t]*"),
      fabr_n_rex("k", "\\$[a-zA-Z0-9_-]+"),
      fabr_rex("[ \t]*:[ \t]*"),
      fabr_n_rex("v", "[^;\r\n]+"),
      fabr_rex(".*"),
      NULL);

  fabr_parser *line =
    fabr_n_rex("l", ".+");

  parser = fabr_alt(definition, line, NULL);
}


char *fara_extrapolate(char *line, flu_dict *vars)
{
  if (parser == NULL) init_parser();

  //printf("line >%s<\n", line);
  //fabr_tree *tt = fabr_parse_f(line, 0, parser, 0);
  //flu_putf(fabr_tree_to_string(tt, line, 1));

  fabr_tree *t = fabr_parse_all(line, 0, parser);

  flu_putf(fabr_tree_to_string(t, line, 1));

  fabr_tree *d = fabr_tree_lookup(t, "d");
  if (d)
  {
    flu_list_setk(
      vars,
      fabr_lookup_string(line, d, "k"),
      fabr_lookup_string(line, d, "v"),
      0);

    return flu_sprintf("//%s", line);
  }

  return NULL;
}

