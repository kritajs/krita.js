#pragma once

#include <Ultralight/CAPI.h>

// Returns a JSObjectRef for the libkis/Qt class named by `propertyName`. Returns a JS undefined value if class is not found.
JSValueRef getClassConstructor(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef *exception);
