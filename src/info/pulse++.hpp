/* This file is part of z10-infodisplay, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#ifndef INFO_PULSE_PP_HPP
#define INFO_PULSE_PP_HPP

#include <pulse/context.h>
#include <pulse/mainloop.h>
#include <pulse/introspect.h>

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
		Operation( pa_operation* operation ) : operation(operation) { }
		Operation( Operation&& other ) : operation(other.operation)
			{ other.operation = nullptr; }
		Operation( const Operation& ) = delete; //TODO:
		~Operation(){ if(operation) pa_operation_unref(operation); }
};

class Context{
	protected:
		pa_context* context{ nullptr };
		Context( pa_context* context ) : context(context) {}
		
	public:
		Context( const Context& ) = delete;
		
		
		template<class T>
		void setStateCallback( T& object ){
			pa_context_set_state_callback( context, [](pa_context *c, void *userdata){
					static_cast<T*>( userdata )->stateCallback( { c } );
				}, static_cast<void*>( &object ) );
		}
		
		template<class T>
		Operation setSinkInfoListCallback( T& object ){
			return { pa_context_get_sink_info_list( context
				,	[](pa_context *c, const pa_sink_info *i, int eol, void *userdata){
						static_cast<T*>( userdata )->sinkInfoListCallback( { c }, i, eol );
					}
				,	static_cast<void*>( &object ) ) };
		}
		
};

class InstancedContext : public Context{
	public:
		InstancedContext( Mainloop& loop, const char* name )
			:	Context( pa_context_new( loop.api(), name ) ) { }
		
		InstancedContext( const InstancedContext& other ) : Context( other.context )
			{ pa_context_ref( context ); }
		
		~InstancedContext() { pa_context_unref( context ); }
};


class Connection : public InstancedContext{
	public:
		Connection( Mainloop& loop, const char* name ) : InstancedContext( loop, name ) { }
		Connection( const Connection& ) = delete;
		~Connection(){ pa_context_disconnect( context ); };
		
		void connect( const char* server=nullptr
			,	pa_context_flags_t flags = PA_CONTEXT_NOFLAGS
			,	const pa_spawn_api* api = nullptr
			) { pa_context_connect( context, server, flags, api ); }
		
		
		
	
};

}

#endif
