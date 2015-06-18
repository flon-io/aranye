
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

// https://github.com/flon-io/aranye

#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <string.h>

#include "flutil.h"
#include "aabro.h"
#include "fara_svar.h"


//  fabr_parser *con = fabr_n_string(">", "/*");
//  fabr_parser *coff = fabr_n_string("<", "*/");
//
//  fabr_parser *literal =
//    //fabr_n_rex("l", "[^\\$;\n\r/]+");
//    fabr_n_rex("l", "(/[^/\\*\\$;\n\r]|[^\\$;\n\r/])+");
//  fabr_parser *reference =
//    fabr_n_rex("r", "\\$[a-zA-Z-0-9_-]+");
//  fabr_parser *semi =
//    fabr_n_rex("s", "[ \t]*;[ \t]*");
//
//  fabr_parser *text =
//    fabr_seq(
//      fabr_alt(reference, con, coff, literal, semi, NULL), fabr_q("+"),
//      eol,
//      NULL);

static fabr_tree *_con(fabr_input *i) { return fabr_str(">", i, "/*"); }
static fabr_tree *_coff(fabr_input *i) { return fabr_str("<", i, "*/"); }

static fabr_tree *_semi(fabr_input *i)
{
  return fabr_rex("s", i, "[ \t]*;[ \t]*");
}

static fabr_tree *_literal(fabr_input *i)
{
  return fabr_rex("l", i, "(/[^/\\*\\$;\n\r]|[^\\$;\n\r/])+");
}

static fabr_tree *_reference(fabr_input *i)
{
  return fabr_rex("r", i, "\\$[a-zA-Z-0-9_-]+");
}

static fabr_tree *_fragment(fabr_input *i)
{
  return fabr_alt(NULL, i, _reference, _con, _coff, _literal, _semi, NULL);
}

static fabr_tree *_eol(fabr_input *i)
{
  //return fabr_seq("/", i, _eoc, fabr_qmark, _lfcr, NULL);
  return fabr_rex("n", i, "(//[^\n\r]+)?[\n\r]+");
}

static fabr_tree *_text(fabr_input *i)
{
  return fabr_seq(NULL, i, _fragment, fabr_plus, _eol, NULL);
}

static fabr_tree *_key(fabr_input *i)
{
  return fabr_rex("k", i, "\\$[a-zA-Z0-9_-]+");
}
static fabr_tree *_col(fabr_input *i)
{
  return fabr_rex(NULL, i, "[ \t]*:[ \t]*");
}
static fabr_tree *_val(fabr_input *i)
{
  return fabr_rename("v", i, _text);
}
static fabr_tree *_ind(fabr_input *i)
{
  return fabr_rex(NULL, i, "[ \t]*");
}

static fabr_tree *_definition(fabr_input *i)
{
  return fabr_seq("d", i, _ind, _key, _col, _val, NULL);
}

static fabr_tree *_def_or_text(fabr_input *i)
{
  return fabr_alt(NULL, i, _definition, _text, NULL);
}

static short lsr_filter(const fabr_tree *t)
{
  if (t->name == NULL) return 0;
  //if (*t->name == 'l' || *t->name == 's' || *t->name == 'r') return 1;
  if (strchr("lsr></", *t->name)) return 1;
  return 0;
}

static char *semitrim(char *s)
{
  for (size_t i = strlen(s); ; --i)
  {
    char c = i > 0 ? s[i - 1] : 'x';
    if (strchr("; \t\n\r", c)) continue;
    s[i] = 0; break;
  }

  return s;
}

char *extrapolate(const char *line, fabr_tree *t, flu_dict *vars)
{
  fabr_puts_tree(t, line, 1);

  fabr_tree *d = fabr_tree_lookup(t, "d");
  if (d)
  {
    char *k = fabr_lookup_string(line, t, "k");
    char *v = extrapolate(line, fabr_tree_lookup(t, "v"), vars);
    flu_list_setk(vars, k, semitrim(v), 0);

    return NULL;
  }

  flu_sbuffer *b = flu_sbuffer_malloc();

  flu_list *l = fabr_tree_list(t, lsr_filter);
  for (flu_node *fn = l->first; fn; fn = fn->next)
  {
    fabr_tree *tt = fn->item;
    //flu_putf(fabr_tree_to_string(tt, line, 1));

    char n = *tt->name;

    if (n == 'r')
    {
      char *k = fabr_tree_string(line, tt);
      char *v = flu_list_get(vars, k);
      if (v) flu_sbputs(b, v);
      free(k);
      continue;
    }

    short con = *((char *)flu_list_getd(vars, "_comment_on", "f")) == 't';

    printf("line >%s< n is '%c' ", line, n);
    printf("match is >%s< ", fabr_tree_string(line, tt));
    printf("fn->next: %p\n", fn->next);

    if (n == '/' && con == 0)
      tt = fabr_tree_lookup(tt, "n");
    else if (n == '<')
      flu_list_set(vars, "_comment_on", strdup("f"));
    else if (n == '>')
      flu_list_set(vars, "_comment_on", strdup("t"));

    flu_sbputs_n(b, line + tt->offset, tt->length);
  }
  flu_list_free(l);

  return flu_sbuffer_to_string(b);
}

static char *trim(char *line)
{
  if (line == NULL) return NULL;

  for (size_t i = strlen(line); ; --i)
  {
    char c = i > 0 ? line[i - 1] : 'x';
    if (c == ' ' || c == '\t' || c == '\n' || c == '\r') continue;
    line[i] = '\n'; line[i + 1] = 0; break;
  }

  if (strcmp(line, "\n") == 0) { free(line); return NULL; }

  return line;
}

char *fara_extrapolate(const char *line, flu_dict *vars)
{
  //printf("line >%s<\n", line);
  //fabr_tree *tt = fabr_parse_f(line, 0, parser, 0);
  //flu_putf(fabr_tree_to_string(tt, line, 1));

  fabr_tree *t = fabr_parse_all(line, _def_or_text);

  //flu_putf(fabr_tree_to_string(t, line, 1));

  char *r = extrapolate(line, t, vars);

  fabr_tree_free(t);

  return trim(r);
  //return r;
}

