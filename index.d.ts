interface Options {
  hide?: boolean
  admin?: boolean
  catchOutput?: boolean
  stdin?: string
}
interface CatchOutputOptions extends Options {
  catchOutput: true
}

interface CommandOutput {
  exitCode: number
  stdout?: string
  stderr?: string
}

declare function runas(command: string, args?: ReadonlyArray<string>, options?: Options): number
declare function runas(command: string, args: ReadonlyArray<string>, options: CatchOutputOptions): CommandOutput

export = runas
