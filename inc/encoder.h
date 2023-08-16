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
#include <vector>
#include <memory>
#include <functional>
#include <zpp/clock.hpp>

namespace device_encoder
{
    using namespace zpp;
    using namespace std::chrono;
#if !DT_NODE_EXISTS(DT_NODELABEL(encoder_pins))
#error "Overlay for gipo node not properly defined."
#endif
    static const struct gpio_dt_spec encoder_pins[2] =
    {
        GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(encoder_pins), gpios, 0),
        GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(encoder_pins), gpios, 1)
    };

    static struct gpio_callback encoder_cb_data;
    static milliseconds m_debounce_time;
    static atomic_var m_atomic_count;
    static condition_variable m_encoder_cv;

    void timer_callback2(timer_base *t) noexcept
    {

    }
    void timer_callback(timer_base *t) noexcept
    {
        gpio_add_callback(encoder_pins[0].port, &encoder_cb_data);
    }

    //{ new basic_timer{timer_callback}};

    basic_timer m_t = basic_timer(timer_callback);
    //timer_base *m_t2 = new basic_timer{timer_callback};
    //timer_base  *m_t = &m_t2;

    //std::unique_ptr<timer_base> m_t(m_t2);
    //m_t = std::move(t);

    void encoder_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pin) noexcept
    {
        if(gpio_pin_get_dt(&encoder_pins[1]))
        {
            ++m_atomic_count;
        }
        else
        {
            --m_atomic_count;
        }
        gpio_remove_callback(encoder_pins[0].port, &encoder_cb_data);
        m_t.start(m_debounce_time);
        m_encoder_cv.notify_all();
    }

    void myExpireCallback(timer_base *self) noexcept
    {

    }

    void myCallback(timer_base *)
    {

    }

    class encoder
    {
    public:

        encoder()
        {
            set_debounce_time();
            
            using ExpireCallbackType = void(*)(timer_base *);
            std::unique_ptr<timer_base> test = std::make_unique<basic_timer<ExpireCallbackType>>(timer_callback2);
            //test->start(m_debounce_time); 
            //basic_timer d = basic_timer(timer_callback2);
            //std::vector<timer_base*> v;
            //v.push_back(&d);


            //std::unique_ptr<timer_base<ExpireCallbackType>> timerPtr( new basic_timer<ExpireCallbackType>(myExpireCallback2));
            //timerPtr->start(m_debounce_time);
            //emplace(basic_timer(timer_callback));

            //v[0]->start(m_debounce_time);

            //zpp::this_thread::sleep_for(std::chrono::milliseconds(1000));


            // Create a unique_ptr to manage a TimerType instance

            // zpp::basic_timer timer(myCallback);

            // Start the timer to trigger the callback after 1000 milliseconds (1 second)
            // timer.start(std::chrono::milliseconds(1000));

            //std::vector<basic_timer<ExpireCallbackType>> vec;

            //k_timer_start(timerPtr->native_handle(), 1000,1000);

            //vec.push_back(std::move(basic_timer<ExpireCallbackType>(myExpireCallback)));
            //basic_timer<ExpireCallbackType> *tt[2];
            //  tt[0] = new basic_timer<ExpireCallbackType>(myExpireCallback);
            //std::unique_ptr<timer_base> m_t = dynamic_cast<std::unique_ptr<timer_base>>(basic_timer{timer_callback});
            //std::unique_ptr<timer_base> tvec(new basic_timer<auto&&>>(timer_callback2));
            //auto d = basic_timer{timer_callback};
            //auto temp = clone(d);
            //vec[0]->start(m_debounce_time);
            //tvec[0]->start(m_debounce_time);
            //std::unique_ptr<timer_base> test= std::make_unique<basic_timer<class&&>>(timer_callback2);
            //test.reset(new basic_timer{timer_callback2});
            // = std::make_unique<basic_timer<timer_base*>>(timer_callback2);
            //auto test = make_unique<basic_timer>(timer_callback2);
            //test->start(m_debounce_time);

            // m_t = basic_timer{timer_callback};;
            //auto t = make_timer(timer_callback);
            //m_t[0] = std::move(t);

            //typedef std::shared_ptr<timer_base> PInstruction;
            //std::vector<PInstruction> v;
            //v.emplace_back(std::make_shared<basic_timer>(make_timer(timer_callback)));


            //std::unique_ptr<timer_base> t(new basic_timer(timer_callback));
            //m_t = std::move(t);
            //std::vector<std::reference_wrapper<timer_base>> polymorphicobjs;
            //polymorphicobjs.emplace_back(basic_timer(timer_callback));
            //polymorphicobjs.push_back(std::make_shared<basic_timer>());


            //std::vector<std::reference_wrapper<timer_base>*> my_list;
            //basic_timer *t =
            //my_list.emplace_back(new );

           basic_timer test3 = basic_timer(timer_callback2);
            //timer_base *test2 = new make_timer(timer_callback2);
            //std::unique_ptr<timer_base> test2(new timer_base);
            timer_base *test2 = &test3;//.release();
            test2->start(m_debounce_time);
            //basic_timer test2 = basic_timer{timer_callback2};
            //std::unique_ptr<timer_base> test(test2);
            //test2.start(m_debounce_time);


            
            gpio_pin_configure_dt(&encoder_pins[0], GPIO_INPUT);
            gpio_pin_configure_dt(&encoder_pins[1], GPIO_INPUT);
            gpio_pin_interrupt_configure_dt(&encoder_pins[0], GPIO_INT_EDGE_TO_ACTIVE);
            gpio_init_callback(&encoder_cb_data, encoder_isr, BIT(encoder_pins[0].pin));
            gpio_add_callback(encoder_pins[0].port, &encoder_cb_data);
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
