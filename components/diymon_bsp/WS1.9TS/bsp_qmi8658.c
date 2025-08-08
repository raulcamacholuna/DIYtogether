/*
 * Fichero: ./diymon_bsp/WS1.9TS/bsp_qmi8658.c
 * Fecha: 08/08/2025 - 02:40
 * Último cambio: Corregido el include para la nueva estructura de ficheros.
 * Descripción: Driver para el sensor IMU QMI8658, adaptado a la API I2C legacy y a la estructura de componentes de ESP-IDF.
 */
#include "bsp_api.h"        // API Pública del BSP
#include "bsp_qmi8658.h"    // Cabecera privada con los registros del sensor
#include "esp_log.h"
#include "driver/i2c.h"

static const char *TAG = "bsp_qmi8658";

#define I2C_PORT_NUM     I2C_NUM_0
#define IMU_I2C_ADDRESS  0x6B
#define I2C_TIMEOUT_MS   100

// Función privada para escribir en un registro usando la API legacy.
static esp_err_t qmi8658_write_reg(uint8_t reg, uint8_t value)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (IMU_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_write_byte(cmd, value, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_PORT_NUM, cmd, pdMS_TO_TICKS(I2C_TIMEOUT_MS));
    i2c_cmd_link_delete(cmd);
    return ret;
}

// Función privada para leer de un registro usando la API legacy.
static esp_err_t qmi8658_read_reg(uint8_t reg, uint8_t *data, size_t len)
{
    if (len == 0) {
        return ESP_OK;
    }
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (IMU_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (IMU_I2C_ADDRESS << 1) | I2C_MASTER_READ, true);
    if (len > 1) {
        i2c_master_read(cmd, data, len - 1, I2C_MASTER_ACK);
    }
    i2c_master_read_byte(cmd, data + len - 1, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_PORT_NUM, cmd, pdMS_TO_TICKS(I2C_TIMEOUT_MS));
    i2c_cmd_link_delete(cmd);
    return ret;
}

esp_err_t bsp_imu_init(void)
{
    ESP_LOGI(TAG, "[IMU DRIVER] <<<< My Custom QMI8658 Driver is Running >>>>");

    uint8_t who_am_i = 0;
    esp_err_t ret = qmi8658_read_reg(QMI8658_WHO_AM_I, &who_am_i, 1);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to communicate with QMI8658. I2C error: %s", esp_err_to_name(ret));
        return ESP_FAIL;
    }
    if (who_am_i != 0x05) {
        ESP_LOGE(TAG, "QMI8658 not found! WhoAmI check failed. Read value: 0x%02X", who_am_i);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "QMI8658 found successfully! WhoAmI: 0x%02X", who_am_i);

    qmi8658_write_reg(QMI8658_CTRL7, 0x03);
    qmi8658_write_reg(QMI8658_CTRL2, 0x23);
    qmi8658_write_reg(QMI8658_CTRL3, 0x53);
    
    ESP_LOGI(TAG, "IMU initialized.");
    return ESP_OK;
}

void bsp_imu_read(float acc[3], float gyro[3])
{
    uint8_t buf_reg[12];
    short raw_acc_xyz[3];
    short raw_gyro_xyz[3];

    qmi8658_read_reg(QMI8658_AX_L, buf_reg, 12);

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