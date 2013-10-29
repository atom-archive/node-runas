runas = require('bindings')('runas.node')

module.exports = (command, args=[], options={}) ->
  options.hide ?= false

  runas.runas(command, args, options)
