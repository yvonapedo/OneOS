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

#include "uv.h"
#include "uv-common.h"
#include "os_memory.h"

#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stddef.h> /* NULL */
#include <stdio.h>
#include <stdlib.h> /* malloc */
#include <string.h> /* memset */

#if defined(_WIN32)
# include <malloc.h> /* malloc */
#else
# include <unistd.h> /* usleep */
// # include <net/if.h> /* if_nametoindex */
 #endif


typedef struct {
  uv_malloc_func local_malloc;
  uv_realloc_func local_realloc;
  uv_calloc_func local_calloc;
  uv_free_func local_free;
} uv__allocator_t;

static void* platform_malloc(size_t size);
static void* platform_realloc(void* ptr, size_t size);
static void* platform_calloc(size_t count, size_t size);
static void platform_free(void* ptr);

static uv__allocator_t uv__allocator = {
  platform_malloc,
  platform_realloc,
  platform_calloc,
  platform_free,
};

static void* platform_malloc(size_t size)
{
  if (0 == size)
    size += 4;
  return os_malloc((os_size_t)size);
}

static void* platform_realloc(void* ptr, size_t size)
{
  if (0 == size)
    size += 4;
  return os_realloc(ptr, (os_size_t)size);
}

static void* platform_calloc(size_t count, size_t size)
{
  if (0 == size)
    size += 4;
  return os_calloc((os_size_t)count, (os_size_t)size);
}

static void platform_free(void* ptr)
{
  if (ptr != OS_NULL)
    os_free(ptr);
}

#if defined(__APPLE__) || defined(_WIN32) || defined(TUV_FEATURE_PIPE) || defined(__ONEOS__)
char* uv__strdup(const char* s) {
  size_t len = strlen(s) + 1;
  char* m = uv__malloc(len);
  if (m == NULL)
    return NULL;
  return memcpy(m, s, len);
}
#endif

void* uv__malloc(size_t size) {
  return uv__allocator.local_malloc(size);
}

void uv__free(void* ptr) {
  int saved_errno;

  /* Libuv expects that free() does not clobber errno.  The system allocator
   * honors that assumption but custom allocators may not be so careful.
   */
  saved_errno = errno;
  uv__allocator.local_free(ptr);
  errno = saved_errno;
}

void* uv__calloc(size_t count, size_t size) {
  return uv__allocator.local_calloc(count, size);
}

void* uv__realloc(void* ptr, size_t size) {
  return uv__allocator.local_realloc(ptr, size);
}

uv_buf_t uv_buf_init(char* base, unsigned int len) {
  uv_buf_t buf;
  buf.base = base;
  buf.len = len;
  return buf;
}


#define UV_ERR_NAME_GEN(name, _) case UV_ ## name: return #name;
const char* uv_err_name(int err) {
  switch (err) {
    UV_ERRNO_MAP(UV_ERR_NAME_GEN)
    default:
      /* TUV_CHANGES@20170517: To figure out what error name is missing */
      TDLOG("uv_err_name for (%d) is missing. Add err name string\n", err);
      return "uv_err_name: Unknown error";
  }
}
#undef UV_ERR_NAME_GEN


#define UV_STRERROR_GEN(name, msg) case UV_ ## name: return msg;
const char* uv_strerror(int err) {
  switch (err) {
    UV_ERRNO_MAP(UV_STRERROR_GEN)
    default:
        return "Unknown error";
    }
}
#undef UV_STRERROR_GEN


int uv_ip4_addr(const char* ip, int port, struct sockaddr_in* addr) {
  memset(addr, 0, sizeof(*addr));
  addr->sin_family = AF_INET;
  addr->sin_port = htons(port);
  return uv_inet_pton(AF_INET, ip, &(addr->sin_addr.s_addr));
}


