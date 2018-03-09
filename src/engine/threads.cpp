
template<typename E>
void atomic_enum<E>::set(E val) {

	global_api->atomic_exchange(&value, (u64)val);
}

template<typename E>
E atomic_enum<E>::get() {

	return (E)value;
}

bool gt(super_job* l, super_job* r) { PROF
	return l->priority > r->priority;
}

template<typename T>
future<T> future<T>::make() {

	future<T> ret;

	global_api->create_mutex(&ret.mut, false);
	global_api->create_semaphore(&ret.sem, 0, INT_MAX);

	return ret;
}

template<typename T>
void future<T>::destroy() {

	global_api->destroy_mutex(&mut);
	global_api->destroy_semaphore(&sem);
}

template<typename T>
T future<T>::wait() {

	global_api->wait_semaphore(&sem, -1);
	return val;
}

template<typename T>
void future<T>::set(T v) {

	global_api->aquire_mutex(&mut);
	val = v;
	global_api->release_mutex(&mut);
	CHECKED(signal_semaphore, &sem, 1);
}

threadpool threadpool::make(i32 num_threads_) { PROF

	return make(CURRENT_ALLOC(), num_threads_);
}

threadpool threadpool::make(allocator* a, i32 num_threads_) { PROF

	threadpool ret;

	ret.num_threads = num_threads_ == 0 ? global_api->get_num_cpus() : num_threads_;

	ret.alloc   = a;
	ret.threads = array<platform_thread>::make(ret.num_threads, a);
	ret.jobs    = locking_heap<super_job*>::make(16, a);
	ret.worker_data = array<worker_param>::make(ret.num_threads, a);
	
	CHECKED(create_semaphore, &ret.jobs_semaphore, 0, INT_MAX);

	return ret;
}

void threadpool::destroy() { PROF

	stop_all();

	threads.destroy();
	worker_data.destroy();
	
	PUSH_ALLOC(alloc);
	FORHEAP_LINEAR(it, jobs) {
		free(*it, (*it)->my_size);
	}
	POP_ALLOC();
	jobs.destroy();

	CHECKED(destroy_semaphore, &jobs_semaphore);
}

void threadpool::renew_priorities(float (*eval)(super_job*, void*), void* param) { PROF

	jobs.renew(eval, param);
}

template<typename T>
void threadpool::queue_job(future<T>* fut, job_work<T> work, void* data, float priority) { PROF

	PUSH_ALLOC(alloc);

	job<T>* j = NEW(job<T>);
	j->priority = priority;
	j->future = fut;
	j->work = work;
	j->data = data;

#ifdef NO_CONCURRENT_JOBS
	POP_ALLOC();
	j->do_work();
#else

	jobs.push(j);

	CHECKED(signal_semaphore, &jobs_semaphore, 1);

	POP_ALLOC();
#endif
}

void threadpool::queue_job(job_work<void> work, void* data, float priority) {

	PUSH_ALLOC(alloc);

	job<void>* j = NEW(job<void>);
	j->priority = priority;
	j->work = work;
	j->data = data;

#ifdef NO_CONCURRENT_JOBS
	POP_ALLOC();
	j->do_work();
#else

	jobs.push(j);

	CHECKED(signal_semaphore, &jobs_semaphore, 1);

	POP_ALLOC();
#endif
}

void threadpool::stop_all() { PROF

	if(online) {
	
		for(i32 i = 0; i < num_threads; i++) {

			worker_data.get(i)->online = false;
		}

		CHECKED(signal_semaphore, &jobs_semaphore, num_threads);

		for(i32 i = 0; i < num_threads; i++) {

			global_api->join_thread(threads.get(i), -1);
			CHECKED(destroy_thread, threads.get(i));
		}

		online = false;
	}
} 

void threadpool::start_all() { PROF

	if(!online) {
	
		FORARR(it, worker_data) {

			it->job_queue 	 	= &jobs;
			it->jobs_semaphore 	= &jobs_semaphore;
			it->online 			= true;
			it->alloc  			= alloc;

			CHECKED(create_thread, threads.get(__it), &worker, it, false);
		}

		online = true;
	}
}

i32 worker(void* data_) { 

	worker_param* data = (worker_param*)data_;

	begin_thread("worker %"_, data->alloc, global_api->this_thread_id());
	this_thread_data.profiling = false;
	global_dbg->register_thread(10);
	
	LOG_DEBUG("Starting worker thread");

	while(data->online) {

		super_job* current_job = null;
		if(data->job_queue->try_pop(&current_job)) {

			PUSH_PROFILE(true) {

				BEGIN_FRAME();

				current_job->do_work();

				PUSH_PROFILE_PROF(false) {
					PUSH_ALLOC(data->alloc) {
						free(current_job, current_job->my_size);
					} POP_ALLOC();

					platform_event a;
					a.type 			 = platform_event_type::async;
					a.async.type 	 = platform_async_type::user;
					global_api->queue_event(a);
				} POP_PROFILE_PROF();
				
				END_FRAME();
			} POP_PROFILE();
		}

		global_api->wait_semaphore(data->jobs_semaphore, -1);
	}

	LOG_DEBUG("Ending worker thread");
	end_thread();

	return 0;
}
