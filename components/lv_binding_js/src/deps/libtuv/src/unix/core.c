/* Copyright 2015-2016 Samsung Electronics Co., Ltd.
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
#include "internal.h"

#include <stddef.h> /* NULL */
#include <stdio.h> /* printf */
#include <stdlib.h>
#include <string.h> /* strerror */
#include <errno.h>
#include <assert.h>
#include <unistd.h>
// #include<sys/unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
// #include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <limits.h> /* INT_MAX, PATH_MAX, IOV_MAX */
#include <pwd.h>
#if !defined(__TIZENRT__) && !defined(__ONEOS__)
# include <sys/uio.h> /* writev */
# include <pwd.h>
#endif

#ifdef __sun
# include <sys/filio.h>
# include <sys/types.h>
# include <sys/wait.h>
#endif

#ifdef __APPLE__
# include <mach-o/dyld.h> /* _NSGetExecutablePath */
# include <sys/filio.h>
# if defined(O_CLOEXEC)
#  define UV__O_CLOEXEC O_CLOEXEC
# endif
#endif

#if defined(__DragonFly__)      || \
    defined(__FreeBSD__)        || \
    defined(__FreeBSD_kernel__)
# include <sys/sysctl.h>
# include <sys/filio.h>
# include <sys/wait.h>
# define UV__O_CLOEXEC O_CLOEXEC
# if defined(__FreeBSD__) && __FreeBSD__ >= 10
#  define uv__accept4 accept4
#  define UV__SOCK_NONBLOCK SOCK_NONBLOCK
#  define UV__SOCK_CLOEXEC  SOCK_CLOEXEC
# endif
# if !defined(F_DUP2FD_CLOEXEC) && defined(_F_DUP2FD_CLOEXEC)
#  define F_DUP2FD_CLOEXEC  _F_DUP2FD_CLOEXEC
# endif
#endif

#if defined(__ANDROID_API__) && __ANDROID_API__ < 21
# include <dlfcn.h>  /* for dlsym */
#endif

#if defined(__MVS__)
#include <sys/ioctl.h>
#endif

static int uv__run_pending(uv_loop_t* loop);

/* Verify that uv_buf_t is ABI-compatible with struct iovec. */
STATIC_ASSERT(sizeof(uv_buf_t) == sizeof(struct iovec));
STATIC_ASSERT(sizeof(&((uv_buf_t*) 0)->base) ==
              sizeof(((struct iovec*) 0)->iov_base));
STATIC_ASSERT(sizeof(&((uv_buf_t*) 0)->len) ==
              sizeof(((struct iovec*) 0)->iov_len));
STATIC_ASSERT(offsetof(uv_buf_t, base) == offsetof(struct iovec, iov_base));
STATIC_ASSERT(offsetof(uv_buf_t, len) == offsetof(struct iovec, iov_len));

/* https://github.com/libuv/libuv/issues/1674 */
int uv_clock_gettime(uv_clock_id clock_id, uv_timespec64_t* ts) {
  struct timespec t;
  int r;

  if (ts == NULL)
    return UV_EFAULT;

  switch (clock_id) {
    default:
      return UV_EINVAL;
    case UV_CLOCK_MONOTONIC:
      r = clock_gettime(CLOCK_MONOTONIC, &t);
      break;
    case UV_CLOCK_REALTIME:
      r = clock_gettime(CLOCK_REALTIME, &t);
      break;
  }

  if (r)
    return UV__ERR(errno);

  ts->tv_sec = t.tv_sec;
  ts->tv_nsec = t.tv_nsec;

  return 0;
}


uint64_t uv_hrtime(void) {
  return uv__hrtime(UV_CLOCK_PRECISE);
}


void uv_close(uv_handle_t* handle, uv_close_cb close_cb) {
  assert(!uv__is_closing(handle));

  handle->flags |= UV_CLOSING;
  handle->close_cb = close_cb;

  switch (handle->type) {
#ifdef TUV_FEATURE_PIPE
  case UV_NAMED_PIPE:
    uv__pipe_close((uv_pipe_t*)handle);
    break;
#endif

  // case UV_TTY:
  //   uv__stream_close((uv_stream_t*)handle);
  //   break;

  case UV_TCP:
    uv__tcp_close((uv_tcp_t*)handle);
    break;

  case UV_UDP:
    uv__udp_close((uv_udp_t*)handle);
    break;

  case UV_IDLE:
    uv__idle_close((uv_idle_t*)handle);
    break;

  case UV_ASYNC:
    uv__async_close((uv_async_t*)handle);
    break;

  case UV_TIMER:
    uv__timer_close((uv_timer_t*)handle);
    break;

// { TUV_CHANGES@20180803:
//   Made signal build time configurable }
#if TUV_FEATURE_PROCESS
  case UV_PROCESS:
    uv__process_close((uv_process_t*)handle);
    break;
#endif

  case UV_POLL:
     uv__poll_close((uv_poll_t*)handle);
     break;

#ifdef TUV_FEATURE_SIGNAL
  case UV_SIGNAL:
    uv__signal_close((uv_signal_t*) handle);
    /* Signal handles may not be closed immediately. The signal code will */
    /* itself close uv__make_close_pending whenever appropriate. */
    return;
#endif

  default:
    assert(0);
  }

  uv__make_close_pending(handle);
}