int uv_ip6_addr(const char* ip, int port, struct sockaddr_in6* addr) {
  char address_part[40];
  size_t address_part_size;
  const char* zone_index;

  memset(addr, 0, sizeof(*addr));
  addr->sin6_family = AF_INET6;
  addr->sin6_port = htons(port);

  zone_index = strchr(ip, '%');
  if (zone_index != NULL) {
    address_part_size = zone_index - ip;
    if (address_part_size >= sizeof(address_part))
      address_part_size = sizeof(address_part) - 1;

    memcpy(address_part, ip, address_part_size);
    address_part[address_part_size] = '\0';
    ip = address_part;

    zone_index++; /* skip '%' */
    /* NOTE: unknown interface (id=0) is silently ignored */
#ifdef _WIN32
    addr->sin6_scope_id = atoi(zone_index);
#elif !defined(__NUTTX__) && !defined(__TIZENRT__) && !defined(__ONEOS__)
    /* No if_nametoindex in nuttx */
    addr->sin6_scope_id = if_nametoindex(zone_index);
#endif
  }

  return uv_inet_pton(AF_INET6, ip, &addr->sin6_addr);
}


int uv_ip4_name(const struct sockaddr_in* src, char* dst, size_t size) {
  return uv_inet_ntop(AF_INET, &src->sin_addr, dst, size);
}


int uv_ip6_name(const struct sockaddr_in6* src, char* dst, size_t size) {
  return uv_inet_ntop(AF_INET6, &src->sin6_addr, dst, size);
}


int uv_tcp_bind(uv_tcp_t* handle,
                const struct sockaddr* addr,
                unsigned int flags) {
  unsigned int addrlen;

  if (handle->type != UV_TCP)
    return UV_EINVAL;

  if (addr->sa_family == AF_INET)
    addrlen = sizeof(struct sockaddr_in);
  else if (addr->sa_family == AF_INET6)
    addrlen = sizeof(struct sockaddr_in6);
  else
    return UV_EINVAL;

  return uv__tcp_bind(handle, addr, addrlen, flags);
}


int uv_udp_bind(uv_udp_t* handle,
                const struct sockaddr* addr,
                unsigned int flags) {
  unsigned int addrlen;

  if (handle->type != UV_UDP)
    return UV_EINVAL;

  if (addr->sa_family == AF_INET)
    addrlen = sizeof(struct sockaddr_in);
  else if (addr->sa_family == AF_INET6)
    addrlen = sizeof(struct sockaddr_in6);
  else
    return UV_EINVAL;

  return uv__udp_bind(handle, addr, addrlen, flags);
}


int uv_tcp_connect(uv_connect_t* req,
                   uv_tcp_t* handle,
                   const struct sockaddr* addr,
                   uv_connect_cb cb) {
  unsigned int addrlen;

  if (handle->type != UV_TCP)
    return UV_EINVAL;

  if (addr->sa_family == AF_INET)
    addrlen = sizeof(struct sockaddr_in);
  else if (addr->sa_family == AF_INET6)
    addrlen = sizeof(struct sockaddr_in6);
  else
    return UV_EINVAL;

  return uv__tcp_connect(req, handle, addr, addrlen, cb);
}


int uv_udp_send(uv_udp_send_t* req,
                uv_udp_t* handle,
                const uv_buf_t bufs[],
                unsigned int nbufs,
                const struct sockaddr* addr,
                uv_udp_send_cb send_cb) {
  unsigned int addrlen;

  if (handle->type != UV_UDP)
    return UV_EINVAL;

  if (addr->sa_family == AF_INET)
    addrlen = sizeof(struct sockaddr_in);
  else if (addr->sa_family == AF_INET6)
    addrlen = sizeof(struct sockaddr_in6);
  else
    return UV_EINVAL;

  return uv__udp_send(req, handle, bufs, nbufs, addr, addrlen, send_cb);
}


