#pragma once
#include "olcPixelGameEngine.h"

namespace ntf
{
	template <typename T = int32_t>
	struct vector2d
	{
		T x = 0;
		T y = 0;

		vector2d() = default;

		vector2d(T x, T y) : x(x), y(y)
		{}

		vector2d(const vector2d& vector) : x(vector.x), y(vector.y)
		{}

		double euclidean_distance(const vector2d<T>& other) const
		{
			T deltaX = other.x - this->x;
			T deltaY = other.y - this->y;

			return std::sqrt(deltaX * deltaX + deltaY * deltaY);
		}

		uint64_t euclidean_distance_squared(const vector2d<T>& other) const
		{
			uint64_t deltaX = static_cast<uint64_t>(other.x) - static_cast<uint64_t>(this->x);
			uint64_t deltaY = static_cast<uint64_t>(other.y) - static_cast<uint64_t>(this->y);

			return deltaX * deltaX + deltaY * deltaY;
		}

		uint64_t manhattan_distance(const vector2d<T>& other) const
		{
			return std::abs(this->x - other.x) + std::abs(this->y - other.y);
		}

		vector2d& operator= (const vector2d& vector) = default;

		vector2d operator+ (const vector2d& rhs) const
		{
			return { this->x + rhs.x, this->y + rhs.y };
		}

		vector2d operator- (const vector2d& rhs) const
		{
			return { this->x - rhs.x, this->y - rhs.y };
		}

		vector2d operator* (const T& rhs) const
		{
			return { this->x * rhs, this->y * rhs };
		}

		vector2d operator* (const vector2d& rhs) const
		{
			return { this->x * rhs.x, this->y * rhs.y };
		}

		vector2d operator/ (const T& rhs) const
		{
			return { this->x / rhs, this->y / rhs };
		}

		vector2d operator/ (const vector2d& rhs) const
		{
			return { this->x / rhs.x, this->y / rhs.y };
		}

		vector2d& operator+= (const vector2d& rhs)
		{
			this->x += rhs.x;
			this->y += rhs.y;

			return *this;
		}

		vector2d& operator-= (const vector2d& rhs)
		{
			this->x -= rhs.x;
			this->y -= rhs.y;

			return *this;
		}

		vector2d& operator*= (const T& rhs)
		{
			this->x *= rhs;
			this->y *= rhs;

			return *this;
		}

		vector2d& operator/= (const T& rhs)
		{
			this->x /= rhs;
			this->y /= rhs;

			return *this;
		}

		vector2d operator+ () const
		{
			return { +x, +y };
		}

		vector2d operator- () const
		{
			return { -x, -y };
		}

		bool operator== (const vector2d& rhs) const
		{
			return this->x == rhs.x && this->y == rhs.y;
		}

		bool operator< (const vector2d& rhs) const
		{
			return this->x < rhs.x&& this->y < rhs.y;
		}

		bool operator<= (const vector2d& rhs) const
		{
			return this->x <= rhs.x && this->y <= rhs.y;
		}

		bool operator> (const vector2d& rhs) const
		{
			return this->x > rhs.x && this->y > rhs.y;
		}

		bool operator>= (const vector2d& rhs) const
		{
			return this->x >= rhs.x && this->y >= rhs.y;
		}

		bool operator!= (const vector2d& rhs) const
		{
			return this->x != rhs.x || this->y != rhs.y;
		}

		const std::string to_string() const
		{
			return "(" + std::to_string(this->x) + "," + std::to_string(this->y) + ")";
		}

		friend std::ostream& operator << (std::ostream& os, const vector2d& rhs)
		{
			os << rhs.to_string();
			return os;
		}

		operator vector2d<int32_t>() const
		{
			return {
				static_cast<int32_t>(this->x),
				static_cast<int32_t>(this->y),
			};
		}

		operator vector2d<float>() const
		{
			return {
				static_cast<float>(this->x),
				static_cast<float>(this->y),
			};
		}

		operator vector2d<double>() const
		{
			return {
				static_cast<double>(this->x),
				static_cast<double>(this->y),
			};
		}

		operator olc::v2d_generic<T>() const
		{
			return { this->x, this->y };
		}

		size_t operator() (const vector2d& vector) const noexcept
		{
			return std::hash<T>{}(vector.x) ^ (std::hash<T>{}(vector.y) << 1);
		}
	};

	using vi16_2d = vector2d<int16_t>;
	using vu16_2d = vector2d<uint16_t>;
	using vi2d = vector2d<int32_t>;
	using vu2d = vector2d<uint32_t>;
}
