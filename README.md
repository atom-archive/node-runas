# Runas

Run command as administrator on Windows.

## Installing

```sh
npm install runas
```

## Building
  * Clone the repository
  * Run `npm install`
  * Run `grunt` to compile the native and CoffeeScript code
  * Run `grunt test` to run the specs

## Docs

```coffeescript
runas = require 'runas'
```

### runas(command, args[, options])

* `options` Object
  * `hide` Boolean - Hide the console window, `true` by default.
  * `admin` Boolean - Run command as administrator, `false` by default.
  * `stdin` String - String which would be passed as stdin input.

Launches a new process with the given `command`, with command line arguments in
`args`.

This function is synchronous and returns the exit code when the `command`
finished.

**Note:** The `hide` and `admin` options have only been implemented on Windows,
and the `stdin` option has only been implemented on POSIX systems.