int uv__socket_sockopt(uv_handle_t* handle, int optname, int* value) {
  int r;
  int fd;
  socklen_t len;

  if (handle == NULL || value == NULL)
    return UV_EINVAL;

  if (handle->type == UV_TCP || handle->type == UV_NAMED_PIPE)
    fd = uv__stream_fd((uv_stream_t*) handle);
  else if (handle->type == UV_UDP)
    fd = ((uv_udp_t *) handle)->io_watcher.fd;
  else
    return UV_ENOTSUP;

  len = sizeof(*value);

  if (*value == 0)
    r = getsockopt(fd, SOL_SOCKET, optname, value, &len);
  else
    r = setsockopt(fd, SOL_SOCKET, optname, (const void*) value, len);

  if (r < 0)
    return UV__ERR(errno);

  return 0;
}

void uv__make_close_pending(uv_handle_t* handle) {
  assert(handle->flags & UV_CLOSING);
  assert(!(handle->flags & UV_CLOSED));
  handle->next_closing = handle->loop->closing_handles;
  handle->loop->closing_handles = handle;
}

int uv__getiovmax(void) {
#if defined(IOV_MAX)
  return IOV_MAX;
#elif defined(_SC_IOV_MAX)
  static int iovmax = -1;
  if (iovmax == -1) {
    iovmax = sysconf(_SC_IOV_MAX);
    /* On some embedded devices (arm-linux-uclibc based ip camera),
     * sysconf(_SC_IOV_MAX) can not get the correct value. The return
     * value is -1 and the errno is EINPROGRESS. Degrade the value to 1.
     */
    if (iovmax == -1) iovmax = 1;
  }
  return iovmax;
#else
  return 1024;
#endif
}


static void uv__finish_close(uv_handle_t* handle) {
  /* Note: while the handle is in the UV_CLOSING state now, it's still possible
   * for it to be active in the sense that uv__is_active() returns true.
   * A good example is when the user calls uv_shutdown(), immediately followed
   * by uv_close(). The handle is considered active at this point because the
   * completion of the shutdown req is still pending.
   */
  assert(handle->flags & UV_CLOSING);
  assert(!(handle->flags & UV_CLOSED));
  handle->flags |= UV_CLOSED;

  switch (handle->type) {
    // case UV_PREPARE:
    // case UV_CHECK:
    case UV_IDLE:
    case UV_ASYNC:
    case UV_TIMER:
    case UV_PROCESS:
    // case UV_FS_EVENT:
    // case UV_FS_POLL:
    case UV_POLL:
    case UV_SIGNAL:
      break;

    case UV_NAMED_PIPE:
    case UV_TCP:
    // case UV_TTY:
      uv__stream_destroy((uv_stream_t*)handle);
      break;

    case UV_UDP:
      uv__udp_finish_close((uv_udp_t*)handle);
      break;

    default:
      assert(0);
      break;
  }

  uv__handle_unref(handle);
  QUEUE_REMOVE(&handle->handle_queue);

  if (handle->close_cb) {
    handle->close_cb(handle);
  }
}


static void uv__run_closing_handles(uv_loop_t* loop) {
  uv_handle_t* p;
  uv_handle_t* q;

  p = loop->closing_handles;
  loop->closing_handles = NULL;

  while (p) {
    q = p->next_closing;
    uv__finish_close(p);
    p = q;
  }
}


int uv_is_closing(const uv_handle_t* handle) {
  return uv__is_closing(handle);
}

// int uv_backend_fd(const uv_loop_t* loop) {
//   return loop->backend_fd;
// }

static int uv__backend_timeout(const uv_loop_t* loop) {
  if (loop->stop_flag == 0 &&
      /* uv__loop_alive(loop) && */
      (uv__has_active_handles(loop) || uv__has_active_reqs(loop)) &&
      QUEUE_EMPTY(&loop->pending_queue) &&
      QUEUE_EMPTY(&loop->idle_handles) &&
      (loop->flags & UV_LOOP_REAP_CHILDREN) == 0 &&
      loop->closing_handles == NULL)
    return uv__next_timeout(loop);
  return 0;
}

int uv_backend_timeout(const uv_loop_t* loop) {
  if (loop->stop_flag != 0)
    return 0;

  if (!uv__has_active_handles(loop) && !uv__has_active_reqs(loop))
    return 0;

  if (!QUEUE_EMPTY(&loop->idle_handles))
    return 0;

  if (!QUEUE_EMPTY(&loop->pending_queue))
    return 0;

  if (loop->closing_handles)
    return 0;

  return uv__next_timeout(loop);
}


static int uv__loop_alive(const uv_loop_t* loop) {
  return uv__has_active_handles(loop) ||
         uv__has_active_reqs(loop) ||
         loop->closing_handles != NULL;
}


int uv_loop_alive(const uv_loop_t* loop) {
    return uv__loop_alive(loop);
}

/* test driver */
int uv_loop_has_active_reqs(const uv_loop_t* loop) {
    return uv__has_active_reqs(loop);
}


int uv_run(uv_loop_t* loop, uv_run_mode mode) {
  int timeout;
  int r;
  int ran_pending;

  r = uv__loop_alive(loop);
  // redundancy
  // if (!r)
  //   uv__update_time(loop);

  while (r != 0 && loop->stop_flag == 0) {
    uv__update_time(loop);
    uv__run_timers(loop);
    ran_pending = uv__run_pending(loop);
    uv__run_idle(loop);

    timeout = 0;
    if ((mode == UV_RUN_ONCE && !ran_pending) || mode == UV_RUN_DEFAULT)
      timeout = uv_backend_timeout(loop);

    uv__io_poll(loop, timeout);
    uv__run_closing_handles(loop);

    if (mode == UV_RUN_ONCE) {
      /* UV_RUN_ONCE implies forward progress: at least one callback must have
       * been invoked when it returns. uv__io_poll() can return without doing
       * I/O (meaning: no callbacks) when its timeout expires - which means we
       * have pending timers that satisfy the forward progress constraint.
       *
       * UV_RUN_NOWAIT makes no guarantees about progress so it's omitted from
       * the check.
       */
      uv__update_time(loop);
      uv__run_timers(loop);
    }

    r = uv__loop_alive(loop);
    if (mode == UV_RUN_ONCE || mode == UV_RUN_NOWAIT)
      break;
  }

  /* The if statement lets gcc compile it to a conditional store. Avoids
   * dirtying a cache line.
   */
  if (loop->stop_flag != 0)
    loop->stop_flag = 0;

  return r;
}


