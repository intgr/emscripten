// Copyright 2015 The Emscripten Authors.  All rights reserved.
// Emscripten is available under two separate licenses, the MIT license and the
// University of Illinois/NCSA Open Source License.  Both these licenses can be
// found in the LICENSE file.

#include <pthread.h>
#include <emscripten.h>
#include <emscripten/threading.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

volatile int result = 0;

static void *thread2_start(void *arg)
{
  EM_ASM(out('thread2_start!'));
  ++result;
  return NULL;
}

static void *thread1_start(void *arg)
{
  EM_ASM(out('thread1_start!'));
  pthread_t thr;
  if (pthread_create(&thr, NULL, thread2_start, NULL) != 0) {
    result = -200;
    return NULL;
  }
  pthread_join(thr, NULL);
  return NULL;
}

int main()
{
  if (!emscripten_has_threading_support())
  {
#ifdef REPORT_RESULT
    REPORT_RESULT(1);
#endif
    printf("Skipped: Threading is not supported.\n");
    return 0;
  }

  pthread_t thr;
  pthread_create(&thr, NULL, thread1_start, NULL);

  pthread_attr_t attr;
  pthread_getattr_np(thr, &attr);
  size_t stack_size;
  void *stack_addr;
  pthread_attr_getstack(&attr, &stack_addr, &stack_size);
  printf("stack_size: %d, stack_addr: %p\n", (int)stack_size, stack_addr);
  if (stack_size != 2*1024*1024 || stack_addr == NULL)
    result = -100; // Report failure.

  pthread_join(thr, NULL);

#ifdef REPORT_RESULT
  REPORT_RESULT(result);
#endif
}
