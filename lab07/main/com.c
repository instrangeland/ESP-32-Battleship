#include "hw.h"
#include "pin.h"
#include "com.h"
#include "esp_log.h"
#include "driver/uart.h"

// Define the UART port number
const uart_port_t PORT_NUM = UART_NUM_2;

// Define the buffer size and baud rate for the UART communication.
#define BUFFER_SIZE UART_HW_FIFO_LEN(PORT_NUM)*2
#define BAUD_RATE 115200

// Initialize the communication channel.
// Return zero if successful, or non-zero otherwise.
int32_t com_init(void){

    // Configure UART communication parameters.
    uart_config_t uart_config = {
        .baud_rate = BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT
    };
    ESP_ERROR_CHECK(uart_param_config(PORT_NUM, &uart_config));

    // Configure pin EX7 to have a pull-up resistor.
    pin_pullup(HW_EX7, true);

    // Set UART pins for transmitting and receiving.
    ESP_ERROR_CHECK(uart_set_pin(PORT_NUM, HW_EX8, HW_EX7, 0, 0));

    // Install UART driver on the port number, with a buffer size of 2 times the FIFO length,
    // and no queue or allocation flags.
    ESP_ERROR_CHECK(uart_driver_install(PORT_NUM, BUFFER_SIZE, 0, 0, NULL, 0));

    return 0;
}

// Free resources used for communication.
// Return zero if successful, or non-zero otherwise.
int32_t com_deinit(void){
    // Check to see if the UART driver is installed. If it is, delete the driver.
    // If is is installed or already uninstalled, return 0. Otherwise, return -1.
    if(uart_is_driver_installed(PORT_NUM)){
        ESP_ERROR_CHECK(uart_driver_delete(PORT_NUM));
        return 0;
    } else if(!uart_is_driver_installed(PORT_NUM)){
        return 0;
    } else {
        return -1;
    }
}

// Write data to the communication channel. Does not wait for data.
// *buf: pointer to data buffer
// size: size of data in bytes to write
// Return number of bytes written, or negative number if error.
int32_t com_write(const void *buf, uint32_t size){
    int32_t bytes_written = uart_tx_chars(PORT_NUM, buf, size);
    return bytes_written;
}

// Read data from the communication channel. Does not wait for data.
// *buf: pointer to data buffer
// size: size of data in bytes to read
// Return number of bytes read, or negative number if error.
int32_t com_read(void *buf, uint32_t size){
    int32_t bytes_read = uart_read_bytes(PORT_NUM, buf, size, 0);
    return bytes_read;
}