int uv_udp_try_send(uv_udp_t* handle,
                    const uv_buf_t bufs[],
                    unsigned int nbufs,
                    const struct sockaddr* addr) {
  unsigned int addrlen;

  if (handle->type != UV_UDP)
    return UV_EINVAL;

  if (addr->sa_family == AF_INET)
    addrlen = sizeof(struct sockaddr_in);
  else if (addr->sa_family == AF_INET6)
    addrlen = sizeof(struct sockaddr_in6);
  else
    return UV_EINVAL;

  return uv__udp_try_send(handle, bufs, nbufs, addr, addrlen);
}


int uv_udp_recv_start(uv_udp_t* handle,
                      uv_alloc_cb alloc_cb,
                      uv_udp_recv_cb recv_cb) {
  if (handle->type != UV_UDP || alloc_cb == NULL || recv_cb == NULL)
    return UV_EINVAL;
  else
    return uv__udp_recv_start(handle, alloc_cb, recv_cb);
}


int uv_udp_recv_stop(uv_udp_t* handle) {
  if (handle->type != UV_UDP)
    return UV_EINVAL;
  else
    return uv__udp_recv_stop(handle);
}


void uv_walk(uv_loop_t* loop, uv_walk_cb walk_cb, void* arg) {
  QUEUE queue;
  QUEUE* q;
  uv_handle_t* h;

  QUEUE_MOVE(&loop->handle_queue, &queue);
  while (!QUEUE_EMPTY(&queue)) {
    q = QUEUE_HEAD(&queue);
    h = QUEUE_DATA(q, uv_handle_t, handle_queue);

    QUEUE_REMOVE(q);
    QUEUE_INSERT_TAIL(&loop->handle_queue, q);

    if (h->flags & UV__HANDLE_INTERNAL) continue;
    walk_cb(h, arg);
  }
}


void uv_ref(uv_handle_t* handle) {
  uv__handle_ref(handle);
}


void uv_unref(uv_handle_t* handle) {
  uv__handle_unref(handle);
}


void uv_stop(uv_loop_t* loop) {
  loop->stop_flag = 1;
}


uint64_t uv_now(const uv_loop_t* loop) {
  return loop->time;
}



size_t uv__count_bufs(const uv_buf_t bufs[], unsigned int nbufs) {
  unsigned int i;
  size_t bytes;

  bytes = 0;
  for (i = 0; i < nbufs; i++)
    bytes += (size_t) bufs[i].len;

  return bytes;
}


/* The windows implementation does not have the same structure layout as
 * the unix implementation (nbufs is not directly inside req but is
 * contained in a nested union/struct) so this function locates it.
*/
static unsigned int* uv__get_nbufs(uv_fs_t* req) {
#ifdef _WIN32
  return &req->fs.info.nbufs;
#else
  return &req->nbufs;
#endif
}

/* uv_fs_scandir() uses the system allocator to allocate memory on non-Windows
 * systems. So, the memory should be released using free(). On Windows,
 * uv__malloc() is used, so use uv__free() to free memory.
*/
#ifdef _WIN32
# define uv__fs_scandir_free uv__free
#else
# define uv__fs_scandir_free free
#endif

void uv__fs_scandir_cleanup(uv_fs_t* req) {
  uv__dirent_t** dents;

  unsigned int* nbufs = uv__get_nbufs(req);

  dents = req->ptr;
  if (*nbufs > 0 && *nbufs != (unsigned int) req->result)
    (*nbufs)--;
  for (; *nbufs < (unsigned int) req->result; (*nbufs)++)
    uv__fs_scandir_free(dents[*nbufs]);

  uv__fs_scandir_free(req->ptr);
  req->ptr = NULL;
}


