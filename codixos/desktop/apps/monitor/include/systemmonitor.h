/*
 * CodixOS System Monitor Header
 * Monitor system resources
 */

#ifndef SYSTEMMONITOR_H
#define SYSTEMMONITOR_H

/* Monitor configuration */
#define MONITOR_DEFAULT_WIDTH    600
#define MONITOR_DEFAULT_HEIGHT   400
#define MONITOR_UPDATE_INTERVAL  1000
#define MONITOR_HISTORY_SIZE     60

/* Colors (Catppuccin Mocha) */
#define MONITOR_BG_COLOR         0x1E1E2E
#define MONITOR_FG_COLOR         0xCDD6F4
#define MONITOR_CARD_BG          0x313244
#define MONITOR_CARD_BORDER      0x45475A
#define MONITOR_GRAPH_BG         0x181825
#define MONITOR_GRAPH_LINE       0x89B4FA
#define MONITOR_GRAPH_LINE_2     0xA6E3A1
#define MONITOR_GRAPH_LINE_3     0xF9E2AF
#define MONITOR_GRAPH_LINE_4     0xF38BA8
#define MONITOR_TEXT_COLOR       0xCDD6F4
#define MONITOR_ACCENT_COLOR    0x89B4FA

/* Process info */
typedef struct {
    int pid;
    char name[64];
    char user[32];
    float cpu_percent;
    long memory_kb;
    long virtual_kb;
    char status[16];
    int threads;
    char command[256];
} MonitorProcess;

/* CPU info */
typedef struct {
    int core_count;
    float usage_percent;
    float* core_usage;
    float temperature;
    float frequency;
    char model[128];
    float* history;
    int history_index;
} CPUInfo;

/* Memory info */
typedef struct {
    long total_kb;
    long used_kb;
    long free_kb;
    long buffers_kb;
    long cached_kb;
    long available_kb;
    float usage_percent;
    float* history;
    int history_index;
} MemoryInfo;

/* Disk info */
typedef struct {
    char name[64];
    char mount_point[256];
    char fs_type[32];
    long total_bytes;
    long used_bytes;
    long free_bytes;
    float usage_percent;
} DiskInfo;

/* Network info */
typedef struct {
    char interface[32];
    long rx_bytes;
    long tx_bytes;
    long rx_speed;
    long tx_speed;
    float* rx_history;
    float* tx_history;
    int history_index;
} NetworkInfo;

/* System monitor state */
typedef struct {
    int id;
    int x, y, width, height;
    int selected_tab;
    int update_interval;
    int running;
    
    /* System info */
    CPUInfo cpu;
    MemoryInfo memory;
    DiskInfo disks[16];
    int disk_count;
    NetworkInfo networks[8];
    int network_count;
    MonitorProcess processes[256];
    int process_count;
    
    /* UI state */
    int scroll_y;
    int selected_process;
    int sort_column;
    int sort_ascending;
    int show_all_processes;
    char filter[256];
    
    /* Tabs */
    int tab_count;
    char tab_names[8][32];
} SystemMonitor;

/* Function prototypes */
SystemMonitor* monitor_create(int x, int y, int width, int height);
void monitor_destroy(SystemMonitor* monitor);
void monitor_render(SystemMonitor* monitor);
void monitor_handle_event(SystemMonitor* monitor, void* event);
void monitor_update(SystemMonitor* monitor);

/* CPU */
void monitor_update_cpu(SystemMonitor* monitor);
void monitor_render_cpu(SystemMonitor* monitor);
float monitor_get_cpu_usage(SystemMonitor* monitor);
float monitor_get_cpu_temperature(SystemMonitor* monitor);

/* Memory */
void monitor_update_memory(SystemMonitor* monitor);
void monitor_render_memory(SystemMonitor* monitor);
long monitor_get_memory_total(SystemMonitor* monitor);
long monitor_get_memory_used(SystemMonitor* monitor);
float monitor_get_memory_usage(SystemMonitor* monitor);

/* Disk */
void monitor_update_disks(SystemMonitor* monitor);
void monitor_render_disks(SystemMonitor* monitor);
DiskInfo* monitor_get_disk(SystemMonitor* monitor, const char* name);

/* Network */
void monitor_update_network(SystemMonitor* monitor);
void monitor_render_network(SystemMonitor* monitor);
NetworkInfo* monitor_get_network(SystemMonitor* monitor, const char* interface);

/* Processes */
void monitor_update_processes(SystemMonitor* monitor);
void monitor_render_processes(SystemMonitor* monitor);
MonitorProcess* monitor_get_process(SystemMonitor* monitor, int pid);
void monitor_sort_processes(SystemMonitor* monitor, int column);
void monitor_kill_process(SystemMonitor* monitor, int pid);
void monitor_filter_processes(SystemMonitor* monitor, const char* filter);

/* Tabs */
void monitor_add_tab(SystemMonitor* monitor, const char* name);
void monitor_set_tab(SystemMonitor* monitor, int index);

/* Graph */
void monitor_draw_graph(float* data, int count, int x, int y, int width, int height, unsigned int color);

#endif /* SYSTEMMONITOR_H */
