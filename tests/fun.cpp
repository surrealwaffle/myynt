#include <iostream>

#include <myynt/emitters.hpp>
#include <myynt/manager.hpp>
#include <myynt/traits.hpp>

struct custom_message {
    char msg[256];
};

struct IncrementModule {
	void myynt_Process(int& i) {
		++i;
	}
};

struct PrintModule {
	void myynt_Process(int const& t) {
		std::cout << "PrintModule:" << t << std::endl;
	}
    
    void myynt_Process(custom_message const& msg) {
        printf("custom_message: %s\n", msg.msg);
    }
};

struct NoProcessModule {
    NoProcessModule() = delete;
    
    NoProcessModule(int) { }
};

int main() {
	using namespace myynt;
	
	static_assert(is_message_processable<int&, IncrementModule>());
	static_assert(!is_message_processable<void*&, IncrementModule>());
    static_assert(!is_message_processable<int&, NoProcessModule>());
	
	manager m{IncrementModule{}, PrintModule{}, NoProcessModule{5}, IncrementModule{}, PrintModule{}};
	m.myynt_Process(1335);
	return 0;
}