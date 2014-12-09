
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

#include <stdlib.h>
#include <string.h>
#include <strings.h>

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
    fabr_string("( x=\"y\" )"); // TODO

  fabr_parser *tic =
    fabr_n_alt(
      "tic",
      fabr_seq(fabr_str("%"), fabr_n_rex("ta", "[a-zA-Z-_0-9]+"), NULL),
      fabr_seq(fabr_str("#"), fabr_n_rex("id", "[a-zA-Z-_0-9]+"), NULL),
      fabr_seq(fabr_str("."), fabr_n_rex("cl", "[a-zA-Z-_0-9]+"), NULL),
      NULL);

  fabr_parser *ind =
    fabr_n_rex("ind", "[ ]*");

  fabr_parser *eval_eol =
    fabr_seq(fabr_str("="), fabr_n_rex("ev", "[^\r\n]*"), NULL);

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
    fabr_n_seq("evl", fabr_string("="), fabr_n_rex("ev", "[^\r\n]*"), NULL);

  fabr_parser *elt_line =
    fabr_n_seq(
      "ell",
      ind,
      tic, fabr_q("+"),
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

  fabr_parser *hentry =
    fabr_n_seq(
      "he",
      fabr_n_rex("k", "[^: \t\n\r]+"),
      fabr_rex("[ \t]*:[ \t]*"),
      fabr_n_rex("v", "[^\n\r]+"),
      fabr_rex("[\n\r]+"),
      NULL);

  fabr_parser *doctype =
    fabr_seq(
      fabr_rex("!!![ \t]*"),
      fabr_n_rex("dt", "[^ \t\n\r]*"),
      fabr_rex("[\n\r]+"),
      NULL);

  fabr_parser *headers =
    fabr_n_seq(
      "hes",
      fabr_rex("---[ \t]*[\n\r]+"),
      hentry, fabr_q("*"),
      fabr_rex("---[ \t]*[\n\r]+"),
      NULL);

  haml_parser =
    fabr_seq(
      headers, fabr_q("?"),
      doctype, fabr_q("?"),
      line,
      fabr_seq(
        fabr_rex("[\n\r]"),
        line,
        fabr_r("*")),
      NULL);
}

static fara_node *stack_ell(fara_node *n, const char *s, fabr_tree *t)
{
  //puts("---"); flu_putf(fabr_tree_to_string(t, s, 1));

  int ci = (int)n->data;
  int i = t->child->length; // first child is "ind"

  fara_node *nn = fara_node_malloc(NULL, flu_list_malloc());
  nn->data = (void *)i; // ;-)

  // attributes

  flu_list *ents = fabr_tree_list_named(t, "jsentry");
  for (flu_node *en = ents->first; en; en = en->next)
  {
    flu_list_setk(
      nn->atts,
      fabr_lookup_string(s, en->item, "k"),
      fabr_lookup_string(s, en->item, "v"));
  }
  flu_list_free(ents);

  // tag

  char *ta = fabr_lookup_string(s, t, "ta");
  nn->t = ta ? ta : strdup("div");

  // class

  flu_list *cs = fabr_tree_list_named(t, "cl");
  for (flu_node *cn = cs->first; cn; cn = cn->next)
  {
    char *cl = fabr_tree_string(s, cn->item);
    fara_node_add_class(nn, cl);
    free(cl);
  }
  flu_list_free(cs);

  // id

  char *id = fabr_lookup_string(s, t, "id");
  if (id) flu_list_set(nn->atts, "id", id);

  // ev (%h1 = title)

  char *ev = fabr_lookup_string(s, t, "ev");
  if (ev)
  {
    fara_node *doc = n; while (doc->parent) doc = doc->parent;
    char *k = flu_strtrim(ev); free(ev);
    char *v = doc->atts ? flu_list_get(doc->atts, k) : v;
    if (v) fara_node_push(nn, fara_t(v));
    free(k);
  }

  // push to parent

  if (i == ci)
  {
    fara_node_push(n->parent, nn);
  }
  else
  {
    while (i < ci) { n = n->parent; ci = (int)n->data; }
    fara_node_push(n, nn);
  }

  return nn;
}

