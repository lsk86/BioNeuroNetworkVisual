// HPS interface for the DE1-SoC pyloric neuron network (ECE5760).
// Chris Parker & Lucas Keith; VGA helpers adapted from Bruce Land.
// Build on the board: gcc -std=gnu99 -O2 interface.c graphics.c -pthread -lm -lrt -o interface
//
// Membrane voltage in mV-scale fixed point; time step ~1/16 ms.

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "address_map.h"
#include "graphics.h"

//////////////////////////////////////////////////////////////
/// Math macros
#define int2fix(a) (((int)(a)) << 16)
#define fix2int(a) ((signed char)((a) >> 16))
#define float2fix(a) (int)(a * 65536.0f)
#define fix2float(a) (((float)(a)) / 65536.0f)

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define CLAMP(a, min, max) (MAX((min), MIN((a), (max))))
#define CLAMP0(a, max) (CLAMP((a), 0, (max)))

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
/// VGA Macros

// Draw an arrow on the screen
static void VGA_arrow(int x0, int y0, int x1, int y1, float mag_max);

// Plot centers
#define ROW1_Y 120
#define ROW2_Y 370
#define COL1_X_START 5
#define COL1_X_END 320
#define COL2_X_START 320
#define COL2_X_END 635
#define COL_WIDTH (COL2_X_END - COL1_X_START)

// Plot scale
#define PLOT_SCALE 35
#define TIME_SCALE 1

#define MIN_V -150.0f
#define RANGE_V 90.0f - MIN_V // 40.0f
#define MIN_U -60.0f
#define RANGE_U 80.0f - MIN_U
#define NUM_ARROWS_HORIZONTAL 40
#define NUM_ARROWS_VERTICAL NUM_ARROWS_HORIZONTAL

#define TRAIL_DEPTH 20
#define TRAIL_BG black

volatile float plot_scale = PLOT_SCALE;
//Number of iterations per pixel
volatile int time_scale = TIME_SCALE;
volatile int neuron_select = 1;
volatile int last_drawtime_ms = 0;

static inline int centered_to_screen_x(float x_c) {
  return ORIGIN_X + (int)lroundf(x_c);
}
static inline int centered_to_screen_y(float y_c) {
  return ORIGIN_Y + (int)lroundf(y_c); // +y up in math space
}

int convert_to_VGAXCoord(float valueV, float minV, float rangeV) {
  float vtoPixel = COL1_X_END / rangeV;
  int x = (int)(((valueV - minV) * vtoPixel) + 0.5f);
  return x;
}

int convert_to_VGAYCoord(float valueU, float minU, float rangeU) {
  float utoPixel = SCREEN_YMAX / rangeU;
  float y = 479 - (int)(((valueU - minU) * utoPixel) + 0.5f);
  return y;
}

int convert_to_FPGA_value(float value) {
  int fpgaValue = float2fix(value / 100.0f);
  return fpgaValue;
}

float convert_FPGA_Value_to_mV(int value) { return fix2float(value) * 100.0f; }

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
/// FPGA I/O

// Reset thread
void *reset();

// the light weight bus base
void *h2p_lw_virtual_base;

// pixel buffer
void *vga_pixel_virtual_base;

// character buffer
void *vga_char_virtual_base;

// /dev/mem file id
int fd;

void *pio_virtual_base;
// initial conditions
volatile int *v1_init_ptr = NULL;
volatile int *v2_init_ptr = NULL;
volatile int *v3_init_ptr = NULL;
volatile int *v4_init_ptr = NULL;
volatile int *v5_init_ptr = NULL;
volatile int *v6_init_ptr = NULL;
volatile int *u1_init_ptr = NULL;
volatile int *u2_init_ptr = NULL;
volatile int *u3_init_ptr = NULL;
volatile int *u4_init_ptr = NULL;
volatile int *u5_init_ptr = NULL;
volatile int *u6_init_ptr = NULL;

volatile int *ab_ptr = NULL;
volatile int *ab56_ptr = NULL;
volatile int *c1_ptr = NULL;
volatile int *c2_ptr = NULL;
volatile int *c3_ptr = NULL;
volatile int *c4_ptr = NULL;
volatile int *c5_ptr = NULL;
volatile int *c6_ptr = NULL;
volatile int *d1_ptr = NULL;
volatile int *d2_ptr = NULL;
volatile int *d3_ptr = NULL;
volatile int *d4_ptr = NULL;
volatile int *d5_ptr = NULL;
volatile int *d6_ptr = NULL;
volatile int *i_ptr = NULL;
volatile int *dt_ptr = NULL;

// control
volatile int *rst_ptr = NULL;
volatile int *clk_ptr = NULL;

// output
volatile int *v1_ptr = NULL;
volatile int *v2_ptr = NULL;
volatile int *v3_ptr = NULL;
volatile int *v4_ptr = NULL;
volatile int *v5_ptr = NULL;
volatile int *v6_ptr = NULL;
volatile int *u1_ptr = NULL;
volatile int *u2_ptr = NULL;
volatile int *u3_ptr = NULL;
volatile int *u4_ptr = NULL;
volatile int *u5_ptr = NULL;
volatile int *u6_ptr = NULL;

volatile int *du1_ptr = NULL;
volatile int *dv1_ptr = NULL;
volatile int *du2_ptr = NULL;
volatile int *dv2_ptr = NULL;
volatile int *du3_ptr = NULL;
volatile int *dv3_ptr = NULL;
volatile int *du4_ptr = NULL;
volatile int *dv4_ptr = NULL;
volatile int *du5_ptr = NULL;
volatile int *dv5_ptr = NULL;
volatile int *du6_ptr = NULL;
volatile int *dv6_ptr = NULL;

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
/// Threading and state

// Protection
pthread_mutex_t speed_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t param_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t hardware_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t running_lock = PTHREAD_MUTEX_INITIALIZER;

// the thread identifiers
pthread_t thread_read, thread_reset, thread_plot;

// semaphores
sem_t update_params; // tells parameter update thread to update parameters
sem_t reset_flag;    // tells reset thread to reset integrator
sem_t plot_start;    // tells plot thread to start plotting

pthread_cond_t paused_cond = PTHREAD_COND_INITIALIZER;

// Program state variables
bool kill_flag;
bool phase_plot;
bool pause_flag = false;
bool dprint_flag = false;

float speed_multiplier = 1.0f;

// measure time
struct timeval t1, t2;
double elapsedTime;

#define DEFAULT_TIME_US 10000

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
/// Neuron parameters

#define NUM_NEURONS 6

// Fallback fixed-point parameters
#define V1_INIT_FIX (0x34CCD)
#define V2_INIT_FIX (0x34CCD)
#define V3_INIT_FIX (0x34CCD)
#define V4_INIT_FIX (0x34CCD)
#define V5_INIT_FIX (0x34CCD)
#define V6_INIT_FIX (0x34CCD)
#define U1_INIT_FIX (0x3CCCD)
#define U2_INIT_FIX (0x3CCCD)
#define U3_INIT_FIX (0x3CCCD)
#define U4_INIT_FIX (0x3CCCD)
#define U5_INIT_FIX (0x3CCCD)
#define U6_INIT_FIX (0x3CCCD)

#define A_FIX (6)
#define B_FIX (2)
#define A1_FIX (6)
#define A2_FIX (6)
#define A3_FIX (5)
#define A4_FIX (3)
#define A5_FIX (3)
#define A6_FIX (5)
#define B1_FIX (1)
#define B2_FIX (2)
#define B3_FIX (0)
#define B4_FIX (0)
#define B5_FIX (1)
#define B6_FIX (0)
#define A1_FIXS ((A1_FIX << A1_SHIFT) & A1_MASK)
#define A2_FIXS ((A2_FIX << A2_SHIFT) & A2_MASK)
#define A3_FIXS ((A3_FIX << A3_SHIFT) & A3_MASK)
#define A4_FIXS ((A4_FIX << A4_SHIFT) & A4_MASK)
#define A5_FIXS ((A5_FIX << A5_SHIFT) & A5_MASK)
#define A6_FIXS ((A6_FIX << A6_SHIFT) & A6_MASK)
#define B1_FIXS ((B1_FIX << B1_SHIFT) & B1_MASK)
#define B2_FIXS ((B2_FIX << B2_SHIFT) & B2_MASK)
#define B3_FIXS ((B3_FIX << B3_SHIFT) & B3_MASK)
#define B4_FIXS ((B4_FIX << B4_SHIFT) & B4_MASK)
#define B5_FIXS ((B5_FIX << B5_SHIFT) & B5_MASK)
#define B6_FIXS ((B6_FIX << B6_SHIFT) & B6_MASK)
#define AB_FIX                                                                 \
  ((A1_FIXS) | (A2_FIXS) | (A3_FIXS) | (A4_FIXS) | (B1_FIXS) | (B2_FIXS) |     \
   (B3_FIXS) | (B4_FIXS))
