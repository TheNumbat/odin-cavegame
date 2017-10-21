
#pragma once

#define MAX_CALL_STACK_DEPTH 	512
#define DEBUG_MSG_BUFFER		256

struct thread_data {
	stack<allocator*> alloc_stack;
	
	string name;
	code_context start_context;

	code_context call_stack[MAX_CALL_STACK_DEPTH] = {};
	u32 call_stack_depth = 0;

	bool profiling = false;
	queue<dbg_msg> dbg_msgs;
};

static thread_local thread_data this_thread_data;

#define begin_thread(fmt, a, frames, frame_size, ...) _begin_thread(fmt, a, frames, frame_size, CONTEXT, ##__VA_ARGS__);
template<typename... Targs> void _begin_thread(string fmt, allocator* alloc, u32 frames, u32 frame_size, code_context start, Targs... args);
void end_thread();
