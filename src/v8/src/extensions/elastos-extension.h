#ifndef V8_EXTENSIONS_ELASTOS_EXTENSION_H_
#define V8_EXTENSIONS_ELASTOS_EXTENSION_H_

#include "include/v8.h"
#include <list>

namespace v8 {
namespace internal {

class ElastosExtension : public v8::Extension {
 public:
  ElastosExtension() : v8::Extension("v8/elastos", kSource) {}
  virtual v8::Local<v8::FunctionTemplate> GetNativeFunctionTemplate(
      v8::Isolate* isolate, v8::Local<v8::String> name);
  static void CreateObject(const v8::FunctionCallbackInfo<v8::Value>& args);

 private:
  static void WeakCallback(const WeakCallbackInfo<v8::Global<v8::Object>>& data);

  static const char* const kSource;
};

}  // namespace internal
}  // namespace v8

#endif  // V8_EXTENSIONS_ELASTOS_EXTENSION_H_
