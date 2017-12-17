
EXPORT game_state* start_up(platform_api* api) { 

	game_state* state = (game_state*)api->platform_heap_alloc(sizeof(game_state));
	state->func_state.this_dll = api->your_dll;

	global_api  = api;
	global_log  = &state->log;
	global_dbg  = &state->dbg;
	global_func = &state->func_state;

	setup_fptrs();

	state->default_platform_allocator = MAKE_PLATFORM_ALLOCATOR("default");
	state->suppressed_platform_allocator = MAKE_PLATFORM_ALLOCATOR("default/suppress");
	state->suppressed_platform_allocator.suppress_messages = true;

	begin_thread("main"_, &state->suppressed_platform_allocator);

	state->dbg_a = MAKE_PLATFORM_ALLOCATOR("dbg");
	state->dbg_a.suppress_messages = true;
	state->dbg = dbg_manager::make(&state->dbg_a);
	state->dbg.register_thread(60, 32768);

	dbg_msg m;
	m.type = dbg_msg_type::begin_frame;
	m.context = CONTEXT;
	POST_MSG(m);

	state->log_a = MAKE_PLATFORM_ALLOCATOR("log");
	state->log_a.suppress_messages = true;
	state->log = log_manager::make(&state->log_a);
	state->dbg.setup_log(&state->log);

	platform_file stdout_file, log_all_file;
	CHECKED(platform_get_stdout_as_file, &stdout_file);
	CHECKED(platform_create_file, &log_all_file, "log_all.html"_, platform_file_open_op::create);
	state->log.add_file(log_all_file, log_level::alloc, log_out_type::html);
	state->log.add_file(stdout_file, log_level::info, log_out_type::plaintext, true);

	LOG_INFO("Beginning startup...");
	LOG_PUSH_CONTEXT_L("");

	LOG_INFO("Setting up events...");
	state->evt_a = MAKE_PLATFORM_ALLOCATOR("event");
	state->evt = evt_manager::make(&state->evt_a);
	state->evt.start();

	LOG_INFO("Starting thread pool...");
	state->thread_pool_a = MAKE_PLATFORM_ALLOCATOR("threadpool");
	state->thread_pool_a.suppress_messages = true;
	state->thread_pool = threadpool::make(&state->thread_pool_a);
	state->thread_pool.start_all();

	LOG_INFO("Allocating transient store...");
	state->transient_arena = MAKE_ARENA("transient"_, MEGABYTES(8), &state->default_platform_allocator, false);

	job_id assets = state->thread_pool.queue_job([](void* s) -> job_callback {
		game_state* state = (game_state*)s;
		LOG_INFO("Setting up asset system...");
		state->default_store_a = MAKE_PLATFORM_ALLOCATOR("asset");
		state->default_store = asset_store::make(&state->default_store_a);
		state->default_store.load("assets/assets.asset"_);
		return null;
	}, state);

	LOG_INFO("Creating window...");
	platform_error err = api->platform_create_window(&state->window, "Exile"_, 1280, 720);

	if (!err.good) {
		LOG_FATAL_F("Failed to create window, error: %", err.error);
	}

	LOG_INFO("Setting up OpenGL...");
	ogl_load_global_funcs();
	state->ogl_a = MAKE_PLATFORM_ALLOCATOR("ogl");
	state->ogl = ogl_manager::make(&state->ogl_a);

	state->thread_pool.wait_job(assets);

	LOG_INFO("Setting up GUI...");
	state->gui_a = MAKE_PLATFORM_ALLOCATOR("gui");
	state->gui = gui_manager::make(&state->ogl, &state->gui_a, &state->window);
	state->gui.add_font(&state->ogl, "gui14"_, &state->default_store);
	state->gui.add_font(&state->ogl, "gui24"_, &state->default_store);
	state->gui.add_font(&state->ogl, "gui40"_, &state->default_store);
	state->gui.add_font(&state->ogl, "guimono"_, &state->default_store, true);

	LOG_INFO("Starting logger...");
	state->log.start();

	LOG_INFO("Done with startup!");
	LOG_POP_CONTEXT();

	m.type = dbg_msg_type::end_frame;
	POST_MSG(m);
	state->dbg.collate();

	state->running = true;
	return state;
}

EXPORT bool main_loop(game_state* state) {

	dbg_msg msg;
	msg.type = dbg_msg_type::begin_frame;
	msg.context = CONTEXT;
	POST_MSG(msg);

	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	glClear((GLbitfield)gl_clear::color_buffer_bit | (GLbitfield)gl_clear::depth_buffer_bit);

	PUSH_ALLOC(&state->transient_arena) {

		gui_input_state input = run_events(state); 
		
		state->gui.begin_frame(input);

		state->dbg.UI();

		state->gui.end_frame(&state->window, &state->ogl);

	} POP_ALLOC();
	RESET_ARENA(&state->transient_arena);

	CHECKED(platform_swap_buffers, &state->window);

#ifndef RELEASE
	state->ogl.try_reload_programs();
	if(state->default_store.try_reload()) {
		state->gui.reload_fonts(&state->ogl);
	}
#endif

	msg.type = dbg_msg_type::end_frame;
	POST_MSG(msg);
	state->dbg.collate();

	return state->running;
}

EXPORT void shut_down(game_state* state) { 

	LOG_INFO("Beginning shutdown...");

	LOG_DEBUG("Destroying OpenGL")
	state->ogl.destroy();

	LOG_DEBUG("Destroying asset system");
	state->default_store.destroy();

	LOG_DEBUG("Destroying thread pool");
	state->thread_pool.stop_all();
	state->thread_pool.destroy();

	LOG_DEBUG("Destroying window");
	CHECKED(platform_destroy_window, &state->window);

	LOG_DEBUG("Destroying events");
	state->evt.destroy();

	LOG_DEBUG("Destroying debug system");
	state->dbg.shutdown_log(&state->log);	
	state->dbg.destroy();

	LOG_DEBUG("Destroying transient store");
	DESTROY_ARENA(&state->transient_arena);

	LOG_DEBUG("Done with shutdown!");

	state->log.stop();
	state->log.destroy();
	state->gui.destroy();

	end_thread();
	cleanup_fptrs();

	state->log_a.destroy();
	state->ogl_a.destroy();
	state->gui_a.destroy();
	state->dbg_a.destroy();
	state->evt_a.destroy();
	state->thread_pool_a.destroy();
	state->default_platform_allocator.destroy();
	state->suppressed_platform_allocator.destroy();
	state->default_store_a.destroy();

	global_api->platform_heap_free(state);
}

EXPORT void on_reload(platform_api* api, game_state* state) { 

	global_api = api;
	global_log = &state->log;
	global_dbg = &state->dbg;
	global_func = &state->func_state;
	state->gui.style = _gui_style();

	state->func_state.reload_all();

	begin_thread("main"_, &state->suppressed_platform_allocator, 600, 32768);

	ogl_load_global_funcs();

	state->evt.start(); // NOTE(max): needed to reset platform function pointer pointing into the game DLL
	state->log.start();
	state->thread_pool.start_all();

	LOG_INFO("End reloading game code");
}

EXPORT void on_unload(game_state* state) {
	
	LOG_INFO("Begin reloading game code");

	state->thread_pool.stop_all();
	state->log.stop();

	end_thread();
}
