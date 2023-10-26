/*
MIT License

Copyright (c) 2023 Ryan Hechenberger

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef ASB_ENV_GRID_HPP
#define ASB_ENV_GRID_HPP

// uncomment to use bot hori and vert grid instead of just a single hori grid
// #define ABS_USE_HORI_VERT_GRID

#include <geo/Point.hpp>
#include <geo/Angle.hxx>
#include <inx/bit_table.hpp>
#include <inx/factory.hpp>
#include <inx/functions.hpp>
#include <vector>
#include <unordered_map>
#include <memory_resource>
#include <any>

namespace asb::env {

class Grid
{
public:
	constexpr static size_t PADDING = 2;
	constexpr static geo::Point POINT_PAD = geo::Point(PADDING, PADDING);

	constexpr static geo::Point pad(geo::Point p) noexcept
	{
		return geo::Point(p.x, p.y) + POINT_PAD;
	}
	constexpr static geo::Point unpad(geo::Point p) noexcept
	{
		return geo::Point(p.x, p.y) - POINT_PAD;
	}

	using table = inx::bit_table<>;

	Grid();

	void setup(const std::vector<bool>& bits, int width, int height);

	uint32_t getWidth() const noexcept { return m_width; }
	uint32_t getHeight() const noexcept { return m_height; }

	// i = 0 for use with ABS_USE_HORI_VERT_GRID
	const auto& getTable(int i[[maybe_unused]] = 0) const noexcept
	{
#ifdef ABS_USE_HORI_VERT_GRID
	return m_table[i];
#else
	return m_table;
#endif
	}

private:
	uint32_t m_width, m_height;
#ifdef ABS_USE_HORI_VERT_GRID
	std::array<table, 2> m_table;
#else
	table m_table;
#endif
};

} // namespace asb::env

#endif // ASB_ENV_GRID_HPP
