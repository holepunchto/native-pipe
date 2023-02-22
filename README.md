# native-pipe

Pipes that only depend on n-api.

```
npm install native-pipe
```

## Usage

``` js
const NativePipe = require('native-pipe')

const stream = new NativePipe('/tmp/server.sock') // or a \\pipe\... on windows

stream.write('hello world')

stream.on('data', function (data) {
  console.log(data)
})
```

## API

#### `stream = new NativePipe(pipeNameOrFd)`

Make a new pipe. Returns a duplex stream.
You can either pass the name of the pipe or an fd, like `0`, `1`, `2` for stdio.

See [streamx](https://github.com/mafintosh/streamx) for the full stream api.

#### `bytesRead = stream.readSync(buffer)`

Do synchrounous IO on the pipe. Only allowed in the first tick of the stream creation.
Useful if you use a pipe for setup of the program itself.

#### `bytesWritten = stream.writeSync(buffer)`

Do synchrounous IO on the pipe. Only allowed in the first tick of the stream creation.
Useful if you use a pipe for setup of the program itself.

## TODO

Pipe servers.

## License

MIT
