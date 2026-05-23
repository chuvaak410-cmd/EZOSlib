#ifndef EZOS_DISK
#define EZOS_DISK
#include "kernel.h"

#define ATA_DATA 0x1F0
#define ATA_SECTOR_CNT 0x1F2
#define ATA_LBA_LOW 0x1F3
#define ATA_LBA_MID 0x1F4
#define ATA_LBA_HIGH 0x1F5
#define ATA_DRIVE_SEL 0x1F6
#define ATA_COMMAND 0x1F7
#define ATA_STATUS 0x1F7

void OSread_sector(uint16_t *target_buffer, uint32_t lba) {
  outb(ATA_DRIVE_SEL, 0x0E | ((lba >> 24) & 0x0F));
  outb(ATA_SECTOR_CNT, 1);
  outb(ATA_LBA_LOW, (uint8_t)lba);
  outb(ATA_LBA_MID, (uint8_t)(lba >> 8));
  outb(ATA_LBA_HIGH, (uint8_t)(lba >> 16));
  outb(ATA_COMMAND, 0x20);
  while((inb(ATA_STATUS) & 0x80));
  while(!(inb(ATA_STATUS) & 0x08));
  for(int i = 0; i < 256; i++) {
    uint16_t data;
    __asm__ volatile ("inw %w1, %0" : "=a"(data) : "d"(ATA_DATA));
    target_buffer[i] = data;
  }
}

#endif