void uv_update_time(uv_loop_t* loop) {
  uv__update_time(loop);
}


int uv_is_active(const uv_handle_t* handle) {
  return uv__is_active(handle);
}


/* Open a socket in non-blocking close-on-exec mode, atomically if possible. */
int uv__socket(int domain, int type, int protocol) {
  int sockfd;
  int err;

#if defined(SOCK_NONBLOCK) && defined(SOCK_CLOEXEC)
  sockfd = socket(domain, type | SOCK_NONBLOCK | SOCK_CLOEXEC, protocol);
  if (sockfd != -1)
    return sockfd;

  if (errno != EINVAL)
    return -errno;
#endif

  sockfd = socket(domain, type, protocol);
  if (sockfd == -1)
    return -errno;

  err = uv__nonblock(sockfd, 1);
  if (err == 0)
    err = uv__cloexec(sockfd, 1);

  if (err) {
    uv__close(sockfd);
    return err;
  }

#if defined(SO_NOSIGPIPE)
  {
    int on = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_NOSIGPIPE, &on, sizeof(on));
  }
#endif

  return sockfd;
}

FILE* uv__open_file(const char* path) {
  int fd;
  FILE* fp;

  fd = uv__open_cloexec(path, O_RDONLY);
  if (fd < 0)
    return NULL;

   fp = fdopen(fd, "r");
   if (fp == NULL)
     uv__close(fd);

   return fp;
}


int uv__accept(int sockfd) {
  int peerfd;
  int err;

  assert(sockfd >= 0);

  while (1) {
#if defined(__linux__) || (defined(__FreeBSD__) && __FreeBSD__ >= 10)
    static int no_accept4;

    if (no_accept4)
      goto skip;

    peerfd = uv__accept4(sockfd,
                         NULL,
                         NULL,
                         UV__SOCK_NONBLOCK|UV__SOCK_CLOEXEC);
    if (peerfd != -1)
      return peerfd;

    if (errno == EINTR)
      continue;

    if (errno != ENOSYS)
      return -errno;

    no_accept4 = 1;
skip:
#endif

    peerfd = accept(sockfd, NULL, NULL);
    if (peerfd == -1) {
      if (errno == EINTR)
        continue;
      return -errno;
    }

    err = uv__cloexec(peerfd, 1);
    if (err == 0)
      err = uv__nonblock(peerfd, 1);

    if (err) {
      uv__close(peerfd);
      return err;
    }

    return peerfd;
  }
}

int uv__close_nocancel(int fd) {
#if defined(__APPLE__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdollar-in-identifier-extension"
#if defined(__LP64__) || TARGET_OS_IPHONE
  extern int close$NOCANCEL(int);
  return close$NOCANCEL(fd);
#else
  extern int close$NOCANCEL$UNIX2003(int);
  return close$NOCANCEL$UNIX2003(fd);
#endif
#pragma GCC diagnostic pop
#elif defined(__linux__) && defined(__SANITIZE_THREAD__) && defined(__clang__)
  long rc;
  __sanitizer_syscall_pre_close(fd);
  rc = syscall(SYS_close, fd);
  __sanitizer_syscall_post_close(rc, fd);
  return rc;
#elif defined(__linux__) && !defined(__SANITIZE_THREAD__)
  return syscall(SYS_close, fd);
#else
  return close(fd);
#endif
}

int uv__close_nocheckstdio(int fd) {
  int saved_errno;
  int rc;

  assert(fd > -1);  /* Catch uninitialized io_watcher.fd bugs. */

  saved_errno = errno;
  rc = close(fd);
  if (rc == -1) {
    rc = -errno;
    if (rc == -EINTR || rc == -EINPROGRESS)
      rc = 0;    /* The close is in progress, not an error. */
    errno = saved_errno;
  }

  return rc;
}


int uv__close(int fd) {
#if !defined(__NUTTX__) && !defined(__TIZENRT__) && !defined(__ONEOS__) /* No STDERR_FILENO in nuttx */
  assert(fd > STDERR_FILENO);  /* Catch stdio close bugs. */
#endif
  return uv__close_nocheckstdio(fd);
}


#if defined(_AIX) || \
    defined(__APPLE__) || \
    defined(__DragonFly__) || \
    defined(__FreeBSD__) || \
    defined(__FreeBSD_kernel__) || \
    defined(__linux__) || \
    defined(__ONEOS__)
int uv__nonblock_ioctl(int fd, int set) {
  int r;

  do
    r = ioctl(fd, FIONBIO, &set);
  while (r == -1 && errno == EINTR);

  if (r)
    return -errno;

  return 0;
}
#endif

int uv__nonblock_fcntl(int fd, int set) {
  int flags;
  int r;

  do
    r = fcntl(fd, F_GETFL);
  while (r == -1 && errno == EINTR);

  if (r == -1)
    return -errno;

  /* Bail out now if already set/clear. */
  if (!!(r & O_NONBLOCK) == !!set)
    return 0;

  if (set)
    flags = r | O_NONBLOCK;
  else
    flags = r & ~O_NONBLOCK;

  do
    r = fcntl(fd, F_SETFL, flags);
  while (r == -1 && errno == EINTR);

  if (r)
    return -errno;

  return 0;
}