#define AB56_FIX ((A5_FIXS) | (A6_FIXS) | (B5_FIXS) | (B6_FIXS))

#define C_FLOAT -55.0f
#define D_FLOAT 4.0f
#define C1_FLOAT (-65.f)
#define C2_FLOAT (-50.f)
#define C3_FLOAT (-60.f)
#define C4_FLOAT (-60.f)
#define C5_FLOAT (-60.f)
#define C6_FLOAT (-60.f)

#define D1_FLOAT (6.f)
#define D2_FLOAT (2.f)
#define D3_FLOAT (4.f)
#define D4_FLOAT (0.f)
#define D5_FLOAT (0.f)
#define D6_FLOAT (4.f)

#define C1_FIX (0x38000)
#define C2_FIX (0x3599A)
#define C3_FIX (0x36667)
#define C4_FIX (0x36667)
#define C5_FIX (0x36667)
#define C6_FIX (0x36667)
#define D1_FIX (0x0051E)
#define D2_FIX (0x00F5C)
#define D3_FIX (0x00A3D)
#define D4_FIX (0x00000)
#define D5_FIX (0x00000)
#define D6_FIX (0x00A3D)
#define I_FIX (0x00800)

/* Neuron u-update uses (du >>> dt); CardiacGanglionSim used >>>4, so 4 is a
 * good default. */
#define DT_FIX (6)
#define DT_FIXS (DT_FIX & DT_MASK)

// Initial neuron parameters
int a_init_vals[NUM_NEURONS] = {A1_FIX, A2_FIX, A3_FIX, A4_FIX, A5_FIX, A6_FIX};
int b_init_vals[NUM_NEURONS] = {B1_FIX, B2_FIX, B3_FIX, B4_FIX, B5_FIX, B6_FIX};
int c_init_vals[NUM_NEURONS] = {C1_FIX, C2_FIX, C3_FIX, C4_FIX, C5_FIX, C6_FIX};
int d_init_vals[NUM_NEURONS] = {D1_FIX, D2_FIX, D3_FIX, D4_FIX, D5_FIX, D6_FIX};
int vi_init_vals[NUM_NEURONS] = {V1_INIT_FIX, V2_INIT_FIX, V3_INIT_FIX,
                                 V4_INIT_FIX, V5_INIT_FIX, V6_INIT_FIX};
int ui_init_vals[NUM_NEURONS] = {U1_INIT_FIX, U2_INIT_FIX, U3_INIT_FIX,
                                 U4_INIT_FIX, U5_INIT_FIX, U6_INIT_FIX};
int i_init_val = I_FIX;

// Current neuron parameters

// 4-bit
int a_params[NUM_NEURONS];
int b_params[NUM_NEURONS];

// 18-bit fixed
int c_params[NUM_NEURONS];
int d_params[NUM_NEURONS];
int vi_params[NUM_NEURONS];
int ui_params[NUM_NEURONS];
// Single 18-bit fixed
int ibase_param;

int dt_param;

typedef struct neuron_params {
  int length; // Number of neurons
  int *i;     // Base current
  int *vi;    // Initial membrane potential
  int *ui;    // Initial membrane recovery variable value
  // Behavioral parameters
  int *a;
  int *b;
  int *c;
  int *d;
} neuron_params_t;

neuron_params_t current_params = {NUM_NEURONS, &ibase_param, vi_params,
                                  ui_params,   a_params,     b_params,
                                  c_params,    d_params};

char *neuron_names[] = {"AB", "VD", "IC", "PY", "LP", "PD"};

void copy_params(neuron_params_t *src, neuron_params_t *dest) {
  // Skip the work if they're the same or NULL
  if (src == dest) {
    return;
  }

  dest->length = src->length;
  *dest->i = *src->i;
  memmove(dest->a, src->a, sizeof(int) * src->length);
  memmove(dest->b, src->b, sizeof(int) * src->length);
  memmove(dest->c, src->c, sizeof(int) * src->length);
  memmove(dest->d, src->d, sizeof(int) * src->length);
  memmove(dest->vi, src->vi, sizeof(int) * src->length);
  memmove(dest->ui, src->ui, sizeof(int) * src->length);
}

void set_defaults(neuron_params_t *params) {
  params->length = NUM_NEURONS;
  *params->i = i_init_val;
  for (int i = 0; i < NUM_NEURONS; i++) {
    params->vi[i] = vi_init_vals[i];
    params->ui[i] = ui_init_vals[i];

    params->a[i] = a_init_vals[i];
    params->b[i] = b_init_vals[i];
    params->c[i] = c_init_vals[i];
    params->d[i] = d_init_vals[i];
  }
}

bool parse_params_raw(char *str, neuron_params_t *params) {
  // format is `<p1>=<v1>; ...; <pn>=<vn>;`
  // 2 delimeters: `;` and `=` with arbitrary whitespace
  // ; separates tripples, = separates params and values

  const char *t_delim = ";";
  const char *p_delim = "=";

  char *saveptr_in = NULL;
  char *saveptr_out = NULL;

  char *str_in, *str_out, *token, *param, *val;

  for (str_out = str;; str_out = NULL) {
    token = strtok_r(str_out, t_delim, &saveptr_out);
    if (token == NULL)
      break;

    if (token != NULL && (*token == '\n' || *token == '\0' || *token == '\t'))
      break;

    str_in = token;

    param = strtok_r(str_in, p_delim, &saveptr_in);
    // Fail if param or val is missing
    if (param == NULL) {
      printf("WARNING: parameter was null.\n");
      return false;
    }

    str_in = NULL;

    val = strtok_r(str_in, p_delim, &saveptr_in);
    if (val == NULL) {
      printf("WARNING: value was null.\n");
      return false;
    }

    // Clear blank spaces
    for (; *param != '\0' && *(param) == ' '; param++)
      ;
    for (; *val != '\0' && *(val) == ' '; val++)
      ;

    // Remove trailing space
    char *curr;
    for (curr = param; *curr != '\0' && *curr != ' '; curr++)
      ;
    *curr = '\0';
    for (curr = val; *curr != '\0' && *curr != ' '; curr++)
      ;
    *curr = '\0';

    int index = 0;
    int status;

    int parsed_value = 0;
    int *chosen_array;
    if (*param == 'i') {
      parsed_value = atoi(val);
      chosen_array = params->i;
      index = 1;
    } else {
      index = atoi(param + 1);

      switch (*param) {
      case 'v':
        chosen_array = params->vi;
        parsed_value = atoi(val);
        break;
      case 'u':
        chosen_array = params->ui;
        parsed_value = atoi(val);
        break;
      case 'a':
        chosen_array = params->a;
        parsed_value = atoi(val);
        break;
      case 'b':
        chosen_array = params->b;
        parsed_value = atoi(val);
        break;
      case 'c':
        chosen_array = params->c;
        parsed_value = atoi(val);
        break;
      case 'd':
        chosen_array = params->d;
        parsed_value = atoi(val);
        break;
      default:
        printf("WARNING: Ignoring unknown parameter %c.\n", *param);
      }
    }

    if (index > params->length || index < 1) {
      printf("WARNING: Index %d out of range for # parameters %d.\n", index,
             params->length);
      return false;
    }

    // Set the appopriate value in the parameter array
    chosen_array[index - 1] = parsed_value;
  }

  return true;
}
// Parse parameters from input buffer
bool parse_params(char *str, neuron_params_t *params) {
  // format is `<p1>=<v1>; ...; <pn>=<vn>;`
  // 2 delimeters: `;` and `=` with arbitrary whitespace
  // ; separates tripples, = separates params and values

  const char *t_delim = ";";
  const char *p_delim = "=";

  char *saveptr_in = NULL;
  char *saveptr_out = NULL;

  char *str_in, *str_out, *token, *param, *val;

  for (str_out = str;; str_out = NULL) {
    token = strtok_r(str_out, t_delim, &saveptr_out);
    if (token == NULL)
      break;

    if (token != NULL && (*token == '\n' || *token == '\0' || *token == '\t'))
      break;

    str_in = token;

    param = strtok_r(str_in, p_delim, &saveptr_in);
    // Fail if param or val is missing
    if (param == NULL) {
      printf("WARNING: parameter was null.\n");
      return false;
    }

    str_in = NULL;

    val = strtok_r(str_in, p_delim, &saveptr_in);
    if (val == NULL) {
      printf("WARNING: value was null.\n");
      return false;
    }

    // Clear blank spaces
    for (; *param != '\0' && *(param) == ' '; param++)
      ;
    for (; *val != '\0' && *(val) == ' '; val++)
      ;

    // Remove trailing space
    char *curr;
    for (curr = param; *curr != '\0' && *curr != ' '; curr++)
      ;
    *curr = '\0';
    for (curr = val; *curr != '\0' && *curr != ' '; curr++)
      ;
    *curr = '\0';

    int index = 0;
    int status;

    int parsed_value = 0;
    int *chosen_array;
    if (*param == 'i') {
      parsed_value = convert_to_FPGA_value(atof(val));
      chosen_array = params->i;
      index = 1;
    } else {
      index = atoi(param + 1);

      switch (*param) {
      case 'v':
        chosen_array = params->vi;
        parsed_value = convert_to_FPGA_value(atof(val));
        break;
      case 'u':
        chosen_array = params->ui;
        parsed_value = convert_to_FPGA_value(atof(val));
        break;
      // a & b are used as (...) >>> a thus 0.5 -> -round(log2f(0.5)) = - (-1) =
      // 1
      case 'a':
        chosen_array = params->a;
        parsed_value = (int)(0.5 - log2f(atof(val)));
        break;
      case 'b':
        chosen_array = params->b;
        parsed_value = (int)(0.5 - log2f(atof(val)));
        break;
      case 'c':
        chosen_array = params->c;
        parsed_value = convert_to_FPGA_value(atof(val));
        break;
      case 'd':
        chosen_array = params->d;
        parsed_value = convert_to_FPGA_value(atof(val));
        break;
      default:
        printf("WARNING: Ignoring unknown parameter %c.\n", *param);
      }
    }

    if (index > params->length || index < 1) {
      printf("WARNING: Index %d out of range for # parameters %d.\n", index,
             params->length);
      return false;
    }

    // Set the appopriate value in the parameter array
    chosen_array[index - 1] = parsed_value;
  }

  return true;
}

