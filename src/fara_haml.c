
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
fabr_parser *header_parser = NULL;

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
    fabr_n_seq("evl", ind, fabr_rex("=[ \t]*"), fabr_n_rex("ev", "[^\r\n]+"), NULL);

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

  fabr_parser *doctype =
    fabr_seq(
      fabr_rex("!!![ \t]*"),
      fabr_n_rex("dt", "[^\n\r]*"),
      fabr_rex("[\n\r]+"),
      NULL);

  fabr_parser *headers =
    fabr_seq(
      fabr_rex("---[ \t]*[\n\r]+"),
      fabr_n_rep("hes", fabr_rex("[^-].*[\n\r]"), 0, -1),
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

  header_parser =
    fabr_rep(
      fabr_n_seq(
        "e",
        fabr_n_rex("k", "[^ \t:]+"),
        fabr_rex("[ \t]*:[ \t]*"),
        fabr_n_rex("v", "[^\n\r]+"),
        fabr_rex("[\n\r]"),
        NULL),
      0, -1);
}

static fara_node *push_to_parent(
  fara_node *current, fabr_tree *newt, fara_node *newn)
{
  int i = newt->child->length; // first child is "ind"
  newn->data = (void *)i; // ;-)

  while (1)
  {
    int ci = (int)current->data;
    if (i > ci) { fara_node_push(current, newn); break; }
    current = current->parent;
  }

  return newn;
}

static void eval_and_push(flu_dict *callbacks, fara_node *n, const char *ev)
{
  char *code = flu_strtrim(ev);

  fara_haml_callback *cb = flu_list_get(callbacks, "eval");
  fara_node *res = cb(code, n, NULL);

  if (res)
  {
    if (strcmp(code, "content") == 0) fara_node_push(n->parent, res);
    else fara_node_push(n, res);
  }

  free(code);
}

static fara_node *stack_ell(
  fara_node *n, const char *s, flu_dict *cbs, void *data, fabr_tree *t)
{
  //puts("---"); flu_putf(fabr_tree_to_string(t, s, 1));

  fara_node *nn = fara_node_malloc(NULL, flu_list_malloc());

  fara_node *nextn = push_to_parent(n, t, nn);

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
    eval_and_push(cbs, nn, ev);
    free(ev);
  }

  // over.

  return nextn;
}

static fara_node *stack_evl(
  fara_node *n, const char *s, flu_dict *cbs, void *data, fabr_tree *t)
{
  char *ev = fabr_lookup_string(s, t, "ev");
  eval_and_push(cbs, n, ev);
  free(ev);

  return n;
}

static fara_node *stack_txl(
  fara_node *n, const char *s, flu_dict *cbs, void *data, fabr_tree *t)
{
  fara_node_push(n, fara_node_malloc(fabr_tree_string(s, t), NULL));

  return n;
}

static fara_node *stack(
  fara_node *n, const char *s, flu_dict *cbs, void *data, fabr_tree *t)
{
  if (strcmp(t->name, "ell") == 0) return stack_ell(n, s, cbs, data, t);
  if (strcmp(t->name, "evl") == 0) return stack_evl(n, s, cbs, data, t);
  if (strcmp(t->name, "txl") == 0) return stack_txl(n, s, cbs, data, t);
  return n;
}

void *default_header_callback(const char *s, fara_node *n, void *data)
{
  if (n->atts == NULL) n->atts = flu_list_malloc();

  fabr_tree *t = fabr_parse_all(s, 0, header_parser);
  //flu_putf(fabr_tree_to_string(t, s, 1));

  flu_list *es = fabr_tree_list_named(t, "e");
  for (flu_node *en = es->first; en; en = en->next)
  {
    flu_list_setk(
      n->atts,
      fabr_lookup_string(s, en->item, "k"),
      fabr_lookup_string(s, en->item, "v"));
  }
  flu_list_free(es);

  fabr_tree_free(t);

  return NULL;
}

