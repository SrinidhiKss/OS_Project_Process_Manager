
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 1024

int main(void) {
    FILE *fp;
    char buf[BUF_SIZE];

    printf(" PID | CPU Bound | CPU Utilization | IO Utilization | Memory Utilization | Process Type \n");
   
printf("----------------------------------------------------------------------------------------------------\n");
printf("USER PROCESSES:\n");
    fp = popen("ps -eo pid,comm,%cpu,%mem,c --sort=-%cpu |head -n 11", "r");
    if (fp == NULL) {
     
        exit(1);
    }

    fgets(buf, BUF_SIZE, fp); // skip header line
    double total_cpu_util = 0, total_io_util = 0, total_mem_util = 0;
    while (fgets(buf, BUF_SIZE, fp) != NULL) {
        char *pid_str = strtok(buf, " ");
        char *user = strtok(NULL, " ");
        char *comm = strtok(NULL, " ");
        char *cpu_str = strtok(NULL, " ");
        char *mem_str = strtok(NULL, " ");
        int pid = atoi(pid_str);
        double cpu = strtod(cpu_str, NULL);
        double mem = strtod(mem_str, NULL);

        char io_file_path[BUF_SIZE];
        sprintf(io_file_path, "/proc/%d/io", pid);
        FILE *io_file = fopen(io_file_path, "r");
        if (io_file == NULL) {
            continue;
        }
        char *line = NULL;
        size_t len = 0;
        double read_bytes = 0, write_bytes = 0;
        while (getline(&line, &len, io_file) != -1) {
            if (strncmp(line, "read_bytes:", 11) == 0) {
                read_bytes = strtod(line + 11, NULL);
            } else if (strncmp(line, "write_bytes:", 12) == 0) {
                write_bytes = strtod(line + 12, NULL);
            }
        }
        free(line);
        fclose(io_file);

        double io_utilization = (read_bytes + write_bytes) / (cpu * 1000000) * 100;
        char *type_str=(comm[0]=='/')?"system":"user";
        char *cpu_bound = (io_utilization > 10) ? "I/O" : "CPU";
       
        printf(" %3d |   %3s     |       %7.3f       |       %7.3f       |        %7.3lf        |  %s\n", pid, cpu_bound, cpu, io_utilization, mem, type_str);

        total_cpu_util += cpu;
        total_io_util += io_utilization;
        total_mem_util += mem;
    }

    pclose(fp);   printf("----------------------------------------------------------------------------------------------------\n");
printf("SYSTEM PROCESSES\n");
    fp = popen("ps -eo pid,comm,%cpu,%mem,c --sort=-%cpu ", "r");
    if (fp == NULL) {
        exit(1);
    }

    fgets(buf, BUF_SIZE, fp); 
    while (fgets(buf, BUF_SIZE, fp) != NULL) {
        char *pid_str = strtok(buf, " ");
        char *user = strtok(NULL, " ");
        char *comm = strtok(NULL, " ");
        char *cpu_str = strtok(NULL, " ");
        char *mem_str = strtok(NULL, " ");
        int pid = atoi(pid_str);
        double cpu = strtod(cpu_str, NULL);
        double mem = strtod(mem_str, NULL);

        char io_file_path[BUF_SIZE];
        sprintf(io_file_path, "/proc/%d/io", pid);
        FILE *io_file = fopen(io_file_path, "r");
        if (io_file == NULL) {
            continue;
        }
        char *line = NULL;
        size_t len = 0;
        double read_bytes = 0, write_bytes = 0;
        while (getline(&line, &len, io_file) != -1) {
            if (strncmp(line, "read_bytes:", 11) == 0) {
                read_bytes = strtod(line + 11, NULL);
            } else if (strncmp(line, "write_bytes:", 12) == 0) {
                write_bytes = strtod(line + 12, NULL);
            }
        }
        free(line);
        fclose(io_file);

        double io_utilization = (read_bytes + write_bytes) / (cpu * 1000000) * 100;
        char *type_str=(comm[0]=='/')?"user":"system";
        char *cpu_bound = (io_utilization > 10) ? "I/O" : "CPU";
        printf(" %3d |   %3s     |       %7.3f       |       %7.3f       |        %7.3lf        |  %s\n", pid, cpu_bound, cpu, io_utilization, mem, type_str);

        total_cpu_util += cpu;
        total_io_util += io_utilization;
        total_mem_util += mem;
    }

    pclose(fp);

    printf("----------------------------------------------------------------------------------------------------\n");
    printf(" Total Utilization |       %7.3f       |       %7.3f       |        %7.3lf        |             \n", total_cpu_util, total_io_util, total_mem_util);
    printf("----------------------------------------------------------------------------------------------------\n");

    return 0;
}
