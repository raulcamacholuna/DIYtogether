/*
  Fichero: ./components/diymon_bsp/WS1.9TS/bsp_qmi8658.c
  Fecha: 13/08/2025 - 12:03 
  Último cambio: Eliminadas las funciones de interrupción no utilizadas.
  Descripción: Driver del IMU. Se han eliminado las implementaciones de las funciones
               de interrupción (bsp_imu_interrupt_init, bsp_imu_enable_motion_interrupt,
               bsp_imu_clear_interrupt) para limpiar el código, ya que la detección
               de 'shake' ahora se realiza por sondeo en la capa de la UI.
*/
#include "bsp_api.h"
#include "bsp_qmi8658.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "bsp_qmi8658";

#define IMU_I2C_ADDRESS  0x6B
#define I2C_CLK_SPEED_HZ 400000

static i2c_master_dev_handle_t g_imu_dev_handle = NULL;

// --- Funciones de ayuda estáticas ---
static void qmi8658_on_demand_cali(void) {
    ESP_LOGI(TAG, "Performing on-demand calibration...");
    
    uint8_t reset_buf[] = {QMI8658_RESET, 0xb0};
    ESP_ERROR_CHECK(i2c_master_transmit(g_imu_dev_handle, reset_buf, sizeof(reset_buf), pdMS_TO_TICKS(100)));
    vTaskDelay(pdMS_TO_TICKS(20));

    uint8_t cali_cmd_buf[] = {QMI8658_CTRL9, QMI8658_CTRL9_CMD_ON_DEMAND_CALI};
    ESP_ERROR_CHECK(i2c_master_transmit(g_imu_dev_handle, cali_cmd_buf, sizeof(cali_cmd_buf), pdMS_TO_TICKS(100)));
    
    vTaskDelay(pdMS_TO_TICKS(2200));

    uint8_t nop_cmd_buf[] = {QMI8658_CTRL9, QMI8658_CTRL9_CMD_NOP};
    ESP_ERROR_CHECK(i2c_master_transmit(g_imu_dev_handle, nop_cmd_buf, sizeof(nop_cmd_buf), pdMS_TO_TICKS(100)));
    vTaskDelay(pdMS_TO_TICKS(100));

    ESP_LOGI(TAG, "On-demand calibration finished.");
}


// --- Funciones públicas ---

esp_err_t bsp_imu_init(void)
{
    ESP_LOGI(TAG, "Initializing IMU QMI8658 with Modern I2C API...");
    
    i2c_master_bus_handle_t bus_handle = bsp_get_i2c_bus_handle();
    if (bus_handle == NULL) {
        ESP_LOGE(TAG, "I2C bus handle is not initialized!");
        return ESP_FAIL;
    }

    i2c_device_config_t dev_cfg = {
        .device_address = IMU_I2C_ADDRESS,
        .scl_speed_hz = I2C_CLK_SPEED_HZ,
    };
    
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &g_imu_dev_handle));
    ESP_LOGI(TAG, "IMU device added to I2C bus successfully.");

    uint8_t who_am_i = 0;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(g_imu_dev_handle, 
                                                (uint8_t[]){QMI8658_WHO_AM_I}, 1, 
                                                &who_am_i, 1, 
                                                pdMS_TO_TICKS(100)));

    if (who_am_i != 0x05) {
        ESP_LOGE(TAG, "QMI8658 not found! WhoAmI check failed. Read value: 0x%02X", who_am_i);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "QMI8658 found successfully! WhoAmI: 0x%02X", who_am_i);

    qmi8658_on_demand_cali();
    
    uint8_t ctrl1_buf[] = {QMI8658_CTRL1, 0x60}; // Habilitar Address Auto-Increment
    ESP_ERROR_CHECK(i2c_master_transmit(g_imu_dev_handle, ctrl1_buf, sizeof(ctrl1_buf), pdMS_TO_TICKS(100)));

    uint8_t ctrl_regs_data[][2] = {
        {QMI8658_CTRL7, 0x03}, // Habilitar Accel y Gyro
        {QMI8658_CTRL2, 0x23}, // Accel: +-8g, 250Hz ODR
        {QMI8658_CTRL3, 0x53}  // Gyro:  +-1024dps, 250Hz ODR
    };

    for (int i = 0; i < sizeof(ctrl_regs_data) / sizeof(ctrl_regs_data[0]); i++) {
        ESP_ERROR_CHECK(i2c_master_transmit(g_imu_dev_handle, ctrl_regs_data[i], 2, pdMS_TO_TICKS(100)));
    }
    
    ESP_LOGI(TAG, "IMU initialized.");
    return ESP_OK;
}

void bsp_imu_read(float acc[3], float gyro[3])
{
    if (g_imu_dev_handle == NULL) {
        ESP_LOGE(TAG, "IMU device handle not initialized, cannot read.");
        acc[0] = acc[1] = acc[2] = 0;
        gyro[0] = gyro[1] = gyro[2] = 0;
        return;
    }

    uint8_t buf_reg[12];
    short raw_acc_xyz[3];
    short raw_gyro_xyz[3];

    esp_err_t ret = i2c_master_transmit_receive(g_imu_dev_handle,
                                (uint8_t[]){QMI8658_AX_L}, 1,
                                buf_reg, 12,
                                pdMS_TO_TICKS(100));

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read IMU data via I2C");
        acc[0] = acc[1] = acc[2] = 0;
        gyro[0] = gyro[1] = gyro[2] = 0;
        return;
    }

    raw_acc_xyz[0] = (int16_t)((buf_reg[1] << 8) | buf_reg[0]);
    raw_acc_xyz[1] = (int16_t)((buf_reg[3] << 8) | buf_reg[2]);
    raw_acc_xyz[2] = (int16_t)((buf_reg[5] << 8) | buf_reg[4]);
    raw_gyro_xyz[0] = (int16_t)((buf_reg[7] << 8) | buf_reg[6]);
    raw_gyro_xyz[1] = (int16_t)((buf_reg[9] << 8) | buf_reg[8]);
    raw_gyro_xyz[2] = (int16_t)((buf_reg[11] << 8) | buf_reg[10]);

    // Conversión a m/s^2 (+-8g)
    acc[0] = ((float)raw_acc_xyz[0] / 4096.0f) * 9.81f;
    acc[1] = ((float)raw_acc_xyz[1] / 4096.0f) * 9.81f;
    acc[2] = ((float)raw_acc_xyz[2] / 4096.0f) * 9.81f;
    
    // Conversión a dps (+-1024dps)
    gyro[0] = (float)raw_gyro_xyz[0] / 32.0f;
    gyro[1] = (float)raw_gyro_xyz[1] / 32.0f;
    gyro[2] = (float)raw_gyro_xyz[2] / 32.0f;
}
