#include <node.h>
#include <node_internals.h>
using namespace v8;

#include "runas.h"

namespace {

Handle<Value> Runas(const Arguments& args) {
  HandleScope scope;

  if (!args[0]->IsString() || !args[1]->IsArray() || !args[2]->IsObject())
    return node::ThrowError("Bad argument");

  std::string command(*String::Utf8Value(args[0]));
  std::vector<std::string> c_args;

  Handle<Array> v_args = Handle<Array>::Cast(args[1]);
  uint32_t length = v_args->Length();

  c_args.reserve(length);
  for (uint32_t i = 0; i < length; ++i) {
    std::string arg(*String::Utf8Value(v_args->Get(i)));
    c_args.push_back(arg);
  }

  Handle<Object> v_options = args[2]->ToObject();
  int options = runas::OPTION_NONE;
  if (v_options->Get(String::New("hide"))->BooleanValue())
    options |= runas::OPTION_HIDE;

  int code;
  if (!runas::Runas(command, c_args, options, &code))
    return node::ThrowError("Failed to call runas");

  return scope.Close(Integer::New(code));
}

void Init(Handle<Object> exports) {
  NODE_SET_METHOD(exports, "runas", Runas);
}

}  // namespace

NODE_MODULE(runas, Init)
