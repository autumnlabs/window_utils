#include "include/window_utils/window_utils_plugin.h"

void WindowUtilsPlugin::RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar)
{
    auto channel =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          registrar->messenger(), "window_utils",
          &flutter::StandardMethodCodec::GetInstance());

    auto plugin = std::make_unique<WindowUtilsPlugin>();

    channel->SetMethodCallHandler(
        [plugin_pointer = plugin.get()](const auto &call, auto result) {
            plugin_pointer->HandleMethodCall(call, std::move(result));
        });

    registrar->AddPlugin(std::move(plugin));
}

WindowUtilsPlugin::WindowUtilsPlugin() {}

WindowUtilsPlugin::~WindowUtilsPlugin() {};

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
        const auto *args = std::get_if<flutter::EncodableMap>(method_call.arguments());

        if (!args) {
            result->Error(
                "Bad Arguments",
                "Argument map is missing or malformed."
            );

            return;
        }

        bool top = std::get<bool>(args->at(flutter::EncodableValue("top")));
        bool bottom = std::get<bool>(args->at(flutter::EncodableValue("bottom")));
        bool left = std::get<bool>(args->at(flutter::EncodableValue("left")));
        bool right = std::get<bool>(args->at(flutter::EncodableValue("right")));
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
        if (GetIsMaximized())
        {
            UnmaximizeWindow();
        }

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
        if (GetIsMaximized())
        {
            UnmaximizeWindow();
        }
        else
        {
            MaximizeWindow();
        }

        flutter::EncodableValue response(true);
        result->Success(&response);
    }
    else if (method.compare("maxWindow") == 0)
    {
        MaximizeWindow();
        flutter::EncodableValue response(true);
        result->Success(&response);
    }
    else if (method.compare("unmaxWindow") == 0)
    {
        UnmaximizeWindow();
        flutter::EncodableValue response(true);
        result->Success(&response);
    }
    else if (method.compare("toggleMaxWindow") == 0)
    {
        if (GetIsMaximized())
        {
            UnmaximizeWindow();
        }
        else
        {
            MaximizeWindow();
        }

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

        if (GetWindowRect(hWndScreen, &rectScreen) && GetWindowRect(hWnd, &rect))
        {
            int width = (int)(rect.right - rect.left);
            int height = (int)(rect.bottom - rect.top);
            int x = ((rectScreen.right - rectScreen.left) / 2 - width / 2);
            int y = ((rectScreen.bottom - rectScreen.top) / 2 - height / 2);
            success = MoveWindow(hWnd, x, y, width, height, true);
        }

        flutter::EncodableValue response(success);
        result->Success(&response);
    }
    else if (method.compare("setSize") == 0)
    {
        const auto *args = std::get_if<flutter::EncodableMap>(method_call.arguments());

        if (!args) {
            result->Error(
                "Bad Arguments",
                "Argument map is missing or malformed."
            );

            return;
        }

        auto width = (int)std::get<double>(args->at(flutter::EncodableValue("width")));
        auto height = (int)std::get<double>(args->at(flutter::EncodableValue("height")));
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
        const auto *args = std::get_if<flutter::EncodableMap>(method_call.arguments());

        if (!args) {
            result->Error(
                "Bad Arguments",
                "Argument map is missing or malformed."
            );

            return;
        }

        auto x = (int)std::get<double>(args->at(flutter::EncodableValue("x")));
        auto y = (int)std::get<double>(args->at(flutter::EncodableValue("y")));
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
        const auto *args = std::get_if<flutter::EncodableMap>(method_call.arguments());

        if (!args) {
            result->Error(
                "Bad Arguments",
                "Argument map is missing or malformed."
            );

            return;
        }

        auto type = std::get<std::string>(args->at(flutter::EncodableValue("type")));
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

bool WindowUtilsPlugin::GetIsMaximized()
{
    HWND hWnd = GetActiveWindow();
    RECT rectScreen;
    GetWindowRect(hWnd, &rectScreen);

    // Get monitor info
    HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo = {0};
    monitorInfo.cbSize = sizeof(MONITORINFO);
    GetMonitorInfo(hMonitor, &monitorInfo);

    return rectScreen.top <= monitorInfo.rcWork.top &&
           rectScreen.bottom >= monitorInfo.rcWork.bottom &&
           rectScreen.left <= monitorInfo.rcWork.left &&
           rectScreen.right >= monitorInfo.rcWork.right;
}

void WindowUtilsPlugin::MaximizeWindow()
{
    HWND hWnd = GetActiveWindow();
    RECT curRect;
    GetWindowRect(hWnd, &curRect);
    premaxWindowRect = curRect;

    // Get monitor info
    HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo = {0};
    monitorInfo.cbSize = sizeof(MONITORINFO);
    GetMonitorInfo(hMonitor, &monitorInfo);

    // Set window size
    int width = (int)(monitorInfo.rcWork.right - monitorInfo.rcWork.left);
    int height = (int)(monitorInfo.rcWork.bottom - monitorInfo.rcWork.top);
    int x = monitorInfo.rcWork.left;
    int y = monitorInfo.rcWork.top;
    MoveWindow(hWnd, x, y, width, height, true);
}

void WindowUtilsPlugin::UnmaximizeWindow()
{
    HWND hWnd = GetActiveWindow();

    int width = (int)(premaxWindowRect.right - premaxWindowRect.left);
    int height = (int)(premaxWindowRect.bottom - premaxWindowRect.top);
    int x = premaxWindowRect.left;
    int y = premaxWindowRect.top;
    MoveWindow(hWnd, x, y, width, height, true);
}

void WindowUtilsPluginRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar)
{
    WindowUtilsPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}