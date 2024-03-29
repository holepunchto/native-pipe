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

## TODO

Pipe servers.

## License

Apache-2.0
