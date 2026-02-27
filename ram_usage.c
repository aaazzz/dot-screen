/*
 * ram_usage.c - Print current RAM usage for GNU Screen status bar (macOS)
 * Output: "RAM:XX% (YY.YG/ZZ.ZG)"
 */
#include <stdio.h>
#include <mach/mach.h>
#include <sys/sysctl.h>

int main(void) {
    /* Total physical memory */
    uint64_t total_mem = 0;
    size_t len = sizeof(total_mem);
    sysctlbyname("hw.memsize", &total_mem, &len, NULL, 0);

    /* VM statistics */
    mach_port_t host = mach_host_self();
    vm_size_t page_size = 0;
    host_page_size(host, &page_size);

    vm_statistics64_data_t vm_stats;
    mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
    host_statistics64(host, HOST_VM_INFO64, (host_info64_t)&vm_stats, &count);

    /*
     * "Used" memory = total - free - (inactive that is cleanly reclaimable)
     * A common approximation: used = active + wire + compressor pages
     */
    uint64_t free_pages     = vm_stats.free_count;
    uint64_t inactive_pages = vm_stats.inactive_count;
    uint64_t used_pages     = (uint64_t)(total_mem / page_size)
                              - free_pages - inactive_pages;

    uint64_t used_bytes  = used_pages  * (uint64_t)page_size;
    uint64_t total_bytes = total_mem;

    double used_gb  = (double)used_bytes  / (1024.0 * 1024.0 * 1024.0);
    double total_gb = (double)total_bytes / (1024.0 * 1024.0 * 1024.0);
    int    pct      = (int)((double)used_bytes / total_bytes * 100.0 + 0.5);

    printf("RAM:%d%% (%.1fG/%.1fG)", pct, used_gb, total_gb);
    return 0;
}