#define DEFAULT_TYPE 0
#define FIX18_TYPE 1
#define NIBBLE_TYPE 2
#define FIX18_SINGLETON_TYPE 3

void print_mvfix(int p) { printf("%f", convert_FPGA_Value_to_mV(p)); }

void print_ab(int p) { printf("%f", 1.0f / powf(2, p)); }

void print_default(int p) { printf("%d", p); }

void print_params(int *param_array, char prefix, int type) {
  printf("\t%c :\n", prefix);

  if (param_array == NULL) {
    printf("\t [ 0] -- <NULL PARAMETER ARRAY>\n");
    return;
  }
  // Print function for given type
  void (*pfunc)(int);

  int size = NUM_NEURONS;
  int index_offset = 1;

  switch (type) {
  case FIX18_TYPE:
    pfunc = print_mvfix;
    break;
  case NIBBLE_TYPE:
    pfunc = print_ab;
    break;
  case FIX18_SINGLETON_TYPE:
    pfunc = print_mvfix;
    size = 1;
    index_offset = 0;
    break;
  case DEFAULT_TYPE:
  default:
    pfunc = print_default;
    break;
  }

  for (int i = 0; i < size; i++) {
    printf("\t [%.2d] = ", i + index_offset);
    pfunc(param_array[i]);
    puts("\n");
  }
}

#define NUM_PARAMS 7
void read_param_file(char *path, neuron_params_t *params) {
  // set_defaults(params);
  FILE *param_file;
  param_file = fopen(path, "r");

  if (param_file == NULL) {
    printf("ERROR: Cannot read parameter file: %s\n", path);
    return;
  }

  if (access(path, F_OK) != 0) {
    printf("WARNING: Cannot find parameter file:%s\n", path);
    return;
  }

  param_file = fopen(path, "r");
  if (param_file == NULL) {
    printf("WARNING: Cannot read from parameter file:%s\n", path);
    return;
  }

  size_t buff_len = sizeof("par000 = 00000; ") * params->length * NUM_PARAMS;
  char *inbuff = malloc(buff_len);
  size_t ret = fread(inbuff, 1, buff_len - 1, param_file);
  fclose(param_file);

  if (ret < buff_len - 2) {
    if (ferror(param_file)) {
      printf("ERROR: File read error.\n");
      free(inbuff);
      return;
    }

    if (!feof(param_file)) {
      printf(
          "WARNING: Could not read entire file. File format may be invalid.\n");
    }
  }

  inbuff[buff_len - 1] = '\0';
  inbuff[ret] = '\0';

  (void)parse_params_raw(inbuff, params);

  free(inbuff);
}

void write_param_file(char *path, const neuron_params_t *params) {
  FILE *param_file;
  // Allocate memory for each key-val pair + formatting for each type of
  // parameter

  // Check if file exists
  if (access(path, F_OK) == 0) {
    printf("WARNING: Parameters not saved! Parameter file already exists.\n");
    return;
  }

  param_file = fopen(path, "w");
  if (param_file == NULL) {
    printf("ERROR: Cannot write parameter file: %s", path);
    return;
  }

  size_t buff_len = sizeof("par000 = 00000; ") * params->length * NUM_PARAMS;
  char *outbuf = malloc(buff_len);
  char *head = outbuf;
  int written = 0;
  int remaining = buff_len;

  written = snprintf(head, remaining, "%c = %d;", 'i', *params->i);
  remaining -= written;
  head += written;

  const int *p_ptrs[] = {params->vi, params->ui, params->a,
                         params->b,  params->c,  params->d};
  const char p_chars[] = {'v', 'u', 'a', 'b', 'c', 'd'};

  for (int j = 0; j < sizeof(p_chars); j++) {
    for (int i = 0; i < params->length; i++) {
      written = snprintf(head, remaining, "%c%d = %d;", p_chars[j], i + 1,
                         (p_ptrs[j])[i]);
      remaining -= written;
      if (remaining <= 1) {
        printf("ERROR: Overran allocated memory when writing parameters... "
               "file not written.\n");
        free(outbuf);
        return;
      }
      head += written;
    }
  }

  *(++head) = '\n';
  *(++head) = '\0';
  remaining -= 2;

  fwrite(outbuf, 1, buff_len, param_file);

  // Cleanup
  fclose(param_file);
  free(outbuf);
}

//////////////////////////////////////////////////////////////
/// Neuron phase plots

typedef struct phase_plot {
  int id;
  float minV;
  float maxV;
  float rangeV;
  float minU;
  float maxU;
  float rangeU;
  int num_samples_v;
  int num_samples_u;
  float
      *du_plot; // 2D array (index with plot[index_u * num_samples_u + index_v])
  float
      *dv_plot; // 2D array (index with plot[index_u * num_samples_u + index_v])

} phase_plot_t;

phase_plot_t *neuron_plots[NUM_NEURONS];
bool plots_populated = false;

void plot_phase_portait(phase_plot_t *plt);

void init_plot(phase_plot_t *p, int id) {
  p->id = id;
  p->minV = 0.;
  p->maxV = 0.;
  p->rangeV = 0.;
  p->minU = 0.;
  p->maxU = 0.;
  p->rangeU = 0.;
  p->num_samples_v = 0;
  p->num_samples_u = 0;
  p->du_plot = NULL;
  p->dv_plot = NULL;
}

void free_plots() {
  for (int i = 0; i < NUM_NEURONS; i++) {
    phase_plot_t *plt = neuron_plots[i];

    if (plt->du_plot != NULL) {
      free(plt->du_plot);
      plt->du_plot = NULL;
    }

    if (plt->dv_plot != NULL) {
      free(plt->dv_plot);
      plt->dv_plot = NULL;
    }
  }
}

