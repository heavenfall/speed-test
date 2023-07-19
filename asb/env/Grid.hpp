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
