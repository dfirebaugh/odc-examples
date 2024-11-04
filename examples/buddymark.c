
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdio.h>
#include <stdlib.h>

#include "odc_debug.h"
#include "odc_engine.h"
#include "odc_input.h"
#include "odc_renderer.h"

#define MAX_FRAME_TIMES 100

static double delta_time = 0.0;

#define MAX_BUDDIES 100000
#define GRAVITY 0.1f
#define DAMPING 0.9f

struct Buddy {
  float x, y;
  float vx, vy;
  struct texture_render_options options;
  int frame_index;
  double frame_time;
};

static struct Buddy bunnies[MAX_BUDDIES];
static int buddy_count = 0;

void add_buddy(int windowWidth, int windowHeight) {
  if (buddy_count >= MAX_BUDDIES)
    return;

  struct Buddy *buddy = &bunnies[buddy_count++];
  buddy->x = rand() % windowWidth;
  buddy->y = rand() % windowHeight;
  buddy->vx = (float)(rand() % 200 - 100) / 60.0f;
  buddy->vy = (float)(rand() % 200 - 100) / 60.0f;
  buddy->frame_index = 0;
  buddy->frame_time = 0.0;

  buddy->options =
      (struct texture_render_options){.x = buddy->x,
                                      .y = buddy->y,
                                      .width = 128,
                                      .height = 32,
                                      .rect_x = 0,
                                      .rect_y = 0,
                                      .rect_width = 32,
                                      .rect_height = 32,
                                      .screen_width = windowWidth,
                                      .screen_height = windowHeight,
                                      .flip_x = 0,
                                      .flip_y = 0,
                                      .scale = 1.0f,
                                      .rotation = 0.0f};
}

void buddymark_example_update(struct engine *e) {
  struct renderer *r = odc_engine_get_renderer(e);
  struct GLFWwindow *window = odc_engine_get_window(e);

  static double last_time = 0.0;
  double current_time = glfwGetTime();
  delta_time = current_time - last_time;
  last_time = current_time;

  int windowWidth, windowHeight;
  glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

  if (odc_is_mouse_button_just_pressed(window, GLFW_MOUSE_BUTTON_LEFT)) {
    for (int i = 0; i < 500; ++i) {
      add_buddy(windowWidth, windowHeight);
    }
  }
  if (odc_is_mouse_button_just_pressed(window, GLFW_MOUSE_BUTTON_RIGHT)) {
    buddy_count = 0;
    odc_renderer_clear_vertices(r);
    return;
  }

  for (int i = 0; i < buddy_count; ++i) {
    struct Buddy *buddy = &bunnies[i];
    buddy->vy += GRAVITY;

    buddy->x += buddy->vx * delta_time * 60.0f;
    buddy->y += buddy->vy * delta_time * 60.0f;

    if (buddy->x < 0) {
      buddy->x = 0;
      buddy->vx *= -DAMPING;
    } else if (buddy->x > windowWidth - 32) {
      buddy->x = windowWidth - 32;
      buddy->vx *= -DAMPING;
    }

    if (buddy->y > windowHeight - 32) {
      buddy->y = windowHeight - 32;
      buddy->vy *= -DAMPING;
    }

    buddy->options.x = buddy->x;
    buddy->options.y = buddy->y;

    buddy->frame_time += delta_time;
    if (buddy->frame_time >= 0.1) {
      buddy->frame_time = 0.0;
      buddy->frame_index = (buddy->frame_index + 1) % 4;
      buddy->options.rect_x = buddy->frame_index * 32;
    }
  }

  odc_debug_update_frame_times((float)(delta_time * 1000.0));
}

void buddymark_example_render(struct engine *e) {
  struct renderer *r = odc_engine_get_renderer(e);
  odc_renderer_clear(r, 41.0f / 255.0f, 44.0f / 255.0f, 60.0f / 255.0f, 1.0f);

  struct GLFWwindow *window = odc_engine_get_window(e);
  odc_renderer_reset_shape_count(r);

  int windowWidth, windowHeight;
  glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

  for (int i = 0; i < buddy_count; ++i) {
    struct Buddy *buddy = &bunnies[i];
    odc_renderer_add_texture(r, &buddy->options);
  }

  float rounded_rect_width = 260.0f;
  float rounded_rect_height = 70.0f;
  float rounded_rect_radius = 20.0f;
  float rounded_rect_color[4] = {48.0f / 255, 52.0f / 255, 70.0f / 255, 1.0f};
  odc_renderer_add_rounded_rect(r, 10, 5, rounded_rect_width,
                                rounded_rect_height, rounded_rect_radius,
                                windowWidth, windowHeight, rounded_rect_color);

  static char fps_text[256] = "";
  static char buddies_text[256] = "";
  static char avg_frame_time_text[256] = "";

  static double text_update_timer = 0.0;
  text_update_timer += delta_time;
  if (text_update_timer >= 0.2) {
    text_update_timer = 0.0;
    sprintf(fps_text, "fps: %d", odc_engine_get_fps(e));
    sprintf(buddies_text, "buddies: %d", buddy_count);
    float avg_frame_time = odc_debug_calculate_average_frame_time();
    sprintf(avg_frame_time_text, "Avg Frame Time: %.1f ms", avg_frame_time);
  }

  float text_color[4] = {231.0f / 255.0f, 130.0f / 255.0f, 132.0f / 255.0f,
                         1.0f};
  odc_renderer_add_text(r, fps_text, 25, 30, 0.4f, windowWidth, windowHeight,
                        text_color);
  odc_renderer_add_text(r, buddies_text, 25, 45, 0.4f, windowWidth,
                        windowHeight, text_color);
  odc_renderer_add_text(r, avg_frame_time_text, 25, 60, 0.4f, windowWidth,
                        windowHeight, text_color);

  odc_debug_render_frame_time_graph(r, windowWidth, windowHeight);

  odc_renderer_draw(r);
}

void flip_image_vertically(unsigned char *data, int width, int height,
                           int channels) {
  int stride = width * channels;
  unsigned char *row = (unsigned char *)malloc(stride);
  if (!row) {
    fprintf(stderr, "Failed to allocate memory for row buffer\n");
    return;
  }

  for (int y = 0; y < height / 2; ++y) {
    unsigned char *top = data + y * stride;
    unsigned char *bottom = data + (height - y - 1) * stride;

    memcpy(row, top, stride);
    memcpy(top, bottom, stride);
    memcpy(bottom, row, stride);
  }

  free(row);
}

int main() {
  struct engine *e = odc_engine_new(240 * 3, 160 * 3);
  if (!e) {
    return -1;
  }
  char *file_path = "./assets/images/buddy_dance.png";
  int width, height, channels;
  unsigned char *data = stbi_load(file_path, &width, &height, &channels, 4);
  if (!data) {
    fprintf(stderr, "Failed to load texture: %s\n", file_path);
    return 1;
  }
  const char *font_path =
      "./assets/fonts/ComicShannsMono/ComicShannsMonoNerdFont-Bold.otf";
  odc_renderer_load_font(odc_engine_get_renderer(e), font_path);

  odc_engine_set_window_title(e, "buddymark");

  flip_image_vertically(data, width, height, channels);
  odc_renderer_upload_texture_atlas(odc_engine_get_renderer(e), data, width,
                                    height);

  odc_engine_set_update_callback(e, buddymark_example_update);
  odc_engine_set_render_callback(e, buddymark_example_render);
  odc_engine_run(e);
  odc_engine_destroy(e);

  stbi_image_free(data);

  return 0;
}