void update_all_phase_portrait_values() {
  int num_arrows_vertical = neuron_plots[0]->num_samples_u;
  int num_arrows_horizontal = neuron_plots[0]->num_samples_v;
  float duf1, duf2, duf3, duf4, duf5, duf6;
  float dvf1, dvf2, dvf3, dvf4, dvf5, dvf6;
  float minV = neuron_plots[0]->minV;
  float minU = neuron_plots[0]->minU;

  float incrV = neuron_plots[0]->rangeV / (float)num_arrows_horizontal;
  float incrU = neuron_plots[0]->rangeU / (float)num_arrows_vertical;

  pthread_mutex_lock(&hardware_lock);
  *rst_ptr = true;

  *clk_ptr = false;
  for (int i = 0; i < num_arrows_horizontal; i++) {
    for (int j = 0; j < num_arrows_vertical; j++) {
      // Set operating point (v,u) in fixed-point hardware space
      *v1_init_ptr = convert_to_FPGA_value(minV + (i * incrV));
      *v2_init_ptr = convert_to_FPGA_value(minV + (i * incrV));
      *v3_init_ptr = convert_to_FPGA_value(minV + (i * incrV));
      *v4_init_ptr = convert_to_FPGA_value(minV + (i * incrV));
      *u1_init_ptr = convert_to_FPGA_value(minU + (j * incrU));
      *u2_init_ptr = convert_to_FPGA_value(minU + (j * incrU));
      *u3_init_ptr = convert_to_FPGA_value(minU + (j * incrU));
      *u4_init_ptr = convert_to_FPGA_value(minU + (j * incrU));
      *u5_init_ptr = convert_to_FPGA_value(minU + (j * incrU));
      *u6_init_ptr = convert_to_FPGA_value(minU + (j * incrU));

      // Step hardware and read derivatives at this operating point
      *clk_ptr = true;
      duf1 = convert_FPGA_Value_to_mV(*du1_ptr);
      dvf1 = convert_FPGA_Value_to_mV(*dv1_ptr);
      duf2 = convert_FPGA_Value_to_mV(*du2_ptr);
      dvf2 = convert_FPGA_Value_to_mV(*dv2_ptr);
      duf3 = convert_FPGA_Value_to_mV(*du3_ptr);
      dvf3 = convert_FPGA_Value_to_mV(*dv3_ptr);
      duf4 = convert_FPGA_Value_to_mV(*du4_ptr);
      dvf4 = convert_FPGA_Value_to_mV(*dv4_ptr);
      duf5 = convert_FPGA_Value_to_mV(*du5_ptr);
      dvf5 = convert_FPGA_Value_to_mV(*dv5_ptr);
      duf6 = convert_FPGA_Value_to_mV(*du6_ptr);
      dvf6 = convert_FPGA_Value_to_mV(*dv6_ptr);
      *clk_ptr = false;

      neuron_plots[0]->du_plot[j * num_arrows_vertical + i] = duf1;
      neuron_plots[1]->du_plot[j * num_arrows_vertical + i] = duf2;
      neuron_plots[2]->du_plot[j * num_arrows_vertical + i] = duf3;
      neuron_plots[3]->du_plot[j * num_arrows_vertical + i] = duf4;
      neuron_plots[4]->du_plot[j * num_arrows_vertical + i] = duf5;
      neuron_plots[5]->du_plot[j * num_arrows_vertical + i] = duf6;

      neuron_plots[0]->dv_plot[j * num_arrows_vertical + i] = dvf1;
      neuron_plots[1]->dv_plot[j * num_arrows_vertical + i] = dvf2;
      neuron_plots[2]->dv_plot[j * num_arrows_vertical + i] = dvf3;
      neuron_plots[3]->dv_plot[j * num_arrows_vertical + i] = dvf4;
      neuron_plots[4]->dv_plot[j * num_arrows_vertical + i] = dvf5;
      neuron_plots[5]->dv_plot[j * num_arrows_vertical + i] = dvf6;
    }
  }
  *rst_ptr = false;
  pthread_mutex_unlock(&hardware_lock);
}

void get_all_phase_portrait_values(float minV, float rangeV, float minU,
                                   float rangeU, int num_arrows_horizontal,
                                   int num_arrows_vertical) {
  float maxV = minV + rangeV;
  float maxU = minU + rangeU;

  float incrV = rangeV / (float)num_arrows_horizontal;
  float incrU = rangeU / (float)num_arrows_vertical;

  // Set parameters
  for (int i = 0; i < NUM_NEURONS; i++) {
    phase_plot_t *plt = neuron_plots[i];
    plt->minV = minV;
    plt->minU = minU;
    plt->maxU = maxU;
    plt->maxV = maxV;
    plt->rangeU = rangeU;
    plt->rangeV = rangeV;
    plt->num_samples_u = num_arrows_vertical;
    plt->num_samples_v = num_arrows_horizontal;

    // Alloc or realloc memory for plot values
    if (plt->dv_plot == NULL) {
      plt->dv_plot = malloc(sizeof(plt->dv_plot) * num_arrows_horizontal *
                            num_arrows_vertical);
    } else {
      plt->dv_plot =
          realloc(plt->dv_plot, sizeof(plt->dv_plot) * num_arrows_horizontal *
                                    num_arrows_vertical);
    }

    if (plt->du_plot == NULL) {
      plt->du_plot = malloc(sizeof(plt->du_plot) * num_arrows_horizontal *
                            num_arrows_vertical);
    } else {
      plt->du_plot =
          realloc(plt->du_plot, sizeof(plt->du_plot) * num_arrows_horizontal *
                                    num_arrows_vertical);
    }
  }

  float duf1 = 0.0f, dvf1 = 0.0f, duf2 = 0.0f, dvf2 = 0.0f, duf3 = 0.0f,
        dvf3 = 0.0f, duf4 = 0.0f, dvf4 = 0.0f, duf5 = 0.0f, dvf5 = 0.0f,
        duf6 = 0.0f, dvf6 = 0.0f;
  float duf = 0.0f, dvf = 0.0f;

  // Get du and dv for each (u,v) sample from hardware
  pthread_mutex_lock(&hardware_lock);
  *rst_ptr = true;

  *clk_ptr = false;
  for (int i = 0; i < num_arrows_horizontal; i++) {
    for (int j = 0; j < num_arrows_vertical; j++) {
      // Set operating point (v,u) in fixed-point hardware space
      *v1_init_ptr = convert_to_FPGA_value(minV + (i * incrV));
      *v2_init_ptr = convert_to_FPGA_value(minV + (i * incrV));
      *v3_init_ptr = convert_to_FPGA_value(minV + (i * incrV));
      *v4_init_ptr = convert_to_FPGA_value(minV + (i * incrV));
      *u1_init_ptr = convert_to_FPGA_value(minU + (j * incrU));
      *u2_init_ptr = convert_to_FPGA_value(minU + (j * incrU));
      *u3_init_ptr = convert_to_FPGA_value(minU + (j * incrU));
      *u4_init_ptr = convert_to_FPGA_value(minU + (j * incrU));
      *u5_init_ptr = convert_to_FPGA_value(minU + (j * incrU));
      *u6_init_ptr = convert_to_FPGA_value(minU + (j * incrU));
      // Step hardware and read derivatives at this operating point
      *clk_ptr = true;
      duf1 = convert_FPGA_Value_to_mV(*du1_ptr);
      dvf1 = convert_FPGA_Value_to_mV(*dv1_ptr);
      duf2 = convert_FPGA_Value_to_mV(*du2_ptr);
      dvf2 = convert_FPGA_Value_to_mV(*dv2_ptr);
      duf3 = convert_FPGA_Value_to_mV(*du3_ptr);
      dvf3 = convert_FPGA_Value_to_mV(*dv3_ptr);
      duf4 = convert_FPGA_Value_to_mV(*du4_ptr);
      dvf4 = convert_FPGA_Value_to_mV(*dv4_ptr);
      duf5 = convert_FPGA_Value_to_mV(*du5_ptr);
      dvf5 = convert_FPGA_Value_to_mV(*dv5_ptr);
      duf6 = convert_FPGA_Value_to_mV(*du6_ptr);
      dvf6 = convert_FPGA_Value_to_mV(*dv6_ptr);
      *clk_ptr = false;

      neuron_plots[0]->du_plot[j * num_arrows_vertical + i] = duf1;
      neuron_plots[1]->du_plot[j * num_arrows_vertical + i] = duf2;
      neuron_plots[2]->du_plot[j * num_arrows_vertical + i] = duf3;
      neuron_plots[3]->du_plot[j * num_arrows_vertical + i] = duf4;
      neuron_plots[4]->du_plot[j * num_arrows_vertical + i] = duf5;
      neuron_plots[5]->du_plot[j * num_arrows_vertical + i] = duf6;
      neuron_plots[0]->dv_plot[j * num_arrows_vertical + i] = dvf1;
      neuron_plots[1]->dv_plot[j * num_arrows_vertical + i] = dvf2;
      neuron_plots[2]->dv_plot[j * num_arrows_vertical + i] = dvf3;
      neuron_plots[3]->dv_plot[j * num_arrows_vertical + i] = dvf4;
      neuron_plots[4]->dv_plot[j * num_arrows_vertical + i] = dvf5;
      neuron_plots[5]->dv_plot[j * num_arrows_vertical + i] = dvf6;
    }
  }
  *rst_ptr = false;
  pthread_mutex_unlock(&hardware_lock);
}

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
/// Command line interface

