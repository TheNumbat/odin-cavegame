
#include "threads.h"
#include "util/threadstate.h"
#include "dbg.h"

bool gt(super_job* l, super_job* r) { 
	if(l->priority_class == r->priority_class) return l->priority > r->priority;
	return l->priority_class > r->priority_class;
}

threadpool threadpool::make(i32 num_threads_) { 

	return make(CURRENT_ALLOC(), num_threads_);
}

threadpool threadpool::make(allocator* a, i32 num_threads_) { 

	threadpool ret;

	ret.num_threads = num_threads_ == 0 ? global_api->get_num_cpus() : num_threads_;

	ret.alloc   = a;
	ret.threads = array<platform_thread>::make(ret.num_threads, a);
	ret.jobs    = locking_heap<super_job*>::make(16, a);
	ret.worker_data = array<worker_param>::make(ret.num_threads, a);
	
	CHECKED(create_semaphore, &ret.jobs_semaphore, 0, INT_MAX);

	return ret;
}

void threadpool::destroy() { 

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

void threadpool::renew_priorities(f32 (*eval)(super_job*, void*), void* param) { 

	jobs.renew(eval, param);
}

template<>
void heap<super_job*>::renew(f32 (*eval)(super_job*, void*), void* param) { 

	heap<super_job*> h = heap<super_job*>::make(capacity);

	FORHEAP_LINEAR(it, *this) {

		super_job* j = *it;

		j->priority = eval(j, param);

		if(j->priority > -FLT_MAX) {
			h.push(j);
		} else {
			
			if(j->cancel)
				j->cancel(j->data);

			// NOTE(max): only works because the elements are allocated with the same allocator passed to the heap (in threadpool)
			PUSH_ALLOC(alloc) {
				free(j, j->my_size);
			} POP_ALLOC();
		}
	}
   
	_memcpy(h.memory, memory, h.size * sizeof(super_job*));
	size = h.size;
	h.destroy();
}

void threadpool::queue_job(job_work<void> work, void* data, f32 priority, i32 priority_class, _FPTR* cancel) {

	PUSH_ALLOC(alloc);

	job<void>* j = NEW(job<void>);
	j->priority = priority;
	j->priority_class = priority_class;
	j->work = work;
	j->data = data;
	j->cancel.set(cancel);

#ifdef NO_CONCURRENT_JOBS
	j->do_work();
	free(j, j->my_size);
	POP_ALLOC();
#else

	jobs.push(j);

	CHECKED(signal_semaphore, &jobs_semaphore, 1);

	POP_ALLOC();
#endif
}

void threadpool::stop_all() { 

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

	PUSH_ALLOC(alloc);
	FORHEAP_LINEAR(it, jobs) {
		if((*it)->cancel)
				(*it)->cancel((*it)->data);
		free(*it, (*it)->my_size);
	}
	POP_ALLOC();
	jobs.clear();
} 

void threadpool::start_all() { 

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
	global_dbg->profiler.register_thread(10);
	
	LOG_DEBUG("Starting worker thread"_); 

	do {
		global_api->wait_semaphore(data->jobs_semaphore, -1);

		super_job* current_job = null;
#ifdef FAST_CLOSE
		while(data->online && data->job_queue->try_pop(&current_job)) {
#else
		while(data->job_queue->try_pop(&current_job)) {
#endif
			BEGIN_FRAME();

			current_job->do_work();

			PUSH_ALLOC(data->alloc) {
				free(current_job, current_job->my_size);
			} POP_ALLOC();

			platform_event a;
			a.type 		 = platform_event_type::async;
			a.async.type = platform_async_type::user;
			global_api->queue_event(a);
			
			END_FRAME();
		}
	} while(data->online);

	LOG_DEBUG("Ending worker thread"_);
	global_dbg->profiler.collate();
	end_thread();

	return 0;
}
