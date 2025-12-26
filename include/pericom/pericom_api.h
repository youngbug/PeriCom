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

typedef void* pericom_handle;

/*Error code*/
#define PERICOM_ERR_SUCCESS            0
#define PERICOM_ERR_BASE              -1000
#define PERICOM_ERR_ARG               (PERICOM_ERR_BASE - 1)    // Invalid argument
#define PERICOM_ERR_FILE              (PERICOM_ERR_BASE - 2)    // File error
#define PERICOM_ERR_ALLOC             (PERICOM_ERR_BASE - 3)    // Memory allocation error
#define PERICOM_ERR_CLOSE_SPI         (PERICOM_ERR_BASE - 4)    // SPI close error
#define PERICOM_ERR_SPI_TRANSFER      (PERICOM_ERR_BASE - 5)    // SPI transfer error

int pericom_initialize(const char* config_file);
int pericom_release();

int pericom_spi_open(pericom_handle* handle, unsigned int mode, uint32_t max_speed);
int pericom_spi_close(pericom_handle handle);
int pericom_spi_transfer(pericom_handle handle, const unsigned char* tx_buffer, unsigned char* rx_buffer, int length);

int pericom_delay(int milliseconds);
int pericom_low_gpio(int pin);
int pericom_high_gpio(int pin);

#endif /* __PERICOM_API_H__ */