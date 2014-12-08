
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
#include "fara_haml.h"


fabr_parser *haml_parser = NULL;

void haml_parser_init()
{
  fabr_parser *attributes =
    fabr_string("{atts}");

  fabr_parser *tag_id_or_class =
    fabr_n_rex("tic", "[%#\.][a-zA-Z-_0-9]+");

  fabr_parser *blank_line =
    fabr_n_rex("bl", "[ \t]*");

  fabr_parser *haml_line =
    fabr_n_seq(
      "hl",
      tag_id_or_class, fabr_q("+"),
      attributes, fabr_q("?"),
      NULL);

  fabr_parser *text_line =
    fabr_n_rex("tl", "[^\r\n]+");

  fabr_parser *line =
    fabr_alt(haml_line, text_line, blank_line, NULL);

  haml_parser =
    fabr_seq(
      line,
      fabr_seq(
        fabr_rex("[\n\r]"),
        line,
        fabr_r("*")),
      NULL);
}

fara_node *fara_haml_parse(const char *s)
{
  if (haml_parser == NULL) haml_parser_init();

  //puts("[1;30m"); puts(fabr_parser_to_string(haml_parser)); puts("[0;0m");
  //fabr_tree *t = fabr_parse_all(s, 0, haml_parser);
  fabr_tree *t = fabr_parse_f(s, 0, haml_parser, 0);
  // TODO: deal with errors (t->result < 0)

  printf(">[0;33m%s[0;0m<\n", s);
  flu_putf(fabr_tree_to_string(t, s, 1));

  return NULL;
}

