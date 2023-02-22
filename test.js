const test = require('brittle')
const net = require('net')
const { spawn } = require('child_process')
const NativePipe = require('./')

test('basic', function (t) {
  t.plan(1)

  const path = pipeName()

  const server = net.createServer(function (sock) {
    sock.pipe(sock)
  })

  server.listen(path, function () {
    const stream = new NativePipe(path)
    let b = []

    stream.write('hello')
    stream.write('world')
    stream.end()

    stream.on('data', function (data) {
      b.push(data)
    })

    stream.on('end', function () {
      t.alike(Buffer.concat(b), Buffer.from('helloworld'))
      server.close()
    })
  })
})

test('sync', function (t) {
  t.plan(4)

  const path = pipeName()

  const proc = spawn(process.execPath, ['-e', `
    const server = net.createServer(function (sock) {
      sock.pipe(sock)
    })

    server.listen(${JSON.stringify(path)}, () => console.log('ready'))
  `])

  proc.stdout.once('data', function () {
    const stream = new NativePipe(path)

    stream.writeSync('hello world')
    const small = Buffer.alloc(1)
    const big = Buffer.alloc(4096)

    t.is(stream.readSync(small), 1)
    t.alike(small.subarray(0, 1), Buffer.from('h'))

    t.is(stream.readSync(big), 10)
    t.alike(big.subarray(0, 10), Buffer.from('ello world'))

    proc.kill()
  })
})

function pipeName () {
  const name = 'native-pipe-' + Math.random().toString(16).slice(2) + Math.random().toString(16).slice(2)
  return process.platform === 'win32'
    ? '\\\\.\\pipe\\' + name
    : '/tmp/' + name + '.sock'
}
