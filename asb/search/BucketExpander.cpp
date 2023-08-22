#include "BucketExpander.hpp"

namespace asb::search {

void BucketExpander::setup(SearchGrid& grid)
{
	for (auto& x : m_bucketLists) {
		x.nodes.reserve(64);
	}
#ifndef ASB_ENABLE_EXPANDER_DFS
	m_bucketLoop.nodes.reserve(64);
#endif
	m_bucketLists[0].relative_f = 0; //                                 0 ( 0,  0) 00000000000000000000000000000000000
	m_bucketLists[1].relative_f = 2 * dist_1 - dist_r2;     // 5857864376 ( 2, -1) 00101011101001001111110101010111000
	assert(m_bucketLists[1].relative_f == 5857864376ull);
	m_bucketLists[2].relative_f = 2 * dist_1;              // 20000000000 ( 2,  0) 10010101000000101111100100000000000  out of ascending order to facilitate fast hashing
	assert(m_bucketLists[2].relative_f == 20000000000ull);
	m_bucketLists[3].relative_f = 2 * dist_r2 - 2 * dist_1; // 8284271248 (-2,  2) 00111101101110001111111001010010000
	assert(m_bucketLists[3].relative_f == 8284271248ull);
	m_bucketLists[4].relative_f = dist_r2;                 // 14142135624 ( 0,  1) 01101001010111011111101110101001000
	assert(m_bucketLists[4].relative_f == 14142135624ull);
	m_bucketLists[5].relative_f = 2 * dist_r2;             // 28284271248 ( 0,  2) 11010010101110111111011101010010000
	assert(m_bucketLists[5].relative_f == 28284271248ull);
	m_grid = &grid;
}

void BucketExpander::setupSearch(SearchId sid, NodeDesc target)
{
	m_sid = sid;
	m_target = target;
	for (auto& bck : m_bucketLists) {
		bck.nodes.clear();
	}
}

bool BucketExpander::expandStartNode(dist_type fvalue, NodeId s)
{
	assert(s.id < m_grid->getNodes().size());
	assert(std::all_of(m_bucketLists.begin(), m_bucketLists.end(), [](const auto& bck) { return bck.nodes.empty(); }));
	Node& snode = m_grid->getNodes()[s.id];
	snode.sid = m_sid;
	snode.g = 0;
	assert(fvalue == Node::octile(snode.p - m_target.p));
	snode.close();
	snode.predNode = s;
	// copy to stack
	struct Expand {
		const SearchId sid;
		const dist_type fvalue;
		BucketPush*const store_pushes;
		Node*const grid;
		const NodeDesc target;
		const int32_t row;
		const NodeId nodeid;

		bool try_push(NodeId succ_nodeid, dist_type g, Dir d) {
			Node& node = grid[succ_nodeid.id];
			node.sid = sid;
			node.predNode = nodeid;
			node.g = g;
			node.f = g + Node::octile(node.p - target.p);
#ifndef NDEBUG
			dist_type rel_f = node.f - fvalue;
			int store_id = f_hash(rel_f); 
			assert(store_pushes[store_id].relative_f == rel_f);
#endif
			store_pushes[f_hash(node.f - fvalue)].nodes.push_back(NodeId{(succ_nodeid.id << 3) | static_cast<uint8_t>(d)});
			return succ_nodeid.id == target.id;
		}
	} expand{m_sid, fvalue, m_bucketLists.data(), m_grid->getNodes().data(), m_target, static_cast<int32_t>(m_grid->getWidth()), s};

	// setup node for expansion
	uint8_t node3x3;
	bool target_found = false;
	{
		uint32_t node3x3tmp = static_cast<uint32_t>(m_grid->getTable().region<3,3>(env::Grid::table::index_t(expand.nodeid.id - expand.row - 1)));
		node3x3 = static_cast<uint8_t>(~((node3x3tmp & 0b1111) | ((node3x3tmp >> 1) & 0b11110000)));
	}

	// try expanding node
	// 0b76543210
	// ====
	// 012
	// 3-4
	// 567
	
	// north
	if ( (node3x3 & 0b00000010) == 0 )
		target_found |= expand.try_push(NodeId{expand.nodeid.id - expand.row}, dist_1, Dir::North);
	// south
	if ( (node3x3 & 0b01000000) == 0 )
		target_found |= expand.try_push(NodeId{expand.nodeid.id + expand.row}, dist_1, Dir::South);
	// east
	if ( (node3x3 & 0b00010000) == 0 )
		target_found |= expand.try_push(NodeId{expand.nodeid.id + 1}, dist_1, Dir::East);
	// west
	if ( (node3x3 & 0b00001000) == 0 )
		target_found |= expand.try_push(NodeId{expand.nodeid.id - 1}, dist_1, Dir::West);
	// north east
	if ( (node3x3 & 0b00010110) == 0 )
		target_found |= expand.try_push(NodeId{expand.nodeid.id - expand.row + 1}, dist_r2, Dir::NorthEast);
	// north west
	if ( (node3x3 & 0b00001011) == 0 )
		target_found |= expand.try_push(NodeId{expand.nodeid.id - expand.row - 1}, dist_r2, Dir::NorthWest);
	// south east
	if ( (node3x3 & 0b11010000) == 0 )
		target_found |= expand.try_push(NodeId{expand.nodeid.id + expand.row + 1}, dist_r2, Dir::SouthEast);
	// south west
	if ( (node3x3 & 0b01101000) == 0 )
		target_found |= expand.try_push(NodeId{expand.nodeid.id + expand.row - 1}, dist_r2, Dir::SouthWest);
	
	if (target_found)
		return true;

	// expand rel 0 bucket if non-empty
	if (!m_bucketLists[0].nodes.empty()) {
#ifndef ASB_ENABLE_EXPANDER_DFS
		m_bucketLoop.nodes.clear();
		std::swap(m_bucketLists[0].nodes, m_bucketLoop.nodes);
		return expandBucket(fvalue, m_bucketLoop.nodes.data(), m_bucketLoop.nodes.size());
#else
		return expandBucket(fvalue);
#endif
	}

	return false;
}

bool BucketExpander::expandNode(dist_type fvalue, const NodeId* bucket, int size)
{
#ifndef ASB_ENABLE_EXPANDER_DFS
	return expandBucket(fvalue, bucket, size);
#else
	m_bucketLists[0].nodes.assign(bucket, bucket + size);
	return expandBucket(fvalue);
#endif
}

#ifndef ASB_ENABLE_EXPANDER_DFS
bool BucketExpander::expandBucket(dist_type fvalue, const NodeId* bucket, int size)
#else
bool BucketExpander::expandBucket(dist_type fvalue)
#endif
{
#ifndef ASB_ENABLE_EXPANDER_DFS
	assert(bucket != 0);
	assert(size > 0);
#endif

	// based off of Dir index
	// 0b76543210
	// ====
	// 012
	// 3-4
	// 567
	//                                        SW        SE       NW       NE       W        E        S        N
	constexpr uint64_t dir_blocked_mask = 0b00010110'00001011'11010000'01101000'10010100'00101001'00000111'11100000ull;
	// this is masks that state successors from (pred-u) can't be on the shortest paths, so do not even consider them
	// (uint8_t)(dir_blocked_mask >> 8*dir) gives the correct mask
	
	// copy to stack
	struct Expand {
		const SearchId sid;
		const dist_type fvalue;
		BucketPush*const store_pushes;
		Node*const grid;
		const NodeDesc target;
		const int32_t row;
		NodeId nodeid;
#ifndef NDEBUG
		Node* d_node;
#endif

		bool try_push(NodeId succ_nodeid, dist_type g, Dir d) {
			assert(succ_nodeid.id != nodeid.id);
			Node& node = grid[succ_nodeid.id];
			assert(d_node->p + dir_point(d) == node.p);
			if (node.can_push(sid, g)) [[likely]] { // successor
				assert(!node.closed());
				node.predNode = nodeid;
				node.f = g + Node::octile(node.p - target.p);
#ifndef NDEBUG
				dist_type rel_f = node.f - fvalue;
				int store_id = f_hash(rel_f); 
				assert(store_id < 6 && store_pushes[store_id].relative_f == rel_f);
#endif
				store_pushes[f_hash(node.f - fvalue)].nodes.push_back(NodeId{(succ_nodeid.id << 3) | static_cast<uint8_t>(d)});
			}
			return succ_nodeid.id == target.id;
		}
	} expand{m_sid, fvalue, m_bucketLists.data(), m_grid->getNodes().data(), m_target, static_cast<int32_t>(m_grid->getWidth()), NodeId{0}
#ifndef NDEBUG
		,nullptr
#endif
	};
	auto& table = m_grid->getTable();

#ifndef ASB_ENABLE_EXPANDER_DFS
	const NodeId* bucket_at = bucket + (size-1);
#else
	BucketPush& bucket_list = expand.store_pushes[0];
#endif

#ifndef ASB_ENABLE_EXPANDER_DFS
	while (true) {
		// a do-while
		// will loop over original bucket
		// then if m_bucketLists[0] is not empty (0 f-value), will loop over that bucket
		while (true) {
			// setup node for expansion
			expand.nodeid = *bucket_at;
#else
	do {
			assert(!bucket_list.nodes.empty());
		// a do-while !bucket_list.empty()
			// setup node for expansion
			expand.nodeid = bucket_list.nodes.back();
			bucket_list.nodes.pop_back();
#endif
			assert((expand.nodeid.id >> 3) < m_grid->getNodes().size());
			Node& expnode = expand.grid[expand.nodeid.id >> 3];
#ifndef NDEBUG
			expand.d_node = &expnode;
#endif
			assert(expnode.sid.id == m_sid.id);
			if (!expnode.closed()) {
				assert(expnode.f == expand.fvalue);
				expnode.close();
				uint8_t node3x3;
				bool target_found = false;
				{
					NodeId expid{expand.nodeid.id >> 3};
					uint32_t node3x3tmp = static_cast<uint32_t>(table.region<3,3>(env::Grid::table::index_t(expid.id - expand.row - 1)));
					node3x3 = static_cast<uint8_t>(~((node3x3tmp & 0b1111) | ((node3x3tmp >> 1) & 0b11110000))) | static_cast<uint8_t>(dir_blocked_mask >> ((expand.nodeid.id & 0b111) << 3));
					expand.nodeid = expid;
				}
				dist_type gvalue = expnode.g;

				// try expanding node
				// 0b76543210
				// ====
				// 012
				// 3-4
				// 567
				
				// north
				if ( (node3x3 & 0b00000010) == 0 )
					target_found |= expand.try_push(NodeId{expand.nodeid.id - expand.row}, gvalue + dist_1, Dir::North);
				// south
				if ( (node3x3 & 0b01000000) == 0 )
					target_found |= expand.try_push(NodeId{expand.nodeid.id + expand.row}, gvalue + dist_1, Dir::South);
				// east
				if ( (node3x3 & 0b00010000) == 0 )
					target_found |= expand.try_push(NodeId{expand.nodeid.id + 1}, gvalue + dist_1, Dir::East);
				// west
				if ( (node3x3 & 0b00001000) == 0 )
					target_found |= expand.try_push(NodeId{expand.nodeid.id - 1}, gvalue + dist_1, Dir::West);
				// north east
				if ( (node3x3 & 0b00010110) == 0 )
					target_found |= expand.try_push(NodeId{expand.nodeid.id - expand.row + 1}, gvalue + dist_r2, Dir::NorthEast);
				// north west
				if ( (node3x3 & 0b00001011) == 0 )
					target_found |= expand.try_push(NodeId{expand.nodeid.id - expand.row - 1}, gvalue + dist_r2, Dir::NorthWest);
				// south east
				if ( (node3x3 & 0b11010000) == 0 )
					target_found |= expand.try_push(NodeId{expand.nodeid.id + expand.row + 1}, gvalue + dist_r2, Dir::SouthEast);
				// south west
				if ( (node3x3 & 0b01101000) == 0 )
					target_found |= expand.try_push(NodeId{expand.nodeid.id + expand.row - 1}, gvalue + dist_r2, Dir::SouthWest);
				
				if (target_found)
					return true;
			}

#ifndef ASB_ENABLE_EXPANDER_DFS
			// end condition
			if (bucket_at == bucket)
				break;
			--bucket_at;
		}

		if (m_bucketLists[0].nodes.empty())
			break;
		// loop rel-f bucket 0, is next pop anyway
		m_bucketLoop.nodes.clear();
		std::swap(m_bucketLists[0].nodes, m_bucketLoop.nodes);
		bucket = m_bucketLoop.nodes.data();
		bucket_at = &m_bucketLoop.nodes.back();
#ifndef NDEBUG
		size = m_bucketLoop.nodes.size();
#endif
	}
#else
	} while (!bucket_list.nodes.empty());
#endif

	return false;
}

} // namespace asb::search
