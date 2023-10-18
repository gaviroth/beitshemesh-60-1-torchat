#include "Encryption.h"
#include <WinSock2.h>
#include <Windows.h>
#include "buffer.h"

#pragma warning(disable:4996) // for inet_addr(got a error)

extern int n;
extern int public_key;
extern int private_key;
extern std::set<int> prime; // a set will be the collection of prime numbers, where we can select random primes p and q

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

// we will run the function only once to fill the globel set of prime numbers
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
    srand(time(NULL));

    int k = rand() % prime.size();
    auto it = prime.begin();
    while (k--)
        it++;
    int ret = *it;
    prime.erase(it);
    return ret;
}

void setkeys()//sets keys
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

// to encrypt the given number
long long int encrypt(double message, int pkToEncodWith, int nToEncodWith)
{
    int e = pkToEncodWith;
    long long int encrpyted_text = 1;
    while (e--) {
        encrpyted_text *= message;
        encrpyted_text %= nToEncodWith;
    }
    return encrpyted_text;
}

// to decrypt the given number
long long int decrypt(int encrpyted_text)
{
    int d = private_key;
    long long int decrypted = 1;
    while (d--) {
        decrypted *= encrpyted_text;
        decrypted %= n;
    }
    return decrypted;
}

// first converting each character to its ASCII value and
// then encoding it then decoding the number to get the
// ASCII and converting it to character
std::vector<int> encoder(std::string message, int pkToEncodWith, int nToEncodWith)
{
    std::vector<int> form;
    // calling the encrypting function in encoding function
    for (auto& letter : message)
        form.push_back(encrypt((int)letter,pkToEncodWith,nToEncodWith));
    return form;
}

std::string decoder(std::vector<int> encoded)
{
    std::string s;
    // calling the decrypting function decoding function
    for (auto& num : encoded)
        s += decrypt(num);
    return s;
}

void sendKeysToRouter()
{
    SOCKET _routerSocket;
    std::string serverIP = "127.0.0.1";

    std::string ns = "";
    std::string msg = "";
    std::string spkns = "";

    ns = std::to_string(n);
    spkns = std::to_string(public_key);

    while (ns.size() != 6)
        ns = "0" + ns;
    while (spkns.size() != 2)
        spkns = "0" + spkns;

    msg = "spk" + spkns + ns;

    // we connect to client that uses TCP. thats why SOCK_STREAM & IPPROTO_TCP
    _routerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (_routerSocket == INVALID_SOCKET)
        throw std::exception(__FUNCTION__ " - socket");   

    struct sockaddr_in sa = { 0 };
    sa.sin_port = htons(2345); // port that server will listen to
    sa.sin_family = AF_INET;   // must be AF_INET
    sa.sin_addr.s_addr = inet_addr(serverIP.c_str());

    // the process will not continue until the server accepts the client
    int status = connect(_routerSocket, (struct sockaddr*)&sa, sizeof(sa));

    if (status == INVALID_SOCKET)
        throw std::exception("Cant connect to client");

    send(_routerSocket, msg.c_str(), msg.size(), 0);  // last parameter: flag. for us will be 0. 

    closesocket(_routerSocket);
}