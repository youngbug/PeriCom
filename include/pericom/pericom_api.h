#ifndef __PERICOM_API_H__
#define __PERICOM_API_H__   

#include <stddef.h>

/* Global configuration structure populated by pericom_initialize() */
typedef struct {
    int log_level;               /* [LOG] LOG_LEVEL */
    int device_type;             /* [DEVICE] DEVICE_TYPE */
    int reset_pin;               /* [DEVICE] RESET_PIN */
    int wakeup_pin;              /* [DEVICE] WAKEUP_PIN */
    char spi0_device[256];       /* [SPI] SPI0_DEVICE */
} pericom_config_t;

/* Global instance (defined in pericom_api.c) */
extern pericom_config_t g_pericom_config;

int pericom_initialize(const char* config_file);
int pericom_release();

int pericom_spi_open(int spi_channel, unsigned int mode, uint32_t max_speed);
int pericom_spi_close(int spi_channel);
int pericom_spi_transfer(int spi_channel, const unsigned char* tx_buffer, unsigned char* rx_buffer, int length);

int pericom_delay(int milliseconds);
int pericom_low_gpio(int pin);
int pericom_high_gpio(int pin);

#endif /* __PERICOM_API_H__ */