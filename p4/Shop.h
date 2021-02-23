#ifndef SHOP_ORG_H_
#define SHOP_ORG_H_

#include <pthread.h>
#include <iostream>
#include <sstream>
#include <string>
#include <queue>

using namespace std;

#define DEFAULT_NUM_WAITING_CHAIRS 3
#define DEFAULT_NUM_BARBERS 1

struct Barber {
    pthread_cond_t working_cond;
    pthread_cond_t paid_cond;
    pthread_cond_t sleeping_cond;
    int id;
    int customer_id;
    bool working;
    bool paid;

    Barber(int _id);
    void log();
};

class Shop {
public:
    Shop(int num_barbers, int num_chairs) :
            num_barbers(num_barbers),
            num_waiting_chairs(num_chairs),
            num_rejections(0) {
        init();
    };

    Shop() :
            num_waiting_chairs(DEFAULT_NUM_WAITING_CHAIRS),
            num_barbers(DEFAULT_NUM_BARBERS),
            num_rejections(0) {
        init();
    };

    // customer methods
    bool enterShop(int customer_id);   // return true only when a customer got a service
    void leaveShop(int customer_id);

//    barber methods
    void acceptCustomer(int id);

    void concludeCustomer(int id);

    int get_num_rejections() const;

    void close();

private:
    const int num_barbers;              // the max number of threads that can wait
    const int num_waiting_chairs;              // the max number of threads that can wait

    Barber** barbers;
    queue<int> customer_queue;  // includes the ids of all waiting threads
    int num_rejections; // the number of customers turned away

    // Mutexes and condition variables to coordinate threads
    // mutex is used in conjunction with all conditional variables
    // originally created array of pthread_cond_t
    pthread_mutex_t mutex{};
    pthread_cond_t cond_customers_waiting{};

    void init();

    static string intToString(int i);
    int waitingChairsAvailable();
    Barber* findAvailableBarber();
    Barber* customerToBarber(int customer_id);

    static void log(string role, int id, string message);
};

#endif