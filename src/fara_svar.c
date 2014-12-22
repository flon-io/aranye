
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

#include "flutil.h"
#include "aabro.h"
#include "fara_svar.h"


fabr_parser *parser = NULL;

static void init_parser()
{
  fabr_parser *literal =
    fabr_n_rex("l", "[^\\$;\n\r]+");
  fabr_parser *reference =
    fabr_n_rex("r", "\\$[a-zA-Z-0-9_-]+");
  fabr_parser *semi =
    fabr_n_rex("s", "[;\n\r]+");

  fabr_parser *text =
    fabr_rep(fabr_alt(reference, literal, semi, NULL), 1, -1);

  fabr_parser *definition =
    fabr_n_seq(
      "d",
      fabr_rex("[ \t]*"),
      fabr_n_rex("k", "\\$[a-zA-Z0-9_-]+"),
      fabr_rex("[ \t]*:[ \t]*"),
      fabr_name("v", text),
      NULL);

  parser = fabr_alt(definition, text, NULL);
}

static short lsr_filter(const fabr_tree *t)
{
  if (t->name == NULL) return 0;
  if (*t->name == 'l' || *t->name == 's' || *t->name == 'r') return 1;
  return 0;
}

char *extrapolate(char *line, fabr_tree *t, flu_dict *vars, int isval)
{
  //flu_putf(fabr_tree_to_string(t, line, 1));

  fabr_tree *d = fabr_tree_lookup(t, "d");
  if (d)
  {
    char *k = fabr_lookup_string(line, t, "k");
    char *v0 = extrapolate(line, fabr_tree_lookup(t, "v"), vars, 0);
    char *v1 = extrapolate(line, fabr_tree_lookup(t, "v"), vars, 1);
    flu_list_setk(vars, k, v1, 0);

    char *r = flu_sprintf("//%s: %s", k, v0);
    free(v0);

    return r;
  }

  flu_sbuffer *b = flu_sbuffer_malloc();

  flu_list *l = fabr_tree_list(t, lsr_filter);
  for (flu_node *fn = l->first; fn; fn = fn->next)
  {
    fabr_tree *tt = fn->item;
    //flu_putf(fabr_tree_to_string(tt, line, 1));

    if (*tt->name == 'l' || (*tt->name == 's' && isval == 0))
    {
      flu_sbputs_n(b, line + tt->offset, tt->length);
    }
    else if (*tt->name == 'r')
    {
      char *k = fabr_tree_string(line, tt);
      char *v = flu_list_get(vars, k);
      if (v) flu_sbputs(b, v);
      free(k);
    }
  }
  flu_list_free(l);

  return flu_sbuffer_to_string(b);
}

char *fara_extrapolate(const char *line, flu_dict *vars)
{
  if (parser == NULL) init_parser();

  //printf("line >%s<\n", line);
  //fabr_tree *tt = fabr_parse_f(line, 0, parser, 0);
  //flu_putf(fabr_tree_to_string(tt, line, 1));

  fabr_tree *t = fabr_parse_all(line, 0, parser);

  //flu_putf(fabr_tree_to_string(t, line, 1));

  char *r = extrapolate(line, t, vars, 0);

  fabr_tree_free(t);

  return r;
}

