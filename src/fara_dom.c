
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

#include "fara_dom.h"


static int validate(fara_node *n)
{
  return 1; // TODO
}

fara_node *fara_node_malloc(char *t, flu_dict *atts)
{
  fara_node *r = calloc(1, sizeof(fara_node));

  r->t = t;
  r->atts = atts;
  r->children = (atts || t == NULL) ? flu_list_malloc() : NULL;

  if ( ! validate(r)) { fara_node_free(r); return NULL; }

  return r;
}

fara_node *fara_text(char *t)
{
  return fara_node_malloc(t, NULL);
}

fara_node *fara_n(const char *tag, ...)
{
  va_list ap; va_start(ap, tag);
  char *ta = flu_svprintf(tag, ap);
  flu_dict *di = flu_vsd(ap);
  va_end(ap);

  return fara_node_malloc(ta, di);
}

fara_node *fara_t(const char *text, ...)
{
  va_list ap; va_start(ap, text);
  char *t = flu_svprintf(text, ap);
  va_end(ap);

  return fara_node_malloc(t, NULL);
}

int fara_node_is_tag(fara_node *n) { return (n->t && n->atts); }
int fara_node_is_text(fara_node *n) { return (n->t && n->atts == NULL); }
int fara_node_is_doc(fara_node *n) { return (n->t == NULL && n->atts == NULL); }

void fara_node_render(flu_sbuffer *b, fara_node *n, int colour, ssize_t depth)
{
  if (n == NULL) { flu_sbputs(b, "(null fara-node)"); return; }

  char *atts = NULL;
  if (n->atts)
  {
    flu_sbuffer *ab = flu_sbuffer_malloc();
    flu_sbputc(ab, '{');
    for (flu_node *fn = n->atts->first; fn; fn = fn->next)
    {
      if (fn != n->atts->first) flu_sbputc(ab, ',');
      flu_sbputs(ab, fn->key);
    }
    flu_sbputc(ab, '}');
    atts = flu_sbuffer_to_string(ab);
  }
  else
  {
    atts = strdup("-1");
  }

  ssize_t cs = n->children ? n->children->size : -1;

  char *con = colour ? "[1;30m" : "";
  char *cson = colour ? "[1;33m" : "";
  char *caon = colour ? "[0;34m" : "";
  char *coff = colour ? "[0;0m" : "";

  char *t = n->t ? strndup(n->t, 24) : NULL;
  if (t && strlen(t) > 20) strcpy(t + 21, "...\0");
  ssize_t ts = n->t ? strlen(n->t) : -1;

  flu_sbprintf(
    b,
    "%*s%s(fara_node %p p%p t%s\"%s\"%s%li a%s%s%s c%li)%s",
    depth < 0 ? 0 : depth * 2, "",
    con,
    n, n->parent, cson, t, con, ts, caon, atts, con, cs,
    coff);

  free(t);
  free(atts);

  if (depth > -1 && n->children) // print children
  {
    for (flu_node *fn = n->children->first; fn; fn = fn->next)
    {
      flu_sbputc(b, '\n');
      fara_node_render(b, fn->item, colour, depth + 1);
    }
  }
}

char *fara_node_to_s(fara_node *n)
{
  flu_sbuffer *b = flu_sbuffer_malloc();

  fara_node_render(b, n, 0, -1);

  return flu_sbuffer_to_string(b);
}

char *fara_node_to_st(fara_node *n, int colour)
{
  flu_sbuffer *b = flu_sbuffer_malloc();

  fara_node_render(b, n, colour, 0);

  return flu_sbuffer_to_string(b);
}

void fara_node_free(fara_node *n)
{
  if (n == NULL) return;

  n->parent = NULL;
  free(n->t);
  flu_list_free_all(n->atts);

  if (n->children) for (flu_node *fn = n->children->first; fn; fn = fn->next)
  {
    fara_node_free(fn->item);
  }
  flu_list_free(n->children);

  free(n);
}

void fara_node_push(fara_node *parent, fara_node *child)
{
  child->parent = parent;
  flu_list_add(parent->children, child);
}

static void to_html(fara_node *n, int flags, flu_sbuffer *b, int indent)
{
  int fi = flags & FARA_F_INDENT;

  if (n->atts == NULL && n->children == NULL)
  {
    flu_sbputs(b, n->t);
    if (fi) flu_sbputc(b, '\n');
    return;
  }

  int i = fi ? indent : 0;

  if (n->t)
  {
    flu_sbprintf(b, "%*s<%s", i * 2, "", n->t);
    //
    flu_dict *atts = flu_list_dtrim(n->atts);
    for (flu_node *fn = atts->first; fn; fn = fn->next)
    {
      flu_sbprintf(b, " %s=\"%s\"", fn->key, (char *)fn->item);
    }
    flu_list_free(atts);
    //
    flu_sbputc(b, '>');
    if (fi) flu_sbputc(b, '\n');
  }

  for (flu_node *fn = n->children->first; fn; fn = fn->next)
  {
    to_html(fn->item, flags, b, indent + (n->t ? 1 : 0));
  }

  if (n->t)
  {
    flu_sbprintf(b, "%*s</%s>", i * 2, "", n->t);
    if (fi && n->parent) flu_sbputc(b, '\n');
  }
}

char *fara_node_to_html(fara_node *n, int flags)
{
  flu_sbuffer *b = flu_sbuffer_malloc();

  to_html(n, flags, b, 0);

  return flu_sbuffer_to_string(b);
}

void fara_node_add_class(fara_node *n, const char *cla)
{
  char *c = flu_list_get(n->atts, "class");

  char *c1 = NULL;
  if (c && strlen(c) > 0) c1 = flu_sprintf("%s %s", c, cla);
  else c1 = strdup(cla);

  flu_list_set(n->atts, "class", c1);
}

