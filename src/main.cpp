/*
    GPIO Hal Library

    Created on: July 31, 2023

    Author: Coskun ERGAN
*/

#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/arch/cpu.h>
#include <zephyr/sys/arch_interface.h>
#include <zephyr/sys/libc-hooks.h>

#include <zpp.hpp>
#include <chrono>
#include "printf_io.h"
#include "buzzer.h"
#include "encoder.h"

#define STACK_SIZE (1024 + CONFIG_TEST_EXTRA_STACK_SIZE)

using namespace zpp;
using namespace std::chrono;
using namespace device_printf;
using namespace device_buzzer;
using namespace device_encoder;

#define NUM_THREADS 2

mutex enc_mutex;
mutex btn_mutex;

namespace
{
    ZPP_THREAD_STACK_ARRAY_DEFINE(tstack, NUM_THREADS, STACK_SIZE);
    thread_data tcb[NUM_THREADS];
    thread t[NUM_THREADS];
}

void btn_task(int my_id) noexcept
{
    unique_lock lk(enc_mutex);

    for(;;)
    {
        encoder.event_cv().wait(lk);
        buzzer.beep(5ms);
        //printf_io.turn_off_bl_enable();
        //printf("\rEnc : %d", encoder.get_count());
    }
}


void enc_task(int my_id) noexcept
{
    unique_lock lk(enc_mutex);

    for(;;)
    {
        encoder.event_cv().wait(lk);
        //buzzer.beep(5ms);
        printf_io.turn_off_bl_enable();
        printf("\rEnc : %d", encoder.get_count());
    }
}

int main(void)
{
    printf_io.turn_off_bl_enable();
    printf("\rRestart..");
    buzzer.beep();

    const thread_attr attrs(
        thread_prio::preempt(10),
        thread_inherit_perms::no,
        thread_suspend::no
    );

    t[0] = thread(tcb[0], tstack(0), attrs, enc_task, 1);
    t[1] = thread(tcb[1], tstack(1), attrs, btn_task, 2);

    for(int i = 0; i < NUM_THREADS; i++)
    {
        t[i].join();
    }

    return 0;
}