int uv__nonblock_sock(int fd, int set) {
  return ioctlsocket(fd, FIONBIO, &set);
}

int uv__cloexec_fcntl(int fd, int set) {
#if defined(__NUTTX__) || defined(__TIZENRT__) || defined(__ONEOS__)
  return 0;
#endif
  int flags;
  int r;

  do
    r = fcntl(fd, F_GETFD);
  while (r == -1 && errno == EINTR);

  if (r == -1)
    return -errno;

  /* Bail out now if already set/clear. */

  if (!!(r & FD_CLOEXEC) == !!set)
    return 0;

  if (set)
    flags = r | FD_CLOEXEC;
  else
    flags = r & ~FD_CLOEXEC;

  do
    r = fcntl(fd, F_SETFD, flags);
  while (r == -1 && errno == EINTR);

  if (r)
    return -errno;

  return 0;
}


int uv__cloexec(int fd, int set) {
  int flags;
  int r;

  flags = 0;
  if (set)
    flags = FD_CLOEXEC;

  do
    r = fcntl(fd, F_SETFD, flags);
  while (r == -1 && errno == EINTR);

  if (r)
    return UV__ERR(errno);

  return 0;
}

#if defined(__NUTTX__) || defined(__TIZENRT__) || defined(__ONEOS__)
ssize_t uv__recvmsg(int fd, struct msghdr* msg, int flags) {
  return -1;
}
#else
ssize_t uv__recvmsg(int fd, struct msghdr* msg, int flags) {
  struct cmsghdr* cmsg;
  ssize_t rc;
  int* pfd;
  int* end;
#if defined(__linux__)
  static int no_msg_cmsg_cloexec;
  if (no_msg_cmsg_cloexec == 0) {
    rc = recvmsg(fd, msg, flags | 0x40000000);  /* MSG_CMSG_CLOEXEC */
    if (rc != -1)
      return rc;
    if (errno != EINVAL)
      return -errno;
    rc = recvmsg(fd, msg, flags);
    if (rc == -1)
      return -errno;
    no_msg_cmsg_cloexec = 1;
  } else {
    rc = recvmsg(fd, msg, flags);
  }
#else
  rc = recvmsg(fd, msg, flags);
#endif
  if (rc == -1)
    return -errno;
  if (msg->msg_controllen == 0)
    return rc;
  for (cmsg = CMSG_FIRSTHDR(msg); cmsg != NULL; cmsg = CMSG_NXTHDR(msg, cmsg))
    if (cmsg->cmsg_type == SCM_RIGHTS)
      for (pfd = (int*) CMSG_DATA(cmsg),
           end = (int*) ((char*) cmsg + cmsg->cmsg_len);
           pfd < end;
           pfd += 1)
        uv__cloexec(*pfd, 1);
  return rc;
}
#endif /* End of !NUTTX */


int uv_cwd(char* buffer, size_t* size) {
  if (buffer == NULL || size == NULL)
    return -EINVAL;

  if (getcwd(buffer, *size) == NULL)
    return -errno;

  *size = strlen(buffer);
  if (*size > 1 && buffer[*size - 1] == '/') {
    buffer[*size-1] = '\0';
    (*size)--;
  }

  return 0;
}


int uv_chdir(const char* dir) {
  if (chdir(dir))
    return -errno;

  return 0;
}

void uv_disable_stdio_inheritance(void) {
  int fd;

  /* Set the CLOEXEC flag on all open descriptors. Unconditionally try the
   * first 16 file descriptors. After that, bail out after the first error.
   */
  for (fd = 0; ; fd++)
    if (uv__cloexec(fd, 1) && fd > 15)
      break;
}

int uv_fileno(const uv_handle_t* handle, uv_os_fd_t* fd) {
  int fd_out;

  switch (handle->type) {
  case UV_TCP:
  case UV_NAMED_PIPE:
  // case UV_TTY:
  //   fd_out = uv__stream_fd((uv_stream_t*) handle);
  //   break;

  case UV_UDP:
    fd_out = ((uv_udp_t *) handle)->io_watcher.fd;
    break;

  case UV_POLL:
    fd_out = ((uv_poll_t *) handle)->io_watcher.fd;
    break;

  default:
    return UV_EINVAL;
  }

  if (uv__is_closing(handle) || fd_out == -1)
    return UV_EBADF;

  *fd = fd_out;
  return 0;
}

static int uv__run_pending(uv_loop_t* loop) {
  QUEUE* q;
  QUEUE pq;
  uv__io_t* w;

  if (QUEUE_EMPTY(&loop->pending_queue))
    return 0;

  QUEUE_MOVE(&loop->pending_queue, &pq);

  while (!QUEUE_EMPTY(&pq)) {
    q = QUEUE_HEAD(&pq);
    QUEUE_REMOVE(q);
    QUEUE_INIT(q);
    w = QUEUE_DATA(q, uv__io_t, pending_queue);
    w->cb(loop, w, POLLOUT);
  }

  return 1;
}


static unsigned int next_power_of_two(unsigned int val) {
  val -= 1;
  val |= val >> 1;
  val |= val >> 2;
  val |= val >> 4;
  val |= val >> 8;
  val |= val >> 16;
  val += 1;
  return val;
}

