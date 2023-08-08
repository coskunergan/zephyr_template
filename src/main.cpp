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

#define STACK_SIZE (1024 + CONFIG_TEST_EXTRA_STACK_SIZE)

using namespace device_printf;

#if !DT_NODE_EXISTS(DT_NODELABEL(buzzer))
#error "Overlay for gipo node not properly defined."
#endif

static const struct gpio_dt_spec buzzer =
    GPIO_DT_SPEC_GET_OR(DT_NODELABEL(buzzer), gpios,
                        {
                            0
                        });

namespace
{
    ZPP_THREAD_STACK_ARRAY_DEFINE(tstack, 1, STACK_SIZE);
    zpp::thread_data tcb;
    zpp::thread t;

} // anonimouse namespace

void test_task(int my_id) noexcept
{
    gpio_pin_set_dt(&buzzer, true);
    zpp::this_thread::sleep_for(std::chrono::milliseconds(50));
    gpio_pin_set_dt(&buzzer, false);
    for(;;)
    {
        zpp::this_thread::sleep_for(std::chrono::milliseconds(500));
       // gpio_pin_toggle_dt(&lcd_bl);
    }
}

int main(void)
{
    printf("\rRestart...");

    if(!gpio_is_ready_dt(&buzzer))
    { 
        return 0;
    }
    if(gpio_pin_configure_dt(&buzzer, GPIO_OUTPUT_INACTIVE) != 0)
    {
        return 0;
    }

    const zpp::thread_attr attrs(
        zpp::thread_prio::preempt(10),
        zpp::thread_inherit_perms::no,
        zpp::thread_suspend::no
    );

    t = zpp::thread(tcb, tstack(0), attrs, test_task, 1);

    t.join();

    return 0;
}
