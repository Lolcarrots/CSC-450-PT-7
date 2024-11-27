#include <thread>
#include <iostream>
#include <mutex>

using namespace std;

class Counter {
private:
    // Using a mutex to protect the shared count from incorrect thread access
    mutable mutex countMutex;
    int countValue{0};
    bool countUpDone{false};
    // A boolean for telling the first thread to stop counting up
    bool stopCounting{false};
    thread::id firstThreadId;

public:
    // Identifying the first thread so the print statements can specify which thread is being used
    void setFirstThreadId(thread::id id) {
        lock_guard<mutex> lock(countMutex);
        firstThreadId = id;
    }
    // Printing which thread is active to prove the correct thread is being utilized
    string getThreadLabel() const {
        lock_guard<mutex> lock(countMutex);
        return (this_thread::get_id() == firstThreadId) ? "Thread 1: " : "Thread 2: ";
    }
    // Used for counting up
    void countUp() {
        while (true) {
            // Ensuring a deadlock does not occur by getting the label before the lock
            string label = getThreadLabel();
            lock_guard<mutex> lock(countMutex);
            if (stopCounting || countValue >= 20) break;
            cout << label << ++countValue << endl;
            if (countValue == 20) {
                countUpDone = true;
                break;
            }
        }
    }
    // Used for counting down
    void countDown() {
        while (true) {
            // Getting the label before the lock
            string label = getThreadLabel();
            lock_guard<mutex> lock(countMutex);
            if (stopCounting || (countUpDone && countValue <= 0)) break;
            if (countUpDone) {
                cout << label << --countValue << endl;
            }
        }
    }
};

int main() {
    try {
        Counter counter;
        // Directly using countUp for the first thread
        thread thread1(&Counter::countUp, &counter);
        counter.setFirstThreadId(thread1.get_id());
        // Directly using countDown for the second thread
        thread thread2(&Counter::countDown, &counter);
        // Waiting for both thread1 and thread2 to finish
        thread1.join();
        thread2.join();
        cout << "Counting completed!" << endl;
    } catch (...) { 
        cerr << "An error occurred!" << endl;
        return 1; 
    }
    return 0;
}