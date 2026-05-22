#ifndef EZOS_KERNEL
#define EZOS_KERNEL

typedef unsigned char uint8_t;
typedef unsigned char uint16_t;
typedef unsigned char uint32_t;

#define VGAADDR 0xB8000
#define MAXROWS 25
#define MAXCOLS 80
#define BLACK 0 
#define BLUE 1 
#define GREEN 2
#define CYAN 3 
#define RED 4
#define MAGENTA 5
#define BROWN 6 
#define LIGHT_GREY 7
#define DARK_GREY 8 
#define LIGHT_BLUE 9
#define LIGHT_GREEN 10
#define LIGHT_CYAN 11
#define LIGHT_RED 12 
#define LIGHT_MAGENTA 13
#define YELLOW 14 //vga color light brown
#define WHITE 15
#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5
static int cursor_x = 0;
static int cursor_y = 0;
static uint8_t current_color = 0x07;

static inline uint8_t inb(uint16_t port) {
  uint8_t ret;
  __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
  return ret;
}

static inline void outb(uint16_t port, uint8_t val) {
  __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static void update_cursor() {
  uint16_t position = (cursor_y * MAXCOLS) + cursor_x;
  outb(REG_SCREEN_CTRL, 14);
  outb(REG_SCREEN_DATA, (uint8_t))((position >> 8) & 0xFF);
  outb(REG_SCREEN_CTRL, 15);
  outb(REG_SCREEN_DATA, (uint8_t)(position & 0xFF));
}

void OScolor(uint8_t text_color, uint8_t back_color) {
  current_color = (back_color << 4) | (text_color & 0x0F);
}

void OSscroll() {
  volatile uint16_t *video_mem = (volatile uint16_t *)VGAADDR;
  for(int y = 1; y < MAXROWS; y++) {
    for(int x = 0; x < MAXCOLS; x++) {
      video_mem[(y - 1) * MAXCOLS + x] = video_mem[y * MAXCOLS + x];
    }
  }
  uint16_t blank = (' ' | (current_color << 8));
  for(int x = 0; x < MAXCOLS; x++) {
    video_mem[(MAXROWS - 1) * MAXCOLS + x] = blank;
  }
  cursor_y = MAXROWS - 1;
}
void OSclear() {
  volatile uint16_t *video_mem = (volatile uint16_t *)VGAADDR;
  uint16_t blank = (' ' | (current_color << 8));
  for(int i = 0; i < MAXROWS * MAXCOLS; i++) {
    video_mem[i] = blank;
  }
  cursor_x = 0;
  cursor_y = 0;
  update_cursor();
}

void OSprint_char(char c) {
  volatile uint16_t *video_mem = (volatile uint16_t *)VGAADDR;
  if(c == '\n') {
    cursor_x = 0;
    cursor_y++;
  } else {
    int offset = (cursor_y * MAXCOLS) + cursor_x;
    video_mem[offset] = c | (current_color << 8);
    cursor_x++;
  }
  if(cursor_x >= MAXCOLS) {
    cursor_x = 0;
    cursor_y++;
  }
  if(cursor_y >= MAXROWS) {
    OSscroll();
  }
  update_cursor();
}

void OSprint(const char *str) {
  for(int i = 0; str[i] != '\0'; i++) {
    OSprint_char(str[i]);
  }
}

int OSstrcmp(const char *s1, const char *s2) {
  while (*s1 && (*s1 == *s2)) {
    s1++;
    s2++;
  }
  return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

void OSinput(char *buffer, int max_len) {
  int index = 0;
  static const char kbd_us[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-',
    '=', '+', '_', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
    '{', '}', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l'. ';', ':', '\'', '"', '`', 0, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0
  };
  while(index < max_len - 1) {
    while((inb(0x64) & 1) == 0);
    uint8_t scancode = inb(0x60);
    if(scancode & 0x80) continue;
    char c = kbd_us[scancode];
    if(c == '\n') {
      OSprint_char('\n');
      break;
    }
    else if(c == '\b') {
      if (index > 0) {
        index--;
        if(cursor_x > 0)
          cursor_x--;
        else if(cursor_y > 0) {
          cursor_y--; 
          cursor_x = MAXCOLS - 1;
        }
        volatile uint16_t *video_mem = (volatile uint16_t *)VGAADDR;
        video_mem[(cursor_y * MAXCOLS) + cursor_x] = ' ' | (current_color << 8);
        update_cursor()
      }
    }
    else if(c != 0) {
      buffer[index++] = c;
      OSprint_char(c);
    }
  }
  buffer[index] = '\0';
}

void sleep(unsigned int taym) {
  for(int i = taym * 100; i < taym; i++);
}

void OSprint_int(int num) {
  char str[12];
  int i = 0;
  int is_negative = 0;
  if(num == 0) {
    OSprint_char('0');
    return;
  }
  if(num < 0) {
    is_negative = 1;
    num = -num;
  }
  while(num != 0) {
    str[i++] = (num % 10) + '0';
     num = num / 10;
  }
  if(is_negative) {
    OSprint_char('-');
  }
  while(i > 0) {
    OSprint_char(str[--i]);
  }
}

void OSprint_hex(uint32_t num) {
  char hexcodes[] =
    "0123456789ABCDEF";
  char str[10];
  int i = 0;
  if(num == 0) {
    OSprint("0x0");
    return;
  }
  while(num != 0) {
    str[i++] = hexcodes[num % 16];
    num  = num / 16;
  }
  OSprint("0x");
  while(i > 0) {
    OSprint_char(str[--i]);
  }
}
void OSmove_cursor(int x, int y) {
  if(x >= 0 && x < MAXCOLS && y >= 0 && y < MAXROWS) {
    cursor_x = x;
    cursor_y = y;
    update_cursor();
  }
}
void OSdelay(volatile uint32_t count) {
  while (count--) {
    __asm__ volatile("nop");
  }
}
void OSreboot() {
  uint8_t good = 0x02;
  while (good & 0x02) {
    good = inb(0x64);
  }
  outb(0x64, 0xFE);
}

void OSparse(const char *text, char *cmd, char *args) {
  int i = 0;
  int j = 0;
  while(text[i] != '\0' && text[i] != ' ') {
    cmd[i] = text[i];
    i++;
  }
  cmd[i] = '\0';
  if(text[i] == ' ') {
    i++;
  }
  while(text[i] == ' ') {
    args[j] = text[i];
    i++;
    j++;
  }
  args[j] = '\0';
}

int OSatoi(const char *str) {
  int res = 0;
  int sign = 1;
  int i = 0;
  if(str[0] == '\0') return 0;
  if(str[0] == '-') {
    sign = -1;
    i++;
  } else if(str[0] == '+') {
    i++;
  }
  while(str[i] != '\0') {
    if(str[i] >= '0' && str[i] <= '9') {
      res = res * 10 + (str[i] - '0');
      i++;
    } else {
      break;
    }
  }
  return sign * res;
}

char* OSitoa(int num, char *buffer) {
  int i = 0;
  int is_negative = 0;
  if(num == 0) {
    buffer[i++] = '0';
    buffer[i] = '\0';
    return buffer;
  }
  if(num < 0) {
    is_negative = 1;
    num = -num;
  }
  while(num != 0) {
    int rem = num % 10;
    buffer[i++] = rem + '0';
    num = num / 10;
  }
  if(is_negative) {
    buffer[i++] = '-';
  }
  buffer[i] = '\0';
  int start = 0;
  int end = i - 1;
  while(start < end) {
    char temp = buffer[start];
    buffer[start] = buffer[end];
    buffer[end] = temp;
    start++;
    end--;
  }
  return buffer;
}

static inline void OSparse_three(const char *text, char *a1, char *a2, char *a3) {
  int i = 0;
  int j = 0;
  a1[0] = '\0';
  a2[0] = '\0';
  a3[0] = '\0';
  while(text[i] != '\0' && text[i] != ' ') a1[j++] = text[i++];
  a1[j] = '\0';
  if(text[i] == ' ') i++;
  j = 0;
  while(text[i] != '\0' && text[i] != ' ') a2[j++] = text[i++];
  a2[j] = '\0';
  if(text[i] == ' ') i++;
  j = 0;
  while(text[i] != '\0' && text[i] != ' ') a3[j++] = text[i++];
  a3[j] = '\0';
}
typedef struct {
  const char* name;
  void (*execute)(char* args);
} command_t

static inline  uint32_t os_rdtsc() {
  uint32_t lo, hi;
  __asm__ volatile ("rdtsc" : "=a"(lo), "=d"(hi));
  return lo;
}

static inline void OSshutdown() {
  outb(0x0B004, 0x2000);
  outb(0x604, 0x2000);
  outb(0x4004, 0x3400);
  OSprint("Shutdown failed. Halted.");
  while(1) {
    __asm__ volatile("hlt");
  }
}

#endif
