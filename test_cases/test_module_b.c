/*
* from https://github.com/axenhook/log.git
*/

#include <assert.h>
#include <stdio.h>
#include <signal.h>

#include "../log.h"
#include "threads_group.h"

MODULE_ID(21);

extern int g_exit_flag;

static void print_log(int level, unsigned int thread_id, unsigned int cnt)
{
        SET_MODULE_LEVEL(level);
        
        LOG_DEBUG("Test very1 very2 very3 very4 very5 very6 very7 very8 very9 very10 very11 very12 very13 very14 very15 very16 long %d, cnt %d\n", thread_id, cnt);
        LOG_INFO("Test very1 very2 very3 very4 very5 very6 very7 very8 very9 very10 very11 very12 very13 very14 very15 very16 long %d, cnt %d\n", thread_id, cnt);
        LOG_WARN("Test very1 very2 very3 very4 very5 very6 very7 very8 very9 very10 very11 very12 very13 very14 very15 very16 long %d, cnt %d\n", thread_id, cnt);
        LOG_ERROR("Test very1 very2 very3 very4 very5 very6 very7 very8 very9 very10 very11 very12 very13 very14 very15 very16 long %d, cnt %d\n", thread_id, cnt);
        LOG_EMERG("Test very1 very2 very3 very4 very5 very6 very7 very8 very9 very10 very11 very12 very13 very14 very15 very16 long %d, cnt %d\n", thread_id, cnt);
        LOG_EVENT("Test very1 very2 very3 very4 very5 very6 very7 very8 very9 very10 very11 very12 very13 very14 very15 very16 long %d, cnt %d\n", thread_id, cnt);
}

static void *thread_test(void *arg, unsigned int thread_id)
{
    unsigned int i = 0;

    while (1)
    {
        if (g_exit_flag)
        {
            break;
        }
        
        print_log(0, thread_id, i++);
        print_log(1, thread_id, i++);
        print_log(2, thread_id, i++);
        print_log(3, thread_id, i++);
        print_log(4, thread_id, i++);
        print_log(5, thread_id, i++);
        print_log(6, thread_id, i++);
        
        // usleep(100 * 1000);
    }

    printf("The thread stopped. [thread_id: %u, thread: %lu]\n",
           thread_id, os_get_thread_id());

    return NULL;
}

void *start_threads_group_module_b(void)
{
    SET_MODULE_NAME("test_module_b");
    return threads_group_start(20, thread_test, NULL, "test_module_b");
}