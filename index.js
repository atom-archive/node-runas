const fs = require('fs')
const path = require('path')
const EventEmitter = require('events')
const binding = require('./build/Release/spawn_as_admin.node')

function resolveCommand (command) {
  if (command[0] === '/') return command

  const paths = process.env.PATH.split(path.delimiter)
  for (const p of paths) {
    try {
      const filename = path.join(p, command)
      if (fs.statSync(filename).isFile()) return filename
    } catch (e) {}
  }

  return command
}

class AdminProcess extends EventEmitter {
  constructor (pid, stdinFD, stdoutFD) {
    super()
    this.stdinFD = stdinFD
    this.stdoutFD = stdoutFD
    this.pid = pid
    this.stdin = stdinFD ? fs.createWriteStream(null, {fd: stdinFD}) : null
    this.stdout = stdoutFD ? fs.createReadStream(null, {fd: stdoutFD}) : null
    this.stderr = null
    this.stdio = [this.stdin, this.stdout, this.stderr]
  }
}

module.exports = function spawnAsAdmin (command, args = [], options = {}) {
  let result = null

  const spawnResult = binding.spawnAsAdmin(resolveCommand(command), args, (exitCode) => {
    result.emit('exit', exitCode)
  }, options && options.testMode)

  if (!spawnResult) {
    throw new Error(`Failed to obtain root priveleges to run ${command}`)
  }

  result = new AdminProcess(spawnResult.pid, spawnResult.stdin, spawnResult.stdout)
  return result
}
