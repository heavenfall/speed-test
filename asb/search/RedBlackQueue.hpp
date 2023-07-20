#ifndef ASB_SEARCH_REDBLACKQUEUE_HPP
#define ASB_SEARCH_REDBLACKQUEUE_HPP

#include "fwd.hpp"
#include "BucketQueue.hpp"
#include <inx/redblack_tree.hpp>
#include <inx/factory.hpp>

namespace asb::search {

struct RedBlackBucket : NodeBucket, inx::redblack_tree_tag<>
{
	dist_type fvalue;
	RedBlackBucket() : fvalue(0)
	{ }
};

class RedBlackQueue
{
public:
	using tree_type = inx::redblack_tree<RedBlackBucket>;
	struct QueueValue
	{
		dist_type f;
		RedBlackBucket* bucket;
	};

	RedBlackQueue();

	void setup();
	void setupSearch(SearchId sid);

	bool empty() const noexcept { return m_tree.empty(); }
	QueueValue top() noexcept
	{
		assert(!m_tree.empty());
		return {m_front->fvalue, m_front};
	}
	void init_top() noexcept
	{
		assert(!m_tree.empty());
		m_front = &m_tree.front();
	}

	void clear()
	{
		m_tree.clear();
	}
	void free(RedBlackBucket& node);

	void mergeBucket(dist_type fvalue, BucketPush& bucketPush);
	void mergeBucketArray(dist_type fvalue, BucketPushArray& bucketPush);

private:
	static constexpr size_t slab_size = 512 * 1024 + 64;
	SearchId m_sid;
	inx::ReclaimFactory<RedBlackBucket, 1024> m_bucketFactory;
	inx::SliceFactory<NodeId, slab_size> m_slices;
	tree_type m_tree;
	RedBlackBucket* m_front;
};

} // namespace asb::search

#endif // ASB_SEARCH_REDBLACKQUEUE_HPP
