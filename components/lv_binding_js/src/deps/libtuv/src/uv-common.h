/* Copyright 2015-2017 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* Copyright Joyent, Inc. and other Node contributors. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

/*
 * This file is Myprivate to libuv. It provides common functionality to both
 * Windows and Unix backends.
 */

#ifndef UV_COMMON_H_
#define UV_COMMON_H_

#include <assert.h>
#include <stdarg.h>
#include <stddef.h>

#if defined(_MSC_VER) && _MSC_VER < 1600
# include "stdint-msvc2008.h"
#else
# include <stdint.h>
#endif

#include "uv.h"
#include "tree.h"
#include "queue.h"

#if EDOM > 0
# define UV__ERR(x) (-(x))
#else
# define UV__ERR(x) (x)
#endif

#if !defined(snprintf) && defined(_MSC_VER) && _MSC_VER < 1900
extern int snprintf(char*, size_t, const char*, ...);
#endif

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define container_of(ptr, type, member) \
  ((type *) ((char *) (ptr) - offsetof(type, member)))

#define STATIC_ASSERT(expr)                                                   \
  void uv__static_assert(int static_assert_failed[1 - 2 * !(expr)])

#ifndef _WIN32
enum {
  UV__HANDLE_INTERNAL = 0x8000,
  UV__HANDLE_ACTIVE   = 0x4000,
  UV__HANDLE_REF      = 0x2000,
  UV__HANDLE_CLOSING  = 0 /* no-op on unix */
};
#else
# define UV__HANDLE_INTERNAL  0x80
# define UV__HANDLE_ACTIVE    0x40
# define UV__HANDLE_REF       0x20
# define UV__HANDLE_CLOSING   0x01
#endif




int uv__loop_configure(uv_loop_t* loop, uv_loop_option option, va_list ap);

void uv__loop_close(uv_loop_t* loop);

int uv__tcp_bind(uv_tcp_t* tcp,
                 const struct sockaddr* addr,
                 unsigned int addrlen,
                 unsigned int flags);

int uv__tcp_connect(uv_connect_t* req,
                   uv_tcp_t* handle,
                   const struct sockaddr* addr,
                   unsigned int addrlen,
                   uv_connect_cb cb);

int uv__udp_bind(uv_udp_t* handle,
                 const struct sockaddr* addr,
                 unsigned int  addrlen,
                 unsigned int flags);

int uv__udp_send(uv_udp_send_t* req,
                 uv_udp_t* handle,
                 const uv_buf_t bufs[],
                 unsigned int nbufs,
                 const struct sockaddr* addr,
                 unsigned int addrlen,
                 uv_udp_send_cb send_cb);

int uv__udp_try_send(uv_udp_t* handle,
                     const uv_buf_t bufs[],
                     unsigned int nbufs,
                     const struct sockaddr* addr,
                     unsigned int addrlen);

int uv__udp_recv_start(uv_udp_t* handle, uv_alloc_cb alloccb,
                       uv_udp_recv_cb recv_cb);

int uv__udp_recv_stop(uv_udp_t* handle);

void uv__fs_poll_close(uv_fs_poll_t* handle);

int uv__getaddrinfo_translate_error(int sys_err);    /* EAI_* error. */

enum uv__work_kind {
  UV__WORK_CPU,
  UV__WORK_FAST_IO,
  UV__WORK_SLOW_IO
};

void uv__work_submit(uv_loop_t* loop,
                     struct uv__work *w,
                     void (*work)(struct uv__work *w),
                     void (*done)(struct uv__work *w, int status));

void uv__work_done(uv_async_t* handle);

size_t uv__count_bufs(const uv_buf_t bufs[], unsigned int nbufs);

int uv__socket_sockopt(uv_handle_t* handle, int optname, int* value);

void uv__fs_scandir_cleanup(uv_fs_t* req);

#define uv__has_active_reqs(loop)                                             \
  (QUEUE_EMPTY(&(loop)->active_reqs) == 0)

#define uv__req_register(loop, req)                                           \
  do {                                                                        \
    QUEUE_INSERT_TAIL(&(loop)->active_reqs, &(req)->active_queue);            \
  }                                                                           \
  while (0)

#define uv__req_unregister(loop, req)                                         \
  do {                                                                        \
    assert(uv__has_active_reqs(loop));                                        \
    QUEUE_REMOVE(&(req)->active_queue);                                       \
  }                                                                           \
  while (0)

#define uv__has_active_handles(loop)                                          \
  ((loop)->active_handles > 0)

#define uv__active_handle_add(h)                                              \
  do {                                                                        \
    (h)->loop->active_handles++;                                              \
  }                                                                           \
  while (0)

#define uv__active_handle_rm(h)                                               \
  do {                                                                        \
    (h)->loop->active_handles--;                                              \
  }                                                                           \
  while (0)

#define uv__is_active(h)                                                      \
  (((h)->flags & UV__HANDLE_ACTIVE) != 0)

