/*
    Encoder Lib

    Created on: March 13, 2023

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
#include <zpp/atomic_var.hpp>

namespace device_encoder
{
    using namespace zpp;
    using namespace std::chrono;
#if !DT_NODE_EXISTS(DT_NODELABEL(encoder_a))
#error "Overlay for gipo node not properly defined."
#endif
#if !DT_NODE_EXISTS(DT_NODELABEL(encoder_b))
#error "Overlay for gipo node not properly defined."
#endif
    static const struct gpio_dt_spec encoder_a =
        GPIO_DT_SPEC_GET_OR(DT_NODELABEL(encoder_a), gpios,
                            {
                                0
                            });
    static const struct gpio_dt_spec encoder_b =
        GPIO_DT_SPEC_GET_OR(DT_NODELABEL(encoder_b), gpios,
                            {
                                0
                            });

    static struct gpio_callback encoder_cb_data;
    static milliseconds m_debounce_time;
    static atomic_var m_atomic_count;
    static condition_variable m_encoder_cv;

    void timer_callback(timer_base *t) noexcept
    {
        gpio_add_callback(encoder_a.port, &encoder_cb_data);
    }

    auto m_t = make_timer(timer_callback);

    void encoder_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pin) noexcept
    {
        if(gpio_pin_get_dt(&encoder_b))
        {
            ++m_atomic_count;
        }
        else
        {
            --m_atomic_count;
        }
        gpio_remove_callback(encoder_a.port, &encoder_cb_data);
        m_t.start(m_debounce_time);
        m_encoder_cv.notify_all();
    }

    class encoder
    {
    public:

        encoder()
        {
            set_debounce_time();
            gpio_pin_configure_dt(&encoder_a, GPIO_INPUT);
            gpio_pin_configure_dt(&encoder_b, GPIO_INPUT);
            gpio_pin_interrupt_configure_dt(&encoder_a, GPIO_INT_EDGE_TO_ACTIVE);
            gpio_init_callback(&encoder_cb_data, encoder_isr, BIT(encoder_a.pin));
            gpio_add_callback(encoder_a.port, &encoder_cb_data);
        }
        ~encoder() = default;
        encoder(encoder &&) = delete;
        encoder &operator=(encoder &&) = delete;
        encoder(const encoder &) = delete;
        encoder &operator=(const encoder &) = delete;

        void set_debounce_time(milliseconds ms = 50ms)
        {
            m_debounce_time = ms;
        }

        milliseconds get_debounce_time()
        {
            return m_debounce_time;
        }

        int get_count()
        {
            return m_atomic_count; // m_atomic_count.load();
        }

        void set_count(int val)
        {
            m_atomic_count = val; // m_atomic_count.store(val);
        }

        condition_variable &event_cv()
        {
            return m_encoder_cv;
        }
    };

    encoder encoder;
}
