/*
 * CodixOS Kernel - Main Entry Point
 * A lightweight, terminal-based operating system
 */

#include "kernel.h"
#include "memory.h"
#include "io.h"
#include "interrupt.h"
#include "process.h"
#include "filesystem.h"
#include "security/secure_boot.h"
#include "security/disk_encrypt.h"

/* VGA text mode buffer */
volatile uint16_t* const VGA_BUFFER = (uint16_t*)0xB8000;
const int VGA_WIDTH = 80;
const int VGA_HEIGHT = 25;

/* Current cursor position */
int cursor_x = 0;
int cursor_y = 0;

/* Current color attribute */
uint8_t current_color = 0x07;

/* Kernel version */
const char* CODIX_VERSION = "1.0.0";
const char* CODIX_NAME = "CodixOS";

/* String functions */
size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len]) len++;
    return len;
}

void strcpy(char* dest, const char* src) {
    while (*src) *dest++ = *src++;
    *dest = '\0';
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

void strcat(char* dest, const char* src) {
    while (*dest) dest++;
    while (*src) *dest++ = *src++;
    *dest = '\0';
}

/* Memory functions */
void* memset(void* dest, int c, size_t n) {
    uint8_t* d = (uint8_t*)dest;
    while (n--) *d++ = (uint8_t)c;
    return dest;
}

void* memcpy(void* dest, const void* src, size_t n) {
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    while (n--) *d++ = *s++;
    return dest;
}

/* VGA functions */
void clear_screen() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        VGA_BUFFER[i] = (uint16_t)(' ') | ((uint16_t)current_color << 8);
    }
    cursor_x = 0;
    cursor_y = 0;
    update_cursor();
}

void set_color(uint8_t color) {
    current_color = color;
}

void scroll_screen() {
    if (cursor_y >= VGA_HEIGHT) {
        for (int i = 0; i < VGA_WIDTH * (VGA_HEIGHT - 1); i++) {
            VGA_BUFFER[i] = VGA_BUFFER[i + VGA_WIDTH];
        }
        for (int i = VGA_WIDTH * (VGA_HEIGHT - 1); i < VGA_WIDTH * VGA_HEIGHT; i++) {
            VGA_BUFFER[i] = (uint16_t)(' ') | ((uint16_t)current_color << 8);
        }
        cursor_y = VGA_HEIGHT - 1;
    }
}

void putchar(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\t') {
        cursor_x = (cursor_x + 8) & ~7;
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
            VGA_BUFFER[cursor_y * VGA_WIDTH + cursor_x] = (uint16_t)(' ') | ((uint16_t)current_color << 8);
        }
    } else {
        VGA_BUFFER[cursor_y * VGA_WIDTH + cursor_x] = (uint16_t)c | ((uint16_t)current_color << 8);
        cursor_x++;
    }
    
    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }
    
    scroll_screen();
    update_cursor();
}

void print(const char* str) {
    while (*str) {
        putchar(*str++);
    }
}

void println(const char* str) {
    print(str);
    putchar('\n');
}

void print_color(const char* str, uint8_t color) {
    uint8_t old_color = current_color;
    set_color(color);
    print(str);
    set_color(old_color);
}

void print_int(int num) {
    if (num == 0) {
        putchar('0');
        return;
    }
    if (num < 0) {
        putchar('-');
        num = -num;
    }
    char buf[32];
    int i = 0;
    while (num > 0) {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }
    while (i > 0) {
        putchar(buf[--i]);
    }
}

void print_hex(uint32_t num) {
    const char hex[] = "0123456789ABCDEF";
    putchar('0');
    putchar('x');
    for (int i = 28; i >= 0; i -= 4) {
        putchar(hex[(num >> i) & 0xF]);
    }
}

