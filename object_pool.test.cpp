#include"object_pool.h"
#include<godefv/error_checking/check_equal/fundamental_types.h>

int main(){
	/** Default object pool. */
	{
		godefv::object_pool_t<int> myObjectPool;

		check_equal(myObjectPool.size(), size_t(0)).on_error(godefv::return_status_t::exit{});

		{
			auto myValue = myObjectPool.make_unique();
			check_equal(myObjectPool.size(), size_t(1)).on_error(godefv::return_status_t::exit{});
		}

		// Pointer was deleted and thus memory freed in the pool -> size decreases.
		check_equal(myObjectPool.size(), size_t(0)).on_error(godefv::return_status_t::exit{});
	}

	/** Non-default object pool. */
	{
		godefv::object_pool_t<int, std::allocator, 10> myObjectPool;

		check_equal(myObjectPool.size(), size_t(0)).on_error(godefv::return_status_t::exit{});

		{
			auto myValue = myObjectPool.make_unique();
			check_equal(myObjectPool.size(), size_t(1)).on_error(godefv::return_status_t::exit{});
		}

		// Pointer was deleted and thus memory freed in the pool -> size decreases.
		check_equal(myObjectPool.size(), size_t(0)).on_error(godefv::return_status_t::exit{});
	}

	/** Create object with default constructor and specific constructor. */
	{
		godefv::object_pool_t<int, std::allocator, 10> myObjectPool;

		check_equal(myObjectPool.size(), size_t(0)).on_error(godefv::return_status_t::exit{});

		auto myValue_1 = myObjectPool.make_unique(-1);
		check_equal(myObjectPool.size(), size_t(1)).on_error(godefv::return_status_t::exit{});
		check_equal(*myValue_1, -1).on_error(godefv::return_status_t::exit{});

		{
			auto myLocalValue = myObjectPool.make_unique();
			check_equal(myObjectPool.size(), size_t(2)).on_error(godefv::return_status_t::exit{});
			check_equal(*myLocalValue, 0).on_error(godefv::return_status_t::exit{});
		}

		auto myValue_2 = myObjectPool.make_unique();
		check_equal(myObjectPool.size(), size_t(2)).on_error(godefv::return_status_t::exit{});
	}

	/** The size of the pool increases dynamically when the pool is size_t(f). */
	{
		godefv::object_pool_t<int, std::allocator, 2> myObjectPool;

		auto myValue_1 = myObjectPool.make_unique(0);
		auto myValue_2 = myObjectPool.make_unique(1);
		auto myValue_3 = myObjectPool.make_unique(2);
		check_equal(myObjectPool.size()    , size_t(3)).on_error(godefv::return_status_t::exit{});
		check_equal(myObjectPool.capacity(), size_t(4)).on_error(godefv::return_status_t::exit{});
		check_equal(*myValue_1, 0).on_error(godefv::return_status_t::exit{});
		check_equal(*myValue_2, 1).on_error(godefv::return_status_t::exit{});
		check_equal(*myValue_3, 2).on_error(godefv::return_status_t::exit{});

		{
			auto myLocalValue_1 = myObjectPool.make_unique(-1);
			auto myLocalValue_2 = myObjectPool.make_unique(-2);
			check_equal(myObjectPool.size()    , size_t(5)).on_error(godefv::return_status_t::exit{});
			check_equal(myObjectPool.capacity(), size_t(6)).on_error(godefv::return_status_t::exit{});
			check_equal(*myLocalValue_1, -1).on_error(godefv::return_status_t::exit{});
			check_equal(*myLocalValue_2, -2).on_error(godefv::return_status_t::exit{});
		}

		// For now: the capacity never decreases.
		auto myValue_4 = myObjectPool.make_unique(3);
		check_equal(myObjectPool.size()    , size_t(4)).on_error(godefv::return_status_t::exit{});
		check_equal(myObjectPool.capacity(), size_t(6)).on_error(godefv::return_status_t::exit{});
		check_equal(*myValue_1, 0).on_error(godefv::return_status_t::exit{});
		check_equal(*myValue_2, 1).on_error(godefv::return_status_t::exit{});
		check_equal(*myValue_3, 2).on_error(godefv::return_status_t::exit{});
		check_equal(*myValue_4, 3).on_error(godefv::return_status_t::exit{});
	}

	/** The deleter destroys the object. */
	{
		struct test_t
		{
			int* i;
			~test_t() { *i = 34; }
		};

		godefv::object_pool_t<test_t, std::allocator, 10> myObjectPool;

		int i = 0;
		myObjectPool.make_unique(&i); //temporary return value is destroyed immediately

		check_equal(i, 34).on_error(godefv::return_status_t::exit{});
	}

	/** The pool recycles the deleted pointers. */
	{
		/* The goal is to have a pool with the first position free while the second position is reserved.
		Since it is not possible to obtain such a state by playing with scopes, we use std::swap. */
		godefv::object_pool_t<int, std::allocator, 10> myObjectPool;

		auto myFirstPostion = myObjectPool.make_unique(0); // Reserve first position.
		auto myFirstPostionAdress = myFirstPostion.get(); // Store the pointer to first position to check later if it's used again.
		auto mySecondPostion = myObjectPool.make_unique(1); // Reserve second position.

		{
			auto myThirdLocalPosition = myObjectPool.make_unique(-1); // Reserve third position.
			// Swap unique pointers pointing to first and third position: the pointer to first position will be freed at the end of the scope.
			std::swap(myFirstPostion, myThirdLocalPosition);
		}

		auto myTestedFirstPostion = myObjectPool.make_unique(3); // Reserve a new pointer: first position should be reused.
		check_equal(myTestedFirstPostion.get(), myFirstPostionAdress).on_error(godefv::return_status_t::exit{}); // Check if the pointer to first position has been recycled.
	}

	std::cout<<"success"<<std::endl;
	return 0;
}
