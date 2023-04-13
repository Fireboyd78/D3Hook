#pragma once
#include <new>

namespace fuz
{
	struct fixed_list_node_pointers;

	struct fixed_list_node_pointers
	{
		fixed_list_node_pointers *prev;
		fixed_list_node_pointers *next;
	};

	template <class _Class, class _NodePointers = fixed_list_node_pointers>
	struct fixed_node {};

	template <class _Class>
	struct fixed_node<_Class, fixed_list_node_pointers> : public fixed_list_node_pointers
	{
		_Class value;
	};

	template <class _Class, size_t _Count, class _NodePointers = fixed_list_node_pointers>
	struct fixed_node_allocator {};

	template <class _Class, size_t _Count>
	struct fixed_node_allocator<_Class, _Count, fixed_list_node_pointers>
	{
		typedef fixed_node_allocator<_Class, _Count, fixed_list_node_pointers> type;

		typedef fixed_list_node_pointers Ptr_t;
		typedef fixed_node<_Class, fixed_list_node_pointers> Data_t;
		typedef _Class Value_t;

		Data_t m_array[_Count];
		Ptr_t *m_next_free;
		size_t m_num_freed;
		size_t m_size;

		Data_t * storage_ptr() const
		{
			auto *ptr = &m_array[0];

			return const_cast<Data_t *>(ptr);
		}

		void set_storage_ptr(Ptr_t *next_free)
		{
			m_next_free = next_free;
		}

		void init()
		{
			m_num_freed = 0;
			m_size = 0;
			m_next_free = storage_ptr();
		}

		template <size_t _OtherCount>
		bool set_linked_allocator(fixed_node_allocator<_Class, _OtherCount> &allocator, fixed_list_node_pointers *end)
		{
			if (_Count > _OtherCount)
				return false;

			// currently, we need to hook when the buffer is completely empty
			if (allocator.m_size != 0 || m_size != 0)
				return false;

			fixed_list_node_pointers *prev = end;
			Data_t *head = allocator.storage_ptr();

			for (int i = 0; i < _OtherCount; i++)
			{
				if (prev != end)
					prev->next = head;

				head->prev = prev;
				head->next = end;

				prev = head;
				head++;

				m_num_freed++;
			}

			// once all free space is used, this will point to the original buffer;
			// ** the only way this may need to be reinitialized is if the buffer is cleared! **
			m_next_free = prev;

			return true;
		}
	};

	template <class _Allocator>
	struct fixed_list_const_iterator
	{
		typedef typename _Allocator::Ptr_t Ptr_t;
		typedef typename _Allocator::Data_t Data_t;
		typedef typename _Allocator::Value_t Value_t;

		union union_debug
		{
			Ptr_t *ptr;
			Data_t *data;
		} debug;

		fixed_list_const_iterator() {}
		fixed_list_const_iterator(const fixed_list_const_iterator &other)
		{
			debug = other.debug;
		}

		fixed_list_const_iterator & operator=(const fixed_list_const_iterator &other)
		{
			debug = other.debug;
			return *this;
		}

		Value_t & operator*()	const { return debug.data->value; }
		Value_t * operator->()	const { return &debug.data->value; }

		template <class TIterator>
		bool equal(const TIterator &other) const
		{
			return debug.data == other.debug.data;
		}

		template <class TIterator>
		bool operator==(const TIterator &other) const { return equal(other); }
		template <class TIterator>
		bool operator!=(const TIterator &other) const { return !equal(other); }

		fixed_list_const_iterator & operator--()	{ --debug.data; return *this; }
		fixed_list_const_iterator & operator++()	{ ++debug.data; return *this; }
		fixed_list_const_iterator operator--(int)
		{
			fixed_list_const_iterator iter = *this;
			debug.data--; return iter;
		}
		fixed_list_const_iterator operator++(int)
		{
			fixed_list_const_iterator iter = *this;
			debug.data++; return iter;
		}
	};

	template <class _Allocator>
	struct fixed_list_iterator
	{
		typedef typename _Allocator::Ptr_t Ptr_t;
		typedef typename _Allocator::Data_t Data_t;
		typedef typename _Allocator::Value_t Value_t;

		union union_debug
		{
			Ptr_t *ptr;
			Data_t *data;
		} debug;

		fixed_list_iterator() {}
		fixed_list_iterator(const fixed_list_iterator &other)
		{
			debug = other.debug;
		}

		fixed_list_iterator & operator=(const fixed_list_iterator &other)
		{
			debug = other.debug;
			return *this;
		}

		Value_t & operator*()	const { return debug.data->value; }
		Value_t * operator->()	const { return &debug.data->value; }

		template <class TIterator>
		inline bool equal(const TIterator &other) const
		{
			return debug.data == other.debug.data;
		}

		template <class TIterator>
		bool operator==(const TIterator &other) const { return equal(other); }
		template <class TIterator>
		bool operator!=(const TIterator &other) const { return !equal(other); }

		fixed_list_iterator & operator--()			{ --debug.data; return *this; }
		fixed_list_iterator & operator++()			{ ++debug.data; return *this; }
		fixed_list_iterator operator--(int)
		{
			fixed_list_iterator iter = *this; 
			debug.data--; return iter;
		}
		fixed_list_iterator operator++(int)
		{
			fixed_list_iterator iter = *this;
			debug.data++; return iter;
		}

		operator fixed_list_const_iterator<_Allocator>() const
		{
			fixed_list_const_iterator<_Allocator> x;
			x.debug.data = debug.data;
			return x;
		}
	};

	template <class _Iterator>
	typename _Iterator::Ptr_t ** node_ptr(_Iterator *iter)
	{
		return &iter->debug.ptr;
	}

	template <class _Iterator>
	typename _Iterator::Ptr_t ** node_ptr(const _Iterator &iter)
	{
		return &iter.debug.ptr;
	}

	template <class _Class, size_t _Count, class _NodePointers = fixed_list_node_pointers>
	struct fixed_list {};

	template <class _Class, size_t _Count>
	struct fixed_list<_Class, _Count, fixed_list_node_pointers>
	{
		fixed_node_allocator<_Class, _Count> m_allocator;
		fixed_list_node_pointers m_end;

		typedef fixed_node_allocator<_Class, _Count> TAllocator;

		using iterator = fixed_list_iterator<TAllocator>;
		using const_iterator = fixed_list_const_iterator<TAllocator>;

		void init()
		{
			m_allocator.init();
			m_end.next = &m_end;
			m_end.prev = &m_end;
		}

		size_t size() const
		{
			return _Count;
		}

		// testing

		iterator		begin()
		{ iterator x; x.debug.ptr = reinterpret_cast<fixed_list_node_pointers *>(m_allocator.storage_ptr()); return x; }
		const_iterator	begin() const	{ return static_cast<const_iterator>(begin()); }
		iterator		end()
		{ iterator x; x.debug.ptr = &m_end; return x; }
		const_iterator	end() const		{ return static_cast<const_iterator>(end()); }

		template <size_t _OtherCount>
		bool set_linked_list(fixed_list<_Class, _OtherCount> &list)
		{
			return m_allocator.set_linked_allocator(list.m_allocator, &m_end);
		}
	};

	// boost::array ?
	template<class _ElementType, size_t _Count>
	struct array
	{
		_ElementType elems[_Count];

		_ElementType * begin()	{ return &elems[0]; }
		_ElementType * end()	{ return &elems[_Count]; }

		size_t size() const
		{
			return _Count;
		}
	};
}