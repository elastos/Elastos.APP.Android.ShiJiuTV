#include "src/extensions/elastos-extension.h"

#include <map>
#include "src/isolate.h"

#if defined(__ANDROID__)
#undef _DEBUG
#include "src/third_party/elastos/arm/inc/elastos.h"
#endif // defined(__ANDROID__)

namespace v8 {
namespace internal {

const char* const ElastosExtension::kSource =
    "native function elastosAcquireModuleInfo();"
    "native function elastosRelease();";
const char* const ElastosExtension::kNativeClassName = "nativeClassName";
const char* const ElastosExtension::kNativeAddress = "nativeAddress";

v8::Local<v8::FunctionTemplate> ElastosExtension::GetNativeFunctionTemplate(
    v8::Isolate* v8Isolate, v8::Local<v8::String> v8Str)
{
  std::string funcName(*v8::String::Utf8Value(v8Str));

#if defined(__ANDROID__)
  if (funcName == "elastosAcquireModuleInfo") {
    return v8::FunctionTemplate::New(v8Isolate, ElastosExtension::AcquireModuleInfo);
  } else if (funcName == "elastosRelease") {
    return v8::FunctionTemplate::New(v8Isolate, ElastosExtension::Release);
  }
#endif // defined(__ANDROID__)

  std::ostringstream errmsg;
  errmsg << "Error: Failed to get native function template: ";
  errmsg << funcName;
  ThrowException(v8Isolate, errmsg.str().c_str());
  return v8::Local<v8::FunctionTemplate>();
}

void ElastosExtension::ThrowException(v8::Isolate* v8Isolate, const char* errmsg)
{
  v8::Local<v8::String> v8ErrMsg = v8::String::NewFromUtf8(v8Isolate, errmsg, v8::NewStringType::kNormal).ToLocalChecked();
  v8Isolate->ThrowException(v8ErrMsg);
  base::OS::PrintError("%s", errmsg);
}

#if defined(__ANDROID__)

void ElastosExtension::AcquireModuleInfo(const v8::FunctionCallbackInfo<v8::Value>& v8Args)
{
  v8::Isolate* v8Isolate = v8Args.GetIsolate();
  v8::HandleScope v8HandleScope(v8Isolate);

  if (v8Args.Length() != 2) {
    const char* errmsg = "TypeError: Failed to execute 'elastosAcquireModuleInfo': 2 argument required.";
    ThrowException(v8Isolate, errmsg);
    return;
  }
  if (v8Args[0]->IsString() == false) {
    const char* errmsg = "TypeError: Failed to execute 'elastosAcquireModuleInfo': string argument required for first.";
    ThrowException(v8Isolate, errmsg);
    return;
  }
  if (v8Args[1]->IsObject() == false) {
    const char* errmsg = "TypeError: Failed to execute 'elastosAcquireModuleInfo': object argument required for second.";
    ThrowException(v8Isolate, errmsg);
    return;
  }

  v8::String::Utf8Value v8LibName(v8Args[0]);
  v8::Local<v8::Object> v8MdlInfo = v8::Local<v8::Object>::Cast(v8Args[1]);

  IModuleInfo* pMdlInfo = nullptr;
  CARAPI ret = Elastos::CReflector::AcquireModuleInfo(Elastos::String(*v8LibName), &pMdlInfo);
  if (ret != NOERROR) {
    base::OS::PrintError("Error: Failed to exec Elastos::CReflector::AcquireModuleInfo(): %s. ECode=0x%08x\n", *v8LibName, ret);
    v8Args.GetReturnValue().Set(v8::Integer::New(v8Isolate, ret));
    return;
  }
  WeakInfo* pWeakInfo = NewWeak(v8Isolate, v8MdlInfo, WeakInfo::kTypeModule, pMdlInfo);

  v8MdlInfo->Set(MakeString(v8Isolate, kNativeClassName), MakeString(v8Isolate, "IModuleInfo"));
  v8MdlInfo->Set(MakeString(v8Isolate, kNativeAddress), v8::External::New(v8Isolate, pWeakInfo));

  auto v8MdlInfoData = v8::Object::New(v8Isolate);
  v8MdlInfoData->Set(kFuncNameIdx, MakeString(v8Isolate, "GetClassInfo"));
  v8MdlInfoData->Set(kFuncPrivIdx, v8::External::New(v8Isolate, pMdlInfo));
  auto v8MdlInfoCallbackTemplate = v8::FunctionTemplate::New(v8Isolate, ElastosExtension::ModuleInfoCallback, v8MdlInfoData);
  v8MdlInfo->Set(MakeString(v8Isolate, "GetClassInfo"), v8MdlInfoCallbackTemplate->GetFunction());

  base::OS::Print("Success to load library: %s. IModuleInfo=%p\n", *v8LibName, pMdlInfo);
  v8Args.GetReturnValue().Set(v8::Integer::New(v8Isolate, ret));
}

void ElastosExtension::Release(const v8::FunctionCallbackInfo<v8::Value>& v8Args)
{
  v8::Isolate* v8Isolate = v8Args.GetIsolate();
  v8::HandleScope v8HandleScope(v8Isolate);

  if (v8Args.Length() != 1) {
    const char* errmsg = "TypeError: Failed to execute 'elastosRelease': 1 argument required.";
    ThrowException(v8Isolate, errmsg);
    return;
  }
  if (v8Args[0]->IsObject() == false) {
    const char* errmsg = "TypeError: Failed to execute 'elastosRelease': object argument required.";
    ThrowException(v8Isolate, errmsg);
    return;
  }

  v8::Local<v8::Object> v8Object = v8::Local<v8::Object>::Cast(v8Args[0]);
  auto v8WeakInfo = v8Object->Get(MakeString(v8Isolate, kNativeAddress));
  WeakInfo* pWeakInfo = static_cast<WeakInfo*>(v8::Local<v8::External>::Cast(v8WeakInfo)->Value());

  DeleteWeak(pWeakInfo);

  v8Object->Set(MakeString(v8Isolate, kNativeAddress), v8::External::New(v8Isolate, nullptr));

  base::OS::Print("Success to release \n");
}

void ElastosExtension::ModuleInfoCallback(const v8::FunctionCallbackInfo<v8::Value>& v8Args)
{
  v8::Isolate* v8Isolate = v8Args.GetIsolate();
  v8::HandleScope v8HandleScope(v8Isolate);

  auto v8MdlInfoData = v8::Local<v8::Object>::Cast(v8Args.Data());
  v8::String::Utf8Value v8FuncName(v8MdlInfoData->Get(kFuncNameIdx));

  std::string funcName(*v8FuncName);
  if(funcName == "GetClassInfo") {
    ModuleInfoGetClassInfo(v8Isolate, v8Args);
  } else {
    base::OS::PrintError("Error: Function %s() not found.\n", funcName.c_str());
  }
}

void ElastosExtension::ModuleInfoGetClassInfo(v8::Isolate* v8Isolate, const v8::FunctionCallbackInfo<v8::Value>& v8Args)
{
  if (v8Args.Length() != 2) {
    const char* errmsg = "TypeError: Failed to execute 'GetClassInfo': 2 argument required.";
    ThrowException(v8Isolate, errmsg);
    return;
  }
  if (v8Args[0]->IsString() == false) {
    const char* errmsg = "TypeError: Failed to execute 'GetClassInfo': string argument required for first.";
    ThrowException(v8Isolate, errmsg);
    return;
  }
  if (v8Args[1]->IsObject() == false) {
    const char* errmsg = "TypeError: Failed to execute 'GetClassInfo': object argument required for second.";
    ThrowException(v8Isolate, errmsg);
    return;
  }

  auto v8MdlInfoData = v8::Local<v8::Object>::Cast(v8Args.Data());
  auto v8MdlInfoPriv = v8::Local<v8::External>::Cast(v8MdlInfoData->Get(kFuncPrivIdx));
  v8::String::Utf8Value v8ClassName(v8Args[0]);
  v8::Local<v8::Object> v8ClassInfo = v8::Local<v8::Object>::Cast(v8Args[1]);

  IModuleInfo* pMdlInfo = static_cast<IModuleInfo*>(v8MdlInfoPriv->Value());
  IClassInfo *pClsInfo = nullptr;
  CARAPI ret = pMdlInfo->GetClassInfo(Elastos::String(*v8ClassName), &pClsInfo);
  if (ret != NOERROR) {
    base::OS::PrintError("Error: Failed to exec Elastos::IModuleInfo::GetClassInfo(): %s. ECode=0x%08x\n", *v8ClassName, ret);
    v8Args.GetReturnValue().Set(v8::Integer::New(v8Isolate, ret));
    return;
  }
  WeakInfo* pWeakInfo = NewWeak(v8Isolate, v8ClassInfo, WeakInfo::kTypeClass, pClsInfo);

  v8ClassInfo->Set(MakeString(v8Isolate, kNativeClassName), MakeString(v8Isolate, "IClassInfo"));
  v8ClassInfo->Set(MakeString(v8Isolate, kNativeAddress), v8::External::New(v8Isolate, pWeakInfo));

  auto v8ClsInfoData = v8::Object::New(v8Isolate);
  v8ClsInfoData->Set(kFuncNameIdx, MakeString(v8Isolate, "CreateObject"));
  v8ClsInfoData->Set(kFuncPrivIdx, v8::External::New(v8Isolate, pClsInfo));
  auto v8ClsInfoCallbackTemplate = v8::FunctionTemplate::New(v8Isolate, ElastosExtension::ClassInfoCallback, v8ClsInfoData);
  v8ClassInfo->Set(MakeString(v8Isolate, "CreateObject"), v8ClsInfoCallbackTemplate->GetFunction());

  //SetWeak(v8Isolate, v8ClassInfo);

  base::OS::Print("Success to exec Elastos::IModuleInfo::GetClassInfo(): %s.\n", *v8ClassName);
  v8Args.GetReturnValue().Set(v8::Integer::New(v8Isolate, ret));
}

void ElastosExtension::ClassInfoCallback(const v8::FunctionCallbackInfo<v8::Value>& v8Args)
{
  v8::Isolate* v8Isolate = v8Args.GetIsolate();
  v8::HandleScope v8HandleScope(v8Isolate);

  auto v8ClsInfoData = v8::Local<v8::Object>::Cast(v8Args.Data());
  v8::String::Utf8Value v8FuncName(v8ClsInfoData->Get(kFuncNameIdx));

  std::string funcName(*v8FuncName);
  if(funcName == "CreateObject") {
    ClassInfoCreateObject(v8Isolate, v8Args);
  } else {
    base::OS::PrintError("Error: Function %s() not found.\n", funcName.c_str());
  }
}

void ElastosExtension::ClassInfoCreateObject(v8::Isolate* v8Isolate, const v8::FunctionCallbackInfo<v8::Value>& v8Args)
{
  if (v8Args.Length() != 1) {
    const char* errmsg = "TypeError: Failed to execute 'CreateObject': 1 argument required.";
    ThrowException(v8Isolate, errmsg);
    return;
  }
  if (v8Args[0]->IsObject() == false) {
    const char* errmsg = "TypeError: Failed to execute 'CreateObject': object argument required.";
    ThrowException(v8Isolate, errmsg);
    return;
  }

  auto v8ClsInfoData = v8::Local<v8::Object>::Cast(v8Args.Data());
  auto v8ClsInfoPriv = v8::Local<v8::External>::Cast(v8ClsInfoData->Get(kFuncPrivIdx));
  v8::Local<v8::Object> v8Interface = v8::Local<v8::Object>::Cast(v8Args[0]);

  IClassInfo* pClsInfo = static_cast<IClassInfo*>(v8ClsInfoPriv->Value());
  IInterface* pInterface = nullptr;
  CARAPI ret = pClsInfo->CreateObject(&pInterface);
  if (ret != NOERROR) {
    base::OS::PrintError("Error: Failed to exec Elastos::IClassInfo::CreateObject(). ECode=0x%08x\n", ret);
    v8Args.GetReturnValue().Set(v8::Integer::New(v8Isolate, ret));
    return;
  }

  Elastos::Int32 nMtdCnt;
  ret = pClsInfo->GetMethodCount(&nMtdCnt);
  if (ret != NOERROR) {
    base::OS::PrintError("Error: Failed to exec Elastos::IClassInfo::GetMethodCount(). ECode=0x%08x\n", ret);
    v8Args.GetReturnValue().Set(v8::Integer::New(v8Isolate, ret));
    return;
  }
  Elastos::ArrayOf<IMethodInfo*> *pMtdInfoArray = Elastos::ArrayOf<IMethodInfo*>::Alloc(nMtdCnt);
  ret = pClsInfo->GetAllMethodInfos(pMtdInfoArray);
  if (ret != NOERROR) {
    base::OS::PrintError("Error: Failed to exec Elastos::IClassInfo::GetAllMethodInfos(). ECode=0x%08x\n", ret);
    v8Args.GetReturnValue().Set(v8::Integer::New(v8Isolate, ret));
    return;
  }
  WeakInfo* pWeakInfo = NewWeak(v8Isolate, v8Interface, WeakInfo::kTypeInterface, pInterface);

  v8Interface->Set(MakeString(v8Isolate, kNativeClassName), MakeString(v8Isolate, "IInterface"));
  v8Interface->Set(MakeString(v8Isolate, kNativeAddress), v8::External::New(v8Isolate, pWeakInfo));

  for(int midx = 0; midx < nMtdCnt; midx++) {
    IMethodInfo *pMtdInfo = (*pMtdInfoArray)[midx];

    Elastos::String szMtdName;
    ret = pMtdInfo->GetName(&szMtdName);
    if (ret != NOERROR) {
      base::OS::PrintError("Error: Failed to exec Elastos::IMethodInfo::GetName(). ECode=0x%08x\n", ret);
      break;
    }

    auto v8MtdInfoData = v8::Object::New(v8Isolate);
    v8MtdInfoData->Set(kFuncNameIdx, MakeString(v8Isolate, szMtdName.string()));
    v8MtdInfoData->Set(kFuncPrivIdx, v8::External::New(v8Isolate, pMtdInfo));
    v8MtdInfoData->Set(kFuncOwnerIdx, v8::External::New(v8Isolate, pInterface));
    auto v8MtdInfoCallbackTemplate = v8::FunctionTemplate::New(v8Isolate, ElastosExtension::MethodInfoCallback, v8MtdInfoData);
    v8Interface->Set(MakeString(v8Isolate, szMtdName.string()), v8MtdInfoCallbackTemplate->GetFunction());
  }
  Elastos::ArrayOf<IMethodInfo*>::Free(pMtdInfoArray);
  if (ret != NOERROR) {
    v8Args.GetReturnValue().Set(v8::Integer::New(v8Isolate, ret));
    return;
  }

  //SetWeak(v8Isolate, v8Interface);

  base::OS::Print("Success to exec Elastos::IClassInfo::CreateObject().\n");
  v8Args.GetReturnValue().Set(v8::Integer::New(v8Isolate, ret));
}

void ElastosExtension::MethodInfoCallback(const v8::FunctionCallbackInfo<v8::Value>& v8Args)
{
  v8::Isolate* v8Isolate = v8Args.GetIsolate();
  v8::HandleScope v8HandleScope(v8Isolate);

  auto v8MtdInfoData = v8::Local<v8::Object>::Cast(v8Args.Data());
  v8::String::Utf8Value v8FuncName(v8MtdInfoData->Get(kFuncNameIdx));
  auto v8MtdInfoPriv = v8::Local<v8::External>::Cast(v8MtdInfoData->Get(kFuncPrivIdx));
  //auto v8Interface = v8::Local<v8::External>::Cast(v8MtdInfoData->Get(kFuncOwnerIdx)); // TODO

  IMethodInfo* pMtdInfo = static_cast<IMethodInfo*>(v8MtdInfoPriv->Value());
  //IInterface* pInterface = static_cast<IInterface*>(v8Interface->Value()); // TODO

  Elastos::Int32 nArgCnt;
  CARAPI ret = pMtdInfo->GetParamCount(&nArgCnt);
  if (ret != NOERROR) {
    base::OS::PrintError("Error: Failed to exec Elastos::IMethodInfo::GetParamCount(). ECode=0x%08x\n", ret);
    v8Args.GetReturnValue().Set(v8::Integer::New(v8Isolate, ret));
    return;
  }
  if (v8Args.Length() != nArgCnt) {
    std::ostringstream errmsg;
    errmsg << "TypeError: Failed to execute '" << *v8FuncName << "': ";
    errmsg << nArgCnt << " argument required.";
    ThrowException(v8Isolate, errmsg.str().c_str());
    return;
  }

  IArgumentList *pArgList = nullptr;
  ret = pMtdInfo->CreateArgumentList(&pArgList);
  if (ret != NOERROR) {
    base::OS::PrintError("Error: Failed to exec Elastos::IMethodInfo::CreateArgumentList(). ECode=0x%08x\n", ret);
    v8Args.GetReturnValue().Set(v8::Integer::New(v8Isolate, ret));
    return;
  }

  struct ArgInfoCache cache[nArgCnt];
  for(int pidx = 0; pidx < nArgCnt; pidx++) {
    ret = CacheArgument(pArgList, v8Args, pidx, cache[pidx]);
    if (ret != NOERROR) {
      break;
    }
  }
  if (ret != NOERROR) {
    v8Args.GetReturnValue().Set(v8::Integer::New(v8Isolate, ret));
    return;
  }

  //ret = pMtdInfo->Invoke(pInterface, pArgList); // TODO
  if(std::string(*v8FuncName) == "Hello") {
    cache[0].value = std::make_shared<Elastos::String>("Elastos Demo Test");
  }
  if (ret != NOERROR) {
    base::OS::PrintError("Error: Failed to exec Elastos::IMethodInfo::Invoke(). ECode=0x%08x\n", ret);
    v8Args.GetReturnValue().Set(v8::Integer::New(v8Isolate, ret));
    return;
  }

  for(int pidx = 0; pidx < nArgCnt; pidx++) {
    ret = RestoreArgument(cache[pidx], pidx, v8Args);
    if (ret != NOERROR) {
      break;
    }
  }
  if (ret != NOERROR) {
    v8Args.GetReturnValue().Set(v8::Integer::New(v8Isolate, ret));
    return;
  }

  base::OS::Print("Success to exec Elastos::IMethodInfo::Invoke().\n");
  v8Args.GetReturnValue().Set(v8::Integer::New(v8Isolate, ret));
}

int ElastosExtension::CacheArgument(void* pElastosArgList,
                                    const v8::FunctionCallbackInfo<v8::Value>& v8Args,
                                    int idx,
                                    struct ArgInfoCache& cache)
{
  IArgumentList* pArgList = static_cast<IArgumentList*>(pElastosArgList);

