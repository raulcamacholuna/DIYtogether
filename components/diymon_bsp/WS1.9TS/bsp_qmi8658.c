/*
  Fichero: ./components/diymon_bsp/WS1.9TS/bsp_qmi8658.c
  Fecha: 12/08/2025 - 09:00
  Último cambio: Corregido para usar el bus I2C global en lugar de pasarlo.
  Descripción: Driver del sensor IMU QMI8658. Ahora utiliza la función pública
               `bsp_get_i2c_bus_handle` para obtener el manejador del bus I2C,
               simplificando su inicialización y asegurando la modularidad.
*/
#include "bsp_api.h"
#include "bsp_qmi8658.h"
#include "esp_log.h"
#include "driver/i2c_master.h"

static const char *TAG = "bsp_qmi8658";

#define IMU_I2C_ADDRESS  0x6B
#define I2C_CLK_SPEED_HZ 400000

static i2c_master_dev_handle_t g_imu_dev_handle = NULL;

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

    uint8_t ctrl_regs_data[][2] = {
        {QMI8658_CTRL7, 0x03},
        {QMI8658_CTRL2, 0x23},
        {QMI8658_CTRL3, 0x53}
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
        return;
    }

    uint8_t buf_reg[12];
    short raw_acc_xyz[3];
    short raw_gyro_xyz[3];

    i2c_master_transmit_receive(g_imu_dev_handle,
                                (uint8_t[]){QMI8658_AX_L}, 1,
                                buf_reg, 12,
                                pdMS_TO_TICKS(100));

    raw_acc_xyz[0] = (int16_t)((buf_reg[1] << 8) | buf_reg[0]);
    raw_acc_xyz[1] = (int16_t)((buf_reg[3] << 8) | buf_reg[2]);
    raw_acc_xyz[2] = (int16_t)((buf_reg[5] << 8) | buf_reg[4]);
    raw_gyro_xyz[0] = (int16_t)((buf_reg[7] << 8) | buf_reg[6]);
    raw_gyro_xyz[1] = (int16_t)((buf_reg[9] << 8) | buf_reg[8]);
    raw_gyro_xyz[2] = (int16_t)((buf_reg[11] << 8) | buf_reg[10]);

    acc[0] = ((float)raw_acc_xyz[0] / 4096.0f) * 9.81f;
    acc[1] = ((float)raw_acc_xyz[1] / 4096.0f) * 9.81f;
    acc[2] = ((float)raw_acc_xyz[2] / 4096.0f) * 9.81f;
    gyro[0] = (float)raw_gyro_xyz[0] / 16.0f;
    gyro[1] = (float)raw_gyro_xyz[1] / 16.0f;
    gyro[2] = (float)raw_gyro_xyz[2] / 16.0f;
}