static void maybe_resize(uv_loop_t* loop, unsigned int len) {
  uv__io_t** watchers;
  void* fake_watcher_list;
  void* fake_watcher_count;
  unsigned int nwatchers;
  unsigned int i;

  if (len <= loop->nwatchers)
    return;

  /* Preserve fake watcher list and count at the end of the watchers */
  if (loop->watchers != NULL) {
    fake_watcher_list = loop->watchers[loop->nwatchers];
    fake_watcher_count = loop->watchers[loop->nwatchers + 1];
  } else {
    fake_watcher_list = NULL;
    fake_watcher_count = NULL;
  }

  nwatchers = next_power_of_two(len + 2) - 2;
  watchers = uv__realloc(loop->watchers,
                         (nwatchers + 2) * sizeof(loop->watchers[0]));

  if (watchers == NULL)
    abort();
  for (i = loop->nwatchers; i < nwatchers; i++)
    watchers[i] = NULL;
  watchers[nwatchers] = fake_watcher_list;
  watchers[nwatchers + 1] = fake_watcher_count;

  loop->watchers = watchers;
  loop->nwatchers = nwatchers;
}


void uv__io_init(uv__io_t* w, uv__io_cb cb, int fd) {
  assert(cb != NULL);
  assert(fd >= -1);
  QUEUE_INIT(&w->pending_queue);
  QUEUE_INIT(&w->watcher_queue);
  w->cb = cb;
  w->fd = fd;
  w->events = 0;
  w->pevents = 0;

#if defined(UV_HAVE_KQUEUE)
  w->rcount = 0;
  w->wcount = 0;
#endif /* defined(UV_HAVE_KQUEUE) */
}


void uv__io_start(uv_loop_t* loop, uv__io_t* w, unsigned int events) {
  assert(0 == (events & ~(POLLIN | POLLOUT | UV__POLLRDHUP)));
  assert(0 != events);
  assert(w->fd >= 0);
  assert(w->fd < INT_MAX);

  w->pevents |= events;
  maybe_resize(loop, w->fd + 1);

#if !defined(__sun)
  /* The event ports backend needs to rearm all file descriptors on each and
   * every tick of the event loop but the other backends allow us to
   * short-circuit here if the event mask is unchanged.
   */
  if (w->events == w->pevents) {
    if (w->events == 0 && !QUEUE_EMPTY(&w->watcher_queue)) {
      QUEUE_REMOVE(&w->watcher_queue);
      QUEUE_INIT(&w->watcher_queue);
    }
    return;
  }
#endif

  if (QUEUE_EMPTY(&w->watcher_queue))
    QUEUE_INSERT_TAIL(&loop->watcher_queue, &w->watcher_queue);

  if (loop->watchers[w->fd] == NULL) {
    loop->watchers[w->fd] = w;
    loop->nfds++;
  }
}


void uv__io_stop(uv_loop_t* loop, uv__io_t* w, unsigned int events) {
  assert(0 == (events & ~(POLLIN | POLLOUT | UV__POLLRDHUP)));
  assert(0 != events);

  if (w->fd == -1)
    return;

  assert(w->fd >= 0);

  /* Happens when uv__io_stop() is called on a handle that was never started. */
  if ((unsigned) w->fd >= loop->nwatchers)
    return;

  w->pevents &= ~events;

  if (w->pevents == 0) {
    QUEUE_REMOVE(&w->watcher_queue);
    QUEUE_INIT(&w->watcher_queue);

    if (loop->watchers[w->fd] != NULL) {
      assert(loop->watchers[w->fd] == w);
      assert(loop->nfds > 0);
      loop->watchers[w->fd] = NULL;
      loop->nfds--;
      w->events = 0;
    }
  }
  else if (QUEUE_EMPTY(&w->watcher_queue))
    QUEUE_INSERT_TAIL(&loop->watcher_queue, &w->watcher_queue);
}


void uv__io_close(uv_loop_t* loop, uv__io_t* w) {
  uv__io_stop(loop, w, POLLIN | POLLOUT | UV__POLLRDHUP);
  QUEUE_REMOVE(&w->pending_queue);

  /* Remove stale events for this file descriptor */
  uv__platform_invalidate_fd(loop, w->fd);
}


void uv__io_feed(uv_loop_t* loop, uv__io_t* w) {
  if (QUEUE_EMPTY(&w->pending_queue))
    QUEUE_INSERT_TAIL(&loop->pending_queue, &w->pending_queue);
}


int uv__io_active(const uv__io_t* w, unsigned int events) {
  assert(0 == (events & ~(POLLIN | POLLOUT | UV__POLLRDHUP)));
  assert(0 != events);
  return 0 != (w->pevents & events);
}

int uv__fd_exists(uv_loop_t* loop, int fd) {
  return (unsigned) fd < loop->nwatchers && loop->watchers[fd] != NULL;
}

// int uv_getrusage(uv_rusage_t* rusage) {
//   struct rusage usage;

//   if (getrusage(RUSAGE_SELF, &usage))
//     return UV__ERR(errno);

//   rusage->ru_utime.tv_sec = usage.ru_utime.tv_sec;
//   rusage->ru_utime.tv_usec = usage.ru_utime.tv_usec;

//   rusage->ru_stime.tv_sec = usage.ru_stime.tv_sec;
//   rusage->ru_stime.tv_usec = usage.ru_stime.tv_usec;

// #if !defined(__MVS__) && !defined(__HAIKU__)
//   rusage->ru_maxrss = usage.ru_maxrss;
//   rusage->ru_ixrss = usage.ru_ixrss;
//   rusage->ru_idrss = usage.ru_idrss;
//   rusage->ru_isrss = usage.ru_isrss;
//   rusage->ru_minflt = usage.ru_minflt;
//   rusage->ru_majflt = usage.ru_majflt;
//   rusage->ru_nswap = usage.ru_nswap;
//   rusage->ru_inblock = usage.ru_inblock;
//   rusage->ru_oublock = usage.ru_oublock;
//   rusage->ru_msgsnd = usage.ru_msgsnd;
//   rusage->ru_msgrcv = usage.ru_msgrcv;
//   rusage->ru_nsignals = usage.ru_nsignals;
//   rusage->ru_nvcsw = usage.ru_nvcsw;
//   rusage->ru_nivcsw = usage.ru_nivcsw;
// #endif

