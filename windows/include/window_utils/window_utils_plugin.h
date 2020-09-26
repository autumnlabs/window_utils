#pragma once

#ifndef PLUGINS_EXAMPLE_window_utils_WINDOWS_H_
#define PLUGINS_EXAMPLE_window_utils_WINDOWS_H_

// This must be included before many other Windows headers.
#include <windows.h>

#include <VersionHelpers.h>
#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <map>
#include <memory>
#include <sstream>
#include <unordered_map>

#ifdef FLUTTER_PLUGIN_IMPL
#define FLUTTER_PLUGIN_EXPORT __declspec(dllexport)
#else
#define FLUTTER_PLUGIN_EXPORT __declspec(dllimport)
#endif

#if defined(__cplusplus)
extern "C"
{
#endif

    FLUTTER_PLUGIN_EXPORT void WindowUtilsPluginRegisterWithRegistrar(
        FlutterDesktopPluginRegistrarRef registrar);

#if defined(__cplusplus)
} // extern "C"
#endif

#endif // PLUGINS_EXAMPLE_window_utils_WINDOWS_H_

class WindowUtilsPlugin : public flutter::Plugin
{
    RECT premaxWindowRect;

public:
    static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

    // Creates a plugin that communicates on the given channel.
    WindowUtilsPlugin();

    virtual ~WindowUtilsPlugin();

private:
    // The MethodChannel used for communication with the Flutter engine.
    std::unique_ptr<flutter::MethodChannel<flutter::EncodableValue>> channel_;

    // Called when a method is called on |channel_|;
    void HandleMethodCall(
        const flutter::MethodCall<flutter::EncodableValue> &method_call,
        std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);

    // Returns whether or not the window is currently maximized.
    bool GetIsMaximized();

    // Maximize window.
    void MaximizeWindow();

    // Unmaximize window.
    void UnmaximizeWindow();
};
