
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>

#include <pericom/spi.h>
#include <pericom/pericom_api.h>

/* Global configuration instance */
pericom_config_t g_pericom_config = {0, 0, -1, -1, {0}};

/* Trim leading/trailing whitespace in-place and return pointer to trimmed string */
static char *trim(char *s)
{
    char *end;
    while (isspace((unsigned char)*s)) s++;
    if (*s == '\0') return s;
    end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) end--;
    *(end+1) = '\0';
    return s;
}

int pericom_initialize(const char* config_file) 
{
    int ret = 0;
    const char *path = config_file ? config_file : "pericom.cnf";
    FILE *f = fopen(path, "r");
    if (!f) 
    {
        /* Could not open config file */
        ret =  PERICOM_ERR_FILE;
        goto EXIT;
    }

    char line[512];
    char section[64] = {0};

    while (fgets(line, sizeof(line), f)) {
        char *p = trim(line);
        if (*p == '\0') continue; /* empty line */
        if (*p == ';' || *p == '#') continue; /* comment */

        if (*p == '[') {
            char *end = strchr(p, ']');
            if (end) {
                size_t n = end - p - 1;
                if (n >= sizeof(section)) n = sizeof(section) - 1;
                strncpy(section, p+1, n);
                section[n] = '\0';
            } else {
                section[0] = '\0';
            }
            continue;
        }

        char *eq = strchr(p, '=');
        if (!eq) continue;
        *eq = '\0';
        char *key = trim(p);
        char *val = trim(eq + 1);

        /* strip surrounding quotes from value if present */
        size_t val_len = strlen(val);
        if (val_len >= 2 && ((val[0] == '"' && val[val_len-1] == '"') || (val[0] == '\'' && val[val_len-1] == '\''))) {
            val[val_len-1] = '\0';
            val++;
        }

        if (strcasecmp(section, "LOG") == 0) {
            if (strcasecmp(key, "LOG_LEVEL") == 0) {
                g_pericom_config.log_level = (int)strtol(val, NULL, 0);
            }
        } else if (strcasecmp(section, "DEVICE") == 0) {
            if (strcasecmp(key, "DEVICE_TYPE") == 0) {
                g_pericom_config.device_type = (int)strtol(val, NULL, 0);
            } else if (strcasecmp(key, "RESET_PIN") == 0) {
                g_pericom_config.reset_pin = (int)strtol(val, NULL, 0);
            } else if (strcasecmp(key, "WAKEUP_PIN") == 0) {
                g_pericom_config.wakeup_pin = (int)strtol(val, NULL, 0);
            }
        } else if (strcasecmp(section, "SPI") == 0) {
            if (strcasecmp(key, "SPI0_DEVICE") == 0) {
                strncpy(g_pericom_config.spi0_device, val, sizeof(g_pericom_config.spi0_device) - 1);
                g_pericom_config.spi0_device[sizeof(g_pericom_config.spi0_device) - 1] = '\0';
            }
        }
    }

    fclose(f);
EXIT:
    return (ret);
}

int pericom_release()
{
    memset(&g_pericom_config, 0, sizeof(g_pericom_config));
    return 0;
}

int pericom_spi_open(pericom_handle* handle, unsigned int mode, uint32_t max_speed)
{
    int ret = 0;
    spi_t *spi = spi_new();
    if (!spi) 
    {
        ret = PERICOM_ERR_ALLOC;
        goto EXIT;
    }
    if (spi_open(spi, g_pericom_config.spi0_device, mode, max_speed) != 0) 
    {
        spi_free(spi);
        return -1;
    }
    *handle = (pericom_handle)spi;
EXIT:
    return (ret);
}

int pericom_spi_close(pericom_handle handle)
{
    spi_t *spi = (spi_t *)handle;
    if (spi_close(spi) != 0) 
    {
        ret = PERICOM_ERR_CLOSE_SPI;
        goto EXIT;
    }
    spi_free(spi);
    return (ret);
}

int pericom_spi_transfer(pericom_handle handle, const unsigned char* tx_buffer, unsigned char* rx_buffer, int length)
{
    if (!handle) 
    {
        ret =  PERICOM_ERR_ARG;
        goto EXIT;
    }
    if (tx_buffer == NULL && rx_buffer == NULL) 
    {
        ret =  PERICOM_ERR_ARG;
        goto EXIT;
    }
    if (length <= 0) 
    {
        ret =  PERICOM_ERR_ARG;
        goto EXIT;
    }
    spi_t *spi = (spi_t *)handle;
    if (spi_transfer(spi, tx_buffer, rx_buffer, length) != 0) 
    {
        return -1;
    }
    return 0;
}

int pericom_gpio_open(pericom_handle* handle, int pin)
{
    // Implementation for opening GPIO pin
    int ret = 0;
    gpio_t *gpio = gpio_new();
    if (!gpio) 
    {
        ret = PERICOM_ERR_ALLOC;
        goto EXIT;
    }
    if (gpio_open(gpio, "/dev/gpiochip0", pin, GPIO_DIR_OUT) != 0) 
    {
        gpio_free(gpio);
        return -1;
    }
    *handle = (pericom_handle)gpio; 
EXIT:
    return (ret);
}