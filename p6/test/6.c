typedef struct node *Node;
struct node
{
	int data;
	Node pNext;
} root,n1;

void main()
{
	int D[5] = {5,3,4,1,2};
	//root.data = 3;
	//root.pNext = &n1;
	//root.pNext->data = root.data;
	int i = 1;
	write(D[0]);
	while (i < 5) {
		write(",");
		write(D[i]);
		i = i + 1;
	}

	write("\nPlease input the number :\n");
	read(i);
	//read(root.data);
	write("D[");
	write(i);
	write("]=");
	write(D[i]);
	write("\n");
	//write(n1.data);
	write("This is Great!!!!");
}