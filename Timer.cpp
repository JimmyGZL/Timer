#include<ctime>
#include<iostream>
using namespace std;
int main(){
    int n;
    cin >> n;
    time_t start = time(0);
    while (time(0) - start != n);
    cout << "Done!" << endl;
    return 0;
}