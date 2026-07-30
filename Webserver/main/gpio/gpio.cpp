#include "gpio/gpio.h"
#include "storage/storage.h"

#include "driver/gpio.h"
#include "esp_log.h"


static const char *TAG = "GPIO_CONTROL";


/* =========================================================
 * PIN DEFINITIONS
 * ========================================================= */

/*
 * External control inputs:
 *
 * GPIO5  -> Control 1
 * GPIO18 -> Control 2
 *
 * Hardware logic:
 * HIGH = idle / logical OFF
 * LOW  = active / logical ON
 */
#define CONTROL_INPUT_1    GPIO_NUM_5
#define CONTROL_INPUT_2    GPIO_NUM_18


/*
 * Relay outputs:
 *
 * GPIO14 -> Relay 1
 * GPIO13 -> Relay 2
 *
 * Relay hardware is ACTIVE LOW:
 *
 * LOW  = Relay ON
 * HIGH = Relay OFF
 */
#define RELAY_1_PIN        GPIO_NUM_14
#define RELAY_2_PIN        GPIO_NUM_13


/*
 * LED outputs:
 *
 * GPIO2  -> LED 1
 * GPIO15 -> LED 2
 *
 * LED hardware is ACTIVE HIGH:
 *
 * HIGH = LED ON
 * LOW  = LED OFF
 */
#define LED_1_PIN          GPIO_NUM_2
#define LED_2_PIN          GPIO_NUM_15


/* =========================================================
 * CURRENT LOGICAL STATUS
 * ========================================================= */

static gpio_status_t gpio_status =
{
    .gpio1_state = OUTPUT_OFF,
    .gpio2_state = OUTPUT_OFF,
    .relay1_count = 0,
    .relay2_count = 0
};


/* =========================================================
 * PHYSICAL OUTPUT 1
 * ========================================================= */

static void set_output_1(
    output_state_t state)
{
    if (state == OUTPUT_ON)
    {
        /*
         * Logical Control 1 ON
         *
         * Relay 1 -> LOW  -> ON
         * LED 1   -> HIGH -> ON
         */
        gpio_set_level(
            RELAY_1_PIN,
            0
        );

        gpio_set_level(
            LED_1_PIN,
            1
        );
    }
    else
    {
        /*
         * Logical Control 1 OFF
         *
         * Relay 1 -> HIGH -> OFF
         * LED 1   -> LOW  -> OFF
         */
        gpio_set_level(
            RELAY_1_PIN,
            1
        );

        gpio_set_level(
            LED_1_PIN,
            0
        );
    }
}


/* =========================================================
 * PHYSICAL OUTPUT 2
 * ========================================================= */

static void set_output_2(
    output_state_t state)
{
    if (state == OUTPUT_ON)
    {
        /*
         * Logical Control 2 ON
         *
         * Relay 2 -> LOW  -> ON
         * LED 2   -> HIGH -> ON
         */
        gpio_set_level(
            RELAY_2_PIN,
            0
        );

        gpio_set_level(
            LED_2_PIN,
            1
        );
    }
    else
    {
        /*
         * Logical Control 2 OFF
         *
         * Relay 2 -> HIGH -> OFF
         * LED 2   -> LOW  -> OFF
         */
        gpio_set_level(
            RELAY_2_PIN,
            1
        );

        gpio_set_level(
            LED_2_PIN,
            0
        );
    }
}


/* =========================================================
 * APPLY CONTROL 1
 * ========================================================= */

static void apply_control_1(
    output_state_t state)
{
    /*
     * If the requested state is already active,
     * don't do anything.
     *
     * This prevents:
     *
     * 1. unnecessary NVS writes
     * 2. incorrect counter increments
     */
    if (gpio_status.gpio1_state == state)
    {
        return;
    }


    /*
     * Count only a real OFF -> ON transition.
     */
    if ((gpio_status.gpio1_state == OUTPUT_OFF) &&
        (state == OUTPUT_ON))
    {
        storage_increment_count(
            STORAGE_RELAY_1
        );
    }


    /*
     * Apply actual relay + LED output.
     */
    set_output_1(state);


    /*
     * Update current RAM state.
     */
    gpio_status.gpio1_state =
        state;


    /*
     * Save logical state into NVS.
     *
     * true  = ON
     * false = OFF
     */
    storage_save_state(
        STORAGE_RELAY_1,
        state == OUTPUT_ON
    );


    /*
     * Update counter value stored in RAM.
     */
    gpio_status.relay1_count =
        storage_get_count(
            STORAGE_RELAY_1
        );


    ESP_LOGI(
        TAG,
        "Control 1 changed to %s",
        state == OUTPUT_ON
            ? "ON"
            : "OFF"
    );
}


