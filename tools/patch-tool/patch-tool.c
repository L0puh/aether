#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <aether.h>
#include <stddef.h>

#define GRANT_USART1 ( 1 << PERIPH_USART1 )
#define GRANT_GPIOC  ( 1 << PERIPH_GPIOC )

// !! AUTO GENERATED !!
#include "symbols.h"

#ifdef FEATURE_CRC_APP
static uint16_t crc_ccitt_table[256];
static int crc_table_initialized = 0;

static void init_crc_table(void)
{
    uint16_t crc;
    for (int i = 0; i < 256; i++) {
        crc = (uint16_t)(i << 8);
        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000)
                crc = (uint16_t)((crc << 1) ^ 0x1021);
            else
                crc = (uint16_t)(crc << 1);
        }
        crc_ccitt_table[i] = crc;
    }
    crc_table_initialized = 1;
}

static uint16_t update_crc_ccitt_host(uint16_t crc, uint8_t data)
{
    return (uint16_t)((crc << 8) ^ crc_ccitt_table[(crc >> 8) ^ data]);
}

static uint16_t calc_crc(const uint8_t* data, size_t size)
{
    if (!crc_table_initialized) {
        init_crc_table();
    }
    
    uint16_t crc = 0xFFFF;
    uint32_t crc_off = offsetof(app_desc_t, crc16);
    
    for (size_t i = 0; i < size; i++) {
        if (i >= crc_off && i < crc_off + 2) {
            continue;
        }
        crc = update_crc_ccitt_host(crc, data[i]);
    }
    
    return crc;
}
#endif

void print_help() 
{
   printf("this tool is used to patch apps before flashing\n");
   printf("usage:\npatch-tool -f {app.bin} -v {version} -o {patched-app.bin}\n");
   printf("--file    | -f              specify path to file\n");
   printf("--output  | -o              specify output path\n");
   printf("--version | -v              version of app\n");
   printf("--magic   | -m              magic number (if differs from defs.h)\n");
   printf("--help    | -h              prints this message\n");
}

bool parse_args(const int argc, char* argv[], char **filename, char **output, u16 *version, u32 *magic, bool *magic_set)
{
   bool version_set = 0, filename_set = 0;
   for (int i = 1; i < argc; i++) {
      if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
         print_help();
         return 0;
      }
      else if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--file") == 0) {
         if (i + 1 < argc) {
            filename_set = 1;
            *filename = argv[++i];
         }
      }
      else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
         if (i + 1 < argc) {
            version_set = 1;
            *version = atoi(argv[++i]);
         }
      }
      else if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--magic") == 0) {
         if (i + 1 < argc) {
            *magic = strtoul(argv[++i], NULL, 0);
            *magic_set = 1;
         }
      }
      else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
         if (i + 1 < argc) 
            *output = argv[++i];
      }
   }

   if (!filename_set || !version_set) {
      printf("error: -f and -v are required\n");
      print_help();
      return 0;
   }

   return 1;
}

bool read_file(const char* filename, u8 buffer[], size_t *size)
{
   FILE *file = fopen(filename, "rb");
   if (!file) {
      perror("failed to open file");
      return 0;
   }

   fseek(file, 0, SEEK_END);
   u64 file_size = ftell(file);
   fseek(file, 0, SEEK_SET);

   if (file_size > FLASH_APP_LENGTH) {
      printf("error: file too large (max %lu bytes)\n", FLASH_APP_LENGTH);
      fclose(file);
      return 0;
   }

   size_t bytes_read = fread(buffer, 1, file_size, file);
   if (bytes_read != file_size) {
      printf("error: read bytes and file size don't match (%lu and %lu)\n", file_size, bytes_read);
      fclose(file);
      return 0;
   }
   
   printf("read %zu bytes\n", bytes_read);

   *size = bytes_read;

   fclose(file);
   return 1;
}

bool write_output(const char* output, u8 *buffer, size_t size)
{
   FILE *out = fopen(output ? output : "patched.bin", "wb");
   if (!out) {
      perror("failed to create patched file");
      return 0;
   }

   size_t written = fwrite(buffer, 1, size, out);
   fclose(out);

   printf("written %zu bytes to %s\n", written, output ? output : "patched.bin");
   return 1;
}

int main(int argc, char *argv[]) 
{
   char *filename = NULL;
   char *output   = NULL;
   u16 version = -1;
   u32 magic = 0;
   bool magic_set = 0, res;
   
   res = parse_args(argc, argv, &filename, &output, &version, &magic, &magic_set);
   if (!res) {
      return 0;
   }

   printf("sizeof of app_desc_t: %zu\n", sizeof(app_desc_t));
   printf("file: %s\n", filename);
   printf("version: %d\n", version);
   if (magic_set) {
      printf("magic: 0x%08x\n", magic);
   }
   
   u8 buffer[FLASH_APP_LENGTH] = {0};
   size_t size = 0;
   if (!read_file(filename, buffer, &size)) {
      return 0;
   }

   app_desc_t *desc = (app_desc_t*)buffer;

   desc->magic = magic_set ? magic : APP_MAGIC;
   desc->version = version;
   desc->size = size;
   desc->entry = SYM_APP_START;
   desc->manifset.granted_periph_mask = GRANT_USART1 | GRANT_GPIOC;
   desc->crc16 = 0;  

#ifdef FEATURE_CRC_APP
   desc->crc16 = calc_crc(buffer, size);
#endif 
  
   printf("app descriptor:\n");
   printf("  magic:   0x%08x\n", desc->magic);
   printf("  version: 0x%04x\n", desc->version);
   printf("  size:    %u bytes\n", desc->size);
   printf("  entry:   0x%08x (offset %u from start)\n", desc->entry, desc->entry);
   printf("  periph mask: 0x%08x\n", desc->manifset.granted_periph_mask);
#ifdef FEATURE_CRC_APP
   printf("  CRC16:   0x%04x\n", desc->crc16);
#endif 

   write_output(output, buffer, size);
   return 0;
}
