/* This file is part of z10-infodisplay, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#ifndef INFO_PULSE_PP_HPP
#define INFO_PULSE_PP_HPP

#include <pulse/context.h>
#include <pulse/mainloop.h>

namespace pulse{

class Mainloop{
	private:
		pa_mainloop* loop{ nullptr };
		
	public:
		Mainloop() : loop(pa_mainloop_new()) { }
		Mainloop( const Mainloop& ) = delete;
		~Mainloop(){ pa_mainloop_quit( loop, 0 ); };
		
		int iterate( int block, int& retval )
			{ return pa_mainloop_iterate( loop, block, &retval ); }
		
		auto api() { return pa_mainloop_get_api( loop ); }
};


class Operation{
	private:
		pa_operation* operation;
		
	public:
		
};


class Connection{
	private:
		pa_context* context{ nullptr };
	
	public:
		Connection( Mainloop& loop, const char* name )
			: context( pa_context_new( loop.api(), name ) ) { }
		
		Connection( const Connection& ) = delete;
		~Connection(){ pa_context_disconnect( context ); };
		
		void connect( const char* server=nullptr
			,	pa_context_flags_t flags = PA_CONTEXT_NOFLAGS
			,	const pa_spawn_api* api = nullptr
			) { pa_context_connect( context, server, flags, api ); }
		
		template<class T>
		void setStateCallback( T& object ){
			pa_context_set_state_callback( context, [](pa_context *c, void *userdata){
				auto obj = static_cast<T*>( userdata );
				obj->stateCallback( c );
			}, (void*)&object );
		}
		
		
	
};

}

#endif
