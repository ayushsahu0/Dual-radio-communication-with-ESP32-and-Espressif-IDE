# ESP1 Tutorial

This tutorial provides a step-by-step guide to create an ESP32 application that initializes a boot button, generates a random number, and sets up BLE (Bluetooth Low Energy) and UDP (User Datagram Protocol) functionalities.
![FlowChart of the Dual-radio-communication-with-ESP32-and-Espressif-IDE](https://github.com/ayushsahu0/Dual-radio-communication-with-ESP32-and-Espressif-IDE/blob/main/esp1/tutorial/ESP1.png)

## Prerequisites

To follow this tutorial, you need:

- ESP32 development board
- ESP-IDF development framework installed
- Basic knowledge of C programming
- Understanding of GPIO and networking concepts

## Setting Up the Project

1. **Create a New Project:**
   Start by creating a new project in your ESP-IDF environment. Make sure the necessary components (`ble`, `wifi`, `esp_err`, etc.) are included in your `sdkconfig.h`.

2. **Include Necessary Libraries and Definitions :**
   Ensure the following libraries are included at the beginning of your main source file (`main.c` or `main.cpp`):

   ```c
   #include <stdio.h>
   #include <stdlib.h>
   #include <string.h>
   #include "driver/gpio.h"   // GPIO driver library
   #include "sdkconfig.h"
   #include "ble.h"          // Bluetooth Low Energy library
   #include "wifi.h"         // WiFi library
   #include "esp_err.h"
   #define BOOT_BUTTON_PIN GPIO_NUM_0 // GPIO pin connected to the boot button
   #define MAINTAG "MAIN"  // Tag for logging
   char buffer[1024]; // Buffer to hold generated data

-   **Includes**: Standard C libraries (`stdio.h`,  `stdlib.h`,  `string.h`) and ESP32-specific headers (`gpio.h`,  `sdkconfig.h`,  `ble.h`,  `wifi.h`,  `esp_err.h`) are included for necessary functionalities.
-   **Definitions**:
    -   `BOOT_BUTTON_PIN`: Defines the GPIO pin number connected to the boot button (in this case, GPIO 0).
    -   `MAINTAG`: Tag used for logging messages.
    -   `buffer`: A character array of size 1024 used for storing data.
    
## Implementing the Delay Function

The  `delay()`  function waits for the boot button to be pressed before continuing with program execution

 ```c
static void random_no_Generator(char *payload, size_t payload_size) {
    if (payload == NULL || payload_size == 0) {
        ESP_LOGE(MAINTAG, "Invalid buffer or size");
        return;
    }

    // Generate a 32-bit random number
    uint32_t random_no = esp_random();

    // Extract the last 2 digits of the random number
    uint8_t rem = random_no % 100;

    // Log the random number to debug
    ESP_LOGI(MAINTAG, "The Random no is: %u", rem);

    // Format the random number as a two-digit string
    int result = snprintf(payload, payload_size, "%02u", rem);

    // Check for errors or truncation during formatting
    if (result < 0) {
        ESP_LOGE(MAINTAG, "Failed to format the random number");
    } else if ((size_t)result >= payload_size) {
        ESP_LOGW(MAINTAG, "Payload truncated: buffer too small");
    }
}

 ```
**Explanation:**

-   **GPIO Configuration (`gpio_config_t`):**  Initializes the GPIO pin connected to the boot button. It sets it as an input with a pull-up resistor enabled to ensure stability when the button is not pressed.
    
-   **Waiting for Button Press:**  The function enters a loop (`while`) and continuously checks the state of the GPIO pin (`BOOT_BUTTON_PIN`). It waits until the button is pressed (`gpio_get_level(BOOT_BUTTON_PIN) == 0`), assuming  `1`indicates the button is not pressed (due to the pull-up resistor configuration).
    
-   **Delay:**  Uses  `vTaskDelay()`  to introduce a short delay between each check (`100 / portTICK_PERIOD_MS`), reducing CPU usage during the waiting period.


## Generating and Formatting a Random Number

The  `random_no_Generator()`  function generates a random number and formats it as a string:

```c
static void random_no_Generator(char *payload, size_t payload_size) {
    if (payload == NULL || payload_size == 0) {
        ESP_LOGE(MAINTAG, "Invalid buffer or size");
        return;
    }

    // Generate a 32-bit random number
    uint32_t random_no = esp_random();

    // Extract the last 2 digits of the random number
    uint8_t rem = random_no % 100;

    // Log the random number to debug
    ESP_LOGI(MAINTAG, "The Random no is: %u", rem);

    // Format the random number as a two-digit string
    int result = snprintf(payload, payload_size, "%02u", rem);

    // Check for errors or truncation during formatting
    if (result < 0) {
        ESP_LOGE(MAINTAG, "Failed to format the random number");
    } else if ((size_t)result >= payload_size) {
        ESP_LOGW(MAINTAG, "Payload truncated: buffer too small");
    }
}
```

**Explanation:**

  

1. **Parameter Check:**

•  The function first checks if the payload buffer is NULL or if `payload_size` is zero. If either condition is true, it logs an error and exits the function.

2. **Random Number Generation:**

•  The function uses `esp_random()` to generate a random number. This function utilizes the ESP32’s hardware random number generator.

3. **Extracting the Last Two Digits:**

•  The last two digits of the random number are obtained using the modulo operation (`random_no % 100`).

4. **Logging the Random Number:**

•  The function logs the generated random number using `ESP_LOGI`.

5. **Formatting the Number:**

•  The random number is formatted as a two-digit string and stored in the payload buffer using `snprintf`.

6. **Error Handling:**

•  The function checks if `snprintf` was successful. If not, it logs an error. If the buffer size was too small, it logs a warning.

  

## App_Main Function 


The app_main function is the main entry point of the program. It initializes the system, generates a random number, initializes BLE, and sets up a UDP server.

```c
void app_main() {
    delay(); // Ensure any necessary initialization is done

    // Generate a random number and store it in the buffer
    random_no_Generator(buffer, sizeof(buffer));

    // Initialize BLE (Bluetooth Low Energy)
    esp_err_t ble_result = ble_init(buffer, sizeof(buffer));
    if (ble_result == ESP_OK) {
        // Log success message if BLE initialization is successful
        ESP_LOGI(TAG, "BLE initialization successful.");

        // Initialize UDP
        esp_err_t udp_result = udp_init_();
        if (udp_result == ESP_OK) {
            // Log success message if UDP initialization is successful
            ESP_LOGI(TAG, "UDP initialization successful.");

            // Create a FreeRTOS task to handle UDP server operations
            if (xTaskCreate(udp_server_task, "udp_server_task", 4096, buffer, 5, NULL) != pdPASS) {
                // Log an error if the task creation fails
                ESP_LOGE(TAG, "Failed to create UDP server task.");
            } else {
                // Log success message if the task is created successfully
                ESP_LOGI(TAG, "UDP server task started.");
            }
        } else {
            // Log an error if UDP initialization fails
            ESP_LOGE(TAG, "UDP initialization failed.");
        }
    } else {
        // Log an error if BLE initialization fails
        ESP_LOGE(TAG, "BLE initialization failed.");
    }

    // Log a message indicating that this point in the code is reached
    ESP_LOGI(TAG, "I am Here");
}
```

**Explanation:**

  

1. **Button Press Delay:**

•  The function calls `delay()` to wait for the boot button to be pressed before continuing. This ensures that any necessary initialization is done before proceeding.

2. **Random Number Generation:**

•  The function calls `random_no_Generator(buffer, sizeof(buffer))` to generate a random number and store it in the buffer.

3. **BLE Initialization:**

•  The function initializes BLE by calling `ble_init(buffer, sizeof(buffer))`. The buffer contains the random number generated earlier.

•  If BLE initialization is successful `(ble_result == ESP_OK)`, it logs a success message.

4. **UDP Initialization:**

•  The function initializes the UDP server by calling `udp_init_()`.

•  If UDP initialization is successful `(udp_result == ESP_OK)`, it logs a success message.

•  The function then attempts to create a FreeRTOS task for the UDP server using `xTaskCreate()`.

•  If task creation is successful, it logs a success message. If it fails, it logs an error.

5. **Logging Final Message:**

•  The function logs a final message indicating that this point in the code has been reached. This helps in verifying that the function executed as expected.

## ble_GATT_client 

A BLE (Bluetooth Low Energy) GATT (Generic Attribute Profile) Client using the ESP-IDF framework for an ESP32 device. The code includes various callbacks and handlers to manage BLE scanning, connecting to a specific GATT server, and handling notifications and data exchange. 

**1. esp_gap_cb**

```c
static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    switch (event) {
    case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT:
        esp_ble_gap_start_scanning(30);
        break;
    case ESP_GAP_BLE_SCAN_RESULT_EVT:
        if (param->scan_rst.search_evt == ESP_GAP_SEARCH_INQ_RES_EVT) {
            if (strlen((const char *)param->scan_rst.ble_adv) == strlen(remote_device_name)) {
                if (strcmp((const char *)param->scan_rst.ble_adv, remote_device_name) == 0) {
                    esp_ble_gap_stop_scanning();
                    esp_ble_gattc_open(gl_profile_tab[PROFILE_A_APP_ID].gattc_if, param->scan_rst.bda, true);
                }
            }
        }
        break;
    case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT:
        break;
    default:
        break;
    }
}
```

•  **Purpose**: Handles GAP (Generic Access Profile) events, which include scanning and connection management.

•  **`ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT`**: Triggered when scan parameters are set. The function starts scanning for 30 seconds.

•  **`ESP_GAP_BLE_SCAN_RESULT_EVT`**: Triggered when a scan result is received. Checks if the scanned device matches the target device name. If it matches, it stops scanning and opens a GATT connection to the device.

•  **`ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT`**: Triggered when scanning stops. No specific action is taken in this case.

**2. esp_gattc_cb**

```c
static void esp_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param) {
    switch (event) {
    case ESP_GATTC_REG_EVT:
        esp_ble_gap_set_scan_params(&ble_scan_params);
        break;
    case ESP_GATTC_CONNECT_EVT:
        esp_ble_gattc_search_service(gattc_if, param->connect.conn_id, NULL);
        break;
    case ESP_GATTC_DISCONNECT_EVT:
        break;
    case ESP_GATTC_SEARCH_RES_EVT:
        if (param->search_res.srvc_id.id.uuid.uuid.uuid16 == REMOTE_SERVICE_UUID) {
            gl_profile_tab[PROFILE_A_APP_ID].service_start_handle = param->search_res.start_handle;
            gl_profile_tab[PROFILE_A_APP_ID].service_end_handle = param->search_res.end_handle;
            get_server = true;
        }
        break;
    case ESP_GATTC_SEARCH_CMPL_EVT:
        if (get_server) {
            esp_ble_gattc_get_characteristic(gattc_if, param->search_cmpl.conn_id, gl_profile_tab[PROFILE_A_APP_ID].service_start_handle, NULL);
        }
        break;
    case ESP_GATTC_GET_CHAR_EVT:
        if (param->get_char.char_id.uuid.uuid.uuid16 == REMOTE_NOTIFY_CHAR_UUID) {
            gl_profile_tab[PROFILE_A_APP_ID].char_handle = param->get_char.char_handle;
            register_for_notify(gattc_if);
        }
        break;
    case ESP_GATTC_NOTIFY_EVT:
        printf("Notify Value: ");
        for (int i = 0; i < param->notify.value_len; i++) {
            printf("%02x", param->notify.value[i]);
        }
        printf("\n");
        break;
    default:
        break;
    }
}
```
•  **Purpose**: Handles GATT (Generic Attribute Profile) client events related to service discovery, characteristic handling, and notifications.

•  **`ESP_GATTC_REG_EVT`**: Triggered when the GATT client is registered. Starts scanning for BLE devices.

•  **`ESP_GATTC_CONNECT_EVT`**: Triggered when a connection to a BLE server is established. Initiates a service search on the connected device.

•  **`ESP_GATTC_DISCONNECT_EVT`**: Triggered when a disconnection occurs. No specific action is taken in this case.

•  **`ESP_GATTC_SEARCH_RES_EVT`**: Triggered when a service is found during the service discovery process. If the service UUID matches the target UUID, it records the start and end handles of the service.

•  **`ESP_GATTC_SEARCH_CMPL_EVT`**: Triggered when the service search is complete. If the target service was found, it starts searching for characteristics within that service.

•  **`ESP_GATTC_GET_CHAR_EVT`**: Triggered when characteristics are retrieved. If the characteristic UUID matches the target UUID, it records the handle and registers for notifications on that characteristic.

•  **`ESP_GATTC_NOTIFY_EVT`**: Triggered when a notification is received from the BLE server. It prints the notification data to the console.

**3. register_for_notify**
```c
static void register_for_notify(esp_gatt_if_t gattc_if) {
    esp_ble_gattc_register_for_notify(gattc_if, gl_profile_tab[PROFILE_A_APP_ID].remote_bda, gl_profile_tab[PROFILE_A_APP_ID].char_handle);
}
```
•  **Purpose**: Registers for notifications on a specific characteristic.

•  **Parameters**:   gattc_if: The GATT client interface.

•  **Function**: Registers the client to receive notifications for the characteristic identified by `char_handle` from the remote device with `remote_bda`.

**Summary**

•  **`esp_gap_cb`** handles BLE scanning and connection setup.
•  **`esp_gattc_cb`** manages GATT service discovery, characteristic handling, and notifications.
•  **`register_for_notify`** enables notifications for a specific characteristic from the remote device.

## wifi_udp_server

**1. Header Guards and Includes**
```c
#ifndef WIFI_H
#define WIFI_H
```
•  **Header Guards**: Prevent multiple inclusions of this header file, which can lead to compilation errors.
```c
#include <unistd.h>
#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include <esp_wifi.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
```

•  **Includes**: These headers provide various functionalities:
•  `unistd.h, string.h, sys/param.h`: Standard C library functions.
•  `freertos/FreeRTOS.h`, `freertos/task.h`, `freertos/event_groups.h`: FreeRTOS functions for task management and event groups.
•  `esp_system.h`, `esp_event.h`, `esp_log.h`, `nvs_flash.h`, `esp_netif.h`: ESP-IDF system functions, event handling, logging, and network interface functions.
•  `protocol_examples_common.h`: Example code for common protocol use.
•  `lwip/err.h`, `lwip/sockets.h`, `lwip/sys.h`, `lwip/netdb.h`: lwIP functions for network and socket operations.
•  `esp_wifi.h`: Wi-Fi driver functions.
•  `arpa/inet.h`, `netinet/in.h`, `errno.h`: Networking and error handling functions.

  

**2. Configuration Macros**
```c
#ifdef CONFIG_EXAMPLE_SOCKET_IP_INPUT_STDIN
#include "addr_from_stdin.h"
#endif

