//#include<stdio.h>
//#include<stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <array>
#include <functional>
// for std::unique_ptr
//#include<math.h>
//#include<time.h>

using namespace std;

std::string CreateRandomString(size_t Length)
{
	std::string s(Length, 'a');
	for (size_t i = 0; i < Length; i++){
		s[i] += rand() % 3;
	}

	return s;
}

void Next(std::vector<int>& next,const char* s, size_t m){
	int k = -1;
	next[0] = -1;
	for (size_t i = 1; i < m; i++)
	{
		while (k >= 0 && s[k] != s[i-1])
			k = next[k];
		next[i] = ++k;
	}
}

int KMP(const string &T, const string &P){
	size_t m = P.length(), n = T.length();
	std::vector<int> next(m);

	Next(next, P.c_str(), m);

	int j = 0;
	for (size_t i = 0; i < n;){
		while (j >= 0 && j < m && i < n && T[i] == P[j])
			++i, ++j;
		if (j == m)
			return i - m;
		j = next[j];
		if (j < 0)
			++i, j=0;
	}

	return -1;
}

int Horspool(const string &T, const string &P){
	const size_t Alphabet_Size = 128;
	size_t m = P.length(), n = T.length();
	std::array<int,Alphabet_Size> Bc;

//	for (i = 1; i < Alphabet_Size; i++) Bc[i] = m ;
	std::fill(Bc.begin(), Bc.end(), m);

	for (size_t i = 0; i < m - 1; i++)
		Bc[P[i]] = m - 1 - i;

	for (size_t i = 0; i < n - m;)
	{
		char c = T[i + m - 1];
		if (P[m - 1] == c && !memcmp(P.c_str(), T.c_str() + i, m))
			return i;
		else
			i += Bc[c];
	}
	return -1;
}

class KarpRabinHasher
{
public:
	int d, q, m;
	// The Value of d^(m-1)
	int dm;
	
	KarpRabinHasher(int length, int degree = 2, int q = 514229)
		: d(degree), q(q), m(length), dm(degree)
	{
		for (size_t i = 0; i < m - 2; i++)
			dm *= d;
	}

	inline
	int hash(const char* str)
	{
		int val = 0;
		for (size_t i = 0; i < m; i++)
			val = ((val * d) + str[i]) % q;
		return val;
	}

	inline
	int rehash(int hash_val, char leaving_char, char coming_char)
	{
		int val = ((hash_val - leaving_char*dm) * d + coming_char) % q;
		return val;
	}

};

int KarpRabin(const string &T, const string &P){
	size_t m = P.length(), n = T.length();

	KarpRabinHasher hasher(m);

	int phash = hasher.hash(P.c_str());
	int thash = hasher.hash(T.c_str());

	for (size_t i = 0; i < n - m; i++)
	{
		if (thash == phash && !memcmp(P.c_str(), T.c_str() + i, m))
			return i;
		else
			thash = hasher.rehash(thash, T[i], T[i + m]);
	}
	//int h = y%q;
	//for (int i = 1; i <= m; i++){
	//	phash = (phash*d + P[i]) % q;
	//	thash = (thash*d + T[i]) % q;
	//}
	//j = 1;
	//while (j <= n - m + 1){
	//	if (phash == thash && memcmp(P, T + j - 1, m) == 0)
	//		return j;
	//	thash = (d*(thash - T[j] * h) + T[j + m])%q;
	//	j++;
	//}
	return -1;
}

void main(){
	int* next;
	//ofstream out1("data1.txt");
	//ofstream out2("data2.txt");

	ofstream fout("output.txt");

	int n, m;
	int x;
	//int num;

	char T_s[] = "cbaacbcbcaacacbaaaaccacccbaacaaa";
	char P_s [] = "accc";
	//cout << "please enter the number of algorithm:\n1¡¢KMP\n2¡¢Horspool\n3¡¢Karp-Rabin\n";
	//cin >> num;

	{
		char *T = T_s, *P = P_s;
		n = strlen(T);
		m = strlen(P);

		cout << "Test case sample" << endl;
		cout << "\tT:\t" << T << endl;
		cout << "\tP:\t" << P << endl;
		cout << endl;

		auto it = std::search(T, T + n, P, P + m);
		if (it == T + n)
			cout << "\tSTL : not found" << endl;
		else
			cout << "\tSTL : " << (int) (it - T) << endl;

		cout << "\tK-M-P : " << KMP(T, P) << endl;

		cout << "\tHorspool : " << Horspool(T, P) << endl;

		cout << "\tKarp-Rabin : " << KarpRabin(T, P) << endl;

		cout << endl;
	}

	for (x = 4; x <= 10; x += 2){
		n = 1 << x;
//		m = 1 << (x / 2);
		m = 4;
		std::string T = CreateRandomString(n);
		std::string P = CreateRandomString(m);
		//char* T = Text.get(), *P = Pattern.get();

		fout << "Test case " << x / 2 - 1 << endl;
		fout << "\tT:\t" << T << endl;
		fout << "\tP:\t" << P << endl;

		auto it = std::search(T.cbegin(), T.cend(), P.cbegin(), P.cend());
		if (it == T.cend())
			fout << "\tSTL : -1" << endl;
		else
			fout << "\tSTL : " << (int) (it - T.cbegin()) << endl;

		fout << "\tK-M-P : " << KMP(T, P) << endl;

		fout << "\tHorspool : " << Horspool(T, P) << endl;

		fout << "\tKarp-Rabin : " << KarpRabin(T, P) << endl;

		fout << endl;
		//free(next);
		//switch (num)
		//{
		//case 1:
		//	//next = (int*)malloc((m + 1)*sizeof(int));
		//	next = new int [m+1];
		//	out2 << KMP(T, P, n, m, next) << ",";
		//	//free(next);
		//	delete next; 
		//	break;
		//case 2:
		//	out2 << Horspool(T, P, n, m) << ",";
		//	break;
		//case 3:
		//	out2 << KR( P,  T,  m,  n,  10,  23) << ",";
		//	break;
		//default:
		//	cout<<"input error"<<endl;
		//	break;
		//}
		//out2 << endl;
	}

	//free(P);
	//free(T);
	//delete P;
	//delete T;

	system("pause");
}