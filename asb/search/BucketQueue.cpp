#include "BucketQueue.hpp"
#include <memory>
#include <algorithm>

namespace asb::search {

BucketQueue::BucketQueue() : m_sid{0}, m_buckets(4096, &m_bucketsRes)
{
	m_buckets.max_load_factor(0.75);
}

void BucketQueue::setup()
{
	m_queue.reserve(1024);
}

void BucketQueue::setupSearch(SearchId sid)
{
	m_sid = sid;
	m_queue.clear();
	m_slices.reset();
}

void BucketQueue::mergeBucket(dist_type fvalue, BucketPush& bucketPush)
{
	NodeBucket& bucket = getBucketByF(fvalue);
	uint32_t size_required = bucketPush.nodes.size();
	uint32_t new_size;
	if (bucket.sid.id != m_sid.id) {
		bucket.sid = m_sid;
		bucket.size = 0;
		push(fvalue, bucket);
	}
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
void BucketQueue::mergeBucketArray(dist_type fvalue, BucketPushArray& bucketPush)
{
	assert(bucketPush[0].nodes.empty());

	for (int i = 1; i < std::tuple_size_v<BucketPushArray>; ++i) {
		auto& bucket = bucketPush[i];
		if (!bucket.nodes.empty())
			mergeBucket(fvalue + bucket.relative_f, bucket);
	}
}

} // namespace asb::search