#if defined(CONFIG_EXAMPLE_IPV4)
#define HOST_IP_ADDR CONFIG_EXAMPLE_IPV4_ADDR
#elif defined(CONFIG_EXAMPLE_IPV6)
#define HOST_IP_ADDR CONFIG_EXAMPLE_IPV6_ADDR
#else
#define HOST_IP_ADDR ""
#endif

#define PORT CONFIG_EXAMPLE_PORT

#define TAG  "UDP_Server"
```
•  **Configuration Macros**: Define the IP address and port based on configuration settings. If the address is specified via standard input, include an additional header file. The TAG macro is used for logging.

**3. udp_server_task**
```C
static void udp_server_task(void *buffer, size_t buffer_size)
{
    // Buffer to hold received data
    char *char_buffer = (char *)buffer; // Cast void * to char *

    // Variables for address family and IP protocol
    int addr_family = 0;
    int ip_protocol = 0;

    // Port where the client will listen
    int client_port = 3333;
  ```
  •  **Task Parameters**: Defines a task that handles UDP server operations. It receives a buffer for storing incoming data.
```C
  #if defined(CONFIG_EXAMPLE_IPV4)
    // Configuration for IPv4
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(HOST_IP_ADDR); // Destination IP address
    dest_addr.sin_family = AF_INET; // Address family for IPv4
    dest_addr.sin_port = htons(client_port); // Port number in network byte order
    addr_family = AF_INET;
    ip_protocol = IPPROTO_IP;
#elif defined(CONFIG_EXAMPLE_IPV6)
    // Configuration for IPv6
    struct sockaddr_in6 dest_addr = { 0 };
    inet6_aton(HOST_IP_ADDR, &dest_addr.sin6_addr); // Destination IP address for IPv6
    dest_addr.sin6_family = AF_INET6; // Address family for IPv6
    dest_addr.sin6_port = htons(client_port); // Port number in network byte order
    dest_addr.sin6_scope_id = esp_netif_get_netif_impl_index(EXAMPLE_INTERFACE); // Scope ID
    addr_family = AF_INET6;
    ip_protocol = IPPROTO_IPV6;
#elif defined(CONFIG_EXAMPLE_SOCKET_IP_INPUT_STDIN)
    // Configuration for destination address from standard input
    struct sockaddr_storage dest_addr = { 0 };
    ESP_ERROR_CHECK(get_addr_from_stdin(client_port, SOCK_DGRAM, &ip_protocol, &addr_family, &dest_addr));
#endif
```

```C
#if defined(CONFIG_EXAMPLE_IPV4)
    // Configuration for IPv4
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(HOST_IP_ADDR); // Destination IP address
    dest_addr.sin_family = AF_INET; // Address family for IPv4
    dest_addr.sin_port = htons(client_port); // Port number in network byte order
    addr_family = AF_INET;
    ip_protocol = IPPROTO_IP;
#elif defined(CONFIG_EXAMPLE_IPV6)
    // Configuration for IPv6
    struct sockaddr_in6 dest_addr = { 0 };
    inet6_aton(HOST_IP_ADDR, &dest_addr.sin6_addr); // Destination IP address for IPv6
    dest_addr.sin6_family = AF_INET6; // Address family for IPv6
    dest_addr.sin6_port = htons(client_port); // Port number in network byte order
    dest_addr.sin6_scope_id = esp_netif_get_netif_impl_index(EXAMPLE_INTERFACE); // Scope ID
    addr_family = AF_INET6;
    ip_protocol = IPPROTO_IPV6;
#elif defined(CONFIG_EXAMPLE_SOCKET_IP_INPUT_STDIN)
    // Configuration for destination address from standard input
    struct sockaddr_storage dest_addr = { 0 };
    ESP_ERROR_CHECK(get_addr_from_stdin(client_port, SOCK_DGRAM, &ip_protocol, &addr_family, &dest_addr));
#endif
```
•  **Address Configuration**: Configures the address family (IPv4 or IPv6) and the destination address based on the build configuration. For IPv6, additional scope ID is set.

```C
    // Create a UDP socket
    int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
    if (sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        vTaskDelete(NULL); // Delete the task if socket creation fails
    }
```
•  **Socket Creation**: Creates a UDP socket with the specified address family and protocol. Logs an error and deletes the task if creation fails.

```C
    // Bind the socket to the local port
    struct sockaddr_in bind_addr;
    bind_addr.sin_family = AF_INET; // IPv4 address family
    bind_addr.sin_addr.s_addr = INADDR_ANY; // Bind to any local IP address
    bind_addr.sin_port = htons(client_port); // Port number in network byte order

    if (bind(sock, (struct sockaddr *)&bind_addr, sizeof(bind_addr)) < 0) {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        close(sock); // Close the socket on bind failure
        vTaskDelete(NULL); // Delete the task
    }
 ```
 •  **Socket Binding**: Binds the socket to the local IP address and port. If binding fails, it logs an error, closes the socket, and deletes the task.
 ```C 
    // Set timeout for receiving data
    struct timeval timeout;
    timeout.tv_sec = 10; // Timeout in seconds
    timeout.tv_usec = 0; // Timeout in microseconds
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);

    ESP_LOGI(TAG, "Socket created and bound to port %d, waiting for data...", client_port);

 ```
 •  **Timeout Configuration**: Sets a timeout for receiving data. This helps prevent the recvfrom function from blocking indefinitely.
  ```C
         // Main loop to receive data
    while (1) {
        struct sockaddr_storage source_addr; // Storage for source address (IPv4 or IPv6)
        socklen_t socklen = sizeof(source_addr);
        int len = recvfrom(sock, char_buffer, sizeof(char_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);

        // Error occurred during receiving
        if (len < 0) {
            ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
            if (errno == ENETUNREACH || errno == EHOSTUNREACH) {
                ESP_LOGE(TAG, "Network is unreachable or no route to host.");
            }
        } 
        // Data received successfully
        else {
            char_buffer[len] = 0; // Null-terminate the received data

            // Log sender's IP address and received data
            if (source_addr.ss_family == AF_INET) {
                char addr_str[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(((struct sockaddr_in *)&source_addr)->sin_addr), addr_str, sizeof(addr_str));
                ESP_LOGI(TAG, "Received %d bytes from %s:", len, addr_str);
                ESP_LOGI(TAG, "%s", char_buffer);
            } else if (source_addr.ss_family == AF_INET6) {
                char addr_str[INET6_ADDRSTRLEN];
                inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)&source_addr)->sin6_addr), addr_str, sizeof(addr_str));
                ESP_LOGI(TAG, "Received %d bytes from %s:", len, addr_str);
            }
 ```
 •  **Receiving Data**: Receives data from the socket. Logs the source address and data received. Handles errors and different address families.
   ```C
                 // Define the acknowledgment message
            const char *ack_message = "Ack_msg";

            vTaskDelay(2000 / portTICK_PERIOD_MS);

            // Send acknowledgment message back to the client
            for(int i =0; i<2 ; i++){
            int err = sendto(sock, ack_message, strlen(ack_message), 0, (struct sockaddr *)&source_addr, sizeof(source_addr));
            if (err < 0) {
                ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
            } else {
                ESP_LOGI(TAG, "Acknowledgment message sent: %s", ack_message);
            }
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            }

            // Exit the loop after sending acknowledgment
            break;
        }

        // Delay before the next receive attempt
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
 ```
 •  **Sending Acknowledgment**: Sends an acknowledgment message to the client after receiving data. Retries sending twice with a delay between attempts.
   ```C
     // Shutdown and close the socket

if (sock != -1) {

ESP_LOGI(TAG, "Shutting down socket and closing...");

shutdown(sock, 0); // Shutdown the socket

close(sock); // Close the socket
 ```
 
