#include "nan.h"
#include "spawn_as_admin.h"

namespace spawn_as_admin {

using namespace v8;

class Worker : public Nan::AsyncWorker {
  ChildProcess child_process;
  int exit_code;
  bool test_mode;

public:
  Worker(Nan::Callback *callback, ChildProcess child_process, bool test_mode) :
    Nan::AsyncWorker(callback),
    child_process(child_process),
    exit_code(-1),
    test_mode(test_mode) {}

  void Execute() {
    exit_code = spawn_as_admin::WaitForChildProcessToExit(&child_process, test_mode);
  }

  void HandleOKCallback() {
    Local<Value> argv[] = {Nan::New<Integer>(exit_code)};
    callback->Call(1, argv);
  }
};

void SpawnAsAdmin(const Nan::FunctionCallbackInfo<Value>& info) {
  if (!info[0]->IsString()) {
    Nan::ThrowTypeError("Command must be a string");
    return;
  }

  std::string command(*String::Utf8Value(info[0]));

  if (!info[1]->IsArray()) {
    Nan::ThrowTypeError("Arguments must be an array");
    return;
  }

  Local<Array> js_args = Local<Array>::Cast(info[1]);
  std::vector<std::string> args;
  args.reserve(js_args->Length());
  for (uint32_t i = 0; i < js_args->Length(); ++i) {
    Local<Value> js_arg = js_args->Get(i);
    if (!js_arg->IsString()) {
      Nan::ThrowTypeError("Arguments must be an array of strings");
      return;
    }

    args.push_back(std::string(*String::Utf8Value(js_arg)));
  }

  if (!info[2]->IsFunction()) {
    Nan::ThrowTypeError("Callback must be a function");
    return;
  }

  bool test_mode = false;
  if (info[3]->IsTrue()) test_mode = true;

  ChildProcess child_process = spawn_as_admin::StartChildProcess(command, args, test_mode);
  if (child_process.pid == -1) return;

  Local<Object> result = Nan::New<Object>();
  result->Set(Nan::New("pid").ToLocalChecked(), Nan::New<Integer>(child_process.pid));
  result->Set(Nan::New("stdin").ToLocalChecked(), Nan::New<Integer>(child_process.stdin_file_descriptor));
  result->Set(Nan::New("stdout").ToLocalChecked(), Nan::New<Integer>(child_process.stdout_file_descriptor));
  info.GetReturnValue().Set(result);

  Nan::AsyncQueueWorker(new Worker(new Nan::Callback(info[2].As<Function>()), child_process, test_mode));
}

void Init(Handle<Object> exports) {
  Nan::SetMethod(exports, "spawnAsAdmin", SpawnAsAdmin);
}

NODE_MODULE(spawn_as_admin, Init)

}  // namespace spawn_as_admin
