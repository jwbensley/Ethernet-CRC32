
/* 
 * Based upon the work of Grant P. Maizels and Ross Williams.
 * Their original licence details are in the header file.
 * This amalgamation is the work of James Bensley Copyright (c) 2018.
 */ 


#include <stdio.h>     // EOF, FILE, fclose(), fprintf(), fscanf(), perror(), printf(), stderr, stdin
#include <stdlib.h>    // calloc(), exit()
#include <inttypes.h>  // intN_t, PRIxN, SCNxN, uintN_t
#include <string.h>    // memset()

#include "crc32.h"

int32_t main(uint16_t argc, char *argv[]) {

  cm_t cm;
  p_cm_t p_cm = &cm;
  memset(p_cm, 0, sizeof(cm));

  p_cm->cm_width  = 32;
  p_cm->cm_poly   = 0x04C11DB7;
  p_cm->cm_init   = 0xFFFFFFFF;
  p_cm->cm_refin  = 1;
  p_cm->cm_refot  = 1;
  p_cm->cm_xorot  = 0xFFFFFFFF;

  FILE *fp = NULL;

  static uint16_t idx = 1;

  do {

    if (argc > 1 && (fp=fopen(argv[idx], "r")) == NULL) {
       fprintf(stderr, "%s: can't open %s\n", argv[0], argv[idx]);
       continue;
    }
    
    cm_ini(p_cm);
   
    printf("%s: ", argv[idx]);

    static uint32_t file_ret   = 0;
    static uint16_t frame_sz   = 0;
    static uint8_t *crc_buffer = NULL;

    crc_buffer = (uint8_t*)calloc(MAX_FRAME_SIZE, 1);

    if (crc_buffer == NULL) {
      printf("Failed to calloc() CRC buffer!\n");
      exit(-1);
    }

    while (file_ret != EOF && (frame_sz < MAX_FRAME_SIZE)) {

      file_ret = fscanf(fp, "%" SCNx8, crc_buffer + frame_sz);

      if (file_ret == EOF) break;
      if (file_ret <= 0) {
        perror("Error reading packet file");
        exit(-1);
      }

      printf("0x%" PRIx8 " ", crc_buffer[frame_sz]);
      frame_sz += 1;

    }

    printf ("\nFile length: %u bytes\n", frame_sz);

    if (fclose(fp) != 0) {
      perror("Error closing file");
      continue;
    }

    int16_t max = frame_sz;

    // If the frame has a 4 byte FCS value at the end,
    // exclude this from the CRC calculation:
    int8_t has_crc = 1;
    if (has_crc) max -= 4;

    uint8_t j;
    for (j = 0; j < max; j += 1) {
      cm_nxt(p_cm, crc_buffer[j]);
    }

    uint32_t crc = cm_crc(p_cm) & 0xffffffff;
    uint32_t frame_fcs = (crc_buffer[frame_sz - 1] << 24) |
                         (crc_buffer[frame_sz - 2] << 16) |
                         (crc_buffer[frame_sz - 3] << 8) |
                          crc_buffer[frame_sz - 4];

    if (has_crc) {
      printf("Calculated CRC: 0x%x, Frame FCS: 0x%x\n", crc, frame_fcs);
      (crc == frame_fcs) ? printf("Matched!\n") : printf("Not matched!\n");
    } else {
      printf("Calculated CRC: 0x%x\n", crc);
    }

  } while (++idx < argc);

  exit(0);
}


static uint32_t reflect(uint32_t v, uint32_t b) {

  static int   i;
  static uint32_t t;

  t = v;

  for (i=0; i < b; i++) {

    if (t & 1L) {
       v|=  BITMASK((b - 1) - i);
    } else {
       v&= ~BITMASK((b - 1) - i);
    }

    t>>=1;

  }

  return v;

}


static uint32_t widmask(p_cm_t p_cm) {

  return (((1L << (p_cm->cm_width - 1)) - 1L) << 1) | 1L;

}


static void cm_ini(p_cm_t p_cm) {

  p_cm->cm_reg = p_cm->cm_init;

}


static void cm_nxt(p_cm_t p_cm, uint32_t ch) {

  static int   i;
  static uint32_t uch, topbit;

  uch    = ch;
  topbit = BITMASK(p_cm->cm_width - 1);

  if (p_cm->cm_refin) uch = reflect(uch, 8);

  p_cm->cm_reg ^= (uch << (p_cm->cm_width - 8));

  for (i=0; i < 8; i++) {

    if (p_cm->cm_reg & topbit) {
      p_cm->cm_reg = (p_cm->cm_reg << 1) ^ p_cm->cm_poly;
    } else {
       p_cm->cm_reg <<= 1;
    }

    p_cm->cm_reg &= widmask(p_cm);

  }

}


static void cm_blk(p_cm_t p_cm, uint8_t *blk_adr, uint32_t blk_len) {

  while (blk_len--) cm_nxt(p_cm, *blk_adr++);

}


static uint32_t cm_crc(p_cm_t p_cm) {

  if (p_cm->cm_refot) {
    return p_cm->cm_xorot ^ reflect(p_cm->cm_reg, p_cm->cm_width);
  } else {
    return p_cm->cm_xorot ^ p_cm->cm_reg;
  }

}


static uint32_t cm_tab(p_cm_t p_cm, uint32_t index) {

  static uint8_t  i;
  static uint32_t r, topbit, inbyte;

  topbit = BITMASK(p_cm->cm_width - 1);
  inbyte = index;

  if (p_cm->cm_refin) inbyte = reflect(inbyte, 8);

  r = inbyte << (p_cm->cm_width - 8);

  for (i=0; i < 8; i++) {

    if (r & topbit) {
      r = (r << 1) ^ p_cm->cm_poly;
    } else {
       r <<= 1;
     }
  }

  if (p_cm->cm_refin) r = reflect(r, p_cm->cm_width);

  return r & widmask(p_cm);

}

