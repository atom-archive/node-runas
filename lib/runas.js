const runas = require('../build/Release/runas.node')
const fs = require('fs')
const path = require('path')

function searchCommand (command) {
  if (command[0] === '/') return command

  const paths = process.env.PATH.split(path.delimiter)
  for (const p of paths) {
    try {
      const filename = path.join(p, command)
      if (fs.statSync(filename).isFile()) return filename
    } catch (e) {}
  }
  return ''
}

module.exports =
function runAs (command, args = [], options = {}) {
  if (options.hide == null) options.hide = true
  if (options.admin == null) options.admin = false
  if (options.admin === true && process.platform === 'darwin') {
    command = searchCommand(command)
  }
  if (options.async === true && process.platform !== 'darwin') {
    throw new Error('The async option is only supported on macOS')
  }

  return runas.runas(command, args, options)
}
