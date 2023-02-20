const binding = require('node-gyp-build')(__dirname)
const fs = require('fs')

const handle = Buffer.alloc(65536)
const req = Buffer.alloc(65536)
const msg = Buffer.from('hello')

const fd = fs.openSync('\\\\.\\pipe\\server')

binding.pipe_init(handle)
binding.pipe_write(req, handle, msg)

// console.log(fd)

// binding.pipe_open(handle, 42)

// console.log(buf)
