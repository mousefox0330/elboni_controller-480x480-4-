/*****************************************************************************
 *  elboni_sd.h 
 *
 ******************************************************************************/

#ifndef __SD_H
#define __SD_H

#define MOUNT_POINT "/sdcard"

void elboni_sdcard_vfat_init(void);
esp_err_t elboni_vfat_write_file(const char *path, char *data);
esp_err_t elboni_vfat_read_file(char *text, const char *path, int length);
void elboni_vfat_test(void);
#endif