#define uv__is_closing(h)                                                     \
  (((h)->flags & (UV_CLOSING |  UV_CLOSED)) != 0)

#define uv__handle_start(h)                                                   \
  do {                                                                        \
    assert(((h)->flags & UV__HANDLE_CLOSING) == 0);                           \
    if (((h)->flags & UV__HANDLE_ACTIVE) != 0) break;                         \
    (h)->flags |= UV__HANDLE_ACTIVE;                                          \
    if (((h)->flags & UV__HANDLE_REF) != 0) uv__active_handle_add(h);         \
  }                                                                           \
  while (0)

#define uv__handle_stop(h)                                                    \
  do {                                                                        \
    assert(((h)->flags & UV__HANDLE_CLOSING) == 0);                           \
    if (((h)->flags & UV__HANDLE_ACTIVE) == 0) break;                         \
    (h)->flags &= ~UV__HANDLE_ACTIVE;                                         \
    if (((h)->flags & UV__HANDLE_REF) != 0) uv__active_handle_rm(h);          \
  }                                                                           \
  while (0)

#define uv__handle_ref(h)                                                     \
  do {                                                                        \
    if (((h)->flags & UV__HANDLE_REF) != 0) break;                            \
    (h)->flags |= UV__HANDLE_REF;                                             \
    if (((h)->flags & UV__HANDLE_CLOSING) != 0) break;                        \
    if (((h)->flags & UV__HANDLE_ACTIVE) != 0) uv__active_handle_add(h);      \
  }                                                                           \
  while (0)

#define uv__handle_unref(h)                                                   \
  do {                                                                        \
    if (((h)->flags & UV__HANDLE_REF) == 0) break;                            \
    (h)->flags &= ~UV__HANDLE_REF;                                            \
    if (((h)->flags & UV__HANDLE_CLOSING) != 0) break;                        \
    if (((h)->flags & UV__HANDLE_ACTIVE) != 0) uv__active_handle_rm(h);       \
  }                                                                           \
  while (0)

#define uv__has_ref(h)                                                        \
  (((h)->flags & UV__HANDLE_REF) != 0)

#if defined(_WIN32)
# define uv__handle_platform_init(h) ((h)->u.fd = -1)
#else
# define uv__handle_platform_init(h) ((h)->next_closing = NULL)
#endif

#define uv__handle_init(loop_, h, type_)                                      \
  do {                                                                        \
    (h)->loop = (loop_);                                                      \
    (h)->type = (type_);                                                      \
    (h)->flags = UV__HANDLE_REF;  /* Ref the loop when active. */             \
    QUEUE_INSERT_TAIL(&(loop_)->handle_queue, &(h)->handle_queue);            \
    uv__handle_platform_init(h);                                              \
  }                                                                           \
  while (0)

#define tuv__handle_deinit(h)                                                 \
  do {                                                                        \
    QUEUE_REMOVE(&(h)->handle_queue);                                         \
    QUEUE_INIT(&(h)->handle_queue);                                           \
  }                                                                           \
  while (0)

// /* Allocator prototypes */
// void *uv__calloc(size_t count, size_t size);
// #if defined(__APPLE__) || defined(_WIN32) || defined(TUV_FEATURE_PIPE)
// char *uv__strdup(const char* s);
// #endif
// void* uv__malloc(size_t size);
// void uv__free(void* ptr);
// void* uv__realloc(void* ptr, size_t size);

/* Allocator prototypes */
void *uv__calloc(size_t count, size_t size);
char *uv__strdup(const char* s);
char *uv__strndup(const char* s, size_t n);
void* uv__malloc(size_t size);
void uv__free(void* ptr);
void* uv__realloc(void* ptr, size_t size);
void* uv__reallocf(void* ptr, size_t size);

// struct uv__loop_metrics_s {
//   uv_metrics_t metrics;
//   uint64_t provider_entry_time;
//   uint64_t provider_idle_time;
//   uv_mutex_t lock;
// };

typedef struct uv__loop_metrics_s uv__loop_metrics_t;
typedef struct uv__loop_internal_fields_s uv__loop_internal_fields_t;



void uv__metrics_update_idle_time(uv_loop_t* loop);
void uv__metrics_set_provider_entry_time(uv_loop_t* loop);

#endif /* UV_COMMON_H_ */

// struct uv__iou {
//   uint32_t* sqhead;
//   uint32_t* sqtail;
//   uint32_t* sqarray;
//   uint32_t sqmask;
//   uint32_t* sqflags;
//   uint32_t* cqhead;
//   uint32_t* cqtail;
//   uint32_t cqmask;
//   void* sq;   /* pointer to munmap() on event loop teardown */
//   void* cqe;  /* pointer to array of struct uv__io_uring_cqe */
//   void* sqe;  /* pointer to array of struct uv__io_uring_sqe */
//   size_t sqlen;
//   size_t cqlen;
//   size_t maxlen;
//   size_t sqelen;
//   int ringfd;
//   uint32_t in_flight;
// };