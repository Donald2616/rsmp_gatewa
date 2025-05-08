#ifndef EVENT_H
#define EVENT_H

typedef enum {
    EVENT_TYPE_INCOMING_CONNECTION,
    EVENT_TYPE_CHALLENGE_PROCESSING,
    EVENT_TYPE_ALLCLEAR_EXCHANGED,
    EVENT_TYPE_CYBER_LOST,
    // Add more event types as needed
    EVENT_TYPE_COUNT
} EventType;

typedef void (*EventCallback)(void *subscriber, void *context);

void subscribe_event(EventType event, EventCallback callback, void *subscriber);
void trigger_event(EventType event, void *context);

#endif // EVENT_MANAGER_H