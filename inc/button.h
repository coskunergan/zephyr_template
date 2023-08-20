/*
    Button Lib

    Created on: July 26, 2023

    Author: Coskun ERGAN
*/

#pragma once
#include <stdio.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

#include <zpp.hpp>
#include <zpp/timer.hpp>
#include <chrono>
#include <zpp/thread.hpp>
#include <zpp/fmt.hpp>
#include <forward_list>
#include <functional>

namespace device_button
{
    using namespace zpp;
    using namespace std::chrono;
    static std::forward_list<std::pair<gpio_dt_spec, timer_base>> forwardListOfPairs;
#define NUM_BUTTON_PINS DT_PROP_LEN(DT_PATH(buttons), gpios)
#if !DT_NODE_EXISTS(DT_NODELABEL(buttons))
#error "Overlay for gipo node not properly defined."
#endif
    static const struct gpio_dt_spec buttons[] =
    {
        GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(buttons), gpios, 0),
#if NUM_BUTTON_PINS >= 2
        GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(buttons), gpios, 1),
#endif
#if NUM_BUTTON_PINS >= 3
        GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(buttons), gpios, 2),
#endif
#if NUM_BUTTON_PINS >= 4
        GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(buttons), gpios, 3),
#endif
#if NUM_BUTTON_PINS >= 5
        GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(buttons), gpios, 4),
#endif
#if NUM_BUTTON_PINS >= 6
        GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(buttons), gpios, 5),
#endif
    };

    static struct gpio_callback button_cb_data;
    static condition_variable button_cv;

    void timer_callback(timer_base *t) noexcept
    {
        {
            //gpio_add_callback(button_a.port, &button_cb_data);
        }
    }

    class timer_wrapper
    {
    public:
        timer_wrapper() noexcept : tmr(basic_timer(timer_callback)) {};
        basic_timer<void(*)(timer_base *)> tmr;
    };
    timer_wrapper timers[NUM_BUTTON_PINS];

    void button_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pin) noexcept
    {
        // for(auto const &currentPair : forwardListOfPairs)
        // {
        //     const gpio_dt_spec button = currentPair.first;
        //     if(button.pin == pin)
        //     {
        //         if(gpio_pin_get_dt(&button))
        //         {
        //             gpio_remove_callback(button.port, &button_cb_data);
        //             //static_cast<basic_timer>(currentPair.second).start();
        //             //make_timer()
        //             //timer
        //         }
        //     }
        // }
        // swtich(pin)
        // {
        // case BIT(button_a.pin):
        //     if(gpio_pin_get_dt(&button_a))
        //     {
        //         m_ta.start(m_debounce_time);
        //
        //     }
        //     break;
        // case BIT(button_b.pin):
        //     if(gpio_pin_get_dt(&button_b))
        //     {
        //         m_tb.start(m_debounce_time);
        //         gpio_remove_callback(button_b.port, &button_b_cb_data);
        //     }
        //     break;
        // }
        button_cv.notify_all();
    }

    timer_base *m_t[NUM_BUTTON_PINS];

    class button
    {
    public:
        button()
        {
            set_debounce_time();
            for(int i = 0; i < NUM_BUTTON_PINS; i++)
            {
                auto t = make_timer(timer_callback);
                m_t[i] = std::move(&t);
            }
        }
        ~button() = default;
        button(button &&) = delete;
        button &operator=(button &&) = delete;
        button(const button &) = delete;
        button &operator=(const button &) = delete;

        void set_debounce_time(milliseconds ms = 50ms)
        {
            m_debounce_time = ms;
        }

        milliseconds get_debounce_time()
        {
            return m_debounce_time;
        }

    private:
        static milliseconds m_debounce_time;
        // std::unique_ptr<sys::timer> m_tim_ptr;
        // std::chrono::milliseconds m_long_press_msec[max_button_num];
        // std::function<bool()> m_button_check_handler[max_button_num];
        // std::function<void()> m_button_press_handler[max_button_num];
        // std::function<void()> m_button_longpress_handler[max_button_num];
        // std::chrono::system_clock::time_point m_long_press_tp[max_button_num];
        // std::unique_ptr<sys::thread> m_thread_ptr;
        // cmsis::thread_flags m_th_flag;

    };
}