//   /* Most platforms report ru_maxrss in kilobytes; macOS and Solaris are
//    * the outliers because of course they are.
//    */
// #if defined(__APPLE__) && !TARGET_OS_IPHONE
//   rusage->ru_maxrss /= 1024;                  /* macOS reports bytes. */
// #elif defined(__sun)
//   rusage->ru_maxrss /= getpagesize() / 1024;  /* Solaris reports pages. */
// #endif

//   return 0;
// }


int uv__open_cloexec(const char* path, int flags) {
  int err;
  int fd;

#if defined(UV__O_CLOEXEC)
  static int no_cloexec;

  if (!no_cloexec) {
    fd = open(path, flags | UV__O_CLOEXEC);
    if (fd != -1)
      return fd;

    if (errno != EINVAL)
      return -errno;

    /* O_CLOEXEC not supported. */
    no_cloexec = 1;
  }
#endif

  fd = open(path, flags);
  if (fd == -1)
    return -errno;

  err = uv__cloexec(fd, 1);
  if (err) {
    uv__close(fd);
    return err;
  }

  return fd;
}

int uv__dup2_cloexec(int oldfd, int newfd) {
#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__linux__)
  int r;

  r = dup3(oldfd, newfd, O_CLOEXEC);
  if (r == -1)
    return UV__ERR(errno);

  return r;
#else
  int err;
  int r;

  r = dup2(oldfd, newfd);  /* Never retry. */
  if (r == -1)
    return UV__ERR(errno);

  err = uv__cloexec(newfd, 1);
  if (err != 0) {
    uv__close(newfd);
    return err;
  }

  return r;
#endif
}


int uv_os_homedir(char* buffer, size_t* size) {
  uv_passwd_t pwd;
  size_t len;
  int r;

  /* Check if the HOME environment variable is set first. The task of
     performing input validation on buffer and size is taken care of by
     uv_os_getenv(). */
  r = uv_os_getenv("HOME", buffer, size);

  if (r != UV_ENOENT)
    return r;

  // /* HOME is not set, so call uv_os_get_passwd() */
  // r = uv_os_get_passwd(&pwd);

  // if (r != 0) {
  //   return r;
  // }

  len = strlen(pwd.homedir);

  if (len >= *size) {
    *size = len + 1;
    uv_os_free_passwd(&pwd);
    return UV_ENOBUFS;
  }

  memcpy(buffer, pwd.homedir, len + 1);
  *size = len;
  // uv_os_free_passwd(&pwd);

  return 0;
}


int uv_os_tmpdir(char* buffer, size_t* size) {
  const char* buf;
  size_t len;

  if (buffer == NULL || size == NULL || *size == 0)
    return UV_EINVAL;

#define CHECK_ENV_VAR(name)                                                   \
  do {                                                                        \
    buf = getenv(name);                                                       \
    if (buf != NULL)                                                          \
      goto return_buffer;                                                     \
  }                                                                           \
  while (0)

  /* Check the TMPDIR, TMP, TEMP, and TEMPDIR environment variables in order */
  CHECK_ENV_VAR("TMPDIR");
  CHECK_ENV_VAR("TMP");
  CHECK_ENV_VAR("TEMP");
  CHECK_ENV_VAR("TEMPDIR");

#undef CHECK_ENV_VAR

  /* No temp environment variables defined */
  #if defined(__ANDROID__)
    buf = "/data/local/tmp";
  #else
    buf = "/tmp";
  #endif

return_buffer:
  len = strlen(buf);

  if (len >= *size) {
    *size = len + 1;
    return UV_ENOBUFS;
  }

  /* The returned directory should not have a trailing slash. */
  if (len > 1 && buf[len - 1] == '/') {
    len--;
  }

  memcpy(buffer, buf, len + 1);
  buffer[len] = '\0';
  *size = len;

  return 0;
}

// static int uv__getpwuid_r(uv_passwd_t *pwd, uid_t uid) {
//   struct passwd pw;
//   struct passwd* result;
//   char* buf;
//   size_t bufsize;
//   size_t name_size;
//   size_t homedir_size;
//   size_t shell_size;
//   int r;

//   if (pwd == NULL)
//     return UV_EINVAL;

//   /* Calling sysconf(_SC_GETPW_R_SIZE_MAX) would get the suggested size, but it
//    * is frequently 1024 or 4096, so we can just use that directly. The pwent
//    * will not usually be large. */
//   for (bufsize = 2000;; bufsize *= 2) {
//     buf = uv__malloc(bufsize);

//     if (buf == NULL)
//       return UV_ENOMEM;

//     do
//       r = getpwuid_r(uid, &pw, buf, bufsize, &result);
//     while (r == EINTR);

//     if (r != 0 || result == NULL)
//       uv__free(buf);

//     if (r != ERANGE)
//       break;
//   }

//   if (r != 0)
//     return UV__ERR(r);

//   if (result == NULL)
//     return UV_ENOENT;

//   /* Allocate memory for the username, shell, and home directory */
//   name_size = strlen(pw.pw_name) + 1;
//   homedir_size = strlen(pw.pw_dir) + 1;
//   shell_size = strlen(pw.pw_shell) + 1;
//   pwd->username = uv__malloc(name_size + homedir_size + shell_size);

//   if (pwd->username == NULL) {
//     uv__free(buf);
//     return UV_ENOMEM;
//   }

  /* Copy the username */
//   memcpy(pwd->username, pw.pw_name, name_size);

