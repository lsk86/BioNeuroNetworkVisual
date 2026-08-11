// VGA drawing helpers for the DE1-SoC neuron display.
// Chris Parker & Lucas Keith (adapted from Bruce Land).

#include "graphics.h"
#include <math.h>
#include <string.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define CLAMP(a, min, max) (MAX((min), MIN((a), (max))))
#define CLAMP0(a, max) (CLAMP((a), 0, (max)))

volatile unsigned int *vga_pixel_ptr = NULL;
volatile unsigned int *vga_char_ptr = NULL;

/****************************************************************************************
 * Subroutine to send a string of text to the VGA monitor
 ****************************************************************************************/
void VGA_text(int x, int y, char *text_ptr) {
  volatile char *character_buffer =
      (char *)vga_char_ptr; // VGA character buffer
  int offset;
  /* assume that the text string fits on one line */
  offset = (y << 7) + x;
  while (*(text_ptr)) {
    // write to the character buffer
    *(character_buffer + offset) = *(text_ptr);
    ++text_ptr;
    ++offset;
  }
}

/****************************************************************************************
 * Subroutine to clear text to the VGA monitor
 ****************************************************************************************/
void VGA_text_clear() {
  volatile char *character_buffer =
      (char *)vga_char_ptr; // VGA character buffer
  int offset, x, y;
  for (x = 0; x < 79; x++) {
    for (y = 0; y < 59; y++) {
      /* assume that the text string fits on one line */
      offset = (y << 7) + x;
      // write to the character buffer
      *(character_buffer + offset) = ' ';
    }
  }
}

/****************************************************************************************
 * Draw a filled rectangle on the VGA monitor
 ****************************************************************************************/
#define SWAP(X, Y)                                                             \
  do {                                                                         \
    int temp = X;                                                              \
    X = Y;                                                                     \
    Y = temp;                                                                  \
  } while (0)

void VGA_box(int x1, int y1, int x2, int y2, short pixel_color) {
  char *pixel_ptr;
  int row, col;

  /* check and fix box coordinates to be valid */
  if (x1 > 639)
    x1 = 639;
  if (y1 > 479)
    y1 = 479;
  if (x2 > 639)
    x2 = 639;
  if (y2 > 479)
    y2 = 479;
  if (x1 < 0)
    x1 = 0;
  if (y1 < 0)
    y1 = 0;
  if (x2 < 0)
    x2 = 0;
  if (y2 < 0)
    y2 = 0;
  if (x1 > x2)
    SWAP(x1, x2);
  if (y1 > y2)
    SWAP(y1, y2);
  for (row = y1; row <= y2; row++)
    for (col = x1; col <= x2; ++col) {
      // 640x480
      // pixel_ptr = (char *)vga_pixel_ptr + (row<<10)    + col ;
      //  set pixel color
      //*(char *)pixel_ptr = pixel_color;
      VGA_PIXEL(col, row, pixel_color);
    }
}

/****************************************************************************************
 * Draw a outline rectangle on the VGA monitor
 ****************************************************************************************/
#define SWAP(X, Y)                                                             \
  do {                                                                         \
    int temp = X;                                                              \
    X = Y;                                                                     \
    Y = temp;                                                                  \
  } while (0)

void VGA_rect(int x1, int y1, int x2, int y2, short pixel_color) {
  char *pixel_ptr;
  int row, col;

  /* check and fix box coordinates to be valid */
  if (x1 > 639)
    x1 = 639;
  if (y1 > 479)
    y1 = 479;
  if (x2 > 639)
    x2 = 639;
  if (y2 > 479)
    y2 = 479;
  if (x1 < 0)
    x1 = 0;
  if (y1 < 0)
    y1 = 0;
  if (x2 < 0)
    x2 = 0;
  if (y2 < 0)
    y2 = 0;
  if (x1 > x2)
    SWAP(x1, x2);
  if (y1 > y2)
    SWAP(y1, y2);
  // left edge
  col = x1;
  for (row = y1; row <= y2; row++) {
    // 640x480
    // pixel_ptr = (char *)vga_pixel_ptr + (row<<10)    + col ;
    //  set pixel color
    //*(char *)pixel_ptr = pixel_color;
    VGA_PIXEL(col, row, pixel_color);
  }

  // right edge
  col = x2;
  for (row = y1; row <= y2; row++) {
    // 640x480
    // pixel_ptr = (char *)vga_pixel_ptr + (row<<10)    + col ;
    //  set pixel color
    //*(char *)pixel_ptr = pixel_color;
    VGA_PIXEL(col, row, pixel_color);
  }

  // top edge
  row = y1;
  for (col = x1; col <= x2; ++col) {
    // 640x480
    // pixel_ptr = (char *)vga_pixel_ptr + (row<<10)    + col ;
    //  set pixel color
    //*(char *)pixel_ptr = pixel_color;
    VGA_PIXEL(col, row, pixel_color);
  }

  // bottom edge
  row = y2;
  for (col = x1; col <= x2; ++col) {
    // 640x480
    // pixel_ptr = (char *)vga_pixel_ptr + (row<<10)    + col ;
    //  set pixel color
    //*(char *)pixel_ptr = pixel_color;
    VGA_PIXEL(col, row, pixel_color);
  }
}

