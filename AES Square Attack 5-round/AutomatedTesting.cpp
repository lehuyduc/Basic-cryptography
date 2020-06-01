#include <iostream>
#include <fstream>
#include <time.h>
using namespace std;

int MAX_KEY = 255, ntest = 1000;

int makeInput()
{
    ofstream fo("key5.txt");
    for (int i=0; i<15; i++) {
        int val = rand() % (MAX_KEY+1);
        if (i%4==0) val = rand()%10;    // we make sure the values of the first row is small so that decrypter.cpp doesn't take too long to run
        fo << val << " ";
    }
    fo << 255 << "\n";  // do this so that MAX_KEY will always be 255
    fo.close();
}

int main()
{
    srand(time(NULL));
    int i, byte1, byte2;

    for (int t=1; t<=ntest; t++) {
        makeInput();

        #ifdef _WIN32
        system("encrypter");
        system("decrypter");
        #endif // _WIN32

        #ifdef __linux__
        system("./encrypter");
        system("./decrypter");
        #endif // __linux__

        /********************************/
        ifstream fin1("key5.txt");
        ifstream fin2("key5guess.txt");

        for (int i=0; i<16; i++) {
            fin1 >> byte1;
            fin2 >> byte2;
            if (byte1!=byte2) {
                cout << "Test failed\n";
                return 0;
            }
        }

        if (t%5==0) {
            cout << "\n***********************************\n\n";
            for (int i=1; i<=10; i++) cout << t << " test success\n";
            cout << "\n***********************************\n\n";
        }
    }
}
