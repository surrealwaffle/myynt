#include <iostream>

#include <myynt/manager.hpp>
#include <myynt/traits.hpp>

struct IncrementModule {
	void myynt_Process(int& i) {
		++i;
	}
};

struct PrintModule {
	template< class T >
	void myynt_Process(T const& t) {
		std::cout << "PrintModule:" << t << std::endl;
	}
};

struct M { };

int main() {
	using namespace myynt;
	
	static_assert(is_message_processable<int&, IncrementModule>());
	static_assert(!is_message_processable<void*&, IncrementModule>());
	
	manager m{IncrementModule{}, PrintModule{}, IncrementModule{}, PrintModule{}};
	m.myynt_Process(1335);
	return 0;
}