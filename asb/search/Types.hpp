#ifndef ASB_SEARCH_TYPES_HPP
#define ASB_SEARCH_TYPES_HPP

#include "fwd.hpp"
#include <geo/Point.hpp>
#include <array>

namespace asb::search {

enum class Dir : uint8_t
{
	North,
	South,
	East,
	West,
	NorthEast,
	NorthWest,
	SouthEast,
	SouthWest
};
constexpr geo::Point dir_point(Dir d) noexcept
{
	switch (d) {
	case Dir::North: return geo::Point(0, -1);
	case Dir::South: return geo::Point(0, 1);
	case Dir::East: return geo::Point(1, 0);
	case Dir::West: return geo::Point(-1, 0);
	case Dir::NorthEast: return geo::Point(1, -1);
	case Dir::NorthWest: return geo::Point(-1, -1);
	case Dir::SouthEast: return geo::Point(1, 1);
	case Dir::SouthWest: return geo::Point(-1, 1);
	default: return geo::Point::zero();
	}
}

struct SearchId
{
	uint32_t id;
};
struct NodeId
{
	uint32_t id;
};
struct NodeDesc : NodeId
{
	geo::Point p;
};

using dist_type = uint64_t;
using dist_s_type = int64_t;
// DO NOT CHANGE
constexpr int dist_log10 = 10;
constexpr dist_type dist_scale = 10'000'000'000ull;
constexpr dist_type dist_1 = dist_scale;
constexpr dist_type dist_r2 =    14'142'135'624ull; // 1.41421356237309504880168872420969807856967187537694807317667973799


struct BucketPush
{
	std::vector<NodeId> nodes;
	dist_type relative_f;
};
using BucketPushArray = std::array<BucketPush, 6>;

} // namespace asb::search

#endif // ASB_SEARCH_TYPES_HPP
