# spawn-as-admin

Run commands with administrator privileges

## Installing

```sh
npm install spawn-as-admin
```

## Docs

### spawnAsAdmin(command, arguments)

Launches a new process with the given `command`, and `arguments`.

Returns an object with the following properties:

* `stdin` - A `WritableStream` representing the process's standard input.
* `stdout` - A `ReadableStream` representing the process's standard output.

The returned object emits the following events:

* `exit` - Emitted when the process exits, and passes the exit code of the process.

## Limitations

* The library only works on macOS and Windows.
* The `stdin` and `stdout` properties are only present on macOS.
