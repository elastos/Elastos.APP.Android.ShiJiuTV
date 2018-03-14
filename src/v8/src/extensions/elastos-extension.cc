#include "src/extensions/elastos-extension.h"

#include "src/isolate.h"

namespace v8 {
namespace internal {

const char* const ElastosExtension::kSource =
    "native function elastosCreateObject();";

v8::Local<v8::FunctionTemplate> ElastosExtension::GetNativeFunctionTemplate(
    v8::Isolate* isolate, v8::Local<v8::String> str)
{
  DCHECK_EQ(strcmp(*v8::String::Utf8Value(isolate, str), "elastosCreateObject"), 0);
  return v8::FunctionTemplate::New(isolate, ElastosExtension::CreateObject);
}


void ElastosExtension::CreateObject(
    const v8::FunctionCallbackInfo<v8::Value>& args)
{
  base::OS::Print("%s begin\n", __PRETTY_FUNCTION__);

  auto v8_isolate = args.GetIsolate();

  auto v8_obj = v8::Object::New(v8_isolate);

  auto v8_obj_ptr = new v8::Global<v8::Object>(v8_isolate, v8_obj);

  v8_obj_ptr->SetWeak(v8_obj_ptr,
                      ElastosExtension::WeakCallback,
                      v8::WeakCallbackType::kParameter);
  v8_obj_ptr->MarkIndependent();

  args.GetReturnValue().Set(v8_obj_ptr->Get(v8_isolate));
  base::OS::Print("%s end\n", __PRETTY_FUNCTION__);
}

void ElastosExtension::WeakCallback(const WeakCallbackInfo<v8::Global<v8::Object>>& data)
{
  base::OS::Print("%s begin\n", __PRETTY_FUNCTION__);
  auto v8_obj_ptr = data.GetParameter();

  v8_obj_ptr->Reset();

  delete v8_obj_ptr;

  base::OS::Print("%s end\n", __PRETTY_FUNCTION__);
}

}  // namespace internal
}  // namespace v8
