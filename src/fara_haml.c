
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

#include <string.h>

#include "aabro.h"
#include "fara_haml.h"


fabr_parser *haml_parser = NULL;

void haml_parser_init()
{
  fabr_parser *jsentry =
    fabr_n_seq(
      "jsentry",
      fabr_n_rex("k", "[^ \t\r\n:]+"),
      fabr_rex("[ \t]*:[ \t]*"),
      fabr_n_rex("v", "[^ \t\r\n,]+"),
      NULL);
  fabr_parser *jsatts =
    fabr_n_seq(
      "jsatts",
      fabr_rex("[ \t]*\\{[ \t]*"),
      fabr_seq(
        jsentry, fabr_seq(fabr_rex("[ \t]*,[ \t]*"), jsentry, fabr_r("*")), NULL
      ), fabr_q("?"),
      fabr_rex("[ \t]*}[ \t]*"),
      NULL);

  fabr_parser *htatts =
    fabr_string("( x=\"y\" )");

  fabr_parser *tag_id_or_class =
    fabr_n_rex("tic", "[%#\\.][a-zA-Z-_0-9]+");

  fabr_parser *ind =
    fabr_n_rex("ind", "[ ]*");

  fabr_parser *eval_eol =
    fabr_n_seq("eveol", fabr_str("="), fabr_rex("[^\r\n]*"), NULL);

  fabr_parser *blank_line =
    fabr_n_rex("bll", "[ \t]*");

  fabr_parser *text_line =
    fabr_n_rex("txl", "[^\r\n]+");

  fabr_parser *haml_comment_line =
    fabr_n_seq("hacol", fabr_string("-#"), fabr_rex("[^\r\n]*"), NULL);

  fabr_parser *html_comment_line =
    fabr_n_seq("htcol", fabr_string("/"), fabr_rex("[^\r\n]*"), NULL);

  fabr_parser *filter_line =
    fabr_n_seq( "fil", ind, fabr_rex(":[a-z]+"), NULL);

  fabr_parser *eval_line =
    fabr_n_seq("evl", fabr_string("="), fabr_rex("[^\r\n]*"), NULL);

  fabr_parser *elt_line =
    fabr_n_seq(
      "ell",
      ind,
      tag_id_or_class, fabr_q("+"),
      jsatts, fabr_q("?"),
      htatts, fabr_q("?"),
      eval_eol, fabr_q("?"),
      NULL);

  fabr_parser *line =
    fabr_n_alt(
      "l",
      elt_line, eval_line, filter_line, html_comment_line, haml_comment_line,
      text_line,
      blank_line,
      NULL);

  haml_parser =
    fabr_seq(
      line,
      fabr_seq(
        fabr_rex("[\n\r]"),
        line,
        fabr_r("*")),
      NULL);
}

static void stack(fara_node *n, const char *s, fabr_tree *t)
{
  //int ni = (int)n->data;

  if (strcmp(t->name, "ell") == 0)
  {
  }
  else if (strcmp(t->name, "txl") == 0)
  {
    fara_node_push(n, fara_node_malloc(fabr_tree_string(s, t), NULL));
  }
}

fara_node *fara_haml_parse(const char *s)
{
  if (haml_parser == NULL) haml_parser_init();

  //puts("[1;30m"); puts(fabr_parser_to_string(haml_parser)); puts("[0;0m");
  //fabr_tree *t = fabr_parse_all(s, 0, haml_parser);

  printf(">[0;33m%s[0;0m<\n", s);

  //fabr_tree *tt = fabr_parse_f(s, 0, haml_parser, 0);
  //flu_putf(fabr_tree_to_string(tt, s, 1));
  //fabr_tree_free(tt);

  fabr_tree *t = fabr_parse_all(s, 0, haml_parser);
  //flu_putf(fabr_tree_to_string(t, s, 1));

  fara_node *r = fara_node_malloc(NULL, NULL); // document node

  flu_list *ls = fabr_tree_list_named(t, "l");

  for (flu_node *n = ls->first; n; n = n->next)
  {
    fabr_tree *nl = ((fabr_tree *)n->item)->child;
    puts("---"); flu_putf(fabr_tree_to_string(nl, s, 1));
    stack(r, s, nl);
  }

  while (r->parent) r = r->parent;

  return r;
}

