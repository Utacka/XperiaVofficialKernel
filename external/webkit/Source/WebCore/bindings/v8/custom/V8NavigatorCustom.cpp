/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 * Copyright (c) 2011, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "V8Navigator.h"

#if ENABLE(MEDIA_STREAM) || (PLATFORM(ANDROID) && ENABLE(APPLICATION_INSTALLED))

#include "Navigator.h"
#include "V8Binding.h"
#include "V8NavigatorUserMediaErrorCallback.h"
#include "V8NavigatorUserMediaSuccessCallback.h"
#include "V8Utilities.h"

#if PLATFORM(ANDROID) && ENABLE(APPLICATION_INSTALLED)
#include "ExceptionCode.h"
#include "V8CustomApplicationInstalledCallback.h"
#include "V8Proxy.h"
#endif

using namespace WTF;

#ifdef PROTEUS_DEVICE_API
#include "INodeProxy.h"
#include <dapi.h>
#endif

namespace WebCore {
#if ENABLE(MEDIA_STREAM) // ANDROID
v8::Handle<v8::Value> V8Navigator::webkitGetUserMediaCallback(const v8::Arguments& args)
{
    INC_STATS("DOM.Navigator.webkitGetUserMedia()");

    v8::TryCatch exceptionCatcher;
    String options = toWebCoreString(args[0]);
    if (exceptionCatcher.HasCaught())
        return throwError(exceptionCatcher.Exception());

    bool succeeded = false;

    RefPtr<NavigatorUserMediaSuccessCallback> successCallback = createFunctionOnlyCallback<V8NavigatorUserMediaSuccessCallback>(args[1], succeeded);
    if (!succeeded)
        return v8::Undefined();

    // Argument is optional, hence undefined is allowed.
    RefPtr<NavigatorUserMediaErrorCallback> errorCallback = createFunctionOnlyCallback<V8NavigatorUserMediaErrorCallback>(args[2], succeeded, CallbackAllowUndefined);
    if (!succeeded)
        return v8::Undefined();

    Navigator* navigator = V8Navigator::toNative(args.Holder());
    navigator->webkitGetUserMedia(options, successCallback.release(), errorCallback.release());
    return v8::Undefined();
}
#endif // ANDROID

#if PLATFORM(ANDROID) && ENABLE(APPLICATION_INSTALLED)
static PassRefPtr<ApplicationInstalledCallback> createApplicationInstalledCallback(
        v8::Local<v8::Value> value, bool& succeeded)
{
    succeeded = true;

    if (!value->IsFunction()) {
        succeeded = false;
        throwError("The second argument should be a function");
        return 0;
    }

    Frame* frame = V8Proxy::retrieveFrameForCurrentContext();
    return V8CustomApplicationInstalledCallback::create(value, frame);
}

v8::Handle<v8::Value> V8Navigator::isApplicationInstalledCallback(const v8::Arguments& args)
{
    INC_STATS("DOM.isApplicationInstalled()");
    bool succeeded = false;

    if (args.Length() < 2)
        return throwError("Two arguments required: an application name and a callback.", V8Proxy::SyntaxError);

    if (!args[0]->IsString())
        return throwError("The first argument should be a string.");

    RefPtr<ApplicationInstalledCallback> callback =
        createApplicationInstalledCallback(args[1], succeeded);
    if (!succeeded)
        return v8::Undefined();

    ASSERT(callback);

    Navigator* navigator = V8Navigator::toNative(args.Holder());
    if (!navigator->isApplicationInstalled(toWebCoreString(args[0]), callback.release()))
        return throwError(INVALID_STATE_ERR);

    return v8::Undefined();
}
#endif // PLATFORM(ANDROID) && ENABLE(APPLICATION_INSTALLED)

#ifdef PROTEUS_DEVICE_API
// window.navigator.loadModule(module, successcb, failurecb)
v8::Handle<v8::Value> V8Navigator::loadModuleCallback(const v8::Arguments& args) {
  v8::HandleScope scope;

  // loadModule takes a module name, a success callback and a optional errorcb
  if (args[0].IsEmpty() || args[1].IsEmpty() || !args[0]->IsString() || !args[1]->IsFunction()) {
    return v8::ThrowException(v8::String::New("Invalid arguments to loadModule"));
  }

  Navigator* navigator = V8Navigator::toNative(args.Holder());
  if (!navigator) {
    return v8::Undefined();
  }

  // node is loaded on demand (on the first loadModule) and  it should not
  // impact the webpage load if its not used

  // BUGFIX: crash with multiple loadModules (fs, camera) in same page
  // A webpage (frame) can have a single node proxy/node instance attached to it
  // If there's already a proxy, use it, otherwise create it
  INodeProxy* inodeProxy = navigator->inodeProxy() ?
    navigator->inodeProxy() : navigator->createINodeProxy();

  NODE_ASSERT(inodeProxy->inode());
  v8::Local<v8::Value> args_[] = { args[0], args[1], args[2] };

  dapi::INodeCore *core;
  inodeProxy->inode()->queryInterface(dapi::INTERFACE_CORE, (void **)&core);
  NODE_ASSERT(core);
  return core->loadModule(args_);
}
#endif

} // namespace WebCore

#endif // ENABLE(MEDIA_STREAM) || PLATFORM(ANDROID) && ENABLE(APPLICATION_INSTALLED)

