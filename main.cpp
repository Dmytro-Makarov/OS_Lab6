#include <iostream>
#include <thread>
#include <mutex>
#include <ctime>
#include <random>
#include <vector>

using namespace std;

const int MAX_N = 1000;
const int N = 100000000;

mt19937 e{random_device{}()};
uniform_int_distribution<int> dist(0, 500);

int A[MAX_N][MAX_N];
int B[MAX_N][MAX_N];
int C[MAX_N][MAX_N];

void multiplyM(int i, int j, int s){
    C[i][j] = 0;
    for (int k = 0; k < s; k++){
        C[i][j] += A[i][k]*B[k][j];
    }
    printf("C[%d][%d] = %d\n",i,j,C[i][j]);
}

void generator(int r, int c, int M[][MAX_N]){
    for (int i = 0; i < r; i++){
        for (int j = 0; j < c; j++){
            M[i][j] = dist(e);
            printf("\t%d",M[i][j]);
        }
        printf("\n");
    }
}

void matrixMultiplication()
{
    int rowA = 0;
    int rowB = 0;
    cout << "A: [n x m]" << endl;
    cout << "n:";
    cin >> rowA;
    cout << "m: ";
    cin >> rowB;
    generator(rowA, rowB, A);
    int colB = 0;
    cout << "B: [m x k]" << endl;
    cout << "k: ";
    cin >> colB;
    generator(rowB, colB, B);

    vector<thread> threads;
    for (int i = 0; i < rowA; i++){
        for (int j =0; j < colB; j++){
            threads.emplace_back(multiplyM, i, j, rowB);
        }
    }

    for (auto& t : threads) {
        t.join();
    }
}

int withMutex() {
    int value = 0;
    mutex mtx;

    auto increment = [&]() {
        int i = N;
        while (i--) {
            lock_guard<mutex> guard(mtx);
            ++value;
        }
    };

    thread t0(increment);
    thread t1(increment);

    t0.join();
    t1.join();

    return value;
}

int withoutProtection() {
    int value = 0;

    auto increment = [&]() {
        int i = N;
        while (i--) ++value;
    };

    thread t0(increment);
    thread t1(increment);

    t0.join();
    t1.join();

    return value;
}

void parallelThreading() {
    clock_t now;
    int result;

    now = clock();
    result = withoutProtection();
    printf("Incrementing a shared value (no protection) yielded: %d in %.3f sec\n", result, double(clock() - now) / CLOCKS_PER_SEC);

    now = clock();
    result = withMutex();
    printf("Incrementing a shared value (with a mutex)  yielded: %d in %.3f sec\n", result, double(clock() - now) / CLOCKS_PER_SEC);
}

int main(int argc, char **argv)
{
    while(true)
    {
        int task;
        printf("Choose Task\n "
               "Matrix multiplication with threads: %d\n"
               "Shared variable parallel threads: %d\n", 1, 2);
        printf("Task: ");
        cin >> task;
        switch(task){
            case 1:
                matrixMultiplication();
                break;
            case 2:
                parallelThreading();
                break;
        }
        return 0;
    }
}