  IFunctionInfo* pFuncInfo = nullptr;
  CARAPI ret = pArgList->GetFunctionInfo(&pFuncInfo);
  if (ret != NOERROR) {
    base::OS::PrintError("Error: Failed to exec Elastos::IArgumentList::GetFunctionInfo(). ECode=0x%08x\n", ret);
    return ret;
  }

  IParamInfo* pParamInfo = nullptr;
  ret = pFuncInfo->GetParamInfoByIndex(idx, &pParamInfo);
  if (ret != NOERROR) {
    base::OS::PrintError("Error: Failed to exec Elastos::IFunctionInfo::GetParamInfoByIndex(). ECode=0x%08x\n", ret);
    return ret;
  }

  ParamIOAttribute ioAttr;
  ret = pParamInfo->GetIOAttribute(&ioAttr);
  if (ret != NOERROR) {
    base::OS::PrintError("Error: Failed to exec Elastos::IParamInfo::GetIOAttribute(). ECode=0x%08x\n", ret);
    return ret;
  }
  cache.direction = ioAttr;

  IDataTypeInfo* pTypeInfo = nullptr;
  ret = pParamInfo->GetTypeInfo(&pTypeInfo);
  if (ret != NOERROR) {
    base::OS::PrintError("Error: Failed to exec Elastos::IParamInfo::GetTypeInfo(). ECode=0x%08x\n", ret);
    return ret;
  }

