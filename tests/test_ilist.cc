#include <stdio.h>
#include "src/ilist.h"

struct Argument {
	char *str;
	InListNode<Argument> listnode;

	Argument(char *s) { str = s; }
};

void print_list(InListNode<Argument> *node)
{
	int i = 0;
	while(node) {
		printf(" [%d]: %s\n", i++, node->item->str);
		node = node->next;
	}
}

int main(int argc, char **argv)
{
	InList<Argument, offsetof(Argument, listnode)> arglist;

	for(int i=1; i<argc; i++) {
		Argument *arg = new Argument(argv[i]);
		arglist.append(arg);
	}

	printf("%d arguments\n", arglist.size());

	print_list(arglist.head());

	printf("deleting list\n");
	while(arglist.head()) {
		InListNode<Argument> *node = arglist.head();
		arglist.unlink(node->item);
		delete node->item;

		printf("unlinked first item ... %d nodes left\n", arglist.size());
		print_list(arglist.head());
	}

	return 0;
}
