#include "event.h"
#include <stdio.h>
#include "log/log.h"

#define MAX_CALLBACKS 10

typedef struct {
    EventCallback callback;
    void *subscriber;
} EventSubscription;

static EventSubscription event_callbacks[EVENT_TYPE_COUNT][MAX_CALLBACKS] = { { {0} } }; 

void subscribe_event(EventType event, EventCallback callback, void *subscriber) {
    for (int i = 0; i < MAX_CALLBACKS; i++) { // \todo passer en liste chainee
        if (event_callbacks[event][i].callback == NULL) {
            event_callbacks[event][i].callback = callback;
            event_callbacks[event][i].subscriber = subscriber;
            return;
        }
    }
    _log_err("No space to subscribe more callbacks for event %d\n", event); 
}

void trigger_event(EventType event, void *context) {
    for (int i = 0; i < MAX_CALLBACKS; i++) {
        if (event_callbacks[event][i].callback != NULL) {
            event_callbacks[event][i].callback(event_callbacks[event][i].subscriber, context);
        }
    }
}