float max_mag = 150.f;
char input_buffer[512];
void *read1() {
  int restart;
  int run_count = 0;

  while (1) {
    restart = false;
    sem_wait(&reset_flag); // wait for reset to complete if it is ongoing
    // wait for print done
    //  lock the input_buffer
    pthread_mutex_lock(&param_lock); // protect parameter access during input
    // the actual enter
    // read and parse input
    printf("Enter Commands: ");
    fgets(input_buffer, sizeof(input_buffer), stdin);

    switch (input_buffer[0]) {
    case '$':
      // speed up animation
      printf("\nSet Speed Multiplier to: ");
      (void)(void *)fgets(input_buffer, sizeof(input_buffer), stdin);
      pthread_mutex_lock(&speed_lock);
      speed_multiplier = atof(input_buffer);
      pthread_mutex_unlock(&speed_lock);
      printf("%f\n", speed_multiplier);
      break;
    case 's':
      // Plot scale
      printf("\nSet scale to: ");
      (void)(void *)fgets(input_buffer, sizeof(input_buffer), stdin);
      pthread_mutex_lock(&hardware_lock);
      plot_scale = atof(input_buffer);
      pthread_mutex_unlock(&hardware_lock);
      printf("%f\n", plot_scale);
      restart = true;
      break;
    case 'x':
      // Time-scale
      printf("\nSet timescale to: ");
      (void)(void *)fgets(input_buffer, sizeof(input_buffer), stdin);
      pthread_mutex_lock(&hardware_lock);
      time_scale = atoi(input_buffer);
      pthread_mutex_unlock(&hardware_lock);
      printf("%f\n", plot_scale);
      break;
    case 'M':
      // Time-scale
      printf("\nSet max magnitude (for color): ");
      (void)(void *)fgets(input_buffer, sizeof(input_buffer), stdin);
      max_mag = atof(input_buffer);
      printf("%f\n", max_mag);
      break;
    case 'c':
      VGA_CLEAR();
      // clear the text
      // redraw_text();
      printf("\nScreen cleared!\n");
      restart = false;
      break;
    case 'q':
      kill_flag = true;
      break;
    case 'r':
      // reset integrator
      restart = true;
      break;
    case 'p':
      pause_flag = !pause_flag; // true to pause, false to resume
      if (!pause_flag) {
        pthread_cond_signal(&paused_cond);
      }
      printf("Paused?: %d\n", pause_flag);
      break;
    case 'D':
      // Print debug info
      dprint_flag = !dprint_flag;
      if (dprint_flag) {
        printf("Printing debug info...\n");
      }
      break;
    case 'S':
      // Set parameters with triples `<param>=<val>`
      printf("Enter parameters as triples (e.g. `<param>=<val>;`)\n");
      if (fgets(input_buffer, sizeof(input_buffer), stdin)) {
        if (!parse_params(input_buffer, &current_params))
          printf("\nFailed to set parameters.\n");
      } else {
        printf("\nFailed to read input.\n");
      }
      // Fall into printing parameters for user verification
      // plots_populated = false;
      // restart = true;
    case 'G':
      // Get current parameter values
      printf("Current parameters are:\n");
      // V_inits
      print_params(current_params.vi, 'v', FIX18_TYPE);
      // U_inits
      print_params(current_params.ui, 'u', FIX18_TYPE);
      // I_Base
      print_params(current_params.i, 'I', FIX18_SINGLETON_TYPE);
      // A
      print_params(current_params.a, 'a', NIBBLE_TYPE);
      // B
      print_params(current_params.b, 'b', NIBBLE_TYPE);
      // C
      print_params(current_params.c, 'c', FIX18_TYPE);
      // D
      print_params(current_params.d, 'd', FIX18_TYPE);
      break;
    case 'V':
      printf("Enter path to new parameter file: ");
      (void)(void *)fgets(input_buffer, sizeof(input_buffer), stdin);
      input_buffer[strcspn(input_buffer, "\r\n")] = '\0';
      write_param_file(input_buffer, &current_params);
      break;
    case 'L':
      printf("Enter path to existing parameter file: ");
      (void)(void *)fgets(input_buffer, sizeof(input_buffer), stdin);
      input_buffer[strcspn(input_buffer, "\r\n")] = '\0';
      read_param_file(input_buffer, &current_params);
      break;
    case 't':
      printf("Enter 1/(2^dt): ");
      (void)(void *)fgets(input_buffer, sizeof(input_buffer), stdin);
      dt_param = atoi(input_buffer);
      printf("dt = %f\n", 1.0f / (dt_param << dt_param));
      break;
    case 'b':
      // begin plotting
      pthread_mutex_lock(&hardware_lock);
      printf("\nSelect neuron to plot (1-%d): ", NUM_NEURONS);
      fgets(input_buffer, sizeof(input_buffer), stdin);
      sscanf(input_buffer, "%d", &neuron_select);
      neuron_select -= 1;
      CLAMP0(neuron_select, NUM_NEURONS - 1);
      pthread_mutex_unlock(&hardware_lock);

      phase_plot = true;
      // fall into plot case
      if (plots_populated) {
        printf("Plots already generated. Restarting with new neuron.\n");
        VGA_box(0, 0, COL1_X_END, SCREEN_XMAX, black);
        plot_phase_portait(neuron_plots[neuron_select]);
        break;
      }
      // Fall into setting plot params if not set yet
    case 'g':
      printf("Enter minV, rangeV, minU, rangeU, #Samples, max mag. (or press "
             "enter for default): ");
      (void)(void *)fgets(input_buffer, sizeof(input_buffer), stdin);
      float minv, maxv, minu, maxu, rangeu, rangev;
      int numx, numy;

      minv = MIN_V;
      maxv = MIN_V + RANGE_V;
      minu = MIN_U;
      maxu = MIN_U + RANGE_U;
      rangeu = RANGE_U;
      rangev = RANGE_V;
      numx = NUM_ARROWS_HORIZONTAL;
      numy = NUM_ARROWS_VERTICAL;
      if (input_buffer[0] != '\n' && input_buffer[0] != '\0') {
        sscanf(input_buffer, "%f , %f , %f , %f , %d , %f", &minv, &rangev,
               &minu, &rangeu, &numx, &max_mag);
        // rangeu = maxu - minu;
        // rangev = maxv - minv;
        numy = numx;
      }

      get_all_phase_portrait_values(minv, rangev, minu, rangeu, numx, numy);
      printf("\nPhase portrait generated for all neurons!\n");
      plots_populated = true;
      phase_plot = true;
      restart = true;
      break;
    case 'u':
      printf("Updating plots...\n");
      update_all_phase_portrait_values();
      plots_populated = true;
      phase_plot = true;
      restart = true;
      break;
    default:
      // do nothing
      dprint_flag = false;
      printf("\n");
      break;
    }

    if (restart) {
      pthread_mutex_unlock(
          &param_lock); // protect parameter access during input
      sem_post(&update_params);
      VGA_CLEAR();
      pthread_create(&thread_reset, NULL, reset, NULL);
      pthread_join(thread_reset, NULL);
      printf("Reset finished!\n");

      restart = false;
    }

    pthread_mutex_unlock(&param_lock); // protect parameter access during input
    sem_post(&update_params); // put reset thread to sleep until reset requested
    sem_post(&reset_flag);

    if (kill_flag) {
      return NULL;
    }
  } // while(1)
}

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
/// Plotting and graphics

#define SHIFT_PX 637
#define DEFAULT_SHIFT 3

static int trail_x[TRAIL_DEPTH];
static int trail_y[TRAIL_DEPTH];
static int trail_count; /* 0 .. TRAIL_DEPTH */

static void trail_shift_push_draw(int x, int y, short color) {
  if (trail_count == TRAIL_DEPTH) {
    int ox = trail_x[TRAIL_DEPTH - 1];
    int oy = trail_y[TRAIL_DEPTH - 1];
    VGA_PIXEL(ox, oy, TRAIL_BG);
  }
  for (int i = TRAIL_DEPTH - 1; i > 0; i--) {
    trail_x[i] = trail_x[i - 1];
    trail_y[i] = trail_y[i - 1];
  }
  trail_x[0] = x;
  trail_y[0] = y;
  if (trail_count < TRAIL_DEPTH)
    trail_count++;
  VGA_PIXEL(x, y, color);
}
static void trail_reset(void) {
  for (int i = 0; i < trail_count; i++)
    VGA_PIXEL(trail_x[i], trail_y[i], TRAIL_BG);
  trail_count = 0;
}

