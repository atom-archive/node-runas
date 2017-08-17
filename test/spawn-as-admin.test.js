const assert = require('assert')
const fs = require('fs')
const os = require('os')
const path = require('path')
const spawnAsAdmin = require('..')

const options = {
  // Comment out to test w/ actual admin privileges. This will require typing a username
  // and password during the tests.
  // admin: false
}

suite('spawnAsAdmin', function () {
  this.timeout(10000) // allow time to type password at the prompt

  test('runs the given command with the given arguments', async () => {
    const filePath = path.join(os.tmpdir(), 'spawn-as-admin-test-' + Date.now())

    const child = spawnAsAdmin(process.execPath, [
      '-e',
      `require("fs").writeFileSync("${filePath}", process.env.USER)`
    ], options)

    await new Promise(resolve => {
      child.on('end', (code) => {
        assert.equal(code, 0)

        const fileContent = fs.readFileSync(filePath, 'utf8').trim()

        if (options.admin === false) {
          assert.equal(fileContent, process.env.USER)
        } else {
          assert.equal(fileContent, 'root')
        }

        resolve()
      })
    })
  })

  if (process.platform === 'darwin') {

    test('returns a child process with a readable stdout and a writable stdin', async () => {
      const child = spawnAsAdmin('/bin/cat', [], options)

      let stdout = ''
      child.stdout.on('data', (data) => stdout += data.toString('utf8'))

      child.stdin.write('hello!');
      child.stdin.end();

      await new Promise(resolve => {
        child.on('end', (code) => {
          assert.equal(code, 0)
          assert.equal(stdout, 'hello!')
          resolve()
        })
      })
    })

    test('runs the child process as the root user', async () => {
      const child = spawnAsAdmin('whoami', [], options)

      let stdout = ''
      child.stdout.on('data', (data) => stdout += data.toString('utf8'))

      await new Promise(resolve => {
        child.on('end', (code) => {
          assert.equal(code, 0)

          if (options.admin === false) {
            assert.equal(stdout, process.env.USER + '\n')
          } else {
            assert.equal(stdout, 'root\n')
          }

          resolve()
        })
      })
    })

  }
})
