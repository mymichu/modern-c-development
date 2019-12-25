//
// Created by memi on 17.12.2019.
//
#include <main_sm.h>
#include "uscxml_main_sm.h"
#include <stdbool.h>

uscxml_ctx ctx;
struct event_t {
    const char* name;
};
struct event_t _eventButtonOn= {
        name: "trButtonOn"
};
struct event_t _eventButtonOff = {
        name: "trButtonOff"
};
struct event_t* _event;

/* state chart is invoking something */
static int invoke(const uscxml_ctx* ctx,
                  const uscxml_state* s,
                  const uscxml_elem_invoke* invocation,
                  unsigned char uninvoke) {
    printf("%s \n",invocation->type);
    if (strcmp(invocation->type, "Off") == 0) {
        printf("OFF ID: %d \n",atoi(invocation->id));
    } else if (strcmp(invocation->type, "On") == 0) {
        printf("ON ID: %d \n",atoi(invocation->id));
    }
}

/* is the event matching */
static int matched(const uscxml_ctx* ctx,
                   const uscxml_transition* transition,
                   const void* event) {
    // we ignore most event name matching rules here
    return strcmp(transition->event, ((const struct event_t*)event)->name) == 0;
}

unsigned long remain;
static int send(const uscxml_ctx* ctx, const uscxml_elem_send* send) {
    printf("SEND\n");
    if (send->delay > 0)
        remain = send->delay;
    return USCXML_ERR_OK;
}
static int raise(const uscxml_ctx* ctx, const char* event){
    printf("RAISE %s\n", event);
    return USCXML_ERR_OK;
}

static void* dequeueExternal(const uscxml_ctx* ctx) {
    // we will only call step when we have an event
    void* tmp = _event;
    _event = NULL;
    return tmp;
}

static bool isInState(const char* stateId) {
    for (size_t i = 0; i < ctx.machine->nr_states; i++) {
        if (ctx.machine->states[i].name &&
            strcmp(ctx.machine->states[i].name, stateId) == 0 &&
            BIT_HAS(i, ctx.config)) {
            return true;
        }
    }

    return false;
}

void uscxml_init() {
    // initilize the state chart
    memset(&ctx, 0, sizeof(uscxml_ctx));
    ctx.machine = &USCXML_MACHINE;
    ctx.invoke = invoke;
    ctx.is_matched = matched;
    ctx.dequeue_external = dequeueExternal;
    ctx.exec_content_send = send;
    ctx.exec_content_raise = raise;

    int err = USCXML_ERR_OK;

    // run until first stable config
    while((err = uscxml_step(&ctx)) != USCXML_ERR_IDLE) {}
    printf("OFF STATE %d\n",isInState("Off"));
}

void uscxml_buttonOn(){
    _event = &_eventButtonOn;
    printf("Set Button ON \n");

}

void uscxml_buttonOff(){
    _event = &_eventButtonOff;
    printf("Set Button OFF \n");
}
void uscxml_update_sm() {
    int err = USCXML_ERR_OK;
    printf("HELLO \n");
    while((err = uscxml_step(&ctx)) != USCXML_ERR_IDLE) {}
    printf("On STATE %d\n",isInState("On"));
    printf("OFF STATE %d\n",isInState("Off"));
}

