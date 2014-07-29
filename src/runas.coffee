runas = require('../build/Release/runas.node')

searchCommand = (command) ->
  return command if command[0] is '/'

  fs = require('fs')
  path = require('path')
  paths = process.env.PATH.split(path.delimiter)
  for p in paths
    try
      filename = path.join(p, command)
      return filename if fs.statSync(filename).isFile()
    catch e
  ''

module.exports = (command, args=[], options={}) ->
  options.hide ?= true
  options.admin ?= false

  # Convert command to its full path when using authorization service
  if process.platform is 'darwin' and options.admin is true
    command = searchCommand(command)

  runas.runas(command, args, options)