/****************************************************************************************
 * Draw a horixontal line on the VGA monitor
 ****************************************************************************************/
#define SWAP(X, Y)                                                             \
  do {                                                                         \
    int temp = X;                                                              \
    X = Y;                                                                     \
    Y = temp;                                                                  \
  } while (0)

void VGA_Hline(int x1, int y1, int x2, short pixel_color) {
  char *pixel_ptr;
  int row, col;

  /* check and fix box coordinates to be valid */
  if (x1 > 639)
    x1 = 639;
  if (y1 > 479)
    y1 = 479;
  if (x2 > 639)
    x2 = 639;
  if (x1 < 0)
    x1 = 0;
  if (y1 < 0)
    y1 = 0;
  if (x2 < 0)
    x2 = 0;
  if (x1 > x2)
    SWAP(x1, x2);
  // line
  row = y1;
  for (col = x1; col <= x2; ++col) {
    // 640x480
    // pixel_ptr = (char *)vga_pixel_ptr + (row<<10)    + col ;
    //  set pixel color
    //*(char *)pixel_ptr = pixel_color;
    VGA_PIXEL(col, row, pixel_color);
  }
}

/****************************************************************************************
 * Draw a vertical line on the VGA monitor
 ****************************************************************************************/
#define SWAP(X, Y)                                                             \
  do {                                                                         \
    int temp = X;                                                              \
    X = Y;                                                                     \
    Y = temp;                                                                  \
  } while (0)

void VGA_Vline(int x1, int y1, int y2, short pixel_color) {
  char *pixel_ptr;
  int row, col;

  /* check and fix box coordinates to be valid */
  if (x1 > 639)
    x1 = 639;
  if (y1 > 479)
    y1 = 479;
  if (y2 > 479)
    y2 = 479;
  if (x1 < 0)
    x1 = 0;
  if (y1 < 0)
    y1 = 0;
  if (y2 < 0)
    y2 = 0;
  if (y1 > y2)
    SWAP(y1, y2);
  // line
  col = x1;
  for (row = y1; row <= y2; row++) {
    // 640x480
    // pixel_ptr = (char *)vga_pixel_ptr + (row<<10)    + col ;
    //  set pixel color
    //*(char *)pixel_ptr = pixel_color;
    VGA_PIXEL(col, row, pixel_color);
  }
}

/****************************************************************************************
 * Draw a filled circle on the VGA monitor
 ****************************************************************************************/

void VGA_disc(int x, int y, int r, short pixel_color) {
  char *pixel_ptr;
  int row, col, rsqr, xc, yc;

  rsqr = r * r;

  for (yc = -r; yc <= r; yc++)
    for (xc = -r; xc <= r; xc++) {
      col = xc;
      row = yc;
      // add the r to make the edge smoother
      if (col * col + row * row <= rsqr + r) {
        col += x; // add the center point
        row += y; // add the center point
        // check for valid 640x480
        if (col > 639)
          col = 639;
        if (row > 479)
          row = 479;
        if (col < 0)
          col = 0;
        if (row < 0)
          row = 0;
        // pixel_ptr = (char *)vga_pixel_ptr + (row<<10) + col ;
        //  set pixel color
        //*(char *)pixel_ptr = pixel_color;
        VGA_PIXEL(col, row, pixel_color);
      }
    }
}

/****************************************************************************************
 * Draw a  circle on the VGA monitor
 ****************************************************************************************/

