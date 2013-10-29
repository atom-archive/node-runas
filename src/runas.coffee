runas = require('bindings')('runas.node')

module.exports = (command, args=[]) -> runas.runas(command, args)
