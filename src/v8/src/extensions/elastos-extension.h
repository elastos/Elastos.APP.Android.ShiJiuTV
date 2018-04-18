#ifndef V8_EXTENSIONS_ELASTOS_EXTENSION_H_
#define V8_EXTENSIONS_ELASTOS_EXTENSION_H_

#include "include/v8.h"
#include <map>

namespace v8 {
namespace internal {

class ElastosExtension : public v8::Extension {
 public:
  ElastosExtension() : v8::Extension("v8/elastos", kSource) {}
  virtual v8::Local<v8::FunctionTemplate> GetNativeFunctionTemplate(
      v8::Isolate* v8Isolate, v8::Local<v8::String> v8Name);

 private:
  struct WeakInfo {
    explicit WeakInfo(v8::Isolate* v8Isolate, v8::Local<v8::Object> obj, int type, void* addr);
    virtual ~WeakInfo();
    void Clear();

    v8::Global<v8::Object> gobj;
    int type;
    void* addr;

    static constexpr int kTypeModule = 1;
    static constexpr int kTypeClass = 2;
    static constexpr int kTypeInterface = 3;
  };

  struct ArgInfoCache {
    ArgInfoCache()
      : type(-1)
      , direction(-1)
      , value() {
    }

    int type;
    int direction;
    std::shared_ptr<void> value;
  };

  static void ThrowException(v8::Isolate* v8Isolate, const char* errmsg);

#if defined(__ANDROID__)
  static void AcquireModuleInfo(const v8::FunctionCallbackInfo<v8::Value>& v8Args);
  static void Release(const v8::FunctionCallbackInfo<v8::Value>& v8Args);

  static void ModuleInfoCallback(const v8::FunctionCallbackInfo<v8::Value>& v8Args);
  static void ModuleInfoGetClassInfo(v8::Isolate* v8Isolate, const v8::FunctionCallbackInfo<v8::Value>& v8Args);

  static void ClassInfoCallback(const v8::FunctionCallbackInfo<v8::Value>& v8Args);
  static void ClassInfoCreateObject(v8::Isolate* v8Isolate, const v8::FunctionCallbackInfo<v8::Value>& v8Args);

  static void MethodInfoCallback(const v8::FunctionCallbackInfo<v8::Value>& v8Args);

  static int CacheArgumentInfo(void* pElastosArgList,
                               int idx,
                               struct ArgInfoCache& cache);
  static int CacheArgument(void* pElastosArgList,
                           const v8::FunctionCallbackInfo<v8::Value>& v8Args,
                           int idx,
                           struct ArgInfoCache& cache);
  static int CacheInputArgument(void* pElastosArgList,
                                const v8::FunctionCallbackInfo<v8::Value>& v8Args,
                                int idx,
                                struct ArgInfoCache& cache);
  static int CacheCalleeAllocOutputArgument(void* pElastosArgList,
                                            const v8::FunctionCallbackInfo<v8::Value>& v8Args,
                                            int idx,
                                            struct ArgInfoCache& cache);
  static int CacheCallerAllocOutputArgument(void* pElastosArgList,
                                            const v8::FunctionCallbackInfo<v8::Value>& v8Args,
                                            int idx,
                                            struct ArgInfoCache& cache);
  static int RestoreArgument(const struct ArgInfoCache& cache, int idx, const v8::FunctionCallbackInfo<v8::Value>& v8Args);

  static WeakInfo* NewWeak(v8::Isolate* v8Isolate, v8::Local<v8::Object> obj, int type, void* addr);
  static void DeleteWeak(WeakInfo* weakInfo);
  static void WeakCallback(const WeakCallbackInfo<WeakInfo>& v8Data);
  static v8::Local<v8::String> MakeString(v8::Isolate* v8Isolate, const char* str);


  static constexpr int kFuncNameIdx = 0;
  static constexpr int kFuncPrivIdx = 1;
  static constexpr int kFuncOwnerIdx = 2;

#endif // defined(__ANDROID__)

  static const char* const kSource;
  static const char* const kNativeClassName;
  static const char* const kNativeAddress;
};

}  // namespace internal
}  // namespace v8

#endif  // V8_EXTENSIONS_ELASTOS_EXTENSION_H_
