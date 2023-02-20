#include <napi-macros.h>
#include <node_api.h>
#include <uv.h>

#include <stdio.h>

static void
on_write (uv_write_t *req, int status) {
  printf("wrote!\n");
}

NAPI_METHOD(pipe_init) {
  NAPI_ARGV(1)

  uv_loop_t *loop;
  napi_get_uv_event_loop(env, &loop);

  NAPI_ARGV_BUFFER_CAST(uv_pipe_t *, handle, 0);
  uv_pipe_init(loop, handle, 0);

  return NULL;
}

NAPI_METHOD(pipe_open) {
  NAPI_ARGV(2)

  NAPI_ARGV_BUFFER_CAST(uv_pipe_t *, handle, 0);
  NAPI_ARGV_INT32(fd, 1);

  uv_pipe_open(handle, fd);

  return NULL;
}

NAPI_METHOD(pipe_write) {
  NAPI_ARGV(3)

  NAPI_ARGV_BUFFER_CAST(uv_write_t *, req, 0);
  NAPI_ARGV_BUFFER_CAST(uv_stream_t *, handle, 1);
  NAPI_ARGV_BUFFER(buf, 2);

  uv_buf_t bufs = {
    .base = buf,
    .len = buf_len
  };

  uv_write(req, handle, &bufs, 1, on_write);

  return NULL;
}
// NAPI_METHOD(pipe_bind) {
//   NAPI_ARGV(4)

//   uv_pipe_connect(req, p, name, on_connect)
// }

NAPI_INIT() {
  NAPI_EXPORT_FUNCTION(pipe_init)
  NAPI_EXPORT_FUNCTION(pipe_open)
  NAPI_EXPORT_FUNCTION(pipe_write)
}