//   /* Copy the home directory */
//   pwd->homedir = pwd->username + name_size;
//   memcpy(pwd->homedir, pw.pw_dir, homedir_size);

//   /* Copy the shell */
//   pwd->shell = pwd->homedir + homedir_size;
//   memcpy(pwd->shell, pw.pw_shell, shell_size);

//   /* Copy the uid and gid */
//   pwd->uid = pw.pw_uid;
//   pwd->gid = pw.pw_gid;

//   uv__free(buf);

//   return 0;
// }

// int uv_os_get_group(uv_group_t* grp, uv_uid_t gid) {
//   struct group gp;
//   struct group* result;
//   char* buf;
//   char* gr_mem;
//   size_t bufsize;
//   size_t name_size;
//   long members;
//   size_t mem_size;
//   int r;

//   if (grp == NULL)
//     return UV_EINVAL;

//   /* Calling sysconf(_SC_GETGR_R_SIZE_MAX) would get the suggested size, but it
//    * is frequently 1024 or 4096, so we can just use that directly. The pwent
//    * will not usually be large. */
//   for (bufsize = 2000;; bufsize *= 2) {
//     buf = uv__malloc(bufsize);

//     if (buf == NULL)
//       return UV_ENOMEM;

//     do
//       r = getgrgid_r(gid, &gp, buf, bufsize, &result);
//     while (r == EINTR);

//     if (r != 0 || result == NULL)
//       uv__free(buf);

//     if (r != ERANGE)
//       break;
//   }

//   if (r != 0)
//     return UV__ERR(r);

//   if (result == NULL)
//     return UV_ENOENT;

//   /* Allocate memory for the groupname and members. */
//   name_size = strlen(gp.gr_name) + 1;
//   members = 0;
//   mem_size = sizeof(char*);
//   for (r = 0; gp.gr_mem[r] != NULL; r++) {
//     mem_size += strlen(gp.gr_mem[r]) + 1 + sizeof(char*);
//     members++;
//   }

//   gr_mem = uv__malloc(name_size + mem_size);
//   if (gr_mem == NULL) {
//     uv__free(buf);
//     return UV_ENOMEM;
//   }

//   /* Copy the members */
//   grp->members = (char**) gr_mem;
//   grp->members[members] = NULL;
//   gr_mem = (char*) &grp->members[members + 1];
//   for (r = 0; r < members; r++) {
//     grp->members[r] = gr_mem;
//     strcpy(gr_mem, gp.gr_mem[r]);
//     gr_mem += strlen(gr_mem) + 1;
//   }
//   assert(gr_mem == (char*)grp->members + mem_size);

//   /* Copy the groupname */
//   grp->groupname = gr_mem;
//   memcpy(grp->groupname, gp.gr_name, name_size);
//   gr_mem += name_size;

//   /* Copy the gid */
//   grp->gid = gp.gr_gid;

//   uv__free(buf);

//   return 0;
// }


// int uv_os_get_passwd(uv_passwd_t* pwd) {
//   return uv__getpwuid_r(pwd, geteuid());
// }


// int uv_os_get_passwd2(uv_passwd_t* pwd, uv_uid_t uid) {
//   return uv__getpwuid_r(pwd, uid);
// }

int uv_translate_sys_error(int sys_errno) {
  /* If < 0 then it's already a libuv error. */
  return sys_errno <= 0 ? sys_errno : -sys_errno;
}

int uv_os_environ(uv_env_item_t** envitems, int* count) {
  int i, j, cnt;
  uv_env_item_t* envitem;

  *envitems = NULL;
  *count = 0;

  for (i = 0; environ[i] != NULL; i++);

  *envitems = uv__calloc(i, sizeof(**envitems));

  if (*envitems == NULL)
    return UV_ENOMEM;

  for (j = 0, cnt = 0; j < i; j++) {
    char* buf;
    char* ptr;

    if (environ[j] == NULL)
      break;

    buf = uv__strdup(environ[j]);
    if (buf == NULL)
      goto fail;

    ptr = strchr(buf, '=');
    if (ptr == NULL) {
      uv__free(buf);
      continue;
    }

    *ptr = '\0';

    envitem = &(*envitems)[cnt];
    envitem->name = buf;
    envitem->value = ptr + 1;

    cnt++;
  }

  *count = cnt;
  return 0;

fail:
  for (i = 0; i < cnt; i++) {
    envitem = &(*envitems)[cnt];
    uv__free(envitem->name);
  }
  uv__free(*envitems);

  *envitems = NULL;
  *count = 0;
  return UV_ENOMEM;
}


int uv_os_getenv(const char* name, char* buffer, size_t* size) {
  char* var;
  size_t len;

  if (name == NULL || buffer == NULL || size == NULL || *size == 0)
    return UV_EINVAL;

  var = getenv(name);

  if (var == NULL)
    return UV_ENOENT;

  len = strlen(var);

  if (len >= *size) {
    *size = len + 1;
    return UV_ENOBUFS;
  }

  memcpy(buffer, var, len + 1);
  *size = len;

  return 0;
}


int uv_os_setenv(const char* name, const char* value) {
  if (name == NULL || value == NULL)
    return UV_EINVAL;

  if (setenv(name, value, 1) != 0)
    return UV__ERR(errno);

  return 0;
}

#ifdef MAXHOSTNAMELEN
# define UV_MAXHOSTNAMESIZE (MAXHOSTNAMELEN + 1)
#else
  /*
    Fallback for the maximum hostname size, including the null terminator. The
    Windows gethostname() documentation states that 256 bytes will always be
    large enough to hold the null-terminated hostname.
  */
# define UV_MAXHOSTNAMESIZE 256
#endif

