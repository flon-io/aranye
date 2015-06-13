
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
#include <strings.h>

#include "aabro.h"
#include "fara_haml.h"


//
// haml parser

static fabr_tree *_nls(fabr_input *i) { return fabr_rex(NULL, i, "[\n\r]*"); }
//static fabr_tree *_nlp(fabr_input *i) { return fabr_rex(NULL, i, "[\n\r]+"); }

static fabr_tree *_indentation(fabr_input *i)
{
  return fabr_rex("i", i, "[ ]*");
}

static fabr_tree *_ht_atts(fabr_input *i)
{
  return fabr_str("ht_atts", i, "( x=\"y\" )"); // TODO
}

static fabr_tree *_eval_eol(fabr_input *i)
{
  return fabr_rex("ev", i, "=[^\r\n]*");
}

static fabr_tree *_blank_line(fabr_input *i)
{
  return fabr_rex("bll", i, "[ \t]*");
}

static fabr_tree *_text_eol(fabr_input *i)
{
  return fabr_rex(NULL, i, "[^\r\n]+");
}
static fabr_tree *_text_line(fabr_input *i)
{
  return fabr_seq("txl", i, _indentation, _text_eol, NULL);
}

static fabr_tree *_haml_comment_line(fabr_input *i)
{
  return fabr_rex("htcol", i, "[ ]*-#[^\r\n]*");
}
static fabr_tree *_html_comment_line(fabr_input *i)
{
  return fabr_rex("htcol", i, "[ ]*/[^\r\n]*");
}

static fabr_tree *_filter_eol(fabr_input *i)
{
  return fabr_rex(NULL, i, ":[a-z]+");
}
static fabr_tree *_filter_line(fabr_input *i)
{
  return fabr_seq("fil", i, _indentation, _filter_eol, NULL);
}

static fabr_tree *_eval_line(fabr_input *i)
{
  return fabr_seq("evl", i, _indentation, _eval_eol, NULL);
}

static fabr_tree *_js_key(fabr_input *i)
{
  return fabr_rex("k", i, "[^ \t\r\n:]+");
}
static fabr_tree *_js_col(fabr_input *i)
{
  return fabr_rex(NULL, i, "[ \t]*:[ \t]*");
}
static fabr_tree *_dq_string(fabr_input *i)
{
  return fabr_rex("dqs", i, "\"[^\"\r\n,]*\"");
}
static fabr_tree *_sq_string(fabr_input *i)
{
  return fabr_rex("sqs", i, "'[^'\r\n,]*'");
}
static fabr_tree *_word(fabr_input *i)
{
  return fabr_rex("wos", i, "[^ \t\r\n,]+");
}
static fabr_tree *_js_val(fabr_input *i)
{
  return fabr_alt("v", i, _dq_string, _sq_string, _word, NULL);
}

static fabr_tree *_js_entry(fabr_input *i)
{
  return fabr_seq("js_entry", i, _js_key, _js_col, _js_val, NULL);
}

static fabr_tree *_js_comma(fabr_input *i)
{
  return fabr_rex(NULL, i, "[ \t]*,[ \t]*");
}

static fabr_tree *_pbs(fabr_input *i) // pointy bracket start
{
  return fabr_rex(NULL, i, "[ \t]*\\{[ \t]*");
}
static fabr_tree *_pbe(fabr_input *i) // pointy bracket end
{
  return fabr_rex(NULL, i, "[ \t]*}[ \t]*");
}

static fabr_tree *_js_atts(fabr_input *i)
{
  return fabr_eseq("js_atts", i, _pbs, _js_entry, _js_comma, _pbe);
}

static fabr_tree *_tic(fabr_input *i) // Tag, Id, Class
{
  return fabr_rex("tic", i, "[%#.][a-zA-Z\\-_0-9]+");
}

static fabr_tree *_elt_line(fabr_input *i)
{
  return fabr_seq("ell", i,
    _indentation,
    _tic, fabr_plus, // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
                     // will it loop in here?
    _js_atts, fabr_qmark,
    _ht_atts, fabr_qmark,
    _eval_eol, fabr_qmark, // or here with the qmark???? (no, 1 or 0)
    NULL);
}