void plot_phase_portait(phase_plot_t *plt) {
  char plot_name[64];
  float incrV = plt->rangeV / plt->num_samples_v;
  float incrU = plt->rangeU / plt->num_samples_u;

  for (int vi = 0; vi < plt->num_samples_v; vi++) {
    for (int ui = 0; ui < plt->num_samples_u; ui++) {
      // Convert operating point to centered math-space units
      float v0 = plt->minV + vi * incrV;
      float u0 = plt->minU + ui * incrU;

      float dvf = plt->dv_plot[plt->num_samples_u * ui + vi];
      float duf = plt->du_plot[plt->num_samples_u * ui + vi];

      // Start/end in centered coordinates mapped to VGA pixels

      int x0 = convert_to_VGAXCoord(v0, plt->minV, plt->rangeV);
      int y0 = convert_to_VGAYCoord(u0, plt->minU, plt->rangeU);
      int x1 = convert_to_VGAXCoord(v0 + dvf, plt->minV, plt->rangeV);
      int y1 = convert_to_VGAYCoord(u0 + duf, plt->minU, plt->rangeU);
      VGA_arrow(x0, y0, x1, y1, max_mag);
    }
  }

  snprintf(plot_name, sizeof(plot_name), "Neuron #%d (%s)", plt->id + 1,
           neuron_names[plt->id]);
  VGA_rect(0, 0, 20, 10, black);
  VGA_text(1, 2, plot_name);
}

// Draw a fixed-length arrow; color encodes derivative magnitude.
static void VGA_arrow(int x0, int y0, int x1_raw, int y1_raw, float mag_max) {
  float dx = (float)(x1_raw - x0);
  float dy = (float)(y1_raw - y0);
  float L = sqrtf(dx * dx + dy * dy);

  const float eps = 1e-5f;
  float invL = 1.0f / fmaxf(L, eps);
  float ux = dx * invL;
  float uy = dy * invL;
  if (L < eps) {
    ux = 1.0f;
    uy = 0.0f;
  }

  const float shaft_len = 3.0f;
  const float head_len = 2.0f;
  const float head_ang = 45.0f * (float)M_PI / 180.0f;

  int x1 = x0 + (int)lrintf(ux * shaft_len);
  int y1 = y0 + (int)lrintf(uy * shaft_len);

  float t = L / mag_max;
  if (t < 0.0f)
    t = 0.0f;
  if (t > 1.0f)
    t = 1.0f;

  short color;
  if (L < 0.20f) {
    color = gray;
  } else {
    // blue -> cyan -> green -> yellow -> red
    float r = 0.0f, g = 0.0f, b = 0.0f;
    if (t < 0.25f) {
      float u = t / 0.25f;
      g = u;
      b = 1.0f;
    } else if (t < 0.50f) {
      float u = (t - 0.25f) / 0.25f;
      g = 1.0f;
      b = 1.0f - u;
    } else if (t < 0.75f) {
      float u = (t - 0.50f) / 0.25f;
      r = u;
      g = 1.0f;
    } else {
      float u = (t - 0.75f) / 0.25f;
      r = 1.0f;
      g = 1.0f - u;
    }

    uint16_t R5 = (uint16_t)lrintf(r * 31.0f);
    uint16_t G6 = (uint16_t)lrintf(g * 63.0f);
    uint16_t B5 = (uint16_t)lrintf(b * 31.0f);
    color = (short)((R5 << 11) | (G6 << 5) | B5);
  }

  VGA_line(x0, y0, x1, y1, color);

  float bx = -ux, by = -uy;
  float c = cosf(head_ang), s = sinf(head_ang);
  float lx = bx * c - by * s;
  float ly = bx * s + by * c;
  float rx = bx * c + by * s;
  float ry = -bx * s + by * c;

  int xL = x1 + (int)lrintf(head_len * lx);
  int yL = y1 + (int)lrintf(head_len * ly);
  int xR = x1 + (int)lrintf(head_len * rx);
  int yR = y1 + (int)lrintf(head_len * ry);

  VGA_line(x1, y1, xL, yL, color);
  VGA_line(x1, y1, xR, yR, color);
}

