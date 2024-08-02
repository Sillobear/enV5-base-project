#define SOURCE_FILE "MAIN"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "unity.h"

// pico-sdk headers
#include "hardware/i2c.h"
#include "hardware/spi.h"
#include "hardware/watchdog.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"

#include "pico/printf.h"

// internal headers
#include "Adxl345b.h"
#include "Common.h"
#include "Esp.h"
#include "FreeRtosMutexWrapper.h"
#include "FreeRtosQueueWrapper.h"
#include "FreeRtosTaskWrapper.h"
#include "Gpio.h"
#include "MqttBroker.h"
#include "Network.h"
#include "Posting.h"
#include "Protocol.h"
#include "enV5HwController.h"
#include "Spi.h"
#include "Flash.h"
#include "EnV5HwConfiguration.h"

queue_t uartRx;
queue_t uartTx;



spiConfiguration_t spi_config_flash = {
    .spiInstance = SPI_FLASH_INSTANCE,
    .baudrate = SPI_FLASH_BAUDRATE,
    .sckPin = SPI_FLASH_SCK,
    .csPin = SPI_FLASH_CS,
    .misoPin = SPI_FLASH_MISO,
    .mosiPin = SPI_FLASH_MOSI,
};

flashConfiguration_t flash_config = {
    .flashBytesPerPage = FLASH_BYTES_PER_PAGE,
    .flashBytesPerSector = FLASH_BYTES_PER_SECTOR,
    .flashSpiConfiguration = &spi_config_flash,
};

static void initialize(void) {
    // check if we crash last time -> if true, reboot into boot rom mode
    if (watchdog_enable_caused_reboot()) {
        reset_usb_boot(0, 0);
    }
    env5HwControllerInit();

    // initialize I/O for Debug purposes
    stdio_init_all();
    while ((!stdio_usb_connected())) { /* wait for serial connection */
    }
    spiInit(&spi_config_flash);
    espInit(); // initialize Wi-Fi chip
    networkTryToConnectToNetworkUntilSuccessful();
    env5HwControllerLedsAllOn();
    sleep_ms(500);
}

_Noreturn void watchdogTask(void) {
    watchdog_enable(10000, 1); // enables watchdog timer (10s)

    while (1) {
        watchdog_update();                  // watchdog update needs to be performed frequent
        freeRtosTaskWrapperTaskSleep(1000); // sleep for 1 second
    }
}

void onUartRx() {
    while (uart_is_readable())
    freeRtosQueueWrapperPushFromInterrupt(receivedPosts, &posting);
}
_Noreturn void handleUartRx(void) {
    while (1) {
        posting_t post;
        if (freeRtosQueueWrapperPop(receivedPosts, &post)) {
            PRINT("Received Message: '%s' via topic '%s'", post.data, post.topic);
            if (NULL != strstr(post.topic, "/DO/MEASUREMENTS")) {
                freeRtosQueueWrapperPush(batchRequest, NULL);
            }
            free(post.topic);
            free(post.data);
        }
        freeRtosTaskWrapperTaskSleep(500);
    }
}


int main() {
    initialize();



    uartRx = freeRtosQueueWrapperCreate(10, sizeof(posting_t));
    //batchRequest = freeRtosQueueWrapperCreate(5, sizeof(NULL));

    //espOccupied = freeRtosMutexWrapperCreate();

    freeRtosTaskWrapperRegisterTask(watchdogTask, "watchdog", configMAX_PRIORITIES / 2-1,
                                    FREERTOS_CORE_0);
    freeRtosTaskWrapperRegisterTask(handleUartRx, "uart_receiver", configMAX_PRIORITIES / 2,
                                    FREERTOS_CORE_0);
    /*
    freeRtosTaskWrapperRegisterTask(handlePublishTask, "sender", configMAX_PRIORITIES,
                                    FREERTOS_CORE_0);
    freeRtosTaskWrapperRegisterTask(recordMeasurementBatchTask, "recorder", configMAX_PRIORITIES,
                                    FREERTOS_CORE_1);
    */
    freeRtosTaskWrapperStartScheduler();
}