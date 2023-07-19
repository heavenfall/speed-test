#ifndef ASB_SEARCH_NODE_HPP
#define ASB_SEARCH_NODE_HPP

#include "fwd.hpp"
#include <geo/Point.hpp>
#include "Types.hpp"

namespace asb::search {

struct Node
{
	NodeId id;
	geo::Point p;
	SearchId sid;
	NodeId predNode;
	dist_type g;
	dist_type f;

	Node() noexcept : sid{}, predNode{}, g{}, f{}
	{ }

	bool can_push(SearchId l_sid, dist_type new_g) noexcept
	{
		if (sid.id != l_sid.id || new_g <  g) {
			sid = l_sid;
			g = new_g;
#ifndef NDEBUG
			f = 1; // ensure opend by default for assert checking
#endif
			return true;
		}
		return false;
	}

	void close() noexcept
	{
		f = 0;
	}
	bool closed() const noexcept
	{
		return f == 0;
	}

	static constexpr dist_type octile(geo::Point uv) noexcept
	{
#if 0
		union {
			uint32_t word;
			struct {
				int16_t x, y;
			} p;
		} x;
		x.p.x = std::abs(uv.x); x.p.y = std::abs(uv.y);
		x.word = x.p.x > x.p.y ? std::rotl(x.word, 16) : x.word;
		return x.p.x * dist_r2 + (x.p.y - x.p.x) * dist_1;
#else
		uv.x = std::abs(uv.x); uv.y = std::abs(uv.y);
		if (uv.x > uv.y) std::swap(uv.x, uv.y);
		assert(uv.x <= uv.y);
		return static_cast<dist_type>(uv.x) * dist_r2 + static_cast<dist_type>(uv.y - uv.x) * dist_1;
#endif
	}
};

} // namespace asb::search

#endif // ASB_SEARCH_NODE_HPP
