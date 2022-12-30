#include "gem.h"
#include "gem_icons.h"

// Function to draw the static interface
static void draw_interface(Canvas *const canvas) {
    // border around the edge of the screen
    //canvas_draw_frame(canvas, 0, 0, FLIPPER_LCD_WIDTH, FLIPPER_LCD_HEIGHT);

    canvas_draw_icon(canvas, TITLE_OFFSET, TITLE_OFFSET, &I_gem_10px);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, (TITLE_OFFSET * 2) + LOGO_WIDTH, TITLE_OFFSET, AlignLeft, AlignTop, TITLE);

    canvas_draw_line(canvas, 0, TITLE_OFFSET + TEXT_OFFSET + 1, FLIPPER_LCD_WIDTH, TITLE_OFFSET + TEXT_OFFSET + 1);

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, FLIPPER_LCD_WIDTH - TITLE_OFFSET, TITLE_OFFSET, AlignRight, AlignTop, SUBTITLE);
}

// Callback function for rendering the plugin
static void render_callback(Canvas *const canvas, void *ctx) {
    const PluginState *plugin_state = acquire_mutex((ValueMutex *) ctx, 25);
    if (plugin_state == NULL) {
        return;
    }

    // Draw static interface
    draw_interface(canvas);

    // Draw interactive elements
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, plugin_state->x, plugin_state->y, AlignLeft, AlignTop, TAG);

    release_mutex((ValueMutex *) ctx, plugin_state);
}

// Callback function for handling input events
static void input_callback(InputEvent *input_event, FuriMessageQueue *event_queue) {
    furi_assert(event_queue);

    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

// Function to initialize the plugin state
static void gem_state_init(PluginState *const plugin_state) {
    plugin_state->x = TITLE_OFFSET;
    plugin_state->y = START_Y + TITLE_OFFSET;
}

// Main function for the plugin
int32_t gem_app(void *p) {
    UNUSED(p);

    // Allocate a message queue for events
    FuriMessageQueue *event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));

    // Allocate memory for the plugin state and initialize it
    PluginState *plugin_state = malloc(sizeof(PluginState));
    gem_state_init(plugin_state);

    // Create a mutex for the plugin state
    ValueMutex state_mutex;
    if (!init_mutex(&state_mutex, plugin_state, sizeof(PluginState))) {
        FURI_LOG_E(TAG, "cannot create mutex\r\n");
        free(plugin_state);
        return 255;
    }

    // Set system callbacks
    ViewPort *view_port = view_port_alloc();
    view_port_set_callbacks(view_port, &render_callback, &input_callback, &state_mutex);

    // Start the event loop
    while (true) {
        PluginEvent event;
        if (furi_message_queue_get(event_queue, &event, FuriWaitForever) == true) {
            switch (event.type) {
                case EventTypeKey:
                    // Handle key events
                    if (event.input.type == InputEventTypePress && event.input.key == KeyA) {
                        FURI_LOG_I(TAG, "Button A pressed\r\n");
                        // Send a message
                        send("hello");
                    }
                    break;
                default:
                    break;
            }
        }
    }

    // Clean up
    view_port_free(view_port);
    furi_message_queue_free(event_queue);
    free(plugin_state);

    return 0;
}