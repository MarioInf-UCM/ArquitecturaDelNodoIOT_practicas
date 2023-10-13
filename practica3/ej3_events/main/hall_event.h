#ifndef HALL_EVENT_H_
#define HALL_EVENT_H_

#ifdef __cplusplus
extern "C" {
#endif

ESP_EVENT_DECLARE_BASE(HALL_EVENT);         // declaration of the task events family

enum {
    HALL_EVENT_NEWSAMPLE                     // raised during an iteration of the loop within the task
};

#ifdef __cplusplus
}
#endif

#endif // #ifndef EVENT_SOURCE_H_
