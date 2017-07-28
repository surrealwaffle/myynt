#include <iostream>

#include <myynt/emitters.hpp>
#include <myynt/manager.hpp>
#include <myynt/traits.hpp>

struct PrintModule;
struct IncrementModule;

struct custom_message {
    using myynt_first = PrintModule;
    using myynt_last = IncrementModule;
    
    char const* msg;
};

struct IncrementModule : myynt::emitter<myynt::callback, custom_message> {    
	void myynt_Process(int& i) {
		++i;
        
        myynt_Emit(custom_message{"foo"});
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
    //manager m{IncrementModule{}};
	m.myynt_Process(custom_message{"foo"});
    
    static_assert(std::is_same<typename myynt::tags::tags_of<IncrementModule>::type, yymp::typelist<IncrementModule>>::value);
    
	return 0;
}