void *plot() {
  int t = 0;
  int v1 = 0;
  int v2 = 0;
  int v3 = 0;
  int v4 = 0;
  int v5 = 0;
  int v6 = 0;
  int u1 = 0;
  int u2 = 0;
  int u3 = 0;
  int u4 = 0;
  int u5 = 0;
  int u6 = 0;
  int v1converted = 0;
  int v2converted = 0;
  int v3converted = 0;
  int v4converted = 0;
  int v5converted = 0;
  int v6converted = 0;
  int v = 0;
  int u = 0;

  float v1f, v2f, v3f, v4f, v5f, v6f, u1f, u2f, u3f, u4f, u5f, u6f, v1fScaled,
      v2fScaled, v3fScaled, v4fScaled, v5fScaled, v6fScaled;
  float v1fTime, v2fTime, v3fTime, v4fTime, v5fTime, v6fTime;

  float last_speed_mult = 1.0f;

  int last_v1[SHIFT_PX];
  int last_v2[SHIFT_PX];
  int last_v3[SHIFT_PX];
  int last_v4[SHIFT_PX];
  int last_v5[SHIFT_PX];
  int last_v6[SHIFT_PX];
  sem_wait(&plot_start);
  sem_wait(&reset_flag);
  struct timespec draw_start;
  struct timespec draw_end;
  long time_diff_ms = 0;
  long time_diff_ns = 0;
  long us_sleep_time = 0;

  unsigned int shift = 1;

  int state_buffer[10][2] = {0};
  int state_buffer_index = 0;
  int state_buffer_size = 10;

  while (1) {
    // Get locks
    pthread_mutex_lock(&speed_lock);
    last_speed_mult = speed_multiplier; // Save the speed for later
    pthread_mutex_unlock(&speed_lock);

    pthread_mutex_lock(&running_lock);
    pthread_mutex_lock(&hardware_lock);

    // Save time for profiling
    clock_gettime(CLOCK_MONOTONIC, &draw_start);
    int num_iter = time_scale;
    int us_sleep_time = (long)(DEFAULT_TIME_US / last_speed_mult);

    // Keep shift non-even to avoid bug
    shift = (int)(last_speed_mult * DEFAULT_SHIFT);
    if (shift % 2 == 0) {
      shift += 1;
    }

    shift = CLAMP(shift, 1, SHIFT_PX);
    int px, py;

    // Get values for all pixels
    int start = 639 - shift;
    for (int i = 0; i < shift; i++) {
      v1fScaled = v2fScaled = v3fScaled = v4fScaled = v5fScaled = v6fScaled =
          0.f;

      v1f = 0.f;
      v2f = 0.f;
      v3f = 0.f;
      v4f = 0.f;
      v5f = 0.f;
      v6f = 0.f;
      u1f = 0.f;
      u2f = 0.f;
      u3f = 0.f;
      u4f = 0.f;
      u5f = 0.f;
      u6f = 0.f;

      int v1n, v2n, v3n, v4n, v5n, v6n;
      int u1n, u2n, u3n, u4n, u5n, u6n;

      *i_ptr = ibase_param;

      for (int i = 0; i < num_iter; i++) {
        phase_plot_t *plot = neuron_plots[i];
        // set clock high
        *clk_ptr = true;

        // grab v & u values
        u1f = ((convert_FPGA_Value_to_mV((*u1_ptr))));
        u2f = ((convert_FPGA_Value_to_mV((*u2_ptr))));
        u3f = ((convert_FPGA_Value_to_mV((*u3_ptr))));
        u4f = ((convert_FPGA_Value_to_mV((*u4_ptr))));
        u5f = ((convert_FPGA_Value_to_mV((*u5_ptr))));
        u6f = ((convert_FPGA_Value_to_mV((*u6_ptr))));
        v1f = ((convert_FPGA_Value_to_mV((*v1_ptr))));
        v2f = ((convert_FPGA_Value_to_mV((*v2_ptr))));
        v3f = ((convert_FPGA_Value_to_mV((*v3_ptr))));
        v4f = ((convert_FPGA_Value_to_mV((*v4_ptr))));
        v5f = ((convert_FPGA_Value_to_mV((*v5_ptr))));
        v6f = ((convert_FPGA_Value_to_mV((*v6_ptr))));

        v1fTime = ((fix2float((*v1_ptr)))); // for plotting time didn't want to
                                            // mess up Chris' plotting code
        v2fTime = ((fix2float((*v2_ptr))));
        v3fTime = ((fix2float((*v3_ptr))));
        v4fTime = ((fix2float((*v4_ptr))));
        v5fTime = ((fix2float((*v5_ptr))));
        v6fTime = ((fix2float((*v6_ptr))));

        v1fScaled += v1fTime * plot_scale;
        v2fScaled += v2fTime * plot_scale;
        v3fScaled += v3fTime * plot_scale;
        v4fScaled += v4fTime * plot_scale;
        v5fScaled += v5fTime * plot_scale;
        v6fScaled += v6fTime * plot_scale;

        v1 = convert_to_VGAXCoord(v1f, plot->minV, plot->rangeV);
        v2 = convert_to_VGAXCoord(v2f, plot->minV, plot->rangeV);
        v3 = convert_to_VGAXCoord(v3f, plot->minV, plot->rangeV);
        v4 = convert_to_VGAXCoord(v4f, plot->minV, plot->rangeV);
        v5 = convert_to_VGAXCoord(v5f, plot->minV, plot->rangeV);
        v6 = convert_to_VGAXCoord(v6f, plot->minV, plot->rangeV);
        u1 = convert_to_VGAYCoord(u1f, plot->minU, plot->rangeU);
        u2 = convert_to_VGAYCoord(u2f, plot->minU, plot->rangeU);
        u3 = convert_to_VGAYCoord(u3f, plot->minU, plot->rangeU);
        u4 = convert_to_VGAYCoord(u4f, plot->minU, plot->rangeU);
        u5 = convert_to_VGAYCoord(u5f, plot->minU, plot->rangeU);
        u6 = convert_to_VGAYCoord(u6f, plot->minU, plot->rangeU);

        if (neuron_select == 0) {
          v = v1;
          u = u1;
        } else if (neuron_select == 1) {
          v = v2;
          u = u2;
        } else if (neuron_select == 2) {
          v = v3;
          u = u3;
        } else if (neuron_select == 3) {
          v = v4;
          u = u4;
        } else if (neuron_select == 4) {
          v = v5;
          u = u5;
        } else if (neuron_select == 5) {
          v = v6;
          u = u6;
        }

        *clk_ptr = false;
      }

      // Scale and convert each sample to get average
      v1fScaled /= num_iter;
      v2fScaled /= num_iter;
      v3fScaled /= num_iter;
      v4fScaled /= num_iter;
      v5fScaled /= num_iter;
      v6fScaled /= num_iter;

      v1converted = -(int)(v1fScaled + 0.5f);
      v2converted = -(int)(v2fScaled + 0.5f);
      v3converted = -(int)(v3fScaled + 0.5f);
      v4converted = -(int)(v4fScaled + 0.5f);
      v5converted = -(int)(v5fScaled + 0.5f);
      v6converted = -(int)(v6fScaled + 0.5f);

      last_v1[i] = v1converted;
      last_v2[i] = v2converted;
      last_v3[i] = v3converted;
      last_v4[i] = v4converted;
      last_v5[i] = v5converted;
      last_v6[i] = v6converted;
    }

    // Shift screen to make scrolling effect
    VGA_shift(shift, COL2_X_START, 0, SCREEN_XMAX, SCREEN_YMAX);

    // Draw center-lines for v vs. t plot
    VGA_Hline(COL2_X_START, 40, 639, gray);
    VGA_Hline(COL2_X_START, 120, 639, gray);
    VGA_Hline(COL2_X_START, 200, 639, gray);
    VGA_Hline(COL2_X_START, 280, 639, gray);
    VGA_Hline(COL2_X_START, 360, 639, gray);
    VGA_Hline(COL2_X_START, 440, 639, gray);

    // Plot on phase plot
    trail_shift_push_draw(v, u, white);

    // Plot each pixel in shifted region on v vs. t plot
    for (int i = 0; i < shift; i++) {
      VGA_PIXEL(start + i, last_v1[i] + 40, white);
      VGA_PIXEL(start + i, last_v2[i] + 120, red);
      VGA_PIXEL(start + i, last_v3[i] + 200, green);
      VGA_PIXEL(start + i, last_v4[i] + 280, cyan);
      VGA_PIXEL(start + i, last_v5[i] + 360, magenta);
      VGA_PIXEL(start + i, last_v6[i] + 440, yellow);
    }

    // Unlock everything
    pthread_mutex_unlock(&hardware_lock);

    clock_gettime(CLOCK_MONOTONIC, &draw_end);

    // Wait if paused
    if (pause_flag) {
      pthread_cond_wait(&paused_cond, &running_lock);
    } else if (t++ > COL_WIDTH) {
      t = 0;
    }

    //get calc & draw time for profiling
    time_diff_ms =
        ((draw_end.tv_sec * 1000) + (draw_end.tv_nsec / 1000000)) -
        ((draw_start.tv_sec * 1000) + (draw_start.tv_nsec / 1000000));
    if (dprint_flag) {
      printf("Last drawtime: %ld ms\n", time_diff_ms);
    }

    // Pace drawing based on speed mult
    usleep(MAX(0, us_sleep_time - (time_diff_ms * 1000)));

    pthread_mutex_unlock(&running_lock);

    // Exit based on CLI input
    if (kill_flag) {
      return NULL;
    }
  }
}

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
/// Hardware reset

// Reset hardware
void *reset() {
  sem_wait(&update_params); // put reset thread to sleep until reset requested
  pthread_mutex_lock(&param_lock); // lock parameters during reset
                                   // block plotting

  pthread_mutex_lock(&hardware_lock);

  // Get phase portrait values if needed (involves lots of resets so do it
  // first)
  if (phase_plot && plots_populated) {
    plot_phase_portait(neuron_plots[neuron_select]);
    phase_plot = false;
  }

  // Set everything to acquired parameters
  *v1_init_ptr = vi_params[0];
  *v2_init_ptr = vi_params[1];
  *v3_init_ptr = vi_params[2];
  *v4_init_ptr = vi_params[3];
  *v5_init_ptr = vi_params[4];
  *v6_init_ptr = vi_params[5];
  *u1_init_ptr = ui_params[0];
  *u2_init_ptr = ui_params[1];
  *u3_init_ptr = ui_params[2];
  *u4_init_ptr = ui_params[3];
  *u5_init_ptr = ui_params[4];
  *u6_init_ptr = ui_params[5];

  *ab_ptr =
      (A1(a_params[0]) | A2(a_params[1]) | A3(a_params[2]) | A4(a_params[3]) |
       B1(b_params[0]) | B2(b_params[1]) | B3(b_params[2]) | B4(b_params[3]));
  *ab56_ptr =
      (A5(a_params[4]) | A6(a_params[5]) | B5(b_params[4]) | B6(b_params[5]));
  *c1_ptr = c_params[0];
  *c2_ptr = c_params[1];
  *c3_ptr = c_params[2];
  *c4_ptr = c_params[3];
  *c5_ptr = c_params[4];
  *c6_ptr = c_params[5];
  *d1_ptr = d_params[0];
  *d2_ptr = d_params[1];
  *d3_ptr = d_params[2];
  *d4_ptr = d_params[3];
  *d5_ptr = d_params[4];
  *d6_ptr = d_params[5];
  *i_ptr = ibase_param;
  *dt_ptr = dt_param;

  // Assert reset (active low)
  *rst_ptr = true;

  // Toggle clock (reset on rising edge)
  *clk_ptr = false;
  *clk_ptr = true;

  // Deassert reset
  *rst_ptr = false;

  // Set clock low (hold)
  *clk_ptr = false;

  // Unlock everything
  pthread_mutex_unlock(&hardware_lock);
  pthread_mutex_unlock(&param_lock);

  // Signal that reset is done and plot can continue
  sem_post(&reset_flag); // reset complete
  sem_post(&plot_start); // ensure plotting continues after reset

  // Always exit (non-looping)
  return NULL;
}

///////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
/// Main

