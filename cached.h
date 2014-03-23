#ifndef KOKU_CACHED
#define KOKU_CACHED

#pragma GCC push_options
#pragma GCC optimize (3)

#include <atomic>
namespace koku
{
	constexpr size_t cached_max = 10; //how many values should be cached
	static_assert(cached_max <= sizeof(unsigned long)*8, "cached_max is too big !");
	template<typename T>
	class cached
	{
		/*
			When you have less than "cached_max" objects
			at the same time in the same scope.
			It reduces the new/delete a lot by caching the objects.

			Example:
				void scopeA()
				{
					cached< vector<int> > my_vector;
					my_vector.data.push_back(123);
				}
				void scopeB()
				{
					cached< vector<int> > my_vector;
					my_vector.data.push_back(456);
				}

				int main()
				{
					scopeA();
					scopeB();
					cached< vector<int> > my_vector;

					// my_vector will now include {123, 456} in it's array.
					// it reused the old memory !
					// therefor the right choise for this usage
					// would be to my_vector.data.clear() before each usage

					my_vector.clear(); //remove valued, but still uses allocated memory from before !

					//actually there is no Warranty that the result will be {123, 456}
					//the compiler might reoder the functions and or
					//reoder the constructor/deconstructor calls of cached<?>
				}
		*/
		private:
			static T m_cached_yes[cached_max];					  //array of cached types
			static std::atomic<unsigned long> m_cached_yes_bitmap; //bitmap of used caches

			unsigned long m_cached_yes_bitmap_mask; //actual used cached_yes[x]
			T *m_cached_no;						  //fallback, not prealocated

			T *m_data; //ptr to cached_yes or cached_no

			void init()
			{
				unsigned long bitmap = m_cached_yes_bitmap.load(std::memory_order_relaxed); //read without lock
				//search a free cached T
				constexpr unsigned long long bitmap_full = ~(std::numeric_limits<unsigned long>::max() << cached_max);
				while(bitmap != bitmap_full) //Does: (4Bit example with cached_max = 2) 1111 << 2 -> ~1100 -> 0011
				{
					int index = __builtin_ffs(~bitmap) - 1;
					unsigned long mask = 1ul << index;
					//try to get it
					unsigned long bitmap = m_cached_yes_bitmap.fetch_or(mask);
					//check if it's free again, because threads !
					if (!(bitmap&mask))
					{
						m_cached_yes_bitmap_mask = mask;
						m_data = m_cached_yes + index;
						break;
					}
				}

				if (m_data == nullptr)
				{
					//create new memory
					m_cached_no = new T;
					m_data = m_cached_no;
				}
			}

		public:
			cached():
				m_cached_yes_bitmap_mask(0),
				m_cached_no(nullptr),
				m_data(nullptr)
			{
				init();
			}

			~cached()
			{
				//remove cached T
				if (m_cached_yes_bitmap_mask != 0)
				{
					m_cached_yes_bitmap.fetch_and(~m_cached_yes_bitmap_mask);
					/*
					m_cached_yes_bitmap_mask = 0;
					*/
				}
				else
				{
					delete m_cached_no;
					/*
					m_cached_no = nullptr;
					m_data = nullptr;
					*/
				}
			}

			cached(const cached<T>& other):
				m_cached_yes_bitmap_mask(0),
				m_cached_no(nullptr),
				m_data(nullptr)
			{
				init();
				//only copy data
				*m_data = *other.m_data;
			}

			void operator=(const cached<T>& other)
			{
				//only copy data
				*m_data = *other.m_data;
			}

			T* operator->()
			{
				return m_data;
			}

			const T* operator->() const
			{
				return m_data;
			}

			T& raw()
			{
				return *m_data;
			}

			const T& raw() const
			{
				return *m_data;
			}
	};

	template<typename T> T cached<T>::m_cached_yes[cached_max];
	template<typename T> std::atomic<unsigned long> cached<T>::m_cached_yes_bitmap;
}

#endif