void VGA_circle(int x, int y, int r, int pixel_color) {
  char *pixel_ptr;
  int row, col, rsqr, xc, yc;
  int col1, row1;
  rsqr = r * r;

  for (yc = -r; yc <= r; yc++) {
    // row = yc;
    col1 = (int)sqrt((float)(rsqr + r - yc * yc));
    // right edge
    col = col1 + x; // add the center point
    row = yc + y;   // add the center point
    // check for valid 640x480
    if (col > 639)
      col = 639;
    if (row > 479)
      row = 479;
    if (col < 0)
      col = 0;
    if (row < 0)
      row = 0;
    // pixel_ptr = (char *)vga_pixel_ptr + (row<<10) + col ;
    //  set pixel color
    //*(char *)pixel_ptr = pixel_color;
    VGA_PIXEL(col, row, pixel_color);
    // left edge
    col = -col1 + x; // add the center point
    // check for valid 640x480
    if (col > 639)
      col = 639;
    if (row > 479)
      row = 479;
    if (col < 0)
      col = 0;
    if (row < 0)
      row = 0;
    // pixel_ptr = (char *)vga_pixel_ptr + (row<<10) + col ;
    //  set pixel color
    //*(char *)pixel_ptr = pixel_color;
    VGA_PIXEL(col, row, pixel_color);
  }
  for (xc = -r; xc <= r; xc++) {
    // row = yc;
    row1 = (int)sqrt((float)(rsqr + r - xc * xc));
    // right edge
    col = xc + x;   // add the center point
    row = row1 + y; // add the center point
    // check for valid 640x480
    if (col > 639)
      col = 639;
    if (row > 479)
      row = 479;
    if (col < 0)
      col = 0;
    if (row < 0)
      row = 0;
    // pixel_ptr = (char *)vga_pixel_ptr + (row<<10) + col ;
    //  set pixel color
    //*(char *)pixel_ptr = pixel_color;
    VGA_PIXEL(col, row, pixel_color);
    // left edge
    row = -row1 + y; // add the center point
    // check for valid 640x480
    if (col > 639)
      col = 639;
    if (row > 479)
      row = 479;
    if (col < 0)
      col = 0;
    if (row < 0)
      row = 0;
    // pixel_ptr = (char *)vga_pixel_ptr + (row<<10) + col ;
    //  set pixel color
    //*(char *)pixel_ptr = pixel_color;
    VGA_PIXEL(col, row, pixel_color);
  }
}

// =============================================
// === Draw a line
// =============================================
// plot a line
// at x1,y1 to x2,y2 with color
// Code is from David Rodgers,
//"Procedural Elements of Computer Graphics",1985
void VGA_line(int x1, int y1, int x2, int y2, short c) {
  int e;
  signed int dx, dy, j, temp;
  signed int s1, s2, xchange;
  signed int x, y;
  char *pixel_ptr;

  /* check and fix line coordinates to be valid */
  if (x1 > 639)
    x1 = 639;
  if (y1 > 479)
    y1 = 479;
  if (x2 > 639)
    x2 = 639;
  if (y2 > 479)
    y2 = 479;
  if (x1 < 0)
    x1 = 0;
  if (y1 < 0)
    y1 = 0;
  if (x2 < 0)
    x2 = 0;
  if (y2 < 0)
    y2 = 0;

  x = x1;
  y = y1;

  // take absolute value
  if (x2 < x1) {
    dx = x1 - x2;
    s1 = -1;
  }

  else if (x2 == x1) {
    dx = 0;
    s1 = 0;
  }

  else {
    dx = x2 - x1;
    s1 = 1;
  }

  if (y2 < y1) {
    dy = y1 - y2;
    s2 = -1;
  }

  else if (y2 == y1) {
    dy = 0;
    s2 = 0;
  }

  else {
    dy = y2 - y1;
    s2 = 1;
  }

  xchange = 0;

  if (dy > dx) {
    temp = dx;
    dx = dy;
    dy = temp;
    xchange = 1;
  }

  e = ((int)dy << 1) - dx;

  for (j = 0; j <= dx; j++) {
    // video_pt(x,y,c); //640x480
    // pixel_ptr = (char *)vga_pixel_ptr + (y<<10)+ x;
    //  set pixel color
    //*(char *)pixel_ptr = c;
    VGA_PIXEL(x, y, c);

    if (e >= 0) {
      if (xchange == 1)
        x = x + s1;
      else
        y = y + s2;
      e = e - ((int)dx << 1);
    }

    if (xchange == 1)
      y = y + s2;
    else
      x = x + s1;

    e = e + ((int)dy << 1);
  }
}

void VGA_shift(unsigned int shift, int x0, int y0, int x1, int y1) {
  x0 = CLAMP0(x0, SCREEN_XMAX);
  y0 = CLAMP0(y0, SCREEN_YMAX);
  x1 = CLAMP(x1, x0, SCREEN_XMAX);
  y1 = CLAMP(y1, y0, SCREEN_YMAX);

  char *screen_ptr = (char *)vga_pixel_ptr;
  int *line_ptr;
  int *offset_ptr;
  int *end_ptr;
  for (int y = y0; y < y1; y++) {
    line_ptr = (int *)(screen_ptr + ((y * 640 + x0) << 1));
    offset_ptr = (int *)(screen_ptr + ((y * 640 + x0 + shift) << 1));
    end_ptr = (int *)(screen_ptr + ((y * 640 + (x1 - shift)) << 1));

    memmove(line_ptr, offset_ptr, sizeof(short) * (x1 - x0 - shift));
    memset(end_ptr, black, sizeof(short) * shift);
  }
}
