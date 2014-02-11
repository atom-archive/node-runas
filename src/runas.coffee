runas = require('bindings')('runas.node')

module.exports = (command, args=[], options={}) ->
  options.hide ?= true
  options.admin ?= false

  runas.runas(command, args, options)
