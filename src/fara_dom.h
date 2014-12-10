
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

#ifndef FLON_FARA_DOM_H
#define FLON_FARA_DOM_H

#include "flutil.h"


typedef struct fara_node {
  struct fara_node *parent;
  char *t;
  flu_dict *atts;
  flu_list *children;
  void *data; // used by 'extensions'
} fara_node;

fara_node *fara_node_malloc(char *tag, flu_dict *atts);
fara_node *fara_text(char *text);
fara_node *fara_n(const char *tag, ...);
fara_node *fara_t(const char *text, ...);

char *fara_node_to_s(fara_node *n);

void fara_node_free(fara_node *n);

void fara_node_push(fara_node *parent, fara_node *child);

enum // flags for fara_node_to_html()
{
  FARA_F_INDENT  = 1 << 0  // indent
};

char *fara_node_to_html(fara_node *n, int flags);

#define fara_h(node) fara_node_to_html(node, 1)

void fara_node_add_class(fara_node *n, const char *cla);

#endif // FLON_FARA_DOM_H

