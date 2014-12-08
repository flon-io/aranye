
//
// Copyright (c) 2013-2014, John Mettraux, jmettraux+flon@gmail.com
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


fara_node *fara_node_malloc(const char *tag, flu_dict *atts)
{
  fara_node *r = calloc(1, sizeof(fara_node));

  r->tag = strdup(tag);
  r->atts = atts ? atts : flu_list_malloc();
  r->children = flu_list_malloc();

  return r;
}

fara_node *fara_n(const char *tag, ...)
{
  va_list ap; va_start(ap, tag);

  fara_node *r = calloc(1, sizeof(fara_node));

  r->tag = flu_svprintf(tag, ap);
  r->atts = flu_vsd(ap);
  r->children = flu_list_malloc();

  va_end(ap);

  return r;
}

void fara_node_free(fara_node *n)
{
  if (n == NULL) return;

  n->parent = NULL;
  free(n->tag);
  flu_list_free_all(n->atts);

  if (n->children) for (flu_node *fn = n->children->first; fn; )
  {
    flu_node *next = fn->next;
    fara_node_free((fara_node *)fn->item);
    fn = next;
  }
  flu_list_free(n->children);

  free(n);
}

char *fara_node_to_html(fara_node *n)
{
  return NULL;
}

