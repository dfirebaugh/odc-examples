#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "odc_engine.h"
#include "odc_input.h"
#include "odc_renderer.h"

#define NUM_BALLS 10
#define SPEED_FACTOR 0.5

typedef struct {
  float x, y;
  float vx, vy;
  float radius;
  float color[4];
} Ball;

Ball balls[NUM_BALLS];
double last_time = 0.0;
double delta_time = 0.0;

void init_balls(int screen_width, int screen_height) {
  srand(time(NULL));
  for (int i = 0; i < NUM_BALLS; i++) {
    balls[i].x = rand() % screen_width;
    balls[i].y = rand() % screen_height;
    balls[i].vx = (rand() % 200 - 100) / 100.0f;
    balls[i].vy = (rand() % 200 - 100) / 100.0f;
    balls[i].radius = 20.0f;
    balls[i].color[0] = (rand() % 256) / 255.0f;
    balls[i].color[1] = (rand() % 256) / 255.0f;
    balls[i].color[2] = (rand() % 256) / 255.0f;
    balls[i].color[3] = 1.0f;
  }
}

void update_balls(int screen_width, int screen_height, double delta_time) {
  for (int i = 0; i < NUM_BALLS; i++) {
    balls[i].x += balls[i].vx * delta_time * 100 * SPEED_FACTOR;
    balls[i].y += balls[i].vy * delta_time * 100 * SPEED_FACTOR;

    if (balls[i].x - balls[i].radius < 0 ||
        balls[i].x + balls[i].radius > screen_width) {
      balls[i].vx = -balls[i].vx;
    }
    if (balls[i].y - balls[i].radius < 0 ||
        balls[i].y + balls[i].radius > screen_height) {
      balls[i].vy = -balls[i].vy;
    }
  }
}

void render_balls(struct renderer *renderer, int screen_width,
                  int screen_height) {
  for (int i = 0; i < NUM_BALLS; i++) {
    odc_renderer_add_circle(renderer, balls[i].x, balls[i].y, balls[i].radius,
                            screen_width, screen_height, balls[i].color);
  }
}

void example_render(struct engine *e) {
  int windowWidth, windowHeight;

  struct renderer *r = odc_engine_get_renderer(e);
  glfwGetFramebufferSize(odc_engine_get_window(e), &windowWidth, &windowHeight);
  odc_renderer_clear_vertices(r);
  odc_renderer_clear(r, 0.1f, 0.1f, 0.1f, 1.0f);

  render_balls(r, windowWidth, windowHeight);

  odc_renderer_draw(r);
  odc_renderer_reset_shape_count(r);
}

void example_update(struct engine *e) {
  int windowWidth, windowHeight;
  glfwGetFramebufferSize(odc_engine_get_window(e), &windowWidth, &windowHeight);

  double current_time = glfwGetTime();
  delta_time = current_time - last_time;
  update_balls(windowWidth, windowHeight, delta_time);
  last_time = current_time;
}

int main() {
  struct engine *e = odc_engine_new(800, 600);
  if (!e) {
    return -1;
  }

  odc_engine_set_window_title(e, "balls");

  init_balls(800, 600);

  odc_engine_set_update_callback(e, example_update);
  odc_engine_set_render_callback(e, example_render);

  odc_engine_run(e);
  odc_engine_destroy(e);

  return 0;
}
