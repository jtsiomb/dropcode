#ifndef INTRUSIVE_LINKED_LIST_H_
#define INTRUSIVE_LINKED_LIST_H_

template <typename T>
class IntrListNode {
public:
	T *owner;
	ListNode<T> *prev, *next;

	ListNode();
	~ListNode();
};

template <typename T>
class IntrList {
private:
	int num_nodes;
	IntrListNode *head, *tail;

public:
	IntrList();
	~IntrList();

	void clear();

	bool empty() const;
	int size() const;
};

#endif	/* INTRUSIVE_LINKED_LIST_H_ */
