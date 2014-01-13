typedef struct node *Node;
struct node
{
	int data;
	Node pNext;
} root,n1;

Node LinkNode(Node p,Node p2)
{
	p->pNext = p2;
	return p;
}

void main()
{
	int D[5] = {5,3,4,1,2};
	struct node n2 = {18,&n1}; 
	root.data = 3;
	root.pNext = &n1;
	root.pNext->data = root.data;
}