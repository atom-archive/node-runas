const assert = require('assert')
const fs = require('fs')
const runAs = require('..')

suite('runAs', () => {
  if (process.platform === 'darwin') {
    test('returns a file descriptor on macOS if the async option is true', async function () {
      this.timeout(10000) // allow time to type password at the prompt

      const fd = runAs('/bin/cat', [], {
        admin: true,
        async: false // this is false so we can test on CI, you can set it to true to test locally
      })

      fs.write(fd, 'hello!')

      const buffer = new Buffer(6)
      await new Promise(resolve => {
        fs.read(fd, buffer, 0, 'hello!'.length, null, () => resolve())
      })
      assert.equal(buffer.toString(), 'hello!')
    })
  }
})
