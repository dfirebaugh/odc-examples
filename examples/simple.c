#include "odc_engine.h"

void simple_render(struct engine *e) {
  (void)e;
}

void simple_update(struct engine *e) {
  (void)e;
}

int main() {
  struct engine *e = odc_engine_new(320, 240);
  if (!e) {
    return -1;
  }

  odc_engine_set_window_title(e, "simple template");

  odc_engine_set_update_callback(e, simple_update);
  odc_engine_set_render_callback(e, simple_render);
  odc_engine_run(e);

  odc_engine_destroy(e);

  return 0;
}