void *default_eval_callback(const char *s, fara_node *n, void *data)
{
  char *k = flu_strtrim(s);
  fara_node *doc = n; while (doc->parent) doc = doc->parent;
  void *v = doc->atts ? flu_list_get(doc->atts, k) : NULL;

  fara_node *r = NULL;
  if (strcmp(k, "content") == 0) r = v;
  else if (v) r = fara_t(v);

  free(k);

  return r;
}

fara_node *fara_haml_parse(
  const char *s, flu_dict *rootd, flu_dict *callbacks, void *data)
{
  if (haml_parser == NULL) haml_parser_init();

  // prepare callbacks

  int own_callbacks = 0;
  if (callbacks == NULL)
  {
    callbacks = flu_list_malloc();
    own_callbacks = 1;
  }

  flu_list_set_last(callbacks, "header", default_header_callback);
  flu_list_set_last(callbacks, "eval", default_eval_callback);

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

  if (rootd)
  {
    r->atts = flu_list_malloc();
    flu_list_concat(r->atts, rootd);
  }

  // headers

  char *hes = fabr_lookup_string(s, t, "hes");

  if (hes)
  {
    fara_haml_callback *hcb = flu_list_get(callbacks, "header");
    hcb(hes, r, NULL);
    free(hes);
  }

  // doctype

  char *dt = fabr_lookup_string(s, t, "dt");
  if (dt) { fara_node_push(r, fara_t("<!DOCTYPE html>")); free(dt); }

  // haml

  flu_list *ls = fabr_tree_list_named(t, "l");
  for (flu_node *n = ls->first; n; n = n->next)
  {
    fabr_tree *nl = ((fabr_tree *)n->item)->child;
    //puts("---"); flu_putf(fabr_tree_to_string(nl, s, 1));
    r = stack(r, s, callbacks, data, nl);
  }
  flu_list_free(ls);

  // over

  fabr_tree_free(t);
  if (own_callbacks) flu_list_free(callbacks);

  while (r->parent) r = r->parent;

  return r;
}

fara_node *fara_haml_parse_s(
  const char *s)
{
  return fara_haml_parse(s, NULL, NULL, NULL);
}

static char *find_layouts_dir(const char *start_path)
{
  char *r = NULL;

  char *pa = flu_dirname(start_path);

  char *pal = flu_path("%s/layouts", pa);
  char *ppa = NULL;

  if (flu_fstat(pal) == 'd') { r = pal; goto _over; }

  if (strlen(pa) < 1) goto _over;
  if (strcmp(pa, ".") == 0) goto _over;

  ppa = flu_path("%s/..", pa);

  r = find_layouts_dir(ppa);

_over:

  free(pa);
  if (pal != r) free(pal);
  free(ppa);

  return r;
}

fara_node *fara_haml_parse_f(const char *path, ...)
{
  va_list ap; va_start(ap, path);
    //
  char *pa = flu_svprintf(path, ap);
  flu_dict *rootd = va_arg(ap, flu_dict *);
  flu_dict *callbacks = va_arg(ap, flu_dict *);
  void *data = va_arg(ap, void *);
    //
  va_end(ap);

  flu_dict *rd = rootd ? rootd : flu_list_malloc();

  fara_node *r = NULL;

  char *s = flu_readall(pa);
  if (s == NULL) goto _over;

  r = fara_haml_parse(s, rd, callbacks, data);

  free(s);

  // layout?

  char *layout = r->atts ? flu_list_get(r->atts, "layout") : NULL;
  if (layout)
  {
    flu_list_concat(rd, r->atts);
    flu_list_free(r->atts); r->atts = NULL;

    flu_list_set(rd, "content", r);

    char *ldir = find_layouts_dir(pa);
      //
    if (ldir == NULL) goto _over;
      //
    s = flu_readall("%s/%s.haml", ldir, layout);
    free(ldir);
      //
    r = fara_haml_parse(s, rd, callbacks, data);
      //
    free(s);
      //
      // TODO: use fara_haml_parse_f()

    for (flu_node *fn = r->atts->first; fn; fn = fn->next)
    {
      if (strcmp(fn->key, "content") == 0) { fn->item = NULL; break; }
    }
  }

_over:

  free(pa);
  if (rootd == NULL) flu_list_free(rd);

  return r;
}

