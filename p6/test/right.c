int heap[11] = {0,91,48,1,80,28,22,15,88,59,5};
int heap_length=11;
int heapSize = heap_length-1;
int t;
/*
     * @param i : ������Ԫ�ص�λ��
     * @param j : ������Ԫ�ص�λ��
     * ����λ��i��λ��j��Ԫ��
     */    
void swap(int i,int j){
	int temp = heap[i];
	heap[i] = heap[j];
	heap[j] = temp;
}
	/*
     * @param i : �������Ӷѵĸ�Ԫ��
     * ������iΪ��������Ϊ����� 
     */
void maxHeap(int i){
	int lc = 2*i;
	int rc = 2*i +1;
	int largest = i;
	int temp = 0;
	if(lc <= heapSize && heap[lc]>heap[i])
		largest = lc;
	else
		largest = i;
	
	if(rc <= heapSize && heap[rc] > heap[largest])
		largest = rc;

	if(largest != i){
		swap(i,largest);
			
		maxHeap(largest);
	}		
}
/*
     * ���������鹹����һ�������
     */
void buildMaxHeap(){
	int i = 0;
	i = heapSize/2;
	while(i>=1){
		maxHeap(i);
		i=i-1;
	}	
}

	 /*
     * �������㷨�������� 
     */ 
void sort(){	
	int i=0;
	buildMaxHeap();
	i = heapSize;
	while(i>=2){
		swap(1,i);	
		heapSize=heapSize-1;
		maxHeap(1);
		i=i-1;
	}
}
    
    
    /*
     * ������е�Ԫ��
     */
void printHeap(){
	int i = 1;
	while(i < heap_length){
		write(heap[i]);
		write("\t");
		i=i+1;
	}
	write("\n");
}

int main(){
	
	write("Before the sort\n");
	write("==================\n");
	printHeap();
	sort();
//	swap(1,4);
//	maxHeap(5);
	write("After the sort\n");
	write("==================\n");
	printHeap();	
}
