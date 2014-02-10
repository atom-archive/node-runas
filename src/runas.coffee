runas = require('bindings')('runas.node')

module.exports = (command, args=[], options={}) ->
  options.hide ?= false
  options.admin ?= true

  runas.runas(command, args, options)
