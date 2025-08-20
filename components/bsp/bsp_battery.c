/* Fichero: components/bsp/bsp_battery.c */
/* Último cambio: Unificado el código de los drivers de la batería de ambas placas en un único fichero. */
/* Descripción: Este fichero contiene la lógica para la lectura del voltaje de la batería. Dado que ambas placas utilizan el mismo canal de ADC (ADC1_CHANNEL_0) y la misma lógica de calibración, el código es idéntico y se ha consolidado en este fichero único, eliminando la duplicación de código. */
/* Último cambio: 20/08/2025 - 05:06 */
#include "bsp_api.h"
#include "bsp_battery.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_log.h"

static char *TAG = "bsp_battery";

static adc_oneshot_unit_handle_t adc1_handle;
static adc_cali_handle_t adc1_cali_chan0_handle = NULL;
static bool do_calibration1_chan0;

static bool example_adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle)
{
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    if (!calibrated)
    {
        ESP_LOGD(TAG, "calibration scheme version is %s", "Curve Fitting");
        adc_cali_curve_fitting_config_t cali_config = {
            .unit_id = unit,
            .chan = channel,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);
        if (ret == ESP_OK)
        {
            calibrated = true;
        }
    }
#endif

#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    if (!calibrated)
    {
        ESP_LOGD(TAG, "calibration scheme version is %s", "Line Fitting");
        adc_cali_line_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
        if (ret == ESP_OK)
        {
            calibrated = true;
        }
    }
#endif

    *out_handle = handle;
    if (ret == ESP_OK)
    {
        ESP_LOGI(TAG, "ADC Calibration Success");
    }
    else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated)
    {
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    }
    else
    {
        ESP_LOGE(TAG, "Invalid arg or no memory for ADC calibration");
    }

    return calibrated;
}


esp_err_t bsp_battery_init(void)
{
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = EXAMPLE_ADC_UNIT,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = EXAMPLE_ADC_ATTEN,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, EXAMPLE_BATTERY_ADC_CHANNEL, &config));

    do_calibration1_chan0 = example_adc_calibration_init(EXAMPLE_ADC_UNIT, EXAMPLE_BATTERY_ADC_CHANNEL, EXAMPLE_ADC_ATTEN, &adc1_cali_chan0_handle);
    ESP_LOGI(TAG, "Battery ADC initialized.");
    return ESP_OK;
}


void bsp_battery_get_voltage(float *voltage, uint16_t *adc_value)
{
    int adc_raw;
    int voltage_int;
    
    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, EXAMPLE_BATTERY_ADC_CHANNEL, &adc_raw));
    
    if (do_calibration1_chan0)
    {
        ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc1_cali_chan0_handle, adc_raw, &voltage_int));
        // The voltage divider on the board is 1:2. The formula multiplies by 3 to calibrate and match the real voltage.
        *voltage = (voltage_int / 1000.0f) * 3.0; 

        if (adc_value)
        {
            *adc_value = adc_raw;
        }   
    }
}