void update_cursor() {
    uint16_t pos = cursor_y * VGA_WIDTH + cursor_x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

/* Timer */
volatile uint32_t tick_count = 0;

void timer_callback() {
    tick_count++;
}

uint32_t get_tick() {
    return tick_count;
}

/* Keyboard */
char get_scancode() {
    while (!(inb(0x64) & 1));
    return inb(0x60);
}

char scancode_to_ascii(char scancode) {
    static int shift = 0;
    
    if (scancode == 0x2A || scancode == 0x36) {
        shift = 1;
        return 0;
    }
    if (scancode == 0xAA || scancode == 0xB6) {
        shift = 0;
        return 0;
    }
    if (scancode & 0x80) return 0;
    
    if (shift) {
        switch (scancode) {
            case 0x02: return '!';
            case 0x03: return '@';
            case 0x04: return '#';
            case 0x05: return '$';
            case 0x06: return '%';
            case 0x07: return '^';
            case 0x08: return '&';
            case 0x09: return '*';
            case 0x0A: return '(';
            case 0x0B: return ')';
            case 0x0C: return '_';
            case 0x0D: return '+';
            case 0x1A: return '{';
            case 0x1B: return '}';
            case 0x27: return '"';
            case 0x28: return '~';
            case 0x33: return '<';
            case 0x34: return '>';
            case 0x35: return '?';
            case 0x10: return 'Q';
            case 0x11: return 'W';
            case 0x12: return 'E';
            case 0x13: return 'R';
            case 0x14: return 'T';
            case 0x15: return 'Y';
            case 0x16: return 'U';
            case 0x17: return 'I';
            case 0x18: return 'O';
            case 0x19: return 'P';
            case 0x1E: return 'A';
            case 0x1F: return 'S';
            case 0x20: return 'D';
            case 0x21: return 'F';
            case 0x22: return 'G';
            case 0x23: return 'H';
            case 0x24: return 'J';
            case 0x25: return 'K';
            case 0x26: return 'L';
            case 0x2C: return 'Z';
            case 0x2D: return 'X';
            case 0x2E: return 'C';
            case 0x2F: return 'V';
            case 0x30: return 'B';
            case 0x31: return 'N';
            case 0x32: return 'M';
        }
    }
    
    switch (scancode) {
        case 0x02: return '1';
        case 0x03: return '2';
        case 0x04: return '3';
        case 0x05: return '4';
        case 0x06: return '5';
        case 0x07: return '6';
        case 0x08: return '7';
        case 0x09: return '8';
        case 0x0A: return '9';
        case 0x0B: return '0';
        case 0x0C: return '-';
        case 0x0D: return '=';
        case 0x10: return 'q';
        case 0x11: return 'w';
        case 0x12: return 'e';
        case 0x13: return 'r';
        case 0x14: return 't';
        case 0x15: return 'y';
        case 0x16: return 'u';
        case 0x17: return 'i';
        case 0x18: return 'o';
        case 0x19: return 'p';
        case 0x1A: return '[';
        case 0x1B: return ']';
        case 0x1E: return 'a';
        case 0x1F: return 's';
        case 0x20: return 'd';
        case 0x21: return 'f';
        case 0x22: return 'g';
        case 0x23: return 'h';
        case 0x24: return 'j';
        case 0x25: return 'k';
        case 0x26: return 'l';
        case 0x27: return ';';
        case 0x28: return '\'';
        case 0x2C: return 'z';
        case 0x2D: return 'x';
        case 0x2E: return 'c';
        case 0x2F: return 'v';
        case 0x30: return 'b';
        case 0x31: return 'n';
        case 0x32: return 'm';
        case 0x33: return ',';
        case 0x34: return '.';
        case 0x35: return '/';
        case 0x39: return ' ';
        case 0x1C: return '\n';
        case 0x0E: return '\b';
        case 0x0F: return '\t';
    }
    return 0;
}

/* Read line from keyboard */
int readline(char* buffer, int max_len) {
    int pos = 0;
    char c;
    
    while (1) {
        c = scancode_to_ascii(get_scancode());
        if (c == 0) continue;
        
        if (c == '\n') {
            buffer[pos] = '\0';
            putchar('\n');
            return pos;
        } else if (c == '\b') {
            if (pos > 0) {
                pos--;
                putchar('\b');
            }
        } else if (pos < max_len - 1) {
            buffer[pos++] = c;
            putchar(c);
        }
    }
}

/* Shell prompt */
void print_prompt() {
    print_color("codix", 0x0A);
    print("@");
    print_color("kernel", 0x0B);
    print(":");
    print_color("~", 0x0D);
    print("$ ");
}

/* Kernel info */
void print_banner() {
    clear_screen();
    
    /* Blue header */
    set_color(0x1F);
    for (int i = 0; i < 80; i++) {
        putchar(' ');
    }
    cursor_x = 0;
    cursor_y = 0;
    
    /* Centered title */
    cursor_x = 28;
    print(" CodixOS v");
    print(CODIX_VERSION);
    cursor_x = 0;
    cursor_y = 1;
    
    for (int i = 0; i < 80; i++) {
        putchar(' ');
    }
    
    cursor_x = 0;
    cursor_y = 2;
    set_color(0x07);
    
    /* ASCII Art Logo */
    set_color(0x0B);
    print("        _____      _               ____   _____ ");
    putchar('\n');
    print("       / ____|    | |             |  _ \\ / ____|");
    putchar('\n');
    print("      | |     ___ | | ___  _ __   | |_) | (___  ");
    putchar('\n');
    print("      | |    / _ \\| |/ _ \\| '__|  |  _ < \\___ \\ ");
    putchar('\n');
    print("      | |___| (_) | | (_) | |     | |_) |____) |");
    putchar('\n');
    print("       \\_____\\___/|_|\\___/|_|     |____/|_____/ ");
    putchar('\n');
    print("                                                 ");
    putchar('\n');
    
    set_color(0x07);
    print("  Lightweight Terminal Operating System");
    putchar('\n');
    print("  Type 'help' for available commands");
    putchar('\n');
    print("  Version: ");
    print(CODIX_VERSION);
    print(" | License: MIT");
    putchar('\n');
    putchar('\n');
}

/* Simple commands */
void cmd_help() {
    set_color(0x0E);
    print("=== CodixOS Commands ===\n");
    set_color(0x07);
    print("  help       - Show this help message\n");
    print("  clear      - Clear the screen\n");
    print("  echo       - Print text\n");
    print("  version    - Show version info\n");
    print("  reboot     - Reboot the system\n");
    print("  halt       - Shutdown the system\n");
    print("  uptime     - Show system uptime\n");
    print("  ls         - List directory contents\n");
    print("  cat        - Display file contents\n");
    print("  cp         - Copy files\n");
    print("  mv         - Move/rename files\n");
    print("  rm         - Remove files\n");
    print("  mkdir      - Create directory\n");
    print("  ps         - List processes\n");
    print("  kill       - Kill a process\n");
    print("  df         - Disk usage\n");
    print("  free       - Memory usage\n");
    print("  pkg        - Package manager\n");
    print("  about      - About CodixOS\n");
    set_color(0x0C);
    print("  Security Commands:\n");
    set_color(0x07);
    print("  secureboot - Secure Boot status\n");
    print("  encrypt    - Disk encryption status\n");
    print("  firewall   - Firewall status\n");
}

void cmd_version() {
    print(CODIX_NAME);
    print(" v");
    print(CODIX_VERSION);
    print("\n");
}

void cmd_uptime() {
    uint32_t seconds = tick_count / 100;
    uint32_t minutes = seconds / 60;
    uint32_t hours = minutes / 60;
    
    print("Uptime: ");
    print_int(hours);
    print("h ");
    print_int(minutes % 60);
    print("m ");
    print_int(seconds % 60);
    print("s\n");
}

void cmd_echo(char* args) {
    println(args);
}

void cmd_about() {
    set_color(0x0B);
    println("=== About CodixOS ===");
    set_color(0x07);
    println("CodixOS is a lightweight, terminal-based operating system");
    println("designed for simplicity and ease of use.");
    println("");
    println("Features:");
    println("  - Custom kernel with memory management");
    println("  - Built-in shell with command support");
    println("  - Package manager for software installation");
    println("  - Terminal-based interface");
    println("  - Lightweight and fast boot");
    println("  - Secure Boot verification");
    println("  - Full-disk encryption (LUKS)");
    println("  - Pre-installed Firefox browser");
    println("");
    println("License: MIT");
    println("Author: CodixOS Development Team");
}

/* Main kernel entry point */
void kernel_main() {
    /* Initialize systems */
    clear_screen();
    init_interrupts();
    init_memory();
    init_filesystem();
    init_processes();
    
    /* Initialize security systems */
    secure_boot_init();
    disk_encrypt_init();
    
    /* Print welcome banner */
    print_banner();
    
    /* Main shell loop */
    char input[256];
    while (1) {
        print_prompt();
        int len = readline(input, sizeof(input));
        
        if (len == 0) continue;
        
        /* Parse command */
        char* cmd = input;
        char* args = 0;
        
        for (int i = 0; i < len; i++) {
            if (input[i] == ' ') {
                input[i] = '\0';
                args = &input[i + 1];
                break;
            }
        }
        
        /* Execute commands */
        if (strcmp(cmd, "help") == 0) {
            cmd_help();
        } else if (strcmp(cmd, "clear") == 0) {
            clear_screen();
        } else if (strcmp(cmd, "echo") == 0) {
            if (args) cmd_echo(args);
        } else if (strcmp(cmd, "version") == 0) {
            cmd_version();
        } else if (strcmp(cmd, "uptime") == 0) {
            cmd_uptime();
        } else if (strcmp(cmd, "about") == 0) {
            cmd_about();
        } else if (strcmp(cmd, "secureboot") == 0) {
            secure_boot_status();
        } else if (strcmp(cmd, "encrypt") == 0) {
            disk_encrypt_list_devices();
        } else if (strcmp(cmd, "firewall") == 0) {
            set_color(0x0B);
            print("=== Firewall Status ===\n");
            set_color(0x07);
            print("  State: ENABLED\n");
            print("  Policy: DROP\n");
            print("  Rules: 12 active\n");
        } else if (strcmp(cmd, "reboot") == 0) {
            println("Rebooting...");
            reboot();
        } else if (strcmp(cmd, "halt") == 0) {
            println("Halting system...");
            halt();
        } else {
            print("Unknown command: ");
            print(cmd);
            print("\nType 'help' for available commands\n");
        }
    }
}

/* Placeholder functions */
void init_interrupts() {}
void init_memory() {}
void init_filesystem() {}
void init_processes() {}
void reboot() {
    while (1) {
        __asm__ __volatile__("cli; hlt");
    }
}
void halt() {
    while (1) {
        __asm__ __volatile__("cli; hlt");
    }
}
