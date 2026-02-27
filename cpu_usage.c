/*
 * cpu_usage.c - Print current CPU usage for GNU Screen status bar (macOS)
 * Samples CPU ticks over ~200 ms and outputs: "CPU:XX%"
 */
#include <stdio.h>
#include <mach/mach.h>
#include <mach/processor_info.h>
#include <mach/mach_host.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct {
    unsigned long long idle;
    unsigned long long total;
} cpu_snapshot_t;

static int take_snapshot(cpu_snapshot_t *snap) {
    natural_t cpu_count = 0;
    processor_info_array_t info_array;
    mach_msg_type_number_t info_count;

    kern_return_t kr = host_processor_info(
        mach_host_self(),
        PROCESSOR_CPU_LOAD_INFO,
        &cpu_count,
        &info_array,
        &info_count
    );
    if (kr != KERN_SUCCESS) return -1;

    unsigned long long total_idle  = 0;
    unsigned long long total_ticks = 0;

    for (natural_t i = 0; i < cpu_count; i++) {
        processor_cpu_load_info_t load =
            (processor_cpu_load_info_t)(info_array + i * CPU_STATE_MAX);
        unsigned long long idle  = load->cpu_ticks[CPU_STATE_IDLE];
        unsigned long long user  = load->cpu_ticks[CPU_STATE_USER];
        unsigned long long sys   = load->cpu_ticks[CPU_STATE_SYSTEM];
        unsigned long long nice  = load->cpu_ticks[CPU_STATE_NICE];
        total_idle  += idle;
        total_ticks += idle + user + sys + nice;
    }

    vm_deallocate(mach_task_self(), (vm_address_t)info_array,
                  info_count * sizeof(natural_t));

    snap->idle  = total_idle;
    snap->total = total_ticks;
    return 0;
}

int main(void) {
    cpu_snapshot_t s1, s2;

    if (take_snapshot(&s1) != 0) {
        printf("CPU:??%%");
        return 1;
    }

    /* 200 ms sample window — fast enough for the status bar */
    usleep(200000);

    if (take_snapshot(&s2) != 0) {
        printf("CPU:??%%");
        return 1;
    }

    unsigned long long delta_total = s2.total - s1.total;
    unsigned long long delta_idle  = s2.idle  - s1.idle;

    int pct = 0;
    if (delta_total > 0) {
        pct = (int)(((double)(delta_total - delta_idle) / delta_total) * 100.0 + 0.5);
    }

    printf("CPU:%d%%", pct);
    return 0;
}
