#include "RedBlackQueue.hpp"
#include "BucketExpander.hpp"
#include <memory>
#include <algorithm>

namespace asb::search {

RedBlackQueue::RedBlackQueue() : m_sid{0}, m_front(nullptr)
{ }

void RedBlackQueue::setup()
{ }

void RedBlackQueue::setupSearch(SearchId sid)
{
	m_sid = sid;
	m_tree.clear();
	m_slices.reset();
	m_bucketFactory.reset();
	m_front = nullptr;
}

void RedBlackQueue::mergeBucket(dist_type fvalue, BucketPush& bucketPush)
{
	auto bucketIt = m_tree.partition_point([fvalue](const RedBlackBucket& bucket) noexcept { return bucket.fvalue < fvalue; });
	if (bucketIt.node() == nullptr || bucketIt->fvalue != fvalue) {
		// node does not exist, create it
		bucketIt = m_tree.insert(bucketIt, *m_bucketFactory.construct());
		bucketIt->size = 0;
		bucketIt->fvalue = fvalue;
	}
	assert(bucketIt.node() != nullptr && bucketIt->fvalue == fvalue);
	RedBlackBucket& bucket = *bucketIt;
	uint32_t size_required = bucketPush.nodes.size();
	uint32_t new_size;
	new_size = bucket.size + size_required;
	if (new_size <= NodeBucket::static_size) { // copy data into static buffer
		std::uninitialized_copy_n(bucketPush.nodes.data(), size_required, bucket.static_data.data() + bucket.size);
	} else if (bucket.size == 0) {
		bucket.data.assign_init(bucketPush.nodes.begin(), bucketPush.nodes.end(), m_slices);
	} else { // data already allocated, resize if necessary and copy to array
		// resize array
		if (bucket.size <= NodeBucket::static_size) { // using static buffer, upgrade to dynamic buffer
			auto bucket_backup = bucket.static_data;
			bucket.data.assign_init(new_size, NodeId{}, m_slices);
			std::copy_n(bucket_backup.data(), bucket.size, bucket.data.data());
		} else if (new_size > bucket.data.size()) { // dynamic buffer too small, enlarge
			inx::SliceArray<NodeId> newArray;
			newArray.assign_init(new_size, NodeId{}, m_slices);
			std::copy_n(bucket.data.data(), bucket.size, newArray.data());
			bucket.data.clear(m_slices);
			bucket.data = newArray;
		}
		// append data to array
		std::copy_n(bucketPush.nodes.data(), size_required, bucket.data.data() + bucket.size);
	}
	bucketPush.nodes.clear();
	bucket.size = new_size;
}
void RedBlackQueue::mergeBucketArray(dist_type fvalue, BucketPushArray& bucketPush)
{
	assert(bucketPush[0].nodes.empty());

	for (uint16_t i = BucketExpander::BALANCED_ORDER; i != 0; i >>= 3) {
		auto& bucket = bucketPush[i & 0b111];
		if (!bucket.nodes.empty())
			mergeBucket(fvalue + bucket.relative_f, bucket);
	}
}

void RedBlackQueue::free(RedBlackBucket& node)
{
	assert(m_front == &node);
	if (node.size > NodeBucket::static_size) {
		node.data.clear(m_slices);
	}
	m_front = static_cast<RedBlackBucket*>(node.find_inorder_id(0));
	m_tree.erase(node);
	m_bucketFactory.destruct(&node);
}

} // namespace asb::search
