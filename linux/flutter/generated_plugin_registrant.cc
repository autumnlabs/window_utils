//
//  Generated file. Do not edit.
//

#include "generated_plugin_registrant.h"

#include <window_utils/window_utils.h>

void fl_register_plugins(FlPluginRegistry* registry) {
  g_autoptr(FlPluginRegistrar) window_utils_registrar =
      fl_plugin_registry_get_registrar_for_plugin(registry, "WindowUtils");
  window_utils_register_with_registrar(window_utils_registrar);
}
