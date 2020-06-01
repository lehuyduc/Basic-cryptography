#include <iostream>
#include <fstream>
#include <time.h>
using namespace std;

int ntest = 1000;

int makeInput()
{
    ofstream fo("mainkey.txt");
    for (int i=0; i<16; i++) fo << rand()%256 << " ";
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
        ifstream fin1("mainkey.txt");
        ifstream fin2("mainkeyGuess.txt");

        for (int i=0; i<16; i++) {
            fin1 >> byte1;
            fin2 >> byte2;
            cout << byte1 << " " << byte2 << "\n";
            if (byte1!=byte2) {
                cout << "Test failed\n";
                return 0;
            }
        }

        if (t%5==0) {
            cout << "\n***********************************\n\n";
            for (int i=1; i<=3; i++) cout << t << " test success\n";
            cout << "\n***********************************\n\n";
        }
    }
}