int uv_os_unsetenv(const char* name) {
  if (name == NULL)
    return UV_EINVAL;

  if (unsetenv(name) != 0)
    return UV__ERR(errno);

  return 0;
}

int gethostname(char *name, size_t len)
{
    if(len < sizeof("mcu_host"))
        return -1;
    strncpy(name, "mcu_host", sizeof("mcu_host"));
    return 0;
}

int uv_os_gethostname(char* buffer, size_t* size) {
  /*
    On some platforms, if the input buffer is not large enough, gethostname()
    succeeds, but truncates the result. libuv can detect this and return ENOBUFS
    instead by creating a large enough buffer and comparing the hostname length
    to the size input.
  */
  char buf[UV_MAXHOSTNAMESIZE];
  size_t len;

  if (buffer == NULL || size == NULL || *size == 0)
    return UV_EINVAL;

  if (gethostname(buf, sizeof(buf)) != 0)
    return UV__ERR(errno);

  buf[sizeof(buf) - 1] = '\0'; /* Null terminate, just to be safe. */
  len = strlen(buf);

  if (len >= *size) {
    *size = len + 1;
    return UV_ENOBUFS;
  }

  memcpy(buffer, buf, len + 1);
  *size = len;
  return 0;
}

uv_os_fd_t uv_get_osfhandle(int fd) {
  return fd;
}

int uv_open_osfhandle(uv_os_fd_t os_fd) {
  return os_fd;
}

uv_pid_t uv_os_getpid(void) {
  return getpid();
}


// uv_pid_t uv_os_getppid(void) {
//   return getppid();
// }

unsigned int uv_available_parallelism(void) {
#ifdef __linux__
  cpu_set_t set;
  long rc;

  memset(&set, 0, sizeof(set));

  /* sysconf(_SC_NPROCESSORS_ONLN) in musl calls sched_getaffinity() but in
   * glibc it's... complicated... so for consistency try sched_getaffinity()
   * before falling back to sysconf(_SC_NPROCESSORS_ONLN).
   */
  if (0 == sched_getaffinity(0, sizeof(set), &set))
    rc = CPU_COUNT(&set);
  else
    rc = sysconf(_SC_NPROCESSORS_ONLN);

  if (rc < 1)
    rc = 1;

  return (unsigned) rc;
#elif defined(__MVS__)
  int rc;

  rc = __get_num_online_cpus();
  if (rc < 1)
    rc = 1;

  return (unsigned) rc;
#else  /* __linux__ */
  long rc;

  rc = sysconf(_SC_NPROCESSORS_ONLN);
  if (rc < 1)
    rc = 1;

  return (unsigned) rc;
#endif  /* __linux__ */
}



// int uv_os_getpriority(uv_pid_t pid, int* priority) {
//   int r;

//   if (priority == NULL)
//     return UV_EINVAL;

//   errno = 0;
//   r = getpriority(PRIO_PROCESS, (int) pid);

//   if (r == -1 && errno != 0)
//     return UV__ERR(errno);

//   *priority = r;
//   return 0;
// }


// int uv_os_setpriority(uv_pid_t pid, int priority) {
//   if (priority < UV_PRIORITY_HIGHEST || priority > UV_PRIORITY_LOW)
//     return UV_EINVAL;

//   if (setpriority(PRIO_PROCESS, (int) pid, priority) != 0)
//     return UV__ERR(errno);

//   return 0;
// }

// int uv_os_uname(uv_utsname_t* buffer) {
//   struct utsname buf;
//   int r;

//   if (buffer == NULL)
//     return UV_EINVAL;

//   if (uname(&buf) == -1) {
//     r = UV__ERR(errno);
//     goto error;
//   }

//   r = uv__strscpy(buffer->sysname, buf.sysname, sizeof(buffer->sysname));
//   if (r == UV_E2BIG)
//     goto error;

// #ifdef _AIX
//   r = snprintf(buffer->release,
//                sizeof(buffer->release),
//                "%s.%s",
//                buf.version,
//                buf.release);
//   if (r >= sizeof(buffer->release)) {
//     r = UV_E2BIG;
//     goto error;
//   }
// #else
//   r = uv__strscpy(buffer->release, buf.release, sizeof(buffer->release));
//   if (r == UV_E2BIG)
//     goto error;
// #endif

//   r = uv__strscpy(buffer->version, buf.version, sizeof(buffer->version));
//   if (r == UV_E2BIG)
//     goto error;

// #if defined(_AIX) || defined(__PASE__)
//   r = uv__strscpy(buffer->machine, "ppc64", sizeof(buffer->machine));
// #else
//   r = uv__strscpy(buffer->machine, buf.machine, sizeof(buffer->machine));
// #endif

//   if (r == UV_E2BIG)
//     goto error;

//   return 0;

// error:
//   buffer->sysname[0] = '\0';
//   buffer->release[0] = '\0';
//   buffer->version[0] = '\0';
//   buffer->machine[0] = '\0';
//   return r;
// }



// int uv_random(uv_loop_t* loop,
//               uv_random_t* req,
//               void *buf,
//               size_t buflen,
//               unsigned flags,
//               uv_random_cb cb) {
//   if (buflen > 0x7FFFFFFFu)
//     return UV_E2BIG;

//   if (flags != 0)
//     return UV_EINVAL;
//   // return uv__random(buf, buflen);
//   if (cb == NULL)
//     return uv__random(buf, buflen);

//   uv__req_init(loop, req, UV_RANDOM);
//   req->loop = loop;
//   req->status = 0;
//   req->cb = cb;
//   req->buf = buf;
//   req->buflen = buflen;

//   uv__work_submit(loop,
//                   &req->work_req,
//                   UV__WORK_CPU,
//                   uv__random_work,
//                   uv__random_done);

//   return 0;
// }