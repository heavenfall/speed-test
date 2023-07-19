#ifndef GMLIB_SLICE_ARRAY_HPP_INCLUDED
#define GMLIB_SLICE_ARRAY_HPP_INCLUDED

#include "inx.hpp"
#include "slice_factory.hpp"

namespace inx
{

namespace details {

// Tollerance is how many levels it will stay within, i.e. 0 will always reallocate is needed levels is less, 1 will keep with +-1 before reallocate
template <typename ValueType, size_t Tollerance>
class SliceArray
{
public:
	static_assert(Tollerance < 32, "Tollerance must be less than 32");
	using self = SliceArray<ValueType, Tollerance>;
	using value_type = ValueType;
	using size_type = size_t;
	using difference_type = std::ptrdiff_t;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using iterator = value_type*;
	using const_iterator = value_type*;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	static constexpr size_t slice_size() noexcept { return sizeof(ValueType); }

	struct Data {
		ValueType* front;
		ValueType* back;
		uint32 level;
		Data() noexcept : front(nullptr), back(nullptr), level(128)
		{ }
	};

	template <size_t SlabSize, size_t SlabMinLevel>
	void assign(size_type count, const value_type& value, details::SliceFactoryImpl<slice_size(), SlabSize, SlabMinLevel>& factory)
	{
		size_t level = (count & ~1ull) != 0 ? clz_index(count-1)+1 : 0;
		clear_();
		resize_(level, factory);
		assert(m_data.level < 32 && count <= (1u << m_data.level));
		m_data.back = m_data.front + count;
		std::uninitialized_fill(m_data.front, m_data.back, value);
	}
	template <typename It, size_t SlabSize, size_t SlabMinLevel>
	void assign(It it, It ite, details::SliceFactoryImpl<slice_size(), SlabSize, SlabMinLevel>& factory)
	{
		size_t count = std::distance(it, ite);
		size_t level = (count & ~1ull) != 0 ? clz_index(count-1)+1 : 0;
		clear_();
		resize_(level, factory);
		assert(m_data.level < 32 && count <= (1u << m_data.level));
		m_data.back = m_data.front + count;
		std::uninitialized_copy(it, ite, m_data.front);
	}
	// will never deallocate, assumes memory has been deallocated
	template <size_t SlabSize, size_t SlabMinLevel>
	void assign_init(size_type count, const value_type& value, details::SliceFactoryImpl<slice_size(), SlabSize, SlabMinLevel>& factory)
	{
		size_t level = (count & ~1ull) != 0 ? clz_index(count-1)+1 : 0;
		resize_init_(level, factory);
		assert(m_data.level < 32 && count <= (1u << m_data.level));
		m_data.back = m_data.front + count;
		std::uninitialized_fill(m_data.front, m_data.back, value);
	}
	template <typename It, size_t SlabSize, size_t SlabMinLevel>
	void assign_init(It it, It ite, details::SliceFactoryImpl<slice_size(), SlabSize, SlabMinLevel>& factory)
	{
		size_t count = std::distance(it, ite);
		size_t level = (count & ~1ull) != 0 ? clz_index(count-1)+1 : 0;
		resize_init_(level, factory);
		assert(m_data.level < 32 && count <= (1u << m_data.level));
		m_data.back = m_data.front + count;
		std::uninitialized_copy(it, ite, m_data.front);
	}

	template <size_t SlabSize, size_t SlabMinLevel>
	void clear(details::SliceFactoryImpl<slice_size(), SlabSize, SlabMinLevel>& factory)
	{
		if ((m_data.level & 128) != 0) {
			clear_();
			dealloc_(factory);
			m_data.front = m_data.back = nullptr;
			m_data.level = 128;
		}
	}
	void clear_init()
	{
		m_data.front = m_data.back = nullptr;
		m_data.level = 128;
	}

	bool empty() const noexcept
	{
		return m_data.front == m_data.back;
	}
	size_type size() const noexcept
	{
		return m_data.back - m_data.front;
	}

	reference at(size_t pos)
	{
		if (pointer p = m_data.front + pos; p >= m_data.back)
			throw std::out_of_range("pos");
		else
			return *p;
	}
	const_reference at(size_t pos) const
	{
		if (const_pointer p = m_data.front + pos; p >= m_data.back)
			throw std::out_of_range("pos");
		else
			return *p;
	}

	reference operator[](size_t pos) noexcept
	{
		assert(pos < size());
		return m_data.front[pos];
	}
	const_reference operator[](size_t pos) const noexcept
	{
		assert(pos < size());
		return m_data.front[pos];
	}


	reference front() noexcept
	{
		assert(!empty());
		return *m_data.front;
	}
	const_reference front() const noexcept
	{
		assert(!empty());
		return *m_data.front;
	}
	reference back() noexcept
	{
		assert(!empty());
		return *(m_data.back-1);
	}
	const_reference back() const noexcept
	{
		assert(!empty());
		return *(m_data.back-1);
	}

	pointer data() noexcept
	{
		return m_data.front;
	}
	const_pointer data() const noexcept
	{
		return m_data.front;
	}

	iterator begin() noexcept
	{
		return m_data.front;
	}
	const_iterator begin() const noexcept
	{
		return m_data.front;
	}
	iterator end() noexcept
	{
		return m_data.back;
	}
	const_iterator end() const noexcept
	{
		return m_data.back;
	}

protected:
	void clear_() noexcept
	{
		std::destroy(m_data.front, m_data.back);
	}
	template <size_t SlabSize, size_t SlabMinLevel>
	void resize_(size_t level, details::SliceFactoryImpl<slice_size(), SlabSize, SlabMinLevel>& factory)
	{
		if (static_cast<uint64>(static_cast<int64>(m_data.level) - static_cast<int64>(level)) > Tollerance) {
			if ((m_data.level & 128) == 0)
				dealloc_(factory);
			m_data.front = static_cast<value_type*>(factory.allocate(level));
			m_data.level = level;
		}
	}
	template <size_t SlabSize, size_t SlabMinLevel>
	void resize_init_(size_t level, details::SliceFactoryImpl<slice_size(), SlabSize, SlabMinLevel>& factory)
	{
		m_data.front = static_cast<value_type*>(factory.allocate(level));
		m_data.level = level;
	}
	template <size_t SlabSize, size_t SlabMinLevel>
	void dealloc_(details::SliceFactoryImpl<slice_size(), SlabSize, SlabMinLevel>& factory)
	{
		assert((m_data.level & 128) == 0);
		factory.deallocate(m_data.front, m_data.level);
	}

	Data m_data;
};

} // namespace details

template <typename ValueType, size_t Tollerance = 0>
using SliceArray = details::SliceArray<ValueType, Tollerance>;


} // namespace inx

#endif // GMLIB_SLICE_ARRAY_HPP_INCLUDED
