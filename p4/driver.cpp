#include <iostream>
#include <unistd.h>
#include "Shop.h"

using namespace std;

void* barber(void*);

void* customer(void*);

// ThreadParam class
class ThreadParam {
public:
    ThreadParam(Shop *shop, int id, int service_time) :
            shop(shop), id(id), service_time(service_time) {};

    Shop *shop;
    int id;
    int service_time;
};

int main(int argc, char *argv[]) {
    if (argc != 5) {
        cout << "Usage: num_barbers | num_chairs | num_customers | service_time" << endl;
        return -1;
    }
    int num_barbers = atoi(argv[1]);
    int num_chairs = atoi(argv[2]);
    int num_customers = atoi(argv[3]);
    int service_time = atoi(argv[4]);

    pthread_t barber_threads[num_barbers];
    pthread_t customer_threads[num_customers];
    Shop shop(num_barbers, num_chairs);

    for (int i = 0; i < num_barbers; i++) {
        auto* barber_param = new ThreadParam(&shop, i, service_time);
        pthread_create(&barber_threads[i], NULL, barber, barber_param);
    }

    for (int i = 0; i < num_customers; i++) {
        usleep(rand() % 1000);
//        usleep(rand() % 8);
        auto* customer_param = new ThreadParam(&shop, i, 0);
        pthread_create(&customer_threads[i], NULL, customer, customer_param);
    }

    // Wait for customers to finish and cancel barber
    for (auto &customer_thread: customer_threads) {
        pthread_join(customer_thread, NULL);
    }

    for (auto &barber_thread: barber_threads) {
        pthread_cancel(barber_thread);
    }

    cout << "# customers who didn't receive a service = " << shop.get_num_rejections() << endl;

    shop.close();
    return 0;
}

void *barber(void *arg) {
    auto *barber_param = (ThreadParam *) arg;
    Shop &shop = *barber_param->shop;
    int service_time = barber_param->service_time;
    int id = barber_param->id;
    delete barber_param;

    while (true) {
        shop.acceptCustomer(id);
        usleep(service_time);
        shop.concludeCustomer(id);
    }
}

void *customer(void *arg) {
    auto *customer_param = (ThreadParam *) arg;
    Shop &shop = *customer_param->shop;
    int id = customer_param->id;
    delete customer_param;

    if (shop.enterShop(id)) {
        shop.leaveShop(id);
    }
    return nullptr;
}