#include <bios.h>
#include <lcom/lcf.h>
#include <machine/int86.h>
#include <minix/sysutil.h>

int bios_call(struct reg86 *r) {
  /* Make the BIOS call */
  if ((sys_int86)(r) != OK) {
    printf("\tvbe_set_mode: sys_int86() failed \n");
    return 1;
  }
  switch (r->ah) {
    case AH_CALL_FAILED: printf("BIOS call failed\n"); return 1;
    case AH_NOT_SUPPORTED_CONF: printf("BIOS not supported config\n"); return 1;
    case AH_NOT_SUPPORTED_MODE: printf("BIOS not supported mode\n"); return 1;
  }
  return 0;
}

int vbe_mode_get_info(uint16_t mode, vbe_mode_info_t *vmi_p) {
  printf("get info");
  mmap_t mem;
  if (lm_alloc(sizeof(vbe_mode_info_t), &mem) == NULL) {
    printf("lm_alloc failed\n");
    return 1;
  }

  struct reg86 r;
  memset(&r, 0, sizeof(r));
  r.intno = BIOS_VID_CARD;
  r.ah = VBE_CALL;
  r.al = GET_VBE_MODE_INFO;
  r.es = PB2BASE(mem.phys);
  r.di = PB2OFF(mem.phys);
  r.cx = mode;
  if (bios_call(&r)) {
    printf("vbe_mode_get_info failed\n");
    if (lm_free(&mem)) {
      printf("lm_free failed\n");
    }
    return 1;
  }
  vbe_mode_info_t *info = (vbe_mode_info_t *) mem.virt;
  *vmi_p = *info;

  if (!lm_free(&mem)) {
    printf("lm_free failed\n");
    return 1;
  }

  return 0;
}

int vbe_set_mode(uint16_t mode) {
  struct reg86 r;
  memset(&r, 0, sizeof(r));
  r.intno = BIOS_VID_CARD; /* BIOS video services */
  r.ah = VBE_CALL;         /* Set Video Mode function */
  r.al = SET_VBE_MODE;
  r.bx = LINEAR_BUFFER | mode;
  if (bios_call(&r)) {
    printf("VBE call failed\n");
    return 1;
  }
  return 0;
}

int vbe_set_display_start(uint16_t start) {
  struct reg86 r;
  memset(&r, 0, sizeof(r));
  r.intno = BIOS_VID_CARD; /* BIOS video services */
  r.ah = VBE_CALL;
  r.al = 0x07;
  r.bh = 0x00;
  r.bl = 0x00;
  r.cx = 0;
  r.dx = start;
  if (bios_call(&r)) {
    printf("VBE call failed\n");
    return 1;
  }
  return 0;
}

void *vbe_map_vram(unsigned int phys_base_ptr, unsigned int vram_size) {
  struct minix_mem_range mr;
  unsigned int vram_base = phys_base_ptr; /* VRAM's physical addresss */
  int r;

  /* Allow memory mapping */
  mr.mr_base = (phys_bytes) vram_base;
  mr.mr_limit = mr.mr_base + vram_size;

  if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);

  /* Map memory */
  void *video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);

  if (video_mem == MAP_FAILED)
    panic("couldn't map video memory");
  return video_mem;
}

void pixmap_from_bitmap(uint8_t *bitmap, uint8_t *output_buffer, uint32_t foreground_color, uint32_t background_color, unsigned bytes_per_pixel, unsigned size) {
  uint8_t *ptr = (uint8_t *) output_buffer;
  // iterate chars
  for (int i = 0; i < 256; ++i) {
    // iterate rows
    for (int j = 0; j < 16; ++j) {
      uint8_t byte = *bitmap;
      // iterate bits
      for (int k = 7; k >= 0; --k) {
        *ptr = (byte & BIT(k)) ? foreground_color : background_color;
        ptr += bytes_per_pixel;
      }
      bitmap++;
    }
  }
}

void *bios_get_font_ptr(uint32_t foreground_color, uint32_t background_color, unsigned bytes_per_pixel) {
  struct reg86 r;
  memset(&r, 0, sizeof(r));
  r.intno = BIOS_VID_CARD;
  r.ax = 0x1130;
  r.bh = 0x06;
  if (bios_call(&r)) {
    printf("bios_call failed \n");
    return NULL;
  }
  uint32_t phys_addr = r.es * 0x10 + r.bp;
  uint16_t table_size = 256 * r.cx;

  uint8_t *font_bit_buf = (uint8_t *) malloc(table_size);
  sys_readbios(phys_addr, font_bit_buf, table_size);

  void *pixmap = (void *) malloc(table_size * 16 * bytes_per_pixel);
  pixmap_from_bitmap(font_bit_buf, pixmap, foreground_color, background_color, bytes_per_pixel, table_size);
  free(font_bit_buf);

  return pixmap;
}
