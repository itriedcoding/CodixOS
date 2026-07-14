/*
 * CodixOS Calculator Header
 * Simple GUI calculator
 */

#ifndef CALCULATOR_H
#define CALCULATOR_H

/* Calculator configuration */
#define CALC_DEFAULT_WIDTH    300
#define CALC_DEFAULT_HEIGHT   450
#define CALC_MIN_WIDTH        250
#define CALC_MIN_HEIGHT       350
#define CALC_BUTTON_SIZE      60
#define CALC_BUTTON_MARGIN    5
#define CALC_DISPLAY_HEIGHT   80

/* Colors (Catppuccin Mocha) */
#define CALC_BG_COLOR         0x1E1E2E
#define CALC_DISPLAY_BG       0x313244
#define CALC_DISPLAY_FG       0xCDD6F4
#define CALC_BUTTON_BG        0x45475A
#define CALC_BUTTON_FG        0xCDD6F4
#define CALC_BUTTON_HOVER     0x585B70
#define CALC_BUTTON_OPERATOR  0x89B4FA
#define CALC_BUTTON_OPERATOR_FG 0x1E1E2E
#define CALC_BUTTON_EQUALS    0xA6E3A1
#define CALC_BUTTON_EQUALS_FG 0x1E1E2E
#define CALC_BUTTON_CLEAR     0xF38BA8
#define CALC_BUTTON_CLEAR_FG  0x1E1E2E

/* Calculator modes */
#define CALC_MODE_STANDARD    0
#define CALC_MODE_SCIENTIFIC  1
#define CALC_MODE_PROGRAMMER  2
#define CALC_MODE_CONVERTER   3

/* Calculator operations */
#define CALC_OP_NONE          0
#define CALC_OP_ADD           1
#define CALC_OP_SUBTRACT      2
#define CALC_OP_MULTIPLY      3
#define CALC_OP_DIVIDE        4
#define CALC_OP_MODULO        5
#define CALC_OP_POWER         6
#define CALC_OP_SQRT          7
#define CALC_OP_PERCENT       8

/* Button types */
#define CALC_BUTTON_NUMBER    0
#define CALC_BUTTON_OPERATOR  1
#define CALC_BUTTON_EQUALS    2
#define CALC_BUTTON_CLEAR     3
#define CALC_BUTTON_FUNCTION  4

/* Button */
typedef struct {
    int x, y, width, height;
    char label[16];
    int type;
    int value;
    int operator_type;
    int hovered;
    int pressed;
    void (*callback)();
} CalcButton;

/* Calculator state */
typedef struct {
    int id;
    int x, y, width, height;
    int mode;
    
    /* Display */
    char display[64];
    char expression[256];
    double result;
    double memory;
    int display_length;
    
    /* State */
    double current_value;
    double stored_value;
    int current_operator;
    int has_stored_value;
    int just_calculated;
    int error;
    char error_message[64];
    
    /* Memory */
    double memory_store[10];
    int memory_count;
    
    /* Buttons */
    CalcButton buttons[32];
    int button_count;
    
    /* History */
    char history[100][128];
    int history_count;
    
    /* Settings */
    int show_thousands_separator;
    int decimal_places;
    int angle_mode; /* 0=radians, 1=degrees */
} Calculator;

/* Function prototypes */
Calculator* calc_create(int x, int y, int width, int height);
void calc_destroy(Calculator* calc);
void calc_render(Calculator* calc);
void calc_handle_event(Calculator* calc, void* event);

/* Display */
void calc_update_display(Calculator* calc);
void calc_clear_display(Calculator* calc);
void calc_set_error(Calculator* calc, const char* message);

/* Operations */
void calc_input_number(Calculator* calc, int number);
void calc_input_decimal(Calculator* calc);
void calc_input_operator(Calculator* calc, int operator);
void calc_calculate(Calculator* calc);
void calc_clear(Calculator* calc);
void calc_clear_entry(Calculator* calc);
void calc_backspace(Calculator* calc);
void calc_negate(Calculator* calc);
void calc_percent(Calculator* calc);

/* Scientific functions */
void calc_sin(Calculator* calc);
void calc_cos(Calculator* calc);
void calc_tan(Calculator* calc);
void calc_log(Calculator* calc);
void calc_ln(Calculator* calc);
void calc_exp(Calculator* calc);
void calc_power(Calculator* calc);
void calc_sqrt(Calculator* calc);
void calc_factorial(Calculator* calc);
void calc_pi(Calculator* calc);
void calc_e(Calculator* calc);

/* Memory */
void calc_memory_store(Calculator* calc);
void calc_memory_recall(Calculator* calc);
void calc_memory_add(Calculator* calc);
void calc_memory_subtract(Calculator* calc);
void calc_memory_clear(Calculator* calc);

/* History */
void calc_add_history(Calculator* calc, const char* entry);
void calc_clear_history(Calculator* calc);

/* Mode */
void calc_set_mode(Calculator* calc, int mode);
void calc_setup_buttons(Calculator* calc);

/* Utility */
double calc_do_operation(double a, double b, int op);
void calc_format_number(Calculator* calc, double number, char* buffer);
int calc_parse_number(const char* str, double* result);

#endif /* CALCULATOR_H */
