#pragma comment(lib, "ws2_32.lib")
#include "WSAInitializer.h"
#include "Sourcew.h"
#include <math.h>
#include <vector>
#include <set>
#include <string>
#include<numeric>
std::string serverIP = "127.0.0.1";

int gcd(int a, int h)
{
    int temp;
    while (1) {
        temp = a % h;
        if (temp == 0)
            return h;
        a = h;
        h = temp;
    }
}
std::set<int> prime; // a set will be the collection of prime numbers,
       // where we can select random primes p and q
int public_key;
int private_key;
int n;
// we will run the function only once to fill the set of
// prime numbers
void primefiller()
{
    // method used to fill the primes set is seive of
    // eratothenes(a method to collect prime numbers)
    std::vector<bool> seive(250, true);
    seive[0] = false;
    seive[1] = false;
    for (int i = 2; i < 250; i++) {
        for (int j = i * 2; j < 250; j += i) {
            seive[j] = false;
        }
    } // filling the prime numbers
    for (int i = 0; i < seive.size(); i++) {
        if (seive[i])
            prime.insert(i);
    }
}
// picking a random prime number and erasing that prime
// number from list because p!=q
int pickrandomprime()
{
    int k = rand() % prime.size();
    auto it = prime.begin();
    while (k--)
        it++;
    int ret = *it;
    prime.erase(it);
    return ret;
}
void setkeys()
{
    int prime1 = pickrandomprime(); // first prime number
    int prime2 = pickrandomprime(); // second prime number
    // to check the prime numbers selected
    // cout<<prime1<<" "<<prime2<<endl;
    n = prime1 * prime2;
    int fi = (prime1 - 1) * (prime2 - 1);
    int e = 2;
    while (1) {
        if (gcd(e, fi) == 1)
            break;
        e++;
    } // d = (k*Φ(n) + 1) / e for some integer k
    public_key = e;
    int d = 2;
    while (1) {
        if ((d * e) % fi == 1)
            break;
        d++;
    }
    private_key = d;
}

int main(int argc, char* argv[])
{
	try
	{
		//mongocxx::instance instance{};
		WSAInitializer wsaInit;
        primefiller();
        setkeys();
        SOCKET cv;
        cv = socket(AF_INET, SOCK_STREAM, 0);

        if (cv == INVALID_SOCKET)
            throw std::exception(__FUNCTION__ " - socket");

        struct sockaddr_in sa = { 0 };
        sa.sin_port = htons(2345);
        sa.sin_family = AF_INET;
        sa.sin_addr.s_addr = inet_addr(serverIP.c_str());

        int status = connect(cv, (struct sockaddr*)&sa, sizeof(sa));

        if (status == INVALID_SOCKET)
            throw std::exception("Cant connect to client");
        
		int num = (int(argv[1][0]) - 48) * 1000 + (int(argv[1][1]) - 48) * 100 + (int(argv[1][2]) - 48) * 10 + (int(argv[1][3]) - 48);
        std::string sn = std::to_string(n);
        std::string pkn = std::to_string(public_key);
        while (sn.size() != 6)
            sn = "0" + sn;
        while (pkn.size() != 2)
            pkn = "0" + pkn;
        std::string ans = "pk" + std::to_string(num) + std::to_string(public_key)+std::to_string(n);
        send(cv, ans.c_str(), strlen(ans.c_str()), 0);

		serve(num);
	}
	catch (std::exception& e)
	{
		std::cout << "Error occured: " << e.what() << std::endl;
	}

	system("PAUSE");
	return 0;
}



