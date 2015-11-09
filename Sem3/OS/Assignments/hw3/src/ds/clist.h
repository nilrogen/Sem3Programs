#pragma once
#include <ds/mlist.h>
#include <include.h>

namespace gorlin {

template<class T>
class List {
	public:
		List();

		~List();

		void insert(T);
		void append(T);
		T deleteAt(int);
		T deleteFront();
		T deleteBack();
	private:
		const List<T> &operator=(const List<T> &);
		List(const List<T> &);

	private:
		struct mlist *internal;
		int length;
};

}
