/*  Temperature Sensor demo implementation using RGB LED and timer

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <sdkconfig.h>
#include <esp_rmaker_core.h>
#include <esp_rmaker_standard_types.h> 
#include <esp_rmaker_standard_params.h> 

#include <ultrasonic.h>
#include <esp_err.h>

#include <app_reset.h>
#include <ws2812_led.h>
#include "app_priv.h"

 // This is the button that is used for toggling the power 
#define BUTTON_GPIO          CONFIG_EXAMPLE_BOARD_BUTTON_GPIO
#define BUTTON_ACTIVE_LEVEL  0
// This is the GPIO on which the power will be set 
#define OUTPUT_GPIO    19 

static TimerHandle_t sensor_timer;

#define DEFAULT_SATURATION  100
#define DEFAULT_BRIGHTNESS  50

#define WIFI_RESET_BUTTON_TIMEOUT       3
#define FACTORY_RESET_BUTTON_TIMEOUT    10

static uint16_t g_hue;
static uint16_t g_saturation = DEFAULT_SATURATION;
static uint16_t g_value = DEFAULT_BRIGHTNESS;
static float g_temperature;

static void app_sensor_update(TimerHandle_t handle)
{
    static float delta = 0.5;
    g_temperature += delta;
    if (g_temperature > 99) {
        delta = -0.5;
    } else if (g_temperature < 1) {
        delta = 0.5;
    }
    g_hue = (100 - g_temperature) * 2;
    ws2812_led_set_hsv(g_hue, g_saturation, g_value);
    esp_rmaker_param_update_and_report(
            esp_rmaker_device_get_param_by_type(US_sensor_device, ESP_RMAKER_PARAM_DISTance),
            esp_rmaker_float(g_temperature));
}

/* float app_get_current_temperature()
{
    return g_temperature;
} */

void ultrasonic_test(void *pvParameters)
{
    ultrasonic_sensor_t sensor = {
        .trigger_pin = TRIGGER_GPIO,
        .echo_pin = ECHO_GPIO
    };

    ultrasonic_init(&sensor);

    while (true)
    {
        float distance;
        esp_err_t res = ultrasonic_measure(&sensor, MAX_DISTANCE_CM, &distance);
        if (res != ESP_OK)
        {
            printf("Error %d: ", res);
            switch (res)
            {
                case ESP_ERR_ULTRASONIC_PING:
                    printf("Cannot ping (device is in invalid state)\n");
                    break;
                case ESP_ERR_ULTRASONIC_PING_TIMEOUT:
                    printf("Ping timeout (no device found)\n");
                    break;
                case ESP_ERR_ULTRASONIC_ECHO_TIMEOUT:
                    printf("Echo timeout (i.e. distance too big)\n");
                    break;
                default:
                    printf("%s\n", esp_err_to_name(res));
            }
        }
        else
            printf("Distance: %0.04f cm\n", distance*100);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

esp_err_t app_sensor_init(void)
{
    esp_err_t err = ws2812_led_init();
    if (err != ESP_OK) {
        return err;
    }

    g_temperature = DEFAULT_DIST;
    sensor_timer = xTimerCreate("app_sensor_update_tm", (REPORTING_PERIOD * 1000) / portTICK_PERIOD_MS,
                            pdTRUE, NULL, app_sensor_update);
    if (sensor_timer) {
        xTimerStart(sensor_timer, 0);
        g_hue = (100 - g_temperature) * 2;
        ws2812_led_set_hsv(g_hue, g_saturation, g_value);
        return ESP_OK;
    }
    return ESP_FAIL;
}

void app_driver_init()
{
    app_sensor_init();
    app_reset_button_register(app_reset_button_create(BUTTON_GPIO, BUTTON_ACTIVE_LEVEL),
                WIFI_RESET_BUTTON_TIMEOUT, FACTORY_RESET_BUTTON_TIMEOUT);
}

