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

namespace
{
    ZPP_THREAD_STACK_ARRAY_DEFINE(tstack, 1, STACK_SIZE);
    thread_data tcb;
    thread t;

}

void test_task(int my_id) noexcept
{
    buzzer.beep();

    for(;;)
    {
        this_thread::sleep_for(3s);
        buzzer.beep(5ms);
        printf("\rEnc : %d",encoder.get_count());
    }
}

int main(void)
{
    printf_io.turn_off_bl_enable();
    printf("\rRestart..");

    encoder.set_event_hook([]
    {
        printf("\rEncoder...");
        buzzer.beep(10ms);
    });

    const thread_attr attrs(
        thread_prio::preempt(10),
        thread_inherit_perms::no,
        thread_suspend::no
    );

    t = thread(tcb, tstack(0), attrs, test_task, 1);

    t.join();

    return 0;
}
