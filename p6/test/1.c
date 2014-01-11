/*递归调用*/
int n,f;

float k = 4.5;

struct foo{
	int i : 2;
	float f : 1;
} *a, *b[5];

void factorial(){
	if(n==1)	f=1;
	if(n>1){
		f=f*n;
		n=n-1;
		factorial();
	}
}
void main(){
	n=3;
	a->i = n;
	b[3] = a;
	f=1;
	factorial();
}
