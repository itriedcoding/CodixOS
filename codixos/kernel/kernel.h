/*
 * CodixOS Kernel Header
 */

#ifndef KERNEL_H
#define KERNEL_H

/* Type definitions */
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed long long int64_t;
typedef uint32_t size_t;
typedef int32_t ssize_t;

/* Constants */
#define NULL ((void*)0)
#define true 1
#define false 0
#define bool _Bool

/* Colors */
#define COLOR_BLACK      0x00
#define COLOR_BLUE       0x01
#define COLOR_GREEN      0x02
#define COLOR_CYAN       0x03
#define COLOR_RED        0x04
#define COLOR_MAGENTA    0x05
#define COLOR_BROWN      0x06
#define COLOR_LIGHT_GREY 0x07
#define COLOR_DARK_GREY  0x08
#define COLOR_LIGHT_BLUE 0x09
#define COLOR_LIGHT_GREEN 0x0A
#define COLOR_LIGHT_CYAN 0x0B
#define COLOR_LIGHT_RED  0x0C
#define COLOR_LIGHT_MAGENTA 0x0D
#define COLOR_YELLOW     0x0E
#define COLOR_WHITE      0x0F

/* String functions */
size_t strlen(const char* str);
void strcpy(char* dest, const char* src);
int strcmp(const char* s1, const char* s2);
int memcmp(const void* s1, const void* s2, size_t n);
void strcat(char* dest, const char* src);

/* Memory functions */
void* memset(void* dest, int c, size_t n);
void* memcpy(void* dest, const void* src, size_t n);

/* VGA functions */
void clear_screen();
void set_color(uint8_t color);
void putchar(char c);
void print(const char* str);
void println(const char* str);
void print_color(const char* str, uint8_t color);
void print_int(int num);
void print_hex(uint32_t num);
void update_cursor();

/* Keyboard */
char get_scancode();
char scancode_to_ascii(char scancode);
int readline(char* buffer, int max_len);

/* Timer */
uint32_t get_tick();

/* System */
void halt();
void reboot();

/* Kernel Entry */
void kernel_main(uint32_t magic, void* mboot_info);

#endif /* KERNEL_H */