/* =========================================================
 * APPLY CONTROL 2
 * ========================================================= */

static void apply_control_2(
    output_state_t state)
{
    if (gpio_status.gpio2_state == state)
    {
        return;
    }


    /*
     * Count only OFF -> ON.
     */
    if ((gpio_status.gpio2_state == OUTPUT_OFF) &&
        (state == OUTPUT_ON))
    {
        storage_increment_count(
            STORAGE_RELAY_2
        );
    }


    /*
     * Apply actual hardware state.
     */
    set_output_2(state);


    /*
     * Update RAM.
     */
    gpio_status.gpio2_state =
        state;


    /*
     * Save state in NVS.
     */
    storage_save_state(
        STORAGE_RELAY_2,
        state == OUTPUT_ON
    );


    /*
     * Refresh counter.
     */
    gpio_status.relay2_count =
        storage_get_count(
            STORAGE_RELAY_2
        );


    ESP_LOGI(
        TAG,
        "Control 2 changed to %s",
        state == OUTPUT_ON
            ? "ON"
            : "OFF"
    );
}


/* =========================================================
 * GPIO INITIALIZATION
 * ========================================================= */

esp_err_t gpio_control_init(void)
{
    ESP_LOGI(
        TAG,
        "Initializing GPIO..."
    );


    /* =====================================================
     * CONFIGURE GPIO5 AND GPIO18 AS INPUTS
     * ===================================================== */

    gpio_config_t input_config = {};


    input_config.pin_bit_mask =
        (1ULL << CONTROL_INPUT_1) |
        (1ULL << CONTROL_INPUT_2);


    input_config.mode =
        GPIO_MODE_INPUT;


    /*
     * Inputs are normally HIGH.
     */
    input_config.pull_up_en =
        GPIO_PULLUP_ENABLE;


    input_config.pull_down_en =
        GPIO_PULLDOWN_DISABLE;


    input_config.intr_type =
        GPIO_INTR_DISABLE;


    esp_err_t err =
        gpio_config(
            &input_config
        );


    if (err != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "Input GPIO configuration failed"
        );

        return err;
    }


    /* =====================================================
     * CONFIGURE RELAYS AND LEDS AS OUTPUTS
     * ===================================================== */

    gpio_config_t output_config = {};


    output_config.pin_bit_mask =
        (1ULL << RELAY_1_PIN) |
        (1ULL << RELAY_2_PIN) |
        (1ULL << LED_1_PIN) |
        (1ULL << LED_2_PIN);


    output_config.mode =
        GPIO_MODE_OUTPUT;


    output_config.pull_up_en =
        GPIO_PULLUP_DISABLE;


    output_config.pull_down_en =
        GPIO_PULLDOWN_DISABLE;


    output_config.intr_type =
        GPIO_INTR_DISABLE;


    err =
        gpio_config(
            &output_config
        );


    if (err != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "Output GPIO configuration failed"
        );

        return err;
    }


    /* =====================================================
     * LOAD SAVED COUNTERS FROM NVS
     * ===================================================== */

    gpio_status.relay1_count =
        storage_get_count(
            STORAGE_RELAY_1
        );


    gpio_status.relay2_count =
        storage_get_count(
            STORAGE_RELAY_2
        );


    /* =====================================================
     * LOAD SAVED STATES FROM NVS
     * ===================================================== */

    bool saved_state_1 =
        storage_get_state(
            STORAGE_RELAY_1
        );


    bool saved_state_2 =
        storage_get_state(
            STORAGE_RELAY_2
        );


    gpio_status.gpio1_state =
        saved_state_1
            ? OUTPUT_ON
            : OUTPUT_OFF;


    gpio_status.gpio2_state =
        saved_state_2
            ? OUTPUT_ON
            : OUTPUT_OFF;


    /* =====================================================
     * RESTORE PHYSICAL OUTPUTS
     * =====================================================
     *
     * IMPORTANT:
     *
     * We directly call set_output_x().
     *
     * We DO NOT call apply_control_x().
     *
     * Otherwise restoring an ON state after reboot
     * could incorrectly increase the ON counter.
     */

    set_output_1(
        gpio_status.gpio1_state
    );


    set_output_2(
        gpio_status.gpio2_state
    );


    ESP_LOGI(
        TAG,
        "Control 1 restored: %s",
        saved_state_1
            ? "ON"
            : "OFF"
    );


    ESP_LOGI(
        TAG,
        "Control 2 restored: %s",
        saved_state_2
            ? "ON"
            : "OFF"
    );


    ESP_LOGI(
        TAG,
        "Relay 1 ON count: %lu",
        (unsigned long)
            gpio_status.relay1_count
    );


    ESP_LOGI(
        TAG,
        "Relay 2 ON count: %lu",
        (unsigned long)
            gpio_status.relay2_count
    );


    ESP_LOGI(
        TAG,
        "GPIO initialization complete"
    );


    return ESP_OK;
}


