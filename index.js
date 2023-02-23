const { Duplex } = require('streamx')
const errors = require('uv-errors')
const binding = require('node-gyp-build')(__dirname)

const DEFAULT_READ_BUFFER = 65536

module.exports = class NativePipe extends Duplex {
  constructor (path, { readBufferSize = DEFAULT_READ_BUFFER, allowHalfOpen = true } = {}) {
    super({ mapWritable })

    const slab = Buffer.alloc(binding.sizeof_native_pipe_t + binding.sizeof_uv_write_t + readBufferSize)

    this._handle = slab.subarray(0, binding.sizeof_native_pipe_t)
    this._req = slab.subarray(binding.sizeof_native_pipe_t, binding.sizeof_native_pipe_t + binding.sizeof_uv_write_t)
    this._buffer = slab.subarray(binding.sizeof_native_pipe_t + binding.sizeof_uv_write_t)

    this._openCallback = null
    this._writeCallback = null
    this._finalCallback = null
    this._destroyCallback = null

    this._connected = 0 // unknown
    this._allowHalfOpen = allowHalfOpen

    binding.native_pipe_init(this._handle, this._buffer, this,
      this._onconnect,
      this._onwrite,
      this._onfinal,
      this._onread,
      this._onclose
    )

    if (typeof path === 'number') {
      binding.native_pipe_open(this._handle, path)
    } else {
      binding.native_pipe_connect(this._handle, path)
    }
  }

  _open (cb) {
    this._openCallback = cb
    this._continueOpen()
  }

  _read (cb) {
    binding.native_pipe_resume(this._handle)
    cb(null)
  }

  _writev (datas, cb) {
    this._writeCallback = cb
    binding.native_pipe_writev(this._req, this._handle, datas)
  }

  _final (cb) {
    this._finalCallback = cb
    binding.native_pipe_end(this._handle)
  }

  _destroy (cb) {
    this._destroyCallback = cb
    binding.native_pipe_close(this._handle)
  }

  _continueOpen () {
    if (this._connected === 0) return
    if (this._openCallback === null) return
    const cb = this._openCallback
    this._openCallback = null
    cb(this._connected < 0 ? makeError(this._connected) : null)
  }

  _continueWrite (err) {
    if (this._writeCallback === null) return
    const cb = this._writeCallback
    this._writeCallback = null
    cb(err)
  }

  _continueFinal (err) {
    if (this._finalCallback === null) return
    const cb = this._finalCallback
    this._finalCallback = null
    cb(err)
  }

  _continueDestroy () {
    if (this._destroyCallback === null) return
    const cb = this._destroyCallback
    this._destroyCallback = null
    cb(null)
  }

  _onconnect (status) {
    this._connected = status < 0 ? status : 1
    this._continueOpen()
  }

  _onwrite (status) {
    const err = status < 0 ? makeError(status) : null
    this._continueWrite(err)
  }

  _onread (read) {
    if (read === 0) {
      this.push(null)
      if (this._allowHalfOpen === false) this.end()
      return
    }

    if (read < 0) {
      this.destroy(makeError(read))
      return
    }

    const copy = Buffer.allocUnsafe(read)
    copy.set(this._buffer.subarray(0, read))

    if (this.push(copy) === false && this.destroying === false) {
      binding.native_pipe_pause(this._handle)
    }
  }

  _onfinal (status) {
    const err = status < 0 ? makeError(status) : null
    this._continueFinal(err)
  }

  _onclose () {
    this._handle = null
    this._continueDestroy()
  }
}

function mapWritable (buf) {
  return typeof buf === 'string' ? Buffer.from(buf) : buf
}

function makeError (errno) {
  const [code, msg] = errors.get(errno)
  const err = new Error(code + ': ' + msg)

  err.code = code
  err.errno = errno

  return err
}
