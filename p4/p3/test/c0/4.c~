/*函数嵌套调用 C0 暂不支持函数声明*/

int n;

void fooB(); //函数声明,C0暂不提供支持

void fooA(){
	n = n - 1;
	if (n > 0) 
		fooB();
}

void fooB(){
	n = n - 1;
	if (n > 0) 
		fooA();
}

void main(){
	n = 2;
	fooA();
}