/* =========================================================
 * READ PHYSICAL GPIO INPUTS
 * ========================================================= */

void gpio_control_update_inputs(void)
{
    /*
     * Store previous physical input states.
     *
     * GPIO5/GPIO18 are normally HIGH.
     * We only take action when an input changes
     * from HIGH -> LOW.
     */
    static int previous_input1 = 1;
    static int previous_input2 = 1;


    /* Read current physical GPIO levels. */

    int current_input1 =
        gpio_get_level(
            CONTROL_INPUT_1
        );

    int current_input2 =
        gpio_get_level(
            CONTROL_INPUT_2
        );


    /* =====================================================
     * CONTROL 1
     * =====================================================
     *
     * Detect:
     *
     * HIGH -> LOW
     *
     * This means the physical control was activated.
     */

    if ((previous_input1 == 1) &&
        (current_input1 == 0))
    {
        /*
         * Toggle current logical state.
         */

        output_state_t new_state =
            (gpio_status.gpio1_state == OUTPUT_ON)
                ? OUTPUT_OFF
                : OUTPUT_ON;


        ESP_LOGI(
            TAG,
            "Physical GPIO5 activated -> Control 1 %s",
            new_state == OUTPUT_ON
                ? "ON"
                : "OFF"
        );


        apply_control_1(
            new_state
        );
    }


    /* =====================================================
     * CONTROL 2
     * ===================================================== */

    if ((previous_input2 == 1) &&
        (current_input2 == 0))
    {
        output_state_t new_state =
            (gpio_status.gpio2_state == OUTPUT_ON)
                ? OUTPUT_OFF
                : OUTPUT_ON;


        ESP_LOGI(
            TAG,
            "Physical GPIO18 activated -> Control 2 %s",
            new_state == OUTPUT_ON
                ? "ON"
                : "OFF"
        );


        apply_control_2(
            new_state
        );
    }


    /*
     * Remember current physical states for
     * next iteration.
     */

    previous_input1 =
        current_input1;

    previous_input2 =
        current_input2;
}


/* =========================================================
 * DASHBOARD CONTROL 1
 * ========================================================= */

void gpio_control_set_1(
    output_state_t state)
{
    apply_control_1(
        state
    );
}


/* =========================================================
 * DASHBOARD CONTROL 2
 * ========================================================= */

void gpio_control_set_2(
    output_state_t state)
{
    apply_control_2(
        state
    );
}


/* =========================================================
 * GET CURRENT STATUS
 * ========================================================= */

gpio_status_t gpio_control_get_status(void)
{
    /*
     * Refresh counters from NVS.
     */

    gpio_status.relay1_count =
        storage_get_count(
            STORAGE_RELAY_1
        );


    gpio_status.relay2_count =
        storage_get_count(
            STORAGE_RELAY_2
        );


    return gpio_status;
}