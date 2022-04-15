/**
 * evaluate race condition value.
 * @author Erez Polak
 */

#include <iostream>
#include <fstream>
#include <thread>

using namespace std;

/**
 * instead of global value i used a binary file to contain the global value.
 * that way all the threads use it the same way.
 */
const string PATH = "C:\\Users\\pwlqr\\source\\repos\\ThreadsAndBinaryFiles\\";

/**
 * choose if the artificial context switch happen, if so the value will be the minimal value from all the options.
 * if it's disabled the answer will be determined be the way yhr system manages the threads.
 */
const bool CONTROL_CONTEXT_SWITCH = true;
/**
 * if the artificial context switch is disabled, there is a need to statistically analyze the results,
 * so there is an option to make the code repeat itself.
 */
const int STATISTICS_ITERATION = 1;
/**
 * the time the program waits for the other process to continue, important to be high!!
 * if the value is low, the threads wont wait enough before resuming, and the expected output wont be as expected!!
 */
const int TIME_TO_WAIT = 100000000;

/**
 * making a thread to busy wait the other one.
 * the function that making a thread to do nothing for a while, allowing the other thread to continue.
 * @param iteration the actual number of iteration
 * @param expectedIteration the expected  number of iteration for the context switch.
 * @param threadId the actual number of thread.
 * @param expectedTreadId the expected number of thread for the context switch.
 */
void contextSwitch(int iteration , int expectedIteration , std::thread::id threadId, int expectedTreadId){
    if(CONTROL_CONTEXT_SWITCH &&
        iteration == expectedIteration &&
        threadId == (std::thread::id)expectedTreadId ){
            for (int j = 0; j < TIME_TO_WAIT; ++j) {  }
        }
}

/**
 * the function that being done by both the threads concretely.
 * the function is incriminating the global value by 1 at the time for 10 times.
 */
void inc10times() {

    std::thread::id this_id = std::this_thread::get_id();

    for (int i = 0; i < 10; i++) {

        contextSwitch(i ,9 , this_id , 3);

        //extraction
        ifstream output(PATH + "globalElement.bin", ios::binary);
        int out;
        output.read((char *) &out, sizeof(out));
        output.close();

        contextSwitch(i ,0 ,this_id ,2 );
        contextSwitch(i ,9 ,this_id ,3 );

        //prints the status of the iteration
        if(CONTROL_CONTEXT_SWITCH)
            cout << "thread: " << this_id << ", iteration: " << i << ", element value for incrementation: " << out << endl;

        //incrementation
        int in = ++out;

        //insert back to the file.
        ofstream input(PATH + "globalElement.bin", ios::binary);
        input.write((char *) &in, sizeof(in));
        input.close();

        contextSwitch(i ,0 ,this_id ,2 );

    }

}

int main() {

    for (int i = 0; i < STATISTICS_ITERATION; ++i) {

        //initialization of the file
        int in = 0;
        ofstream input(PATH + "globalElement.bin", ios::binary);
        input.write((char *) &in, sizeof(in));
        input.close();

        //threading
        thread t1(inc10times);
        thread t2(inc10times);
        t1.join();
        t2.join();

        //result extracting the final value out of the file.
        ifstream output(PATH + "globalElement.bin", ios::binary);
        int out;
        output.read((char *) &out, sizeof(out));
        output.close();
        cout << "the final value is:  " << out << endl;

    }

    return 0;
}
