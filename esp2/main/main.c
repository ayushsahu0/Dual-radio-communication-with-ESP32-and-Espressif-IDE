#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "ble_GATT_server.h"
#include "wifi_udp_client.h"
#include <inttypes.h>
#include <stdint.h>

#define BOOT_BUTTON_PIN GPIO_NUM_0 // Change this to the actual GPIO number connected to the boot button

#define MAX_MESSAGE_SIZE 256  // Define maximum size for received_message

// Global variables to store received message and its length
// Define the global variable


static void delay (){
    // Initialize the GPIO pin as input with a pull-up resistor
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << BOOT_BUTTON_PIN);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    printf("Waiting for boot button press...\n");

    // Wait indefinitely until the boot button is pressed
    while (gpio_get_level(BOOT_BUTTON_PIN) == 1) {
        // Button is not pressed (assuming HIGH means not pressed)
        // Add a small delay to reduce CPU usage
        vTaskDelay(100 / portTICK_PERIOD_MS); // Delay for 100 milliseconds
    }

    printf("Boot button pressed! Continuing with the program...\n");
    return;
}


void app_main()
{
    delay(); // Ensure any necessary initialization is done

    // Initialize BLE (Bluetooth Low Energy)
    esp_err_t ble_result = ble_init();
    if (ble_result == ESP_OK)
    {
        ESP_LOGI(TAG, "BLE initialization successful.");

        // Initialize UDP
        esp_err_t udp_result = udp_init_();
        if (udp_result == ESP_OK)
        {
            ESP_LOGI(TAG, "UDP initialization successful.");

            // Create a FreeRTOS task to handle UDP server operations
            if (xTaskCreate(udp_client_task, "udp_client_task", 4096 / sizeof(StackType_t), NULL, 5, NULL) != pdPASS) 
            {
                ESP_LOGE(TAG, "Failed to create UDP server task.");
            }
            else
            {
                ESP_LOGI(TAG, "UDP server task started.");
            }
        }
        else
        {
            ESP_LOGE(TAG, "UDP initialization failed.");
        }
    }
    else
    {
        ESP_LOGE(TAG, "BLE initialization failed.");
    }

    // Code here should be reached if tasks were successfully created
    ESP_LOGI(TAG, "I am Here");
}


