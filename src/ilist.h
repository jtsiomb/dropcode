// vi:filetype=cpp:ts=4:sw=4
#ifndef INTRUSIVE_LINKED_LIST_H_
#define INTRUSIVE_LINKED_LIST_H_

#include <stddef.h>

template <typename T>
class InListNode {
public:
	T *item;
	InListNode<T> *prev, *next;

	InListNode();
	~InListNode();

	void unlink();
};

template <typename T, int offs>
class InList {
private:
	int num_nodes;
	InListNode<T> *headp, *tailp;

public:
	InList();
	~InList();

	void clear();

	bool empty() const;
	int size() const;

	void append(T *item);
	void prepend(T *item);
	void unlink(T *item);

	InListNode<T> *head() const;
	InListNode<T> *tail() const;
};

// ---- InListNode implementation ----
template <typename T>
InListNode<T>::InListNode()
{
	item = 0;
	prev = next = 0;
}

template <typename T>
InListNode<T>::~InListNode()
{
	unlink();
}

template <typename T>
void InListNode<T>::unlink()
{
	if(prev) prev->next = next;
	if(next) next->prev = prev;
	prev = next = 0;
}

// ---- InList implementation ----
template <typename T, int offs>
InList<T,offs>::InList()
{
	num_nodes = 0;
	headp = tailp = 0;
}

template <typename T, int offs>
InList<T,offs>::~InList()
{
	clear();
}

template <typename T, int offs>
void InList<T,offs>::clear()
{
	while(headp) {
		InListNode<T> *node = headp;
		headp = headp->next;
		node->unlink();
	}
	headp = tailp = 0;
	num_nodes = 0;
}

template <typename T, int offs>
bool InList<T,offs>::empty() const
{
	return headp == 0;
}

template <typename T, int offs>
int InList<T,offs>::size() const
{
	return num_nodes;
}

template <typename T, int offs>
void InList<T,offs>::append(T *item)
{
	InListNode<T> *node = (InListNode<T>*)((char*)item + offs);
	node->item = item;
	node->next = 0;
	node->prev = tailp;

	if(tailp) {
		tailp->next = node;
		tailp = node;
	} else {
		headp = tailp = node;
	}
	++num_nodes;
}

template <typename T, int offs>
void InList<T,offs>::prepend(T *item)
{
	InListNode<T> *node = (InListNode<T>*)((char*)item + offs);
	node->item = item;
	node->next = headp;
	node->prev = 0;

	if(headp) {
		headp->prev = node;
		headp = node;
	} else {
		headp = tailp = node;
	}
	++num_nodes;
}

template <typename T, int offs>
void InList<T,offs>::unlink(T *item)
{
	InListNode<T> *node = (InListNode<T>*)((char*)item + offs);
	if(headp == node) {
		headp = node->next;
	}
	if(tailp == node) {
		tailp = node->prev;
	}
	node->unlink();
	--num_nodes;
}

template <typename T, int offs>
InListNode<T> *InList<T,offs>::head() const
{
	return headp;
}

template <typename T, int offs>
InListNode<T> *InList<T,offs>::tail() const
{
	return tailp;
}

#endif	/* INTRUSIVE_LINKED_LIST_H_ */