static fara_node *stack_txl(fara_node *n, const char *s, fabr_tree *t)
{
  fara_node_push(n, fara_node_malloc(fabr_tree_string(s, t), NULL));

  return n;
}

static fara_node *stack(fara_node *n, const char *s, fabr_tree *t)
{
  if (strcmp(t->name, "ell") == 0)
    return stack_ell(n, s, t);
  if (strcmp(t->name, "txl") == 0)
    return stack_txl(n, s, t);
  //else
  return n;
}

static fara_node *doctype(const char *dt)
{
  char *s =
    "<!DOCTYPE html PUBLIC"
    " \"-//W3C//DTD XHTML 1.0 Transitional//EN\""
    " \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">";

  if (strcasecmp(dt, "strict") == 0)
    s =
      "<!DOCTYPE html PUBLIC"
      " \"-//W3C//DTD XHTML 1.0 Strict//EN\""
      " \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">";

  else if (strcasecmp(dt, "frameset") == 0)
    s =
      "<!DOCTYPE html PUBLIC"
      " \"-//W3C//DTD XHTML 1.0 Frameset//EN\""
      " \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-frameset.dtd\">";

  else if (strcmp(dt, "5") == 0)
    s =
      "<!DOCTYPE html>";

  else if (strcmp(dt, "1.1") == 0)
    s =
      "<!DOCTYPE html PUBLIC"
      " \"-//W3C//DTD XHTML 1.1//EN\""
      " \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">";

  else if (strcasecmp(dt, "basic") == 0)
    s =
      "<!DOCTYPE html PUBLIC"
      " \"-//W3C//DTD XHTML Basic 1.1//EN\""
      " \"http://www.w3.org/TR/xhtml-basic/xhtml-basic11.dtd\">";

  else if (strcasecmp(dt, "mobile") == 0)
    s =
      "<!DOCTYPE html PUBLIC"
      " \"-//WAPFORUM//DTD XHTML Mobile 1.2//EN\""
      " \"http://www.openmobilealliance.org/tech/DTD/xhtml-mobile12.dtd\">";

  else if (strcasecmp(dt, "rdfa") == 0)
    s =
      "<!DOCTYPE html PUBLIC"
      " \"-//W3C//DTD XHTML+RDFa 1.0//EN\""
      " \"http://www.w3.org/MarkUp/DTD/xhtml-rdfa-1.dtd\">";

  return fara_t(s);
}

fara_node *fara_haml_parse(const char *s)
{
  if (haml_parser == NULL) haml_parser_init();

  //puts("[1;30m"); puts(fabr_parser_to_string(haml_parser)); puts("[0;0m");
  //fabr_tree *t = fabr_parse_all(s, 0, haml_parser);

  //printf(">[0;33m%s[0;0m<\n", s);

  //fabr_tree *tt = fabr_parse_f(s, 0, haml_parser, 0);
  //flu_putf(fabr_tree_to_string(tt, s, 1));
  //fabr_tree_free(tt);

  fabr_tree *t = fabr_parse_all(s, 0, haml_parser);
  //flu_putf(fabr_tree_to_string(t, s, 1));

  fara_node *r = fara_node_malloc(NULL, NULL); // document node
  r->data = (void *)-1; // ;-)

  // headers

  flu_list *hes = fabr_tree_list_named(t, "he");

  if (hes->size > 0) r->atts = flu_list_malloc();

  for (flu_node *n = hes->first; n; n = n->next)
  {
    flu_list_setk(
      r->atts,
      fabr_lookup_string(s, n->item, "k"),
      fabr_lookup_string(s, n->item, "v"));
  }

  flu_list_free(hes);

  // doctype

  char *dt = fabr_lookup_string(s, t, "dt");
  if (dt) { fara_node_push(r, doctype(dt)); free(dt); }

  // haml

  flu_list *ls = fabr_tree_list_named(t, "l");
  for (flu_node *n = ls->first; n; n = n->next)
  {
    fabr_tree *nl = ((fabr_tree *)n->item)->child;
    //puts("---"); flu_putf(fabr_tree_to_string(nl, s, 1));
    r = stack(r, s, nl);
  }
  flu_list_free(ls);

  while (r->parent) r = r->parent;

  // over

  fabr_tree_free(t);

  return r;
}

