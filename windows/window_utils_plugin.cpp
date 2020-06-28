#include "window_utils_plugin.h"

#include <windows.h>

#include <VersionHelpers.h>
#include <flutter/method_channel.h>
#include <flutter/plugin_registrar.h>
#include <flutter/standard_method_codec.h>
#include <memory>
#include <sstream>
#include <unordered_map>

namespace
{

class WindowUtilsPlugin : public flutter::Plugin
{
  RECT normalRect;

public:
  static void RegisterWithRegistrar(flutter::PluginRegistrar *registrar);

  // Creates a plugin that communicates on the given channel.
  WindowUtilsPlugin(
      std::unique_ptr<flutter::MethodChannel<flutter::EncodableValue>> channel);

  virtual ~WindowUtilsPlugin();

private:
  // Called when a method is called on |channel_|;
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);

  // The MethodChannel used for communication with the Flutter engine.
  std::unique_ptr<flutter::MethodChannel<flutter::EncodableValue>> channel_;
};

// static
void WindowUtilsPlugin::RegisterWithRegistrar(flutter::PluginRegistrar *registrar)
{
  auto channel =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          registrar->messenger(), "window_utils",
          &flutter::StandardMethodCodec::GetInstance());
  auto *channel_pointer = channel.get();

  auto plugin = std::make_unique<WindowUtilsPlugin>(std::move(channel));

  channel_pointer->SetMethodCallHandler(
      [plugin_pointer = plugin.get()](const auto &call, auto result) {
        plugin_pointer->HandleMethodCall(call, std::move(result));
      });

  registrar->AddPlugin(std::move(plugin));
}

WindowUtilsPlugin::WindowUtilsPlugin(
    std::unique_ptr<flutter::MethodChannel<flutter::EncodableValue>> channel)
    : channel_(std::move(channel)) {}

WindowUtilsPlugin::~WindowUtilsPlugin(){};

