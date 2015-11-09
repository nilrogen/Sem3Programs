#include "clist.h"

namespace gorlin {

template <class T>
List<T>::List() {
	this->length = 0;
	this->internal = malloc(sizeof(struct mlist));
	mlist_init(this->internal);
}

template <class T>
List<T>::List(const List<T> &cpy) {
	// Cant do this
}

template <class T>
List<T>::~List() {
	mlist_destroy(this->internal);
	free(this->interal);
}


template <class T>
const List<T> &List<T>::operator=(const List<T> &ref) {
	// cant do this
}

template <class T>
void List<T>::insert(T value) {
	if (mlist_insert(this->internal, value)) {
		throw 1; // ERROR
	}
	this->length++;
}

}
