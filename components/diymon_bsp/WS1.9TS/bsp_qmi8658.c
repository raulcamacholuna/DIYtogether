#include "bsp_qmi8658.h" // Cabecera privada del driver
#include "bsp_api.h"      // El contrato que implementamos

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "bsp_qmi8658";

// Dirección I2C del sensor según el esquemático y la demo
#define IMU_I2C_ADDRESS 0x6B

// Handle del dispositivo I2C, privado para este driver
static i2c_master_dev_handle_t g_i2c_dev_handle = NULL;

// Función privada para escribir en un registro (adaptada de la demo)
static esp_err_t qmi8658_write_reg(uint8_t reg, uint8_t value)
{
    uint8_t write_buf[2] = {reg, value};
    return i2c_master_transmit(g_i2c_dev_handle, write_buf, sizeof(write_buf), -1);
}

// Función privada para leer de un registro (adaptada de la demo)
static esp_err_t qmi8658_read_reg(uint8_t reg, uint8_t *data, size_t len)
{
    return i2c_master_transmit_receive(g_i2c_dev_handle, &reg, 1, data, len, -1);
}

// Implementación de la función pública de la API
esp_err_t bsp_imu_init(void)
{
    ESP_LOGI(TAG, "Initializing IMU sensor (QMI8658)...");
    
    // Obtenemos el bus I2C que ya ha inicializado bsp_i2c
    i2c_master_bus_handle_t bus_handle = bsp_get_i2c_bus_handle();

    // Configuración del dispositivo IMU en el bus I2C
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = IMU_I2C_ADDRESS,
        .scl_speed_hz = 400000,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &g_i2c_dev_handle));

    // Verificamos el "WhoAmI"
    uint8_t who_am_i = 0;
    qmi8658_read_reg(0x00, &who_am_i, 1);
    if (who_am_i != 0x05) {
        ESP_LOGE(TAG, "QMI8658 not found! WhoAmI: 0x%02X", who_am_i);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "QMI8658 found successfully! WhoAmI: 0x%02X", who_am_i);

    // Secuencia de inicialización básica (adaptada de la demo)
    qmi8658_write_reg(0x02, 0x60); // CTRL1: Habilitar interrupciones
    qmi8658_write_reg(0x03, 0x23); // CTRL2: Accel 8g, 1000Hz
    qmi8658_write_reg(0x04, 0x53); // CTRL3: Gyro 2048dps, 1000Hz
    qmi8658_write_reg(0x05, 0x00); // CTRL4: Sin filtros
    qmi8658_write_reg(0x06, 0x00); // CTRL5: Sin filtros
    qmi8658_write_reg(0x07, 0x03); // CTRL7: Habilitar Accel y Gyro
    
    ESP_LOGI(TAG, "IMU initialized.");
    return ESP_OK;
}

// Implementación de la función pública de lectura
void bsp_imu_read(float acc[3], float gyro[3])
{
    uint8_t buf_reg[12];
    short raw_acc_xyz[3];
    short raw_gyro_xyz[3];

    // Leemos los 12 bytes de datos de sensores de una vez
    qmi8658_read_reg(0x33, buf_reg, 12);

    raw_acc_xyz[0] = (int16_t)((buf_reg[1] << 8) | buf_reg[0]);
    raw_acc_xyz[1] = (int16_t)((buf_reg[3] << 8) | buf_reg[2]);
    raw_acc_xyz[2] = (int16_t)((buf_reg[5] << 8) | buf_reg[4]);

    raw_gyro_xyz[0] = (int16_t)((buf_reg[7] << 8) | buf_reg[6]);
    raw_gyro_xyz[1] = (int16_t)((buf_reg[9] << 8) | buf_reg[8]);
    raw_gyro_xyz[2] = (int16_t)((buf_reg[11] << 8) | buf_reg[10]);

    // Convertimos los datos crudos a unidades físicas (m/s^2 y dps)
    // La sensibilidad para 8g es 4096 LSB/g
    acc[0] = (float)raw_acc_xyz[0] / 4096.0f * 9.81f;
    acc[1] = (float)raw_acc_xyz[1] / 4096.0f * 9.81f;
    acc[2] = (float)raw_acc_xyz[2] / 4096.0f * 9.81f;

    // La sensibilidad para 2048 dps es 16 LSB/dps
    gyro[0] = (float)raw_gyro_xyz[0] / 16.0f;
    gyro[1] = (float)raw_gyro_xyz[1] / 16.0f;
    gyro[2] = (float)raw_gyro_xyz[2] / 16.0f;
}