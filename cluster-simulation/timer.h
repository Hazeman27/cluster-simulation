#pragma once
#include <chrono>

namespace ntf
{
    using seconds = std::chrono::seconds;
    using milliseconds = std::chrono::milliseconds;
    using microseconds = std::chrono::microseconds;

    template <typename T = microseconds>
    class timer
    {
    private:
        T& _elapsed_time;

    public:
        using high_res_clock = std::chrono::high_resolution_clock;
        using time_point = high_res_clock::time_point;

        time_point start = high_res_clock::now();
        time_point end = high_res_clock::now();

        timer() = default;

        timer(T& elapsed_time) : _elapsed_time(elapsed_time)
        {}

        ~timer()
        {
            this->take_time_point();
            _elapsed_time = std::chrono::duration_cast<T>(this->end - this->start);
        }

        void take_time_point()
        {
            this->end = high_res_clock::now();
        }

        T elapsed_time()
        {
            this->take_time_point();
            return std::chrono::duration_cast<T>(this->end - this->start);
        }
    };
}


