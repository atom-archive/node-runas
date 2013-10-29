# Runas - Run command as administrator on Windows

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

### runas(command, [args])

Launches a new process with the given `command`, with command line arguments in
`args`. If omitted, `args` defaults to an empty Array.

This function is synchronous and returns the exit code when the `command`
finished.
