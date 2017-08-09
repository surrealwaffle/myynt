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
    
    // should not be used in below code 
    NoProcessModule(NoProcessModule const&) { std ::cout << "NoProcessModule(NoProcessModule const&)" << std::endl; }
    
    // should not be used in below code
    NoProcessModule(NoProcessModule&&) { std ::cout << "NoProcessModule(NoProcessModule&&)" << std::endl; }
    
    // should be used in below code
    explicit NoProcessModule(int&&) { std::cout << "NoProcessModule(int&&)" << std::endl; }
    
    // should be used in below code
    explicit NoProcessModule(int const&) { std::cout << "NoProcessModule(int const&)" << std::endl;}
};

int main() {
	using namespace myynt;
    
    auto u = complete<NoProcessModule>(5);
	
	manager m{IncrementModule{}, PrintModule{}, u, IncrementModule{}, complete<NoProcessModule>(6), PrintModule{}};
	m.myynt_Process(custom_message{"bar"});
	m.myynt_Process(5);
    
    static_assert(std::is_same<typename myynt::tags::tags_of<IncrementModule>::type, yymp::typelist<IncrementModule>>::value);
    
	return 0;
}