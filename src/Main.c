#define SOURCE_FILE "MAIN"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "hardware/spi.h"
#include "pico/printf.h"
#include "pico/stdlib.h"

#include "EnV5HwConfiguration.h"
#include "EnV5HwController.h"
#include "stub.h"
#include "Flash.h"
#include "Spi.h"
#include "Common.h"

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

void blinkLED(size_t count) {
    for (size_t i=0; i < count; i++) {
        env5HwControllerLedsAllOn();
        sleep_ms(500);

        env5HwControllerLedsAllOff();
        sleep_ms(500);
    }
}

void initHardware(void) {
    env5HwControllerInit();
    stdio_init_all();
    spiInit(&spi_config_flash);
    sleep_ms(500);

    blinkLED(2);
}

void eraseSectorsForBinFile(uint32_t length) {
    size_t index;
    for (index = 0; index < ceil((double)length / FLASH_BYTES_PER_SECTOR); index++) {
        flashEraseSector(&flash_config, index * FLASH_BYTES_PER_SECTOR);

    }
}

size_t getBufferLength(uint32_t length, size_t index){
    if (length-index * FLASH_BYTES_PER_PAGE > 0) {
        return FLASH_BYTES_PER_PAGE;
    } else {
        return length % FLASH_BYTES_PER_PAGE;
    }
}

uint8_t *readUartBuffer(size_t buffer_length){

    uint8_t *input_buffer = malloc(sizeof(int)*buffer_length);
    for (size_t buffer_index = 0; buffer_index < buffer_length; buffer_index++) {
        input_buffer[buffer_index] = getchar();
    }
    return input_buffer;
}

void getBinFile(uint32_t length){

    size_t index;
    for (index = 0; index < ceil((double)length/FLASH_BYTES_PER_PAGE); index++) {
        printf("ack: %zu \n", index);
        size_t buffer_length = 0;
        buffer_length = getBufferLength(length, index);
        printf("ack: %zu", buffer_length);

        printf("ack: start Read UART \n");
        uint8_t *uart_buffer = readUartBuffer(buffer_length);
        printf("ack: finish Read UART\n");

        printf("ack: start Write flash\n");
        flashWritePage(&flash_config, index*FLASH_BYTES_PER_PAGE, uart_buffer, buffer_length);
        printf("ack: finish Write flash\n");

        free(uart_buffer);

        printf("ack: start Read Buffer\n");
        uint8_t test_buffer[sizeof(uart_buffer)];
        data_t readbuffer = {.data = test_buffer, .length=sizeof(uart_buffer)};
        flashReadData(&flash_config, FLASH_BYTES_PER_PAGE, &readbuffer);
        PRINT_BYTE_ARRAY("ack: data ", readbuffer.data, readbuffer.length);
        printf("ack: finish Read Buffer\n");
        //printf("read_from_flash:");
        //blinkLED(1);
        //printf("ack: page\n");
    }
    printf("ack: binfile\n");
}


void eraseAndLoadBinFile(void) {
    char raw_length [9];
    size_t index = 0;
    while (1) {
        raw_length[index] = getchar();
        if(raw_length[index] == '\n') {
            break;
        }
        index++;
    }
    printf("ack: read length\n");
    uint32_t length = strtol(raw_length, NULL, 10);

    printf("ack: length=%lu\n", length);
    printf("ack: start eraseSectors\n");
    eraseSectorsForBinFile(length);
    printf("ack: finish eraseSectors\n");
    printf("ack: start getBinFile\n");
    getBinFile(length);
    printf("ack: finish getBinFile\n");
}

void runTest(void) {
    //Input Data length
    char raw_length [9];
    memset(raw_length, 0, sizeof(raw_length));
    size_t index = 0;
    while (1) {
        raw_length[index] = getchar();
        if(raw_length[index] == '\n') {
            break;
        }
        index++;
    }
    uint32_t length_in = strtol(raw_length, NULL, 10);

    //Output Data length
    memset(raw_length, 0, sizeof(raw_length));
    index = 0;
    while (1) {
        raw_length[index] = getchar();
        if(raw_length[index] == '\n') {
            break;
        }
        index++;
    }
    uint32_t length_out = strtol(raw_length, NULL, 10);

    //Read input
    uint8_t input_buffer [length_in];
    index = 0;
    while (index < length_in) {
        input_buffer[index] = getchar();
        index++;
    }
    blinkLED(2);
    printf("ack: input\n");

    //Prediction on FPGA
    uint8_t output_buffer [length_out];
    modelPredict(input_buffer, length_in, output_buffer, length_out);

    //send output
    printf("result: ");
    for (index = 0; index < length_out; index++) {
        printf("%02X", output_buffer[index]);
    }
    printf("\n");
}

void sendConfig(void) {
    printf("page: %i\n", FLASH_BYTES_PER_PAGE);
    printf("sector: %i\n", FLASH_BYTES_PER_SECTOR);
}

_Noreturn void run(void) {
    while(1) {
        char input = getchar_timeout_us(UINT32_MAX);
        switch (input) {
        case 'F' :
            env5HwControllerFpgaPowersOn();
            break;
        case 'f':
            env5HwControllerFpgaPowersOff();
            break;
        case 'b':
            printf("ack: start erase and load binfile\n");
            eraseAndLoadBinFile();
            break;
        case 't':
            runTest();
            break;
        case 'c':
            sendConfig();
            break;
        case 'l':
            blinkLED(5);
            break;
        default:
            break;
        }
    }
}

int main(void) {
    initHardware();
    run();
}