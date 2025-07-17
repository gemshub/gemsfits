#pragma once

/**************************************************************************************************
 * Ref counting disposable containers.
 *
 * IOWOW library
 *
 * MIT License
 *
 * Copyright (c) 2012-2022 Softmotions Ltd <info@softmotions.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *************************************************************************************************/

#include "basedefs.h"

#include <stdlib.h>
#include <stdatomic.h>
#include <stdbool.h>

struct iwref_holder {
  void       *data;
  atomic_long refs;
  void (*freefn)(void*);
};

static void iwref_init(struct iwref_holder *h, void *data, void (*freefn)(void*)) {
  *h = (struct iwref_holder) {
    .refs = 1,
    .data = data,
    .freefn = freefn,
  };
}

static void iwref_reset(struct iwref_holder *h, void *data, void (*freefn)(void*)) {
  h->refs = 1;
  if (freefn) {
    h->data = data;
    h->freefn = freefn;
  }
}

static void iwref_ref(struct iwref_holder *h) {
  ++h->refs;
}

static bool iwref_unref(struct iwref_holder *h) {
  if (h) {
    if (--h->refs == 0) {
      if (h->freefn) {
        h->freefn(h->data);
      }
      return true;
    }
  }
  return false;
}
