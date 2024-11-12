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

#include "WarthogSearch.hpp"
#ifndef NDEBUG
#include <unordered_set>
#endif


namespace asb::search {

WarthogSearch::WarthogSearch() : m_sid{0}, m_grid(nullptr)
{ }

void WarthogSearch::setup(WarthogSearchGrid& l_grid)
{
	m_grid = &l_grid;
	m_queue.setup();
	m_expander.setup(l_grid);
}

void WarthogSearch::search(geo::Point s, geo::Point t)
{
	// setting up search
	m_sid.id += 1;
	m_path.clear();
	if (s == t)
		return;
	NodeDesc start{0, env::WarthogGrid::pad(s)}; start.id = getNodeId(start.p);
	NodeDesc target{0, env::WarthogGrid::pad(t)}; target.id = getNodeId(target.p);
	m_queue.setupSearch(m_sid);
	m_expander.setupSearch(m_sid, target);
	// node s
	bool foundTarget = false;
	dist_type fvalue = Node::octile(start.p - target.p);
	foundTarget = m_expander.expandStartNode(fvalue, start);
	if (!foundTarget) {
		auto& bucketLists = m_expander.getBucketLists();
		m_queue.mergeBucketArray(fvalue, bucketLists);
		m_queue.init_top();
		while (!m_queue.empty())
		{
			auto element = m_queue.top();
			fvalue = element.f;
			foundTarget = m_expander.expandNode(fvalue, element.bucket->getData(), element.bucket->size);
			if (foundTarget)
				break;
			m_queue.mergeBucketArray(fvalue, bucketLists);
			m_queue.free(*element.bucket);
		}
	}
	if (foundTarget) {
		// generate path
		NodeId point = target;
		Node* nodes = m_grid->getNodes().data();
#ifndef NDEBUG
		std::unordered_set<uint32_t> debugNoLoops;
#endif
		while (true) {
			Node& node = nodes[point.id];
			assert(node.sid.id == m_sid.id);
			m_path.push_back(env::WarthogGrid::unpad(node.p));
#ifndef NDEBUG
			assert(!debugNoLoops.contains(point.id));
			debugNoLoops.insert(point.id);
#endif
			if (node.predNode.id == point.id) { // start node
				assert(point.id == start.id);
				std::reverse(m_path.begin(), m_path.end());
				break;
			}
			point = node.predNode;
		}
	}
}

} // namespace asb::search
