
template<typename T>
void destroy_queue(queue<T>* q) { PROF

	destroy_vector(&q->contents);
}

template<typename T>
queue<T> make_queue(u32 capacity, allocator* a) { PROF

	queue<T> ret;

	ret.contents = make_vector<T>(capacity, a);
	
	return ret;
}

template<typename T>
queue<T> make_queue(u32 capacity) { PROF

	queue<T> ret;

	ret.contents = make_vector<T>(capacity);

	return ret;
}

template<typename T>
T* queue_push(queue<T>* q, T value) { PROF

	return vector_push(&q->contents, value);
}

template<typename T>
T queue_pop(queue<T>* q) { PROF

	if(q->contents.size > 0) {
		
		T top = *queue_front(q);

		vector_pop_front(&q->contents);

		return top;	
	}

	LOG_FATAL("Trying to pop empty queue!");
	T ret = {};
	return ret;
}

template<typename T>
T* queue_back(queue<T>* q) { PROF

	if(q->contents.size > 0) {

		return vector_back(&q->contents);
	}

	LOG_FATAL("Trying to get back of empty queue!");
	return null;
}

template<typename T>
T* queue_front(queue<T>* q) { PROF

	if(q->contents.size > 0) {

		return vector_front(&q->contents);
	}

	LOG_FATAL("Trying to get front of empty queue!");
	return null;
}

template<typename T>
bool queue_empty(queue<T>* q) { PROF
	return vector_empty(&q->contents);
}
