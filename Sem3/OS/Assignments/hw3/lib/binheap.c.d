#include <ds/binheap.h>

/* Heap Node Structure */
typedef struct HEAPNODE {
	int order;
	void *value;

	struct HEAPNODE *parent;
	struct HEAPNODE *child;
	struct HEAPNODE *sibling;
} node_t;

static node_t *create_node(void *_v) {
	node_t *retv = (node_t *) calloc(sizeof(node_t), 0);
	if (retv != NULL) {
		retv->value = _v;
		retv->order = 0;
	}
	return retv;
}

struct BINHEAP {
	node_t *head;
	int (*cmp_f)(void *, void *);
};

/**************************************
 * Binary Heap Functions
 **************************************/

static node_t *bubble_up(node_t *, int, int (*)(void *, void *));
static int remove_root(binheap_t *, node_t *, node_t *);
static int link_tree(node_t *, node_t *);

extern int init_heap(binheap_t *heap, int (*cmp)(void *, void *)) {
	if (heap == NULL) {
		return -1;
	}
	heap->head = NULL;
	heap->cmp_f = cmp;
	return 0;
}

extern int destroy_heap(binheap_t *heap) {
	// TODO: IMPLEMENT?
}

extern int bheap_insert(binheap_t *heap, void *value) {
	binheap_t *newtree;

	if (heap == NULL || value == NULL) {
		return -1;
	}

	newtree = (binheap_t *) malloc(sizeof(binheap_t));
	if (newtree == NULL) {
		return -1;
	}

	newtree->head = create_node(value);
	if (newtree->head == NULL) {
		return -1;
	}

	bheap_merge(heap, newtree);
	return 0;
}

extern void *bheap_minimum(binheap_t *heap) {
	node_t *min, *next;
	if (heap == NULL) {
		return NULL;
	}

	min = heap->head;
	next = min->sibling;

	while (next != NULL) {
		if ((*heap->cmp_f)(min->value, next->value) < 0) {
			min = next;
		}
		next = next->sibling;
	}
	return min->value;
}

static node_t *bubble_up(node_t *node, int toroot, 
						 int (*cmp_f)(void *, void *)) {
	node_t *parent;
	void *tmp;

	assert(node != NULL);

	parent = node->parent;
	while (parent != NULL && (toroot || (*cmp_f)(node, parent) < 0)) {
		tmp = node->value;
		parent->value = tmp;
		node = parent;
		parent = parent->parent;
	}
	return node;
}
static int remove_root(binheap_t *tree, node_t *root, node_t *prev) {
	node_t *newh, *child, *next;
	binheap_t *newheap;

	if (tree == NULL) {
		return -1;
	}
	if (root == tree->head) {
		tree->head = root->sibling;
	}
	else {
		prev->sibling = root->sibling;
	}

	newh = NULL;
	child = root->child;
	while (child != NULL) {
		next = child->sibling; 
		child->sibling = newh; 
		child->parent = NULL;
		newh = child;
		child = next;
	}
	
	//tree->head = btunionwn(tree, newh);

	return 1;
}

static void link_tree(node_t *min, node_t *other) {
	other->parent = min;
	other->sibling = min->child;
	min->child = other;
	min->order++;
}

