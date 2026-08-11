// VGA constants and drawing API for the DE1-SoC display.
// Chris Parker & Lucas Keith.

// VGA Bounds
#define SCREEN_XMAX 640
#define SCREEN_YMAX 480

#define ORIGIN_X (SCREEN_XMAX / 2) // 320
#define ORIGIN_Y (SCREEN_YMAX / 2) // 240

// Pointers used in implementation and assigned in main
extern volatile unsigned int *vga_pixel_ptr;
extern volatile unsigned int *vga_char_ptr;

/****************************************************************************************
 * Subroutine to send a string of text to the VGA monitor
 ****************************************************************************************/
void VGA_text(int x, int y, char *text_ptr);

/****************************************************************************************
 * Subroutine to clear text to the VGA monitor
 ****************************************************************************************/
void VGA_text_clear();

/****************************************************************************************
 * Draw a filled rectangle on the VGA monitor
 ****************************************************************************************/
void VGA_box(int x1, int y1, int x2, int y2, short pixel_color);

/****************************************************************************************
 * Draw a outline rectangle on the VGA monitor
 ****************************************************************************************/
void VGA_rect(int x1, int y1, int x2, int y2, short pixel_color);

// =============================================
// === Draw a line
// =============================================
// plot a line
// at x1,y1 to x2,y2 with color
// Code is from David Rodgers,
//"Procedural Elements of Computer Graphics",1985
void VGA_line(int x1, int y1, int x2, int y2, short c);

/****************************************************************************************
 * Draw a vertical line on the VGA monitor
 ****************************************************************************************/
void VGA_Vline(int x1, int y1, int y2, short pixel_color);

/****************************************************************************************
 * Draw a horixontal line on the VGA monitor
 ****************************************************************************************/
void VGA_Hline(int x1, int y1, int x2, short pixel_color);

/****************************************************************************************
 * Draw a filled circle on the VGA monitor
 ****************************************************************************************/
void VGA_disc(int x, int y, int r, short pixel_color);

/****************************************************************************************
 * Draw a circle on the VGA monitor
 ****************************************************************************************/
void VGA_circle(int x, int y, int r, int pixel_color);

/****************************************************************************************
 * Move screen content in bounding box by shift amount.
 ****************************************************************************************/
void VGA_shift(unsigned int shift, int x0, int y0, int x1, int y1);

// pixel macro
#define VGA_PIXEL(x, y, color)                                                 \
  do {                                                                         \
    int *pixel_ptr;                                                            \
    pixel_ptr = (int *)((char *)vga_pixel_ptr + (((y) * 640 + (x)) << 1));     \
    *(short *)pixel_ptr = (color);                                             \
  } while (0)

#define VGA_CLEAR()                                                            \
  do {                                                                         \
    VGA_box(0, 0, 639, 479, 0x0000);                                           \
  } while (0)

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
/// Colors

// 16-bit primary colors
#define red (short)(0 + (0 << 5) + (31 << 11))
#define dark_red (short)(0 + (0 << 5) + (15 << 11))
#define green (short)(0 + (63 << 5) + (0 << 11))
#define dark_green (short)(0 + (31 << 5) + (0 << 11))
#define blue (short)(31 + (0 << 5) + (0 << 11))
#define dark_blue (short)(15 + (0 << 5) + (0 << 11))
#define yellow (short)(0 + (63 << 5) + (31 << 11))
#define cyan (short)(31 + (63 << 5) + (0 << 11))
#define magenta (short)(31 + (0 << 5) + (31 << 11))
#define black (short)(0x0000)
#define gray (short)(15 + (31 << 5) + (51 << 11))
#define white (short)(0xffff)

//////////////////////////////////////////////////////////////