static fabr_tree *_l(fabr_input *i)
{
  return fabr_alt("l", i,
    _elt_line, _eval_line, _filter_line,
    _html_comment_line, _haml_comment_line,
    _text_line, _blank_line,
      // <<<< it's rather here...
    NULL);
}

static fabr_tree *_line(fabr_input *i)
{
  return fabr_seq(NULL, i, _nls, _l, NULL);
}

static fabr_tree *_dta(fabr_input *i) { return fabr_rex(NULL, i, "!!![ \t]*"); }
static fabr_tree *_dt(fabr_input *i) { return fabr_rex("dt", i, "[^\n\r]*"); }
static fabr_tree *_dtz(fabr_input *i) { return fabr_rex(NULL, i, "[\n\r]+"); }

static fabr_tree *_doctype(fabr_input *i)
{
  return fabr_seq(NULL, i, _dta, _dt, _dtz, NULL);
}

static fabr_tree *_dash_line(fabr_input *i)
{
  return fabr_rex(NULL, i, "---[ \t]*[\n\r]+");
}
static fabr_tree *_header_line(fabr_input *i)
{
  return fabr_rex(NULL, i, "[^-].*[\n\r]+");
}
static fabr_tree *_header_lines(fabr_input *i)
{
  return fabr_rep("hls", i, _header_line, 0, 0);
}

static fabr_tree *_headers(fabr_input *i)
{
  return fabr_seq(NULL, i,
    _dash_line,
    _header_lines,
    _dash_line,
    NULL);
}

static fabr_tree *_haml(fabr_input *i)
{
  return fabr_seq(NULL, i,
    _headers, fabr_qmark,
    _doctype, fabr_qmark,
    _line, fabr_star, // <<<<<<<<<<<<<<<<<<<<<<<
    NULL);
}

//
// header parser

static fabr_tree *_hk(fabr_input *i)
{
  return fabr_rex("k", i, "[^ \t:]+");
}
static fabr_tree *_hcolon(fabr_input *i)
{
  return fabr_rex(NULL, i, "[ \t]*:[ \t]*");
}
static fabr_tree *_hv(fabr_input *i)
{
  return fabr_rex("v", i, "[^\n\r]+");
}
static fabr_tree *_heol(fabr_input *i)
{
  return fabr_rex(NULL, i, "[\n\r]");
}
static fabr_tree *_he(fabr_input *i)
{
  return fabr_seq("e", i, _hk, _hcolon, _hv, _heol, NULL);
}

static fabr_tree *_header(fabr_input *i)
{
  return fabr_rep(NULL, i, _he, 0, 0); // 0 or more
}


//
// /parsers

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

static void eval_and_push(
  fabr_tree *t, const char *ev, fara_node *n, flu_dict *callbacks, void *data)
{
  char *code = flu_strtrim(ev);

  fara_haml_callback *cb =
    flu_list_get(
      callbacks,
      strncmp(code, "include ", 8) == 0 ? "include" : "eval");

  fara_node *res = cb(code, n, callbacks, data);

  if (res)
  {
    if (t == NULL) fara_node_push(n, res);
    else push_to_parent(n, t, res);
  }

  free(code);
}

