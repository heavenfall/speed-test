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

#ifndef ASB_SEARCH_NODE_HPP
#define ASB_SEARCH_NODE_HPP

#include "fwd.hpp"
#include <geo/Point.hpp>
#include "Types.hpp"
#include <bit>

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
#if 1
		union {
			uint32_t word;
			struct {
				int16_t x, y;
			} p;
		} q;
		q.p.x = std::abs(uv.x); q.p.y = std::abs(uv.y);
		q.word = q.p.x > q.p.y ? std::rotl(q.word, 16) : q.word;
		return q.p.x * dist_r2 + (q.p.y - q.p.x) * dist_1;
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