int uv_fs_scandir_next(uv_fs_t* req, uv_dirent_t* ent) {
  uv__dirent_t** dents;
  uv__dirent_t* dent;

  unsigned int* nbufs = uv__get_nbufs(req);

  dents = req->ptr;

  /* Free previous entity */
  if (*nbufs > 0)
    uv__fs_scandir_free(dents[*nbufs - 1]);

  /* End was already reached */
  if (*nbufs == (unsigned int) req->result) {
    uv__fs_scandir_free(dents);
    req->ptr = NULL;
    return UV_EOF;
  }

  dent = dents[(*nbufs)++];

  ent->name = dent->d_name;
#ifdef HAVE_DIRENT_TYPES
  switch (dent->d_type) {
    case UV__DT_DIR:
      ent->type = UV_DIRENT_DIR;
      break;
    case UV__DT_FILE:
      ent->type = UV_DIRENT_FILE;
      break;
    case UV__DT_LINK:
      ent->type = UV_DIRENT_LINK;
      break;
    case UV__DT_FIFO:
      ent->type = UV_DIRENT_FIFO;
      break;
    case UV__DT_SOCKET:
      ent->type = UV_DIRENT_SOCKET;
      break;
    case UV__DT_CHAR:
      ent->type = UV_DIRENT_CHAR;
      break;
    case UV__DT_BLOCK:
      ent->type = UV_DIRENT_BLOCK;
      break;
    default:
      ent->type = UV_DIRENT_UNKNOWN;
  }
#else
  ent->type = UV_DIRENT_UNKNOWN;
#endif

  return 0;
}


static uv_loop_t default_loop_struct;
static uv_loop_t* default_loop_ptr;


uv_loop_t* uv_default_loop(void) {
  if (default_loop_ptr != NULL)
    return default_loop_ptr;

  if (uv_loop_init(&default_loop_struct))
    return NULL;

  default_loop_ptr = &default_loop_struct;
  return default_loop_ptr;
}


int uv_loop_close(uv_loop_t* loop) {
  QUEUE* q;
  uv_handle_t* h;
#ifndef NDEBUG
  void* saved_data;
#endif

  if (!QUEUE_EMPTY(&(loop)->active_reqs))
    return UV_EBUSY;

  tuv_async_deinit(loop, &loop->wq_async);

  QUEUE_FOREACH(q, &loop->handle_queue) {
    h = QUEUE_DATA(q, uv_handle_t, handle_queue);
    if (!(h->flags & UV__HANDLE_INTERNAL))
      return UV_EBUSY;
  }

  uv__loop_close(loop);

#ifndef NDEBUG
  saved_data = loop->data;
  memset(loop, -1, sizeof(*loop));
  loop->data = saved_data;
#endif
  if (loop == default_loop_ptr)
    default_loop_ptr = NULL;

  return 0;
}

/* Pause the calling thread for a number of milliseconds. */
void uv_sleep(int msec) {
#ifdef _WIN32
  Sleep(msec);
#else
//  Sleep(((msec * 1000)+1000)/1000)
  usleep(msec * 1000);
#endif
}

void uv_os_free_passwd(uv_passwd_t* pwd) {
  if (pwd == NULL)
    return;

  /* On unix, the memory for name, shell, and homedir are allocated in a single
   * uv__malloc() call. The base of the pointer is stored in pwd->username, so
   * that is the field that needs to be freed.
   */
  uv__free(pwd->username);
#ifdef _WIN32
  uv__free(pwd->homedir);
#endif
  pwd->username = NULL;
  pwd->shell = NULL;
  pwd->homedir = NULL;
}

void uv_os_free_group(uv_group_t *grp) {
  if (grp == NULL)
    return;

  /* The memory for is allocated in a single uv__malloc() call. The base of the
   * pointer is stored in grp->members, so that is the only field that needs to
   * be freed.
   */
  uv__free(grp->members);
  grp->members = NULL;
  grp->groupname = NULL;
}

void uv_os_free_environ(uv_env_item_t* envitems, int count) {
  int i;

  for (i = 0; i < count; i++) {
    uv__free(envitems[i].name);
  }

  uv__free(envitems);
}

void uv_free_cpu_info(uv_cpu_info_t* cpu_infos, int count) {
#ifdef __linux__
  (void) &count;
  uv__free(cpu_infos);
#else
  int i;

  for (i = 0; i < count; i++)
    uv__free(cpu_infos[i].model);

  uv__free(cpu_infos);
#endif  /* __linux__ */
}