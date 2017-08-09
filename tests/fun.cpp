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

template< class Manager >
struct NoProcessModule {
    NoProcessModule() = delete;
    
    explicit NoProcessModule(int) { }
};



int main() {
	using namespace myynt;
	
	manager m{IncrementModule{}, PrintModule{}, myynt::complete<NoProcessModule>(5), IncrementModule{}, PrintModule{}};
	m.myynt_Process(custom_message{"bar"});
	m.myynt_Process(5);
    
    //NoProcessModule<PrintModule> m = myynt::complete<NoProcessModule>(5);
    
    static_assert(std::is_same<typename myynt::tags::tags_of<IncrementModule>::type, yymp::typelist<IncrementModule>>::value);
    
	return 0;
}