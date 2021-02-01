#include <iostream>
#include <signal.h>
#include <unistd.h>
using namespace std;

static bool alarmSet = false;
static int int_count = 0;

static void sig_alarm(int signo) {
	cout << "Alarm fired!!! " << endl;
	alarmSet = false;
}

static void sig_int(int signo) {
	if (int_count < 3) {
		int_count++;
		cout << "Ouch!!! " << endl;
  } else {
		exit(-1);
	}
}      
    

int main() {
	if (signal(SIGALRM, sig_alarm) == SIG_ERR) {
		cerr << "Error setting signal" << endl;
		exit(-1);
	}

	if (signal(SIGINT, sig_int) == SIG_ERR) {
		cerr << "Error setting signal" << endl;
		exit(-1);
	}

	while (true) {
		sleep(1);
		// when entering the while loop, since alarmSet == false:
			// set an alarm to go off in 5 seconds
			// set alarm to true to duplicate alarms are not sent
		// when the alarm goes off in 5, set alarm to false so it will be reset
		// if alarmSet is not set to true, the alarm countdown will constantly be set to 5
		// never reaching 0 and setting off sig_alarm

		if (! alarmSet) {
			alarm(5);
			// alarmSet = true;
		}
		cout << "Main loop... " << endl;
	}
}
