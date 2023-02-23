#include <napi-macros.h>
#include <node_api.h>
#include <uv.h>
#include <stdlib.h>

typedef struct {
  uv_pipe_t pipe;
  uv_connect_t conn;
  uv_buf_t read_buf;
  uv_shutdown_t end;
  napi_env env;
  napi_ref ctx;
  napi_ref on_connect;
  napi_ref on_write;
  napi_ref on_end;
  napi_ref on_read;
  napi_ref on_close;
} native_pipe_t;

static void
on_connect (uv_connect_t *req, int status) {
  native_pipe_t *self = (native_pipe_t *) req->data;
  napi_env env = self->env;

  napi_handle_scope scope;
  napi_open_handle_scope(env, &scope);

  napi_value ctx;
  napi_get_reference_value(env, self->ctx, &ctx);
  napi_value callback;
  napi_get_reference_value(env, self->on_connect, &callback);
  napi_value argv[1];
  napi_create_int32(env, status, &(argv[0]));

  if (napi_make_callback(env, NULL, ctx, callback, 1, argv, NULL) == napi_pending_exception) {
    napi_value fatal_exception;
    napi_get_and_clear_last_exception(env, &fatal_exception);
    napi_fatal_exception(env, fatal_exception);
  }

  napi_close_handle_scope(env, scope);
}

static void
on_write (uv_write_t *req, int status) {
  native_pipe_t *self = (native_pipe_t *) req->data;
  napi_env env = self->env;

  napi_handle_scope scope;
  napi_open_handle_scope(env, &scope);

  napi_value ctx;
  napi_get_reference_value(env, self->ctx, &ctx);
  napi_value callback;
  napi_get_reference_value(env, self->on_write, &callback);
  napi_value argv[1];
  napi_create_int32(env, status, &(argv[0]));

  if (napi_make_callback(env, NULL, ctx, callback, 1, argv, NULL) == napi_pending_exception) {
    napi_value fatal_exception;
    napi_get_and_clear_last_exception(env, &fatal_exception);
    napi_fatal_exception(env, fatal_exception);
  }

  napi_close_handle_scope(env, scope);
}

static void
on_shutdown (uv_shutdown_t *req, int status) {
  native_pipe_t *self = (native_pipe_t *) req->data;
  napi_env env = self->env;

  napi_handle_scope scope;
  napi_open_handle_scope(env, &scope);

  napi_value ctx;
  napi_get_reference_value(env, self->ctx, &ctx);
  napi_value callback;
  napi_get_reference_value(env, self->on_end, &callback);
  napi_value argv[1];
  napi_create_int32(env, status, &(argv[0]));

  if (napi_make_callback(env, NULL, ctx, callback, 1, argv, NULL) == napi_pending_exception) {
    napi_value fatal_exception;
    napi_get_and_clear_last_exception(env, &fatal_exception);
    napi_fatal_exception(env, fatal_exception);
  }

  napi_close_handle_scope(env, scope);
}

static void
on_read (uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
  if (nread == UV_EOF) nread = 0;
  else if (nread == 0) return;

  native_pipe_t *self = (native_pipe_t *) stream;
  napi_env env = self->env;

  napi_handle_scope scope;
  napi_open_handle_scope(env, &scope);

  napi_value ctx;
  napi_get_reference_value(env, self->ctx, &ctx);
  napi_value callback;
  napi_get_reference_value(env, self->on_read, &callback);
  napi_value argv[1];
  napi_create_int32(env, nread, &(argv[0]));

  if (napi_make_callback(env, NULL, ctx, callback, 1, argv, NULL) == napi_pending_exception) {
    napi_value fatal_exception;
    napi_get_and_clear_last_exception(env, &fatal_exception);
    napi_fatal_exception(env, fatal_exception);
  }

  napi_close_handle_scope(env, scope);
}

static void
on_close (uv_handle_t *handle) {
  native_pipe_t *self = (native_pipe_t *) handle;
  napi_env env = self->env;

  napi_handle_scope scope;
  napi_open_handle_scope(env, &scope);

  napi_value ctx;
  napi_get_reference_value(env, self->ctx, &ctx);
  napi_value callback;
  napi_get_reference_value(env, self->on_close, &callback);

  if (napi_make_callback(env, NULL, ctx, callback, 0, NULL, NULL) == napi_pending_exception) {
    napi_value fatal_exception;
    napi_get_and_clear_last_exception(env, &fatal_exception);
    napi_fatal_exception(env, fatal_exception);
  }

  napi_close_handle_scope(env, scope);
}

