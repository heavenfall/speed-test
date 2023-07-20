#include "Search.hpp"
#ifndef NDEBUG
#include <unordered_set>
#endif


namespace asb::search {

Search::Search() : m_sid{0}, m_grid(nullptr)
{ }

void Search::setup(SearchGrid& l_grid)
{
	m_grid = &l_grid;
	m_queue.setup();
	m_expander.setup(l_grid);
}

void Search::search(geo::Point s, geo::Point t)
{
	// setting up search
	m_sid.id += 1;
	m_path.clear();
	if (s == t)
		return;
	NodeDesc start{0, env::Grid::pad(s)}; start.id = getNodeId(start.p);
	NodeDesc target{0, env::Grid::pad(t)}; target.id = getNodeId(target.p);
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
			foundTarget = m_expander.expandBucket(fvalue, element.bucket->getData(), element.bucket->size);
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
			m_path.push_back(env::Grid::unpad(node.p));
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