int main() {

  kill_flag = false;
  phase_plot = false;
  // === need to mmap: =======================
  // FPGA_CHAR_BASE
  // FPGA_ONCHIP_BASE
  // HW_REGS_BASE

  // === get FPGA addresses ==================
  // Open /dev/mem
  if ((fd = open("/dev/mem", (O_RDWR | O_SYNC))) == -1) {
    printf("ERROR: could not open \"/dev/mem\"...\n");
    return (1);
  }

  // get virtual addr that maps to physical
  h2p_lw_virtual_base = mmap(NULL, HW_REGS_SPAN, (PROT_READ | PROT_WRITE),
                             MAP_SHARED, fd, HW_REGS_BASE);
  if (h2p_lw_virtual_base == MAP_FAILED) {
    printf("ERROR: mmap1() failed...\n");

    close(fd);
    return (1);
  }

  // === get VGA char addr =====================
  // get virtual addr that maps to physical
  vga_char_virtual_base = mmap(NULL, FPGA_CHAR_SPAN, (PROT_READ | PROT_WRITE),
                               MAP_SHARED, fd, FPGA_CHAR_BASE);
  if (vga_char_virtual_base == MAP_FAILED) {
    printf("ERROR: mmap2() failed...\n");

    close(fd);
    return (1);
  }

  // Get the address that maps to the FPGA LED control
  vga_char_ptr = (unsigned int *)(vga_char_virtual_base);

  // === get VGA pixel addr ====================
  // get virtual addr that maps to physical
  vga_pixel_virtual_base = mmap(NULL, SDRAM_SPAN, (PROT_READ | PROT_WRITE),
                                MAP_SHARED, fd, SDRAM_BASE);
  if (vga_pixel_virtual_base == MAP_FAILED) {
    printf("ERROR: mmap3() failed...\n");
    close(fd);
    return (1);
  }

  // Get the address that maps to the FPGA pixel buffer
  vga_pixel_ptr = (unsigned int *)(vga_pixel_virtual_base);

  pio_virtual_base = h2p_lw_virtual_base;

  rst_ptr = (int *)(pio_virtual_base + NRN_RST_OFFSET);
  clk_ptr = (int *)(pio_virtual_base + NRN_CLK_OFFSET);

  // initial values used to set phase portrait parameters
  v1_init_ptr = (int *)(pio_virtual_base + NRN_V1_INIT_OFFSET);
  v2_init_ptr = (int *)(pio_virtual_base + NRN_V2_INIT_OFFSET);
  v3_init_ptr = (int *)(pio_virtual_base + NRN_V3_INIT_OFFSET);
  v4_init_ptr = (int *)(pio_virtual_base + NRN_V4_INIT_OFFSET);
  v5_init_ptr = (int *)(pio_virtual_base + NRN_V5_INIT_OFFSET);
  v6_init_ptr = (int *)(pio_virtual_base + NRN_V6_INIT_OFFSET);
  u1_init_ptr = (int *)(pio_virtual_base + NRN_U1_INIT_OFFSET);
  u2_init_ptr = (int *)(pio_virtual_base + NRN_U2_INIT_OFFSET);
  u3_init_ptr = (int *)(pio_virtual_base + NRN_U3_INIT_OFFSET);
  u4_init_ptr = (int *)(pio_virtual_base + NRN_U4_INIT_OFFSET);
  u5_init_ptr = (int *)(pio_virtual_base + NRN_U5_INIT_OFFSET);
  u6_init_ptr = (int *)(pio_virtual_base + NRN_U6_INIT_OFFSET);

  // input parameters for neuorns
  ab_ptr = (int *)(pio_virtual_base + NRN_AB_OFFSET);
  ab56_ptr = (int *)(pio_virtual_base + NRN_AB56_OFFSET);
  c1_ptr = (int *)(pio_virtual_base + NRN_C1_OFFSET);
  c2_ptr = (int *)(pio_virtual_base + NRN_C2_OFFSET);
  c3_ptr = (int *)(pio_virtual_base + NRN_C3_OFFSET);
  c4_ptr = (int *)(pio_virtual_base + NRN_C4_OFFSET);
  c5_ptr = (int *)(pio_virtual_base + NRN_C5_OFFSET);
  c6_ptr = (int *)(pio_virtual_base + NRN_C6_OFFSET);
  d1_ptr = (int *)(pio_virtual_base + NRN_D1_OFFSET);
  d2_ptr = (int *)(pio_virtual_base + NRN_D2_OFFSET);
  d3_ptr = (int *)(pio_virtual_base + NRN_D3_OFFSET);
  d4_ptr = (int *)(pio_virtual_base + NRN_D4_OFFSET);
  d5_ptr = (int *)(pio_virtual_base + NRN_D5_OFFSET);
  d6_ptr = (int *)(pio_virtual_base + NRN_D6_OFFSET);
  i_ptr = (int *)(pio_virtual_base + NRN_I_OFFSET);

  dt_ptr = (int *)(pio_virtual_base + NRN_DT_OFFSET);
  // output parameters for neuorns
  v1_ptr = (int *)(pio_virtual_base + NRN_V1_OFFSET);
  v2_ptr = (int *)(pio_virtual_base + NRN_V2_OFFSET);
  v3_ptr = (int *)(pio_virtual_base + NRN_V3_OFFSET);
  v4_ptr = (int *)(pio_virtual_base + NRN_V4_OFFSET);
  v5_ptr = (int *)(pio_virtual_base + NRN_V5_OFFSET);
  v6_ptr = (int *)(pio_virtual_base + NRN_V6_OFFSET);
  u1_ptr = (int *)(pio_virtual_base + NRN_U1_OFFSET);
  u2_ptr = (int *)(pio_virtual_base + NRN_U2_OFFSET);
  u3_ptr = (int *)(pio_virtual_base + NRN_U3_OFFSET);
  u4_ptr = (int *)(pio_virtual_base + NRN_U4_OFFSET);
  u5_ptr = (int *)(pio_virtual_base + NRN_U5_OFFSET);
  u6_ptr = (int *)(pio_virtual_base + NRN_U6_OFFSET);
  du1_ptr = (int *)(pio_virtual_base + NRN_DU1_OFFSET);
  dv1_ptr = (int *)(pio_virtual_base + NRN_DV1_OFFSET);
  du2_ptr = (int *)(pio_virtual_base + NRN_DU2_OFFSET);
  dv2_ptr = (int *)(pio_virtual_base + NRN_DV2_OFFSET);
  du3_ptr = (int *)(pio_virtual_base + NRN_DU3_OFFSET);
  dv3_ptr = (int *)(pio_virtual_base + NRN_DV3_OFFSET);
  du4_ptr = (int *)(pio_virtual_base + NRN_DU4_OFFSET);
  dv4_ptr = (int *)(pio_virtual_base + NRN_DV4_OFFSET);
  du5_ptr = (int *)(pio_virtual_base + NRN_DU5_OFFSET);
  dv5_ptr = (int *)(pio_virtual_base + NRN_DV5_OFFSET);
  du6_ptr = (int *)(pio_virtual_base + NRN_DU6_OFFSET);
  dv6_ptr = (int *)(pio_virtual_base + NRN_DV6_OFFSET);

  // Set default values
  c_init_vals[0] = convert_to_FPGA_value(C1_FLOAT);
  c_init_vals[1] = convert_to_FPGA_value(C2_FLOAT);
  c_init_vals[2] = convert_to_FPGA_value(C3_FLOAT);
  c_init_vals[3] = convert_to_FPGA_value(C4_FLOAT);
  c_init_vals[4] = convert_to_FPGA_value(C5_FLOAT);
  c_init_vals[5] = convert_to_FPGA_value(C6_FLOAT);

  d_init_vals[0] = convert_to_FPGA_value(D1_FLOAT);
  d_init_vals[1] = convert_to_FPGA_value(D2_FLOAT);
  d_init_vals[2] = convert_to_FPGA_value(D3_FLOAT);
  d_init_vals[3] = convert_to_FPGA_value(D4_FLOAT);
  d_init_vals[4] = convert_to_FPGA_value(D5_FLOAT);
  d_init_vals[5] = convert_to_FPGA_value(D6_FLOAT);

  // Set the actual parameters using defaults
  set_defaults(&current_params);
  dt_param = DT_FIX;

  // Initial phase plot values
  phase_plot_t plts[NUM_NEURONS];

  plots_populated = false;
  for (int i = 0; i < NUM_NEURONS; i++) {
    neuron_plots[i] = &plts[i];
    init_plot(neuron_plots[i], i);
  }

  // Clear the screen and draw the text
  VGA_CLEAR();

  // Screen bounding box to show program is running
  VGA_rect(0, 0, SCREEN_XMAX - 1, SCREEN_YMAX - 1, white);

  // the semaphore inits
  // read is not ready because nothing has been input yet
  sem_init(&update_params, 0, 1);
  // print is ready at init time
  sem_init(&reset_flag, 0, 1);
  sem_init(&plot_start, 0, 0);
  // For portability, explicitly create threads in a joinable state
  //  thread attribute used here to allow JOIN
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  // now the threads
  pthread_create(&thread_read, NULL, read1, NULL);
  // Reset is called by read
  // pthread_create(&thread_reset, NULL, reset, NULL);
  pthread_create(&thread_plot, NULL, plot, NULL);

  // second copy of counter
  // pthread_create(&thread_count2,NULL,counter1,NULL);

  pthread_join(thread_read, NULL);
  // pthread_join(thread_reset, NULL);
  pthread_join(thread_plot, NULL);

  free_plots();

  VGA_text_clear();
  VGA_CLEAR();

  return 0;
}

//////////////////////////////////////////////////////////////
