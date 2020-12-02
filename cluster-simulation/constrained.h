#pragma once
#include <cstdint>

namespace ntf
{
    template <typename T, T min, T max, T initial>
    struct constrained
    {
        T value = initial;

        constrained() = default;
        
        constrained(const constrained& other)
        {
            if (other.value >= min && other.value <= max)
                this->value = other.value;
        }

        constrained& operator= (const constrained& rhs)
        {
            if (rhs.value >= min && rhs.value <= max)
                this->value = rhs.value;
            return *this;
        };

        constrained& operator+= (const T rhs)
        {
            T new_value = this->value + rhs;
            
            if (new_value < min || new_value > max)
                return *this;

            this->value = new_value;
            return *this;
        }

        constrained& operator+= (const constrained& rhs)
        {
            return this->operator+=(rhs.value);
        }

        constrained& operator-= (const T rhs)
        {
            T new_value = this->value - rhs;

            if (new_value < min || new_value > max)
                return *this;

            this->value = new_value;
            return *this;
        }

        constrained& operator-= (const constrained& rhs)
        {
            return this->operator-=(rhs.value);
        }

        constrained operator-- (int)
        {
            constrained tmp(*this);

            if (this->value > min)
                this->value--;

            return tmp;
        }

        constrained operator++ (int)
        {
            constrained tmp(*this);

            if (this->value < max)
                this->value++;

            return tmp;
        }

        operator T() const
        {
            return this->value;
        }

        bool operator== (const constrained& rhs)
        {
            return this->value == rhs.value;
        }

        bool operator!= (const constrained& rhs)
        {
            return this->value != rhs.value;
        }

        bool operator< (const constrained& rhs)
        {
            return this->value < rhs.value;
        }

        bool operator> (const constrained& rhs)
        {
            return this->value > rhs.value;
        }

        bool operator<= (const constrained& rhs)
        {
            return this->value <= rhs.value;
        }

        bool operator>= (const constrained& rhs)
        {
            return this->value >= rhs.value;
        }

        const std::string to_string()
        {
            return (
                "[min: " + std::to_string(min) +
                "; max: " + std::to_string(max) +
                "; val: " + std::to_string(value) + "]"
            );
        }

        friend std::ostream& operator << (std::ostream& os, const constrained& rhs)
        {
            os << rhs.to_string();
            return os;
        }
    };
}