void WindowUtilsPlugin::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result)
{
  std::string method = method_call.method_name();

  if (method.compare("hideTitleBar") == 0)
  {
    HWND hWnd = GetActiveWindow();
    SetMenu(hWnd, NULL);
    LONG lStyle = GetWindowLong(hWnd, GWL_STYLE);
    // lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
    // lStyle &= WS_DLGFRAME;
    lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU | WS_DLGFRAME);
    SetWindowLong(hWnd, GWL_STYLE, lStyle);
    LONG flags = SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER;
    SetWindowPos(hWnd, NULL, 0, 0, 0, 0, flags);
    flutter::EncodableValue response(true);
    result->Success(&response);
  }
  else if (method.compare("showTitleBar") == 0)
  {
    HWND hWnd = GetActiveWindow();
    SetMenu(hWnd, NULL);
    // SetWindowLong(hWnd,GWL_STYLE,WS_EX_LAYERED);
    flutter::EncodableValue response(true);
    result->Success(&response);
  }
  else if (method.compare("getScreenSize") == 0)
  {
    flutter::EncodableMap umap;
    HWND hWnd = GetDesktopWindow();
    RECT rect;
    if (GetWindowRect(hWnd, &rect))
    {
      double width = rect.right;
      double height = rect.bottom;
      umap[flutter::EncodableValue("width")] = flutter::EncodableValue(width);
      umap[flutter::EncodableValue("height")] = flutter::EncodableValue(height);
    }
    flutter::EncodableValue response(umap);
    result->Success(&response);
  }
  else if (method.compare("startResize") == 0)
  {
    const flutter::EncodableValue *args = method_call.arguments();
    const flutter::EncodableMap &map = args->MapValue();
    bool top = map.at(flutter::EncodableValue("top")).BoolValue();
    bool bottom = map.at(flutter::EncodableValue("bottom")).BoolValue();
    bool left = map.at(flutter::EncodableValue("left")).BoolValue();
    bool right = map.at(flutter::EncodableValue("right")).BoolValue();
    HWND hWnd = GetActiveWindow();
    ReleaseCapture();
    LONG command = SC_SIZE;
    if (top && !bottom && !right && !left)
    {
      command |= WMSZ_TOP;
    }
    else if (top && left && !bottom && !right)
    {
      command |= WMSZ_TOPLEFT;
    }
    else if (left && !top && !bottom && !right)
    {
      command |= WMSZ_LEFT;
    }
    else if (right && !top && !left && !bottom)
    {
      command |= WMSZ_RIGHT;
    }
    else if (top && right && !left && !bottom)
    {
      command |= WMSZ_TOPRIGHT;
    }
    else if (bottom && !top && !right && !left)
    {
      command |= WMSZ_BOTTOM;
    }
    else if (bottom && left && !top && !right)
    {
      command |= WMSZ_BOTTOMLEFT;
    }
    else if (bottom && right && !top && !left)
    {
      command |= WMSZ_BOTTOMRIGHT;
    }
    SendMessage(hWnd, WM_SYSCOMMAND, command, 0);
    flutter::EncodableValue response(true);
    result->Success(&response);
  }
  else if (method.compare("startDrag") == 0)
  {
    HWND hWnd = GetActiveWindow();
    ReleaseCapture();
    SendMessage(hWnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
    flutter::EncodableValue response(true);
    result->Success(&response);
  }
  else if (method.compare("closeWindow") == 0)
  {
    HWND hWnd = GetActiveWindow();
    SendMessage(hWnd, WM_CLOSE, 0, NULL);
    flutter::EncodableValue response(true);
    result->Success(&response);
  }
  else if (method.compare("minWindow") == 0)
  {
    HWND hWnd = GetActiveWindow();
    ShowWindow(hWnd, SW_MINIMIZE);
    flutter::EncodableValue response(true);
    result->Success(&response);
  }
  else if (method.compare("windowTitleDoubleTap") == 0)
  {
    HWND hWnd = GetActiveWindow();
    HWND hWndScreen = GetDesktopWindow();
    RECT rectScreen;
    GetWindowRect(hWndScreen, &rectScreen);
    double padding = 5.0;
    bool isMaximized = false;
    RECT activeRect;
    GetWindowRect(hWnd, &activeRect);
    if (activeRect.top <= rectScreen.top + padding)
    {
      isMaximized = true;
    }
    if (activeRect.bottom >= rectScreen.bottom - padding)
    {
      isMaximized = true;
    }
    if (activeRect.left <= rectScreen.left + padding)
    {
      isMaximized = true;
    }
    if (activeRect.right >= rectScreen.right - padding)
    {
      isMaximized = true;
    }
    if (!isMaximized)
    {
      GetWindowRect(hWnd, &normalRect);
    }
    if (isMaximized)
    {
      RECT rect = normalRect;
      int width = (int)(rect.right - rect.left);
      int height = (int)(rect.bottom - rect.top);
      int x = rect.left;
      int y = rect.top;
      MoveWindow(hWnd, x, y, width, height, true);
    }
    else
    {
      RECT rect = rectScreen;
      int width = (int)(rect.right - rect.left);
      int height = (int)(rect.bottom - rect.top);
      int x = rect.left;
      int y = rect.top;
      MoveWindow(hWnd, x, y, width, height, true);
    }
    //  ReleaseCapture();
    // // InvalidateRect(hWnd, NULL, TRUE);
    // UpdateWindow(hWnd);
    flutter::EncodableValue response(true);
    result->Success(&response);
  }
  else if (method.compare("maxWindow") == 0)
  {
    HWND hWnd = GetActiveWindow();
    ShowWindow(hWnd, SW_SHOWMAXIMIZED);
    flutter::EncodableValue response(true);
    result->Success(&response);
  }
  else if (method.compare("centerWindow") == 0)
  {
    HWND hWnd = GetActiveWindow();
    RECT rect;
    bool success = false;
    HWND hWndScreen = GetDesktopWindow();
    RECT rectScreen;
    if (GetWindowRect(hWndScreen, &rectScreen))
    {
      // double screenWidth = rectScreen.right;
      // double screenHeight = rectScreen.bottom;
      // double centerX = screenWidth / 2;
      // double centerY = screenHeight / 2;

      if (GetWindowRect(hWnd, &rect))
      {
        int width = (int)(rect.right - rect.left);
        int height = (int)(rect.bottom - rect.top);
        int x = ((rectScreen.right - rectScreen.left) / 2 - width / 2);
        int y = ((rectScreen.bottom - rectScreen.top) / 2 - height / 2);
        success = MoveWindow(hWnd, x, y, width, height, true);
      }
    }
    flutter::EncodableValue response(success);
    result->Success(&response);
  }
  else if (method.compare("setSize") == 0)
  {
    const flutter::EncodableValue *args = method_call.arguments();
    const flutter::EncodableMap &map = args->MapValue();
    int width = (int)map.at(flutter::EncodableValue("width")).DoubleValue();
    int height = (int)map.at(flutter::EncodableValue("height")).DoubleValue();
    HWND hWnd = GetActiveWindow();
    RECT rect;
    bool success = false;
    if (GetWindowRect(hWnd, &rect))
    {
      int x = (int)rect.left;
      int y = (int)rect.top;
      success = MoveWindow(hWnd, x, y, width, height, true);
    }
    flutter::EncodableValue response(success);
    result->Success(&response);
  }
  else if (method.compare("setPosition") == 0)
  {
    const flutter::EncodableValue *args = method_call.arguments();
    const flutter::EncodableMap &map = args->MapValue();
    int x = (int)map.at(flutter::EncodableValue("x")).DoubleValue();
    int y = (int)map.at(flutter::EncodableValue("y")).DoubleValue();
    HWND hWnd = GetActiveWindow();
    RECT rect;
    bool success = false;
    if (GetWindowRect(hWnd, &rect))
    {
      int width = (int)(rect.right - rect.left);
      int height = (int)(rect.bottom - rect.top);
      success = MoveWindow(hWnd, x, y, width, height, true);
    }
    flutter::EncodableValue response(success);
    result->Success(&response);
  }
  else if (method.compare("getWindowSize") == 0)
  {
    flutter::EncodableMap umap;
    HWND hWnd = GetActiveWindow();
    RECT rect;
    if (GetWindowRect(hWnd, &rect))
    {
      double width = rect.right - rect.left;
      double height = rect.bottom - rect.top;
      umap[flutter::EncodableValue("width")] = flutter::EncodableValue(width);
      umap[flutter::EncodableValue("height")] = flutter::EncodableValue(height);
    }
    flutter::EncodableValue response(umap);
    result->Success(&response);
  }
  else if (method.compare("getWindowOffset") == 0)
  {
    flutter::EncodableMap umap;
    HWND hWnd = GetActiveWindow();
    RECT rect;
    if (GetWindowRect(hWnd, &rect))
    {
      double offsetX = rect.left;
      double offsetY = rect.top;
      umap[flutter::EncodableValue("offsetX")] = flutter::EncodableValue(offsetX);
      umap[flutter::EncodableValue("offsetY")] = flutter::EncodableValue(offsetY);
    }
    flutter::EncodableValue response(umap);
    result->Success(&response);
  }
  else if (method.compare("setCursor") == 0)
  {
    const flutter::EncodableValue *args = method_call.arguments();
    const flutter::EncodableMap &map = args->MapValue();
    std::string type = map.at(flutter::EncodableValue("type")).StringValue();
    LPWSTR cursor = IDC_ARROW;

    // Map type to cursor
    if (type == "beamVertical")
    {
      cursor = IDC_IBEAM;
    }
    else if (type == "beamHorizontial")
    {
      cursor = IDC_IBEAM;
    }
    else if (type == "crossHair")
    {
      cursor = IDC_CROSS;
    }
    else if (type == "closedHand")
    {
      cursor = IDC_SIZEALL;
    }
    else if (type == "openHand")
    {
      cursor = IDC_SIZEALL;
    }
    else if (type == "pointingHand")
    {
      cursor = IDC_HAND;
    }
    else if (type == "resizeLeft")
    {
      cursor = IDC_SIZEWE;
    }
    else if (type == "resizeRight")
    {
      cursor = IDC_SIZEWE;
    }
    else if (type == "resizeLeftRight")
    {
      cursor = IDC_SIZEWE;
    }
    else if (type == "resizeUp")
    {
      cursor = IDC_SIZENS;
    }
    else if (type == "resizeDown")
    {
      cursor = IDC_SIZENS;
    }
    else if (type == "resizeUpDown")
    {
      cursor = IDC_SIZENS;
    }
    else if (type == "notAllowed")
    {
      cursor = IDC_NO;
    }

    // Set cursor
    SetCursor(LoadCursor(NULL, cursor));

    flutter::EncodableValue response(true);
    result->Success(&response);
  }
  else if (method.compare("resetCursor") == 0)
  {
    // Set cursor to default arrow
    SetCursor(LoadCursor(NULL, IDC_ARROW));

    flutter::EncodableValue response(true);
    result->Success(&response);
  }
  else
  {
    result->NotImplemented();
  }
}

} // namespace

void WindowUtilsPluginRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar)
{
  // The plugin registrar owns the plugin, registered callbacks, etc., so must
  // remain valid for the life of the application.
  static auto *plugin_registrar = new flutter::PluginRegistrar(registrar);

  WindowUtilsPlugin::RegisterWithRegistrar(plugin_registrar);
}
