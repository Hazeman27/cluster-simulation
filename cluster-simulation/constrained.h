#pragma once

namespace ntf
{
    template <typename T, T min, T max, T default_value>
    struct constrained
    {
        T value = default_value;

        constrained() = default;

        constrained(T value)
        {
            if (is_in_range(value))
                this->value = value;
        }

        static bool is_in_range(T value) { return value >= min && value <= max; }

        constrained& operator= (const constrained& rhs) = default;

        constrained& operator= (T rhs)
        {
            if (rhs >= min && rhs <= max)
                this->value = rhs;
            return *this;
        };

        constrained operator++ (int)
        {
            constrained tmp{ this->value };

            if (this->value < max)
                this->value++;

            return tmp;
        }

        constrained operator-- (int)
        {
            constrained tmp{ this->value };

            if (this->value > min)
                this->value--;

            return tmp;
        }

        operator T() { return this->value; }
    };
}