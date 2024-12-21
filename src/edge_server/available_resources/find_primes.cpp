#include <iostream>
using namespace std;

bool prime(int n){
	if (n < 2) return 0;
	if(n == 2) return 1;
	if(n % 2 == 0) return 0;
	for(int d = 3; d * d <= n; d += 2)
		if(n % d == 0)
			return 0;
	return 1;
}

int main(){
	int n = 100;
	cout << "Primes up to " << n << ":\n"; 
	for(int i = 0; i <= n; ++i)
		if(prime(i) == true)
			cout << i << " ";
	return 0;
}