static void
on_alloc (uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
  native_pipe_t *self = (native_pipe_t *) handle;
  *buf = self->read_buf;
}

NAPI_METHOD(native_pipe_init) {
  NAPI_ARGV(8)

  uv_loop_t *loop;
  napi_get_uv_event_loop(env, &loop);

  NAPI_ARGV_BUFFER_CAST(native_pipe_t *, self, 0)
  uv_pipe_init(loop, &(self->pipe), 0);

  NAPI_ARGV_BUFFER(read_buf, 1)

  self->read_buf.base = read_buf;
  self->read_buf.len = read_buf_len;

  napi_create_reference(env, argv[2], 1, &(self->ctx));
  napi_create_reference(env, argv[3], 1, &(self->on_connect));
  napi_create_reference(env, argv[4], 1, &(self->on_write));
  napi_create_reference(env, argv[5], 1, &(self->on_end));
  napi_create_reference(env, argv[6], 1, &(self->on_read));
  napi_create_reference(env, argv[7], 1, &(self->on_close));

  self->env = env;

  return NULL;
}

NAPI_METHOD(native_pipe_connect) {
  NAPI_ARGV(2)

  NAPI_ARGV_BUFFER_CAST(native_pipe_t *, self, 0)
  NAPI_ARGV_UTF8(path, 4096, 1)

  uv_connect_t *conn = &(self->conn);

  conn->data = self;
  uv_pipe_connect(conn, (uv_pipe_t *) self, path, on_connect);

  return NULL;
}

NAPI_METHOD(native_pipe_open) {
  NAPI_ARGV(2)

  NAPI_ARGV_BUFFER_CAST(uv_pipe_t *, self, 0)
  NAPI_ARGV_UINT32(fd, 1)

  uv_pipe_open(self, fd);

  return NULL;
}

NAPI_METHOD(native_pipe_writev) {
  NAPI_ARGV(3)

  NAPI_ARGV_BUFFER_CAST(uv_write_t *, req, 0)
  NAPI_ARGV_BUFFER_CAST(uv_stream_t *, handle, 1)

  napi_value arr = argv[2];
  napi_value item;

  uint32_t nbufs;
  napi_get_array_length(env, arr, &nbufs);

  uv_buf_t *bufs = malloc(sizeof(uv_buf_t) * nbufs);

  for (uint32_t i = 0; i < nbufs; i++) {
    napi_get_element(env, arr, i, &item);
    uv_buf_t *buf = &(bufs[i]);
    napi_get_buffer_info(env, item, (void **) &(buf->base), &(buf->len));
  }

  req->data = handle;

  int err = uv_write(req, handle, bufs, nbufs, on_write);
  free(bufs);

  if (err < 0) {
    napi_throw_error(env, uv_err_name(err), uv_strerror(err));
    return NULL;
  }

  return NULL;
}

NAPI_METHOD(native_pipe_end) {
  NAPI_ARGV(1)
  NAPI_ARGV_BUFFER_CAST(native_pipe_t *, self, 0)

  uv_shutdown_t *req = &(self->end);
  int err;

  req->data = self;
  NAPI_UV_THROWS(err, uv_shutdown(req, (uv_stream_t *) self, on_shutdown))

  return NULL;
}

NAPI_METHOD(native_pipe_resume) {
  NAPI_ARGV(1)
  NAPI_ARGV_BUFFER_CAST(uv_stream_t *, handle, 0)

  uv_read_start(handle, on_alloc, on_read);

  return NULL;
}

NAPI_METHOD(native_pipe_pause) {
  NAPI_ARGV(1)
  NAPI_ARGV_BUFFER_CAST(uv_stream_t *, handle, 0)

  uv_read_stop(handle);

  return NULL;
}

NAPI_METHOD(native_pipe_close) {
  NAPI_ARGV(1)
  NAPI_ARGV_BUFFER_CAST(uv_handle_t *, handle, 0)

  uv_close(handle, on_close);

  return NULL;
}

NAPI_INIT() {
  NAPI_EXPORT_SIZEOF(native_pipe_t);
  NAPI_EXPORT_SIZEOF(uv_write_t)

  NAPI_EXPORT_FUNCTION(native_pipe_init)
  NAPI_EXPORT_FUNCTION(native_pipe_connect)
  NAPI_EXPORT_FUNCTION(native_pipe_writev)
  NAPI_EXPORT_FUNCTION(native_pipe_end)
  NAPI_EXPORT_FUNCTION(native_pipe_resume)
  NAPI_EXPORT_FUNCTION(native_pipe_pause)
  NAPI_EXPORT_FUNCTION(native_pipe_close)
}