static fara_node *stack_ell(
  fara_node *n, const char *s, flu_dict *cbs, void *data, fabr_tree *t)
{
  //puts("---"); fabr_puts_tree(t, s, 1);

  fara_node *nn = fara_node_malloc(NULL, flu_list_malloc());

  fara_node *nextn = push_to_parent(n, t, nn);

  // attributes

  flu_list *ents = fabr_tree_list_named(t, "js_entry");
  for (flu_node *en = ents->first; en; en = en->next)
  {
    fabr_tree *ent = en->item;
    //puts("{{{"); fabr_puts_tree(ent, s, 1);

    fabr_tree *entv = fabr_tree_lookup(ent, "v")->child;
    //puts("{{{"); fabr_puts_tree(entv, s, 1);

    char *ks = fabr_lookup_string(s, en->item, "k");

    char *vs = fabr_tree_string(s, entv);
    if (strcmp(entv->name, "wos") != 0)
    {
      vs[strlen(vs) - 1] = 0;
      strcpy(vs, vs + 1);
    }

    flu_list_setk(nn->atts, ks, vs, 1);
  }
  flu_list_free(ents);

  // tic

  flu_list *tics = fabr_tree_list_named(t, "tic");
  for (flu_node *tn = tics->first; tn; tn = tn->next)
  {
    //puts("###"); fabr_puts_tree(tn->item, s, 1);

    fabr_tree *tnt = tn->item;
    char *tns = fabr_tree_string(s, tnt);

    if (*fabr_tree_str(s, tnt) == '%')
    {
      free(nn->t); nn->t = strdup(tns + 1);
    }
    else if (*fabr_tree_str(s, tnt) == '#')
    {
      flu_list_set(nn->atts, "id", strdup(tns + 1));
    }
    else // '.'
    {
      fara_node_add_class(nn, tns + 1);
    }

    free(tns);
  }
  flu_list_free(tics);

  if (nn->t == NULL) nn->t = strdup("div");

  // ev (%h1 = title)

  char *ev = fabr_lookup_string(s, t, "ev");
  if (ev)
  {
    eval_and_push(NULL, ev, nn, cbs, data);
    free(ev);
  }

  // over.

  return nextn;
}

static fara_node *stack_evl(
  fara_node *n, const char *s, flu_dict *callbacks, void *data, fabr_tree *t)
{
  char *ev = fabr_lookup_string(s, t, "ev");
  eval_and_push(t, ev, n, callbacks, data);
  free(ev);

  return n;
}

