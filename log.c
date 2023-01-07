#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <assert.h>

#include "log.h"

//============================================================================
// basic log functions
//============================================================================

#define LOG_NAME_LEN      256
#define LOG_BUF_LEN       1024
#define DATA_TIME_STR_LEN 40

typedef struct log
{
    FILE *hnd;
    char  name[LOG_NAME_LEN];
    unsigned char mode;
    unsigned char max_lines;

    unsigned int  lines;
    unsigned int  count; // number of log file changes
} log_t;

void get_date_time_string(char *str, unsigned int str_size, char *fmt)
{
    time_t curr_time = 0;
    struct tm *pt = NULL;

    assert(str);

    curr_time = time(NULL);
    pt = localtime(&curr_time);
    assert(pt);

    snprintf(str, str_size, fmt,
             pt->tm_year + 1900, pt->tm_mon + 1, pt->tm_mday,
             pt->tm_hour, pt->tm_min, pt->tm_sec);
}

void add_one_trace(log_t *log, const char *trace)
{
    char date_time[DATA_TIME_STR_LEN];
    get_date_time_string(date_time, DATA_TIME_STR_LEN, "%04d%02d%02d %02d:%02d:%02d");

    if (log->mode & LOG_TO_FILE)
    {
        fprintf(log->hnd, "%s %s", date_time, trace);
        log->lines++;
    }

    if (log->mode & LOG_TO_SCREEN)
    {
        printf("%s %s", date_time, trace);
    }
}

void add_only_file_trace(FILE *hnd, const char *trace)
{
    char date_time[DATA_TIME_STR_LEN];

    get_date_time_string(date_time, DATA_TIME_STR_LEN, "%04d%02d%02d %02d:%02d:%02d");
    fprintf(hnd, "%s %s", date_time, trace);
}

FILE *create_log_file(const char *log_name, unsigned int count)
{
    char date_time[DATA_TIME_STR_LEN];
    char name[LOG_NAME_LEN];

    get_date_time_string(date_time, DATA_TIME_STR_LEN, "%04d%02d%02d_%02d%02d%02d");
    snprintf(name, LOG_NAME_LEN, "%s_%s_%04u.log", log_name, date_time, count);
    FILE *hnd = fopen(name, "wb+");
    if (!hnd)
    {
        return NULL;
    }

    add_only_file_trace(hnd, "log file created!!!\n");

    return hnd;
}

void *log_create(const char *log_name, unsigned int mode, unsigned int max_lines)
{
    log_t *log = (log_t *)malloc(sizeof(log_t));
    if (!log)
    {
        return NULL;
    }

    memset(log, 0, sizeof(log_t));
	
    if (mode & LOG_TO_FILE)
    {
        log->hnd = create_log_file(log_name, log->count);
        if (!log->hnd)
        {
            free(log);
            return NULL;
        }

        strncpy(log->name, log_name, sizeof(log->name));
    }

    log->mode = mode;
    log->max_lines = max_lines;
    log->lines = 0;
    log->count++;

    return log;
}

void log_close(void *log)
{
    log_t *tmp_log = (log_t *)log;
    if (!tmp_log)
    {
        return;
    }

    if (tmp_log->hnd)
    {
        add_only_file_trace(tmp_log->hnd, "log file closed!!!\n");
        fclose(tmp_log->hnd);
        tmp_log->hnd = NULL;
    }

    free(tmp_log);
}

void log_set_max_lines(void *log, unsigned int max_lines)
{
    log_t *tmp_log = (log_t *)log;
    if (!tmp_log)
    {
        return;
    }

    tmp_log->max_lines = max_lines;
}

void log_add_one_trace(void *log, void *buf)
{
    log_t *tmp_log = (log_t *)log;
    if (!tmp_log)
    {
        return;
    }

    add_one_trace(tmp_log, buf);
    if ((tmp_log->lines >= tmp_log->max_lines) && (tmp_log->mode & LOG_TO_FILE))
    {
        FILE *hnd = create_log_file(tmp_log->name, tmp_log->count);
        if (hnd)
        {
            add_only_file_trace(tmp_log->hnd, "log file closed!!!\n");

            fclose(tmp_log->hnd);
            tmp_log->hnd = hnd;

            tmp_log->lines = 0;
            tmp_log->count++;
        }
    }
}

//============================================================================
// log module manager functions
//============================================================================

log_mgr_t g_log;

void log_set_level(unsigned int mid, int level)
{
    if (mid >= MIDS_NUM)
    {
        return;
    }
    
    g_log.level[mid] = level;

    return;
}

void log_set_name(unsigned int mid, const char *name)
{
    if (mid >= MIDS_NUM)
    {
        return;
    }
    
    strncpy(g_log.name[mid], name, MNAME_SIZE);
    g_log.name[mid][MNAME_SIZE - 1] = '\0';

    return;
}

int log_init(const char *log_name, unsigned int mode, unsigned int max_lines)
{
    memset(&g_log, 0, sizeof(log_mgr_t));
    g_log.hnd = log_create(log_name, mode, max_lines);
    if (g_log.hnd)
    {
        return 0;  // success
    }
    
    return -1; // failed
}

void log_destroy(void)
{
    if (g_log.hnd)
    {
        log_close(g_log.hnd);
        g_log.hnd = NULL;
    }
}

void log_trace(unsigned int mid, unsigned char level, const char *fmt, ...)
{
    if (level > g_log.level[mid])
    {
        return;
    }
    
    char buf[LOG_BUF_LEN];
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(buf, LOG_BUF_LEN, fmt, ap);
    va_end(ap);

    log_add_one_trace(g_log.hnd, buf);
}
