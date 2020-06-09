#include "commander.h"

static void commanderTask(void *arg)
{
    (void)arg;

    char   message[64];
    size_t messageLen;

    for (;;) {
        messageLen = xMessageBufferReceive(usbRxMessages, message, sizeof(message), portMAX_DELAY);

        usbWriteString(cc_sprintf("RECV%d\n", messageLen));
    }
}

void commanderInit(void)
{
    xTaskCreate(commanderTask, "Commander", 200, NULL, configMAX_PRIORITIES - 1, NULL);
}