static fara_node *stack_txl(
  fara_node *n, const char *s, flu_dict *cbs, void *data, fabr_tree *t)
{
  push_to_parent(n, t, fara_node_malloc(fabr_tree_string(s, t), NULL));

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

static void *default_header_callback(
  const char *s, fara_node *n, flu_dict *callbacks, void *data)
{
  if (n->atts == NULL) n->atts = flu_list_malloc();

  fabr_tree *t = fabr_parse_all(s, _header);
  //flu_putf(fabr_tree_to_string(t, s, 1));

  flu_list *es = fabr_tree_list_named(t, "e");
  for (flu_node *en = es->first; en; en = en->next)
  {
    flu_list_setk(
      n->atts,
      fabr_lookup_string(s, en->item, "k"),
      fabr_lookup_string(s, en->item, "v"),
      0);
  }
  flu_list_free(es);

  fabr_tree_free(t);

  return NULL;
}

static char *find_dir(
  const char *start_path, const char *dirname)
{
  char *r = NULL;

  char *pa = flu_dirname(start_path);

  char *pal = flu_path("%s/%s", pa, dirname);
  char *ppa = NULL;

  if (flu_fstat(pal) == 'd') { r = pal; goto _over; }

  if (strlen(pa) < 1) goto _over;
  if (strcmp(pa, ".") == 0) goto _over;

  ppa = flu_path("%s/..", pa);

  r = find_dir(ppa, dirname);

_over:

  free(pa);
  if (r != pal) free(pal);
  free(ppa);

  return r;
}

static char *find_file(
  const char *start_path, const char *dirname, const char *fname)
{
  char *dir = find_dir(start_path, dirname);
  if (dir == NULL) return NULL;

  char *fn = NULL;

  fn = flu_path("%s/%s.haml", dir, fname);
  if (flu_fstat(fn) == 'f') goto _over;

  free(fn); fn = flu_path("%s/%s.html", dir, fname);
  if (flu_fstat(fn) == 'f') goto _over;

  free(fn); fn = NULL;

_over:

  free(dir);

  return fn;
}

static void *default_include_callback(
  const char *s, fara_node *n, flu_dict *callbacks, void *data)
{
  fara_node *r = NULL;

  char *fn = strchr(s, ' '); if (fn) fn = fn + 1;

  char *fpath = find_file(fara_doc_lookup(n, "path"), "includes", fn);

  if (fpath == NULL) return NULL;

  char *suff = strrchr(fpath, '.');

  if (strcmp(suff, ".haml") == 0)
  {
    r = fara_haml_parse_f(fpath, NULL, callbacks, data);
  }
  else
  {
    char *ss = flu_readall(fpath);
    if (ss)
    {
      r = fara_text(ss);
      if (strcmp(suff, ".html") == 0) r->atts = flu_sd("html", "true", NULL);
    }
  }

  free(fpath);

  return r;
}

static void *default_eval_callback(
  const char *s, fara_node *n, flu_dict *callbacks, void *data)
{
  void *v = fara_doc_lookup(n, s);

  fara_node *r = NULL;
  if (strcmp(s, "content") == 0) r = v;
  else if (v) r = fara_t(v);

  return r;
}

fara_node *fara_haml_parse(
  const char *s, flu_dict *rootd, flu_dict *callbacks, void *data)
{
  // prepare callbacks

  int own_callbacks = 0;
  if (callbacks == NULL)
  {
    callbacks = flu_list_malloc();
    own_callbacks = 1;
  }

  flu_list_set_last(callbacks, "header", default_header_callback);
  flu_list_set_last(callbacks, "eval", default_eval_callback);
  flu_list_set_last(callbacks, "include", default_include_callback);

  //printf(">[0;33m%s[0;0m<\n", s);
  //
  //fabr_tree *tt = fabr_parse_f(s, _haml, 0);
  //fabr_puts_tree(tt, s, 1);
  //fabr_tree_free(tt);

  fabr_tree *t = fabr_parse_all(s, _haml);
  fabr_puts_tree(t, s, 1);

  fara_node *r = fara_node_malloc(NULL, NULL); // document node
  r->data = (void *)-1; // ;-)

  r->atts = rootd;

  // headers

  char *hls = fabr_lookup_string(s, t, "hls");

  if (hls)
  {
    fara_haml_callback *hcb = flu_list_get(callbacks, "header");
    hcb(hls, r, callbacks, data);
    free(hls);
  }

  // doctype

  char *dt = fabr_lookup_string(s, t, "dt");
  if (dt) { fara_node_push(r, fara_t("<!DOCTYPE html>")); free(dt); }

  // haml

  flu_list *ls = fabr_tree_list_named(t, "l");
  for (flu_node *n = ls->first; n; n = n->next)
  {
    fabr_tree *nl = ((fabr_tree *)n->item)->child;
    //puts("###"); fabr_puts_tree(nl, s, 1);
    r = stack(r, s, callbacks, data, nl);
  }
  flu_list_free(ls);

  // over

  fabr_tree_free(t);
  if (own_callbacks) flu_list_free(callbacks);

  while (r->parent) r = r->parent;

  //flu_putf(fara_node_to_st(r, 1));

  return r;
}

fara_node *fara_haml_parse_s(
  const char *s)
{
  return fara_haml_parse(s, NULL, NULL, NULL);
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
  flu_list_set(rd, "path", pa);

  fara_node *r = NULL;

  char *s = flu_readall(pa);
  if (s == NULL) goto _over;

  r = fara_haml_parse(s, rd, callbacks, data);

  free(s);

  // layout?

  char *layout = r->atts ? flu_list_get(r->atts, "layout") : NULL;
  if (layout)
  {
    flu_dict *lrootd = flu_list_malloc();
    flu_dict *new_atts = flu_list_malloc();
    //
    for (flu_node *fn = r->atts->first; fn; fn = fn->next)
    {
      flu_dict *d =
        (
          strcmp(fn->key, "path") == 0 ||
          strcmp(fn->key, "layout") == 0
        ) ?
        new_atts :
        lrootd;
      flu_list_set(d, fn->key, fn->item);
    }
    //
    flu_list_free(r->atts); r->atts = new_atts;

    flu_list_set(lrootd, "content", r);

    char *lfile = find_file(pa, "layouts", layout);
    if (lfile == NULL) goto _over;

    r = fara_haml_parse_f(lfile, lrootd, callbacks, data);

    free(lfile);

    for (flu_node *fn = lrootd->first; fn; fn = fn->next)
    {
      if (strcmp(fn->key, "content") == 0) { fn->item = NULL; break; }
    }
  }

_over:

  //flu_putf(fara_node_to_st(r, 1));

  if (r) return r; // success

  if (rd != rootd) flu_list_free(rd);
  free(pa);

  return NULL;
}

