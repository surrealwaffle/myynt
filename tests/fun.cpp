#include <iostream>

#include <myynt/manager.hpp>

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

int main() {
	myynt::manager m{IncrementModule{}, PrintModule{}};
	m.myynt_Process(1336);
	return 0;
}