#ifndef __BIOS__
#define __BIOS__
#include <lcom/lcf.h>
#include <machine/int86.h>

/** @defgroup bios bios
 * @{
 *
 * BIOS call related
 */

// BIOS services
#define BIOS_VID_CARD 0x10

// AH consts
#define AH_CALL_FAILED 0X01
#define AH_NOT_SUPPORTED_CONF 0X02 
#define AH_NOT_SUPPORTED_MODE 0X03

#define VBE_CALL 0x4F

// Functions
#define SET_VBE_MODE 0x02 // Passed to BX register
#define GET_VBE_MODE_INFO 0x01
#define GET_VBE_CONTROLLER_INFO 0x00

// Graphics modes
#define MODE_1024x768_INDEX 0x105
#define MODE_640x480_DIRECT 0x110
#define MODE_800x600_DIRECT 0x115
#define MODE_1280x1024_DIRECT 0x11A
#define MODE_1152x864_DIRECT 0x14C
#define MODE_CGA 0x03

// Buffer model
#define LINEAR_BUFFER BIT(14)

/**
 * @brief Make a sys_int86 call and check for errors
 * 
 * @param r pointer to reg86 sent to sys_int86
 * @return Return 0 upon success and non-zero otherwise
 */
int bios_call(struct reg86 *r);

/**
 * @brief Return pointer containing a pixmap of the font used by the bios
 * 
 * @param foreground_color Foreground Color
 * @param background_color Background Color
 * @param bytes_per_pixel Bytes per Pixel of graphics mode used
 * @return Return pointer to pixmap upon success and NULL otherwise
 */
void * bios_get_font_ptr(uint32_t foreground_color, uint32_t background_color, unsigned bytes_per_pixel);

/**
 * @brief Convert a bitmap to a pixelmap
 * 
 * @param bitmap Original bitmap
 * @param output_buffer Output buffer, should have the at least the size of the original bitmap times bytes_per_pixel of graphics mode used
 * @param foreground_color Foreground Color
 * @param background_color Background Color
 * @param bytes_per_pixel Bytes per Pixel of graphics mode used
 * @param size Size of the bitmap
 */
void pixmap_from_bitmap(uint8_t * bitmap, uint8_t * output_buffer, uint32_t foreground_color, uint32_t background_color,  unsigned bytes_per_pixel, unsigned size);

/**
 * @brief Get info of a given VBE mode
 * 
 * @param mode VBE Mode number
 * @param vmi_p Pointer to vbe_mode_info_t to be filled
 * @return Return 0 upon success and non-zero otherwise
 */
int vbe_mode_get_info(uint16_t mode, vbe_mode_info_t *vmi_p);

/**
 * @brief Set VBE mode
 * 
 * @param mode VBE Mode number
 * @return Return 0 upon success and non-zero otherwise
 */
int vbe_set_mode(uint16_t mode);

/**
 * @brief Set Display Start
 * 
 * @param start First scanline number
 * @return Return 0 upon success and non-zero otherwise
 */ 
int vbe_set_display_start(uint16_t start);

/**
 * @brief Map VRAM
 * 
 * @param phys_base_ptr VRAM Physical Base Pointer
 * @param vram_size VRAM Size to be mapped
 * @return Return pointer to vram upon success and NULL otherwise
 */
void * vbe_map_vram(unsigned int phys_base_ptr, unsigned int vram_size);

#endif
