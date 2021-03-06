
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

#ifndef FLON_FARA_HAML_H
#define FLON_FARA_HAML_H

#include "fara_dom.h"


typedef void *fara_haml_callback(
  const char *s, fara_node *n, flu_dict *callbacks, void *data);

/* Interprets the given HAML and returns the fara_node corresponding to it.
 */
fara_node *fara_haml_parse(
  const char *s, flu_dict *rootd, flu_dict *callbacks, void *data);

/* Interprets the given HAML and returns the fara_node corresponding to it.
 */
fara_node *fara_haml_parse_s(
  const char *s);

/* Reads a file and generates a fara_node from it.
 *
 * Expects the 3 last arguments to be
 * ```flu_dict *rootd, flu_dict *callbacks, void *data```
 * thus matching fara_haml_parse().
 */
fara_node *fara_haml_parse_f(
  const char *path, ...);

#endif // FLON_FARA_HAML_H

