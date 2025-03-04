/* 
 *	SD card and FAT filesystem .
 *	elboni_sd.c
 *  HS speed sdcard, only use for read Simple and minimal information
*/

#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "driver/i2c.h"
#include "esp_io_expander_tca9554.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "elboni_sd.h"

#define SDCARD_DEBUG 1
#define TAG "SDCARD"

#if SDCARD_DEBUG
	#define ELBONI_DBG_PRINT_SDCARD(...) ESP_LOGI(__VA_ARGS__)
#else
	#define ELBONI_DBG_PRINT_SDCARD(...)
#endif

#define SDMMC_MAX_CHAR_SIZE    64

static sdmmc_card_t *card;

void elboni_sdcard_vfat_init(void)
{
    esp_err_t ret;

	esp_vfs_fat_sdmmc_mount_config_t mount_config = {
#if CONFIG_SD_FORMAT_IF_MOUNT_FAILED
        .format_if_mount_failed = true,
#else
		.format_if_mount_failed = false,
#endif
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    
    const char mount_point[] = MOUNT_POINT;
    // Use settings defined above to initialize SD card and mount FAT filesystem.
    // Note: esp_vfs_fat_sdmmc/sdspi_mount is all-in-one convenience functions.
    // Please check its source code and implement error recovery when developing
    // production applications.

    ELBONI_DBG_PRINT_SDCARD(TAG, "Using SDMMC peripheral");

    // By default, SD card frequency is initialized to SDMMC_FREQ_DEFAULT (20MHz)
    // For setting a specific frequency, use host.max_freq_khz (range 400kHz - 40MHz for SDMMC)
    // Example: for fixed frequency of 10MHz, use host.max_freq_khz = 10000;
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
#if CONFIG_SDMMC_SPEED_HS
    host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;
#elif CONFIG_SDMMC_SPEED_UHS_I_SDR50
    host.slot = SDMMC_HOST_SLOT_0;
    host.max_freq_khz = SDMMC_FREQ_SDR50;
    host.flags &= ~SDMMC_HOST_FLAG_DDR;
#elif CONFIG_SDMMC_SPEED_UHS_I_DDR50
    host.slot = SDMMC_HOST_SLOT_0;
    host.max_freq_khz = SDMMC_FREQ_DDR50;
#endif

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
#if CONFIG_EXAMPLE_SDMMC_SPEED_UHS_I_SDR50 || CONFIG_EXAMPLE_SDMMC_SPEED_UHS_I_DDR50
    slot_config.flags |= SDMMC_SLOT_FLAG_UHS1;
#endif

    // Set bus width to use:
#ifdef CONFIG_SDMMC_BUS_WIDTH_4
    slot_config.width = 4;
#else
    slot_config.width = 1;
#endif //CONFIG_SDMMC_BUS_WIDTH_4

    slot_config.clk = CONFIG_SDMMC_PIN_CLK;
    slot_config.cmd = CONFIG_SDMMC_PIN_CMD;
    slot_config.d0 = CONFIG_SDMMC_PIN_DO;
#ifdef CONFIG_SDMMC_BUS_WIDTH_4
    slot_config.d1 = CONFIG_SDMMC_PIN_D1;
    slot_config.d2 = CONFIG_SDMMC_PIN_D2;
    slot_config.d3 = CONFIG_SDMMC_PIN_D3;
#endif  // CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4


    // Enable internal pullups on enabled pins. The internal pullups
    // are insufficient however, please make sure 10k external pullups are
    // connected on the bus. This is for debug / example purpose only.
    slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

    ELBONI_DBG_PRINT_SDCARD(TAG, "Mounting filesystem");
    ret = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                     "If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                     "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return;
    }
    ELBONI_DBG_PRINT_SDCARD(TAG, "Filesystem mounted");
    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, card);

	return;
}

//
// Never unmount

esp_err_t elboni_vfat_write_file(const char *path, char *data)
{
    ELBONI_DBG_PRINT_SDCARD(TAG, "Opening file %s", path);
    FILE *f = fopen(path, "w");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return ESP_FAIL;
    }
    fprintf(f, data);
    fclose(f);
    ELBONI_DBG_PRINT_SDCARD(TAG, "File written");

    return ESP_OK;
}

esp_err_t elboni_vfat_read_file(char *text, const char *path, int length)
{
    ELBONI_DBG_PRINT_SDCARD(TAG, "Reading file %s", path);
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return ESP_FAIL;
    }
    char line[SDMMC_MAX_CHAR_SIZE];
    fgets(line, sizeof(line), f);
    fclose(f);

	if(text != NULL) {
		memcpy(text, line, length);
	} else {
		return ESP_FAIL;
	}

#if SDCARD_DEBUG
    // strip newline
    char *pos = strchr(line, '\n');
    if (pos) {
        *pos = '\0';
    }
    ELBONI_DBG_PRINT_SDCARD(TAG, "Read from file: '%s'", line);
#endif

    return ESP_OK;
}

void elboni_vfat_test(void)
{
	esp_err_t ret;

	const char *file_hello = MOUNT_POINT"/hello.txt";
    char data[SDMMC_MAX_CHAR_SIZE];
    snprintf(data, SDMMC_MAX_CHAR_SIZE, "%s %s!\n", "Hello", card->cid.name);
    ret = elboni_vfat_write_file(file_hello, data);
    if (ret != ESP_OK) {
        return;
    }

    const char *file_foo = MOUNT_POINT"/foo.txt";
    // Check if destination file exists before renaming
    struct stat st;
    if (stat(file_foo, &st) == 0) {
        // Delete it if it exists
        unlink(file_foo);
    }

    // Rename original file
    ELBONI_DBG_PRINT_SDCARD(TAG, "Renaming file %s to %s", file_hello, file_foo);
    if (rename(file_hello, file_foo) != 0) {
        ESP_LOGE(TAG, "Rename failed");
        return;
    }

    ret = elboni_vfat_read_file(data, file_foo, 12);
    if (ret != ESP_OK) {
        return;
    }

    // Format FATFS
#ifdef CONFIG_EXAMPLE_FORMAT_SD_CARD
    ret = esp_vfs_fat_sdcard_format(mount_point, card);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to format FATFS (%s)", esp_err_to_name(ret));
        return;
    }

    if (stat(file_foo, &st) == 0) {
        ELBONI_DBG_PRINT_SDCARD(TAG, "file still exists");
        return;
    } else {
        ELBONI_DBG_PRINT_SDCARD(TAG, "file doesn't exist, formatting done");
    }
#endif // CONFIG_EXAMPLE_FORMAT_SD_CARD

    const char *file_nihao = MOUNT_POINT"/nihao.txt";
    memset(data, 0, SDMMC_MAX_CHAR_SIZE);
    snprintf(data, SDMMC_MAX_CHAR_SIZE, "%s %s!\n", "Nihao", card->cid.name);
    ret = elboni_vfat_write_file(file_nihao, data);
    if (ret != ESP_OK) {
        return;
    }

    //Open file for reading
    ret = elboni_vfat_read_file(data, file_nihao, 12);
    if (ret != ESP_OK) {
        return;
    }
	
}