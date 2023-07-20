#ifndef ASB_SEARCH_BUCKETQUEUE_HPP
#define ASB_SEARCH_BUCKETQUEUE_HPP

#include "fwd.hpp"
#include "Types.hpp"
#include <inx/slice_array.hpp>
#include <inx/slice_factory.hpp>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <memory_resource>

namespace asb::search {

struct NodeBucket
{
	static constexpr uint32_t static_size = 4;
	SearchId sid;
	uint32_t size;
#ifndef NDEBUG
	dist_type d_fvalue;
#endif
	union {
		std::array<NodeId, static_size> static_data;
		inx::SliceArray<NodeId> data;
	};

	NodeBucket() : sid{}, size{}, static_data{}
	{ }

	NodeId* getData() noexcept { return size <= static_size ? static_data.data() : data.data(); }
	const NodeId* getData() const noexcept { return size <= static_size ? static_data.data() : data.data(); }
};

class BucketQueue
{
public:
	struct QueueValue
	{
		dist_type f;
		NodeBucket* bucket;
		bool operator==(const QueueValue& r) const noexcept { return f == r.f; }
		bool operator!=(const QueueValue& r) const noexcept { return f != r.f; }
		bool operator<(const QueueValue& r) const noexcept { return f > r.f; }
		bool operator>(const QueueValue& r) const noexcept { return r.f > f; }
	};

	BucketQueue();

	void setup();
	void setupSearch(SearchId sid);

	const auto& base() const noexcept { return m_queue; }
	auto& base() noexcept { return m_queue; }

	bool empty() const noexcept { return m_queue.empty(); }
	QueueValue top() const noexcept { assert(!m_queue.empty()); return m_queue.front(); }
	void init_top() noexcept { }

	void heapify()
	{
		std::make_heap(m_queue.begin(), m_queue.end());
	}

	void push(dist_type f, NodeBucket& v)
	{
		m_queue.push_back({f, &v});
		std::push_heap(m_queue.begin(), m_queue.end());
	}

	void pop()
	{
		assert(!m_queue.empty());
		std::pop_heap(m_queue.begin(), m_queue.end());
		m_queue.pop_back();
	}

	void clear()
	{
		m_queue.clear();
	}
	void free(NodeBucket& node);

	NodeBucket& getBucketByF(dist_type f)
	{
	#ifndef NDEBUG
		if (!m_buckets.contains(f)) {
			m_buckets[f].d_fvalue = f;
		}
	#endif
		return m_buckets[f];
	}

	void mergeBucket(dist_type fvalue, BucketPush& bucketPush);
	void mergeBucketArray(dist_type fvalue, BucketPushArray& bucketPush);

private:
	static constexpr size_t slab_size = 512 * 1024 + 64;
	SearchId m_sid;
	std::pmr::unsynchronized_pool_resource m_bucketsRes;
	inx::SliceFactory<NodeId, slab_size, 4> m_slices;
	std::vector<QueueValue> m_queue;
	std::pmr::unordered_map<dist_type, NodeBucket> m_buckets;
};

} // namespace asb::search

#endif // ASB_SEARCH_BUCKETQUEUE_HPP