  CarDataType nType = 0;
  ret = pTypeInfo->GetDataType(&nType);
  if (ret != NOERROR) {
    base::OS::PrintError("Error: Failed to exec Elastos::IDataTypeInfo::GetDataType(). ECode=0x%08x\n", ret);
    return ret;
  }
  cache.type = nType;

  if(ioAttr == ParamIOAttribute_In) {
    base::OS::PrintError("Error: TODO %s:%d\n", __FILE__, __LINE__);
    return E_NOT_IMPLEMENTED;
  } else {
    if(nType == CarDataType_String) {
      cache.value = std::make_shared<Elastos::String>();
      ret = pArgList->SetOutputArgumentOfStringPtr(0, static_cast<Elastos::String*>(cache.value.get()));
      if (ret != NOERROR) {
        base::OS::PrintError("Error: Failed to exec Elastos::IArgumentList::SetOutputArgumentOfStringPtr(). ECode=0x%08x\n", ret);
        return ret;
      }
    } else {
      base::OS::PrintError("Error: TODO %s:%d\n", __FILE__, __LINE__);
      return E_NOT_IMPLEMENTED;
    }
  }

  return NOERROR;
}

int ElastosExtension::RestoreArgument(const struct ArgInfoCache& cache,
                                      int idx,
                                      const v8::FunctionCallbackInfo<v8::Value>& v8Args)
{
  if(cache.direction == ParamIOAttribute_In) {
    return NOERROR;
  }

  v8::Isolate* v8Isolate = v8Args.GetIsolate();

  if(v8Args[idx]->IsObject() == false) {
    auto v8MtdInfoData = v8::Local<v8::Object>::Cast(v8Args.Data());
    v8::String::Utf8Value v8FuncName(v8MtdInfoData->Get(kFuncNameIdx));
    std::ostringstream errmsg;
    errmsg << "TypeError: Failed to execute '" << *v8FuncName << "': ";
    errmsg << "object argument required for index " << idx;
    ThrowException(v8Isolate, errmsg.str().c_str());
    return E_INVALID_ARGUMENT;
  }

  v8::Local<v8::Object> v8Value = v8::Local<v8::Object>::Cast(v8Args[idx]);

  if(cache.type == CarDataType_String) {
    Elastos::String* value = static_cast<Elastos::String*>(cache.value.get());
    v8Value->Set(MakeString(v8Isolate, "value"),
                 MakeString(v8Isolate, value->string()));
  } else {
    base::OS::PrintError("Error: TODO %s:%d\n", __FILE__, __LINE__);
    return E_NOT_IMPLEMENTED;
  }

  return NOERROR;
}

ElastosExtension::WeakInfo* ElastosExtension::NewWeak(v8::Isolate* v8Isolate, v8::Local<v8::Object> obj, int type, void* addr)
{
  WeakInfo* pWeakInfo = new WeakInfo(v8Isolate, obj, type, addr);

  pWeakInfo->gobj.SetWeak(pWeakInfo,
                          ElastosExtension::WeakCallback,
                          v8::WeakCallbackType::kParameter);
  pWeakInfo->gobj.MarkIndependent();

  return pWeakInfo;
}

void ElastosExtension::DeleteWeak(WeakInfo* weakInfo)
{
  if(weakInfo == nullptr) {
    return;
  }

  delete weakInfo;
}

void ElastosExtension::WeakCallback(const WeakCallbackInfo<WeakInfo>& v8Data)
{
  base::OS::Print("%s %d", __PRETTY_FUNCTION__, __LINE__);

  auto weakInfo = v8Data.GetParameter();

  DeleteWeak(weakInfo);
}

v8::Local<v8::String> ElastosExtension::MakeString(v8::Isolate* v8Isolate, const char* str)
{
  v8::Local<v8::String> v8Str = v8::String::NewFromUtf8(v8Isolate, str, v8::NewStringType::kNormal).ToLocalChecked();

  return v8Str;
}

ElastosExtension::WeakInfo::WeakInfo(v8::Isolate* v8Isolate, v8::Local<v8::Object> obj, int type, void* addr)
    : gobj(v8Isolate, obj)
    , type(type)
    , addr(addr)
{
  base::OS::Print("%s type=%d addr=%p", __PRETTY_FUNCTION__, type, addr);
}

ElastosExtension::WeakInfo::~WeakInfo()
{
  base::OS::Print("%s type=%d addr=%p", __PRETTY_FUNCTION__, type, addr);
  Clear();
}

void ElastosExtension::WeakInfo::Clear()
{
  if(addr == nullptr) {
    return;
  }

  if(type == kTypeModule) {
    IModuleInfo* pMdlInfo = static_cast<IModuleInfo*>(addr);
    pMdlInfo->Release();
  } else if(type == kTypeClass) {
    IClassInfo* pClsInfo = static_cast<IClassInfo*>(addr);
    pClsInfo->Release();
  } else if(type == kTypeInterface) {
    IInterface* pInterface = static_cast<IInterface*>(addr);
    pInterface->Release();
  } else {
    base::OS::PrintError("%s Unknown type: %d", __PRETTY_FUNCTION__, type);
  }

  type = -1;
  addr = nullptr;
  gobj.Reset();
}

#endif // defined(__ANDROID__)

}  // namespace internal
}  // namespace v8
