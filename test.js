const test = require('brittle')
const net = require('net')
const NativePipe = require('./')

test('basic', function (t) {
  t.plan(1)

  const path = pipeName()

  const server = net.createServer(function (sock) {
    sock.pipe(sock)
  })

  server.listen(path, function () {
    const stream = new NativePipe(path)
    const b = []

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

function pipeName () {
  const name = 'native-pipe-' + Math.random().toString(16).slice(2) + Math.random().toString(16).slice(2)
  return process.platform === 'win32'
    ? '\\\\.\\pipe\\' + name
    : '/tmp/' + name + '.sock'
}
