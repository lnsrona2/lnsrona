//int heap[11] = {0,91,48,1,80,28,22,15,88,59,5};
int heap[10];


int swap(int i,int j){
	int temp = heap[i];
	heap[i] = heap[j];
	heap[j] = temp;
}

int factor(int n)
{
	if (!n) return 1;
	else{
		{
		//swap(3,5);
		return n*factor(n-1);
		}
	}
}

void main()
{
	int a;
	read(&a);
	write(factor(a));
}
