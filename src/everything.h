
#pragma once

#ifdef __clang__
#define NOREFLECT __attribute__((annotate("noreflect")))
#else
#define NOREFLECT
#endif

#include "basic_types.h"
#include "math.h"
#include "str/strings.h"

struct code_context {
	string file;
	string function;
	u32 line = 0;
};

#include "alloc.h"
#include "platform_api.h"

#include "ds/vector.h"
#include "ds/stack.h"
#include "ds/array.h"
#include "ds/queue.h"
#include "ds/map.h"

#include "type_table.h"

#ifdef _DEBUG
#define BOUNDS_CHECK
#define BLOCK_ON_ERROR
// #define ZERO_ARENA
// #define CONSTRUCT_DS_ELEMENTS // removes need for zero-cleared allocation
#endif

#define MAX_CALL_STACK_DEPTH 256
struct thread_data {
	stack<allocator*> alloc_stack;
	
	string name;
	code_context start_context;

	code_context call_stack[MAX_CALL_STACK_DEPTH];
	u32 call_stack_depth = 0;
};

thread_local thread_data this_thread_data;

#define FOR(num) 			for(i32 __i = 0; __i < num; __i++)
#define FORVEC(v,code) 		{u32 __i = 0; for(auto it = (v).memory; it != (v).memory + (v).size; __i++, it++) {code}}
#define FORVECCAP(v,code) 	{u32 __i = 0; for(auto it = (v).memory; it != (v).memory + (v).capacity; __i++, it++) {code}}
#define FORVEC_R(v,code)	{u32 __i = (v).size; for(auto it = (v).memory + (v).size; it != (v).memory; __i--, it--) {code}}
#define FORMAP(m,code) 		{u32 __i = 0; for(auto it = (m).contents.memory; it != (m).contents.memory + (m).contents.capacity; __i++, it++) if(it->occupied) {code}}
#define FORARR(a,code) 		FORVECCAP(a,code)
#define INC__COUNTER__ 		{u32 i = __COUNTER__; i = 0;}

string np_substring(string str, u32 start, u32 end);
i32    np_string_last_slash(string str);
string np_string_literal(const char* literal);
string np_string_from_c_str(char* c_str);

#define CONTEXT _make_context(np_string_literal(__FILE__), np_string_literal(__func__), __LINE__)

inline code_context _make_context(string file, string function, i32 line);

#define begin_thread(fmt, a, ...) _begin_thread(fmt, a, CONTEXT, __VA_ARGS__);
template<typename... Targs> void _begin_thread(string fmt, allocator* alloc, code_context start, Targs... args);
void end_thread();

#include "ds/threadpool.h"

#include "log.h"
#include "asset.h"
#include "render.h"
#include "opengl.h"
#include "gui.h"
#include "events.h"
#include "dbg.h"

#include "game.h"
static game_state* global_state = NULL;

#ifdef _DEBUG

struct func_scope { // @NORTTI
	func_scope(code_context context) {
		LOG_DEBUG_ASSERT(this_thread_data.call_stack_depth < MAX_CALL_STACK_DEPTH);
		this_thread_data.call_stack[this_thread_data.call_stack_depth++] = context;
	}
	~func_scope() {
		this_thread_data.call_stack_depth--;
	}
};

struct func_scope_nocs { // @NORTTI
	func_scope_nocs(code_context context) {
		
	}
	~func_scope_nocs() {
		
	}
};

#define FUNC func_scope __f(CONTEXT);
#define FUNC_NOCS func_scope_nocs __f(CONTEXT);
#else
#define FUNC 
#define FUNC_NOCS
#endif

#include <meta_types.h>

// IMPLEMENTATIONS
#include "str/strings.cpp"
#include "math.cpp"

#include "alloc.cpp"
#include "log.cpp"
#include "events.cpp"
#include "asset.cpp"
#include "opengl.cpp"
#include "render.cpp"
#include "gui.cpp"
#include "dbg.cpp"

#include "ds/vector.cpp"
#include "ds/stack.cpp"
#include "ds/array.cpp"
#include "ds/queue.cpp"
#include "ds/map.cpp"
#include "ds/threadpool.cpp"

#include "everything.cpp"
// /IMPLEMENTATIONS
