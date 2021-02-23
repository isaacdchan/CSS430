#include "Shop.h"
#include <pthread.h>

// barber struct to hold each
Barber::Barber(int _id) {
    id = _id;
    customer_id = -1;

    pthread_cond_init(&working_cond, NULL);
    pthread_cond_init(&paid_cond, NULL);
    pthread_cond_init(&sleeping_cond, NULL);
}

void Shop::init() {
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_customers_waiting, NULL);

    barbers = new Barber*[num_barbers];

    for (int i = 0; i < num_barbers; i++) {
        barbers[i] = new Barber(i);
    }
}

string Shop::intToString(int i) {
    stringstream out;
    out << i;
    return out.str();
}

void Shop::log(string role, int id, string message) {
    if (role == "customer") {
        cout << "C" << id << ": " << message << endl;
    } else {
        cout << "B" << id << ": " << message << endl;
    }
}

int Shop::get_num_rejections() const {
    return num_rejections;
}

bool Shop::enterShop(int customer_id) {
    log("customer", customer_id, "enters the shop");
    pthread_mutex_lock(&mutex);

    // If all chairs are full then leave shop
    if (customer_queue.size() == num_waiting_chairs) {
        log("customer", customer_id, "leaves the shop because of no available waiting chairs.");
        num_rejections++;
        pthread_mutex_unlock(&mutex);
        return false;
    }

    Barber* barber = NULL;
    if (customer_queue.empty()) {
        barber = findAvailableBarber();
        log("customer", customer_id, "queue is empty");
    }
    if (barber == NULL) { // even though the queue is empty, the barber is occupied
        customer_queue.push(customer_id);
        log("customer", customer_id, "no barbers available. takes a waiting chair. seats available = " +
                                     intToString(waitingChairsAvailable()));

        // while each customer will be signaled that a barber is free
        // only the barber next in the queue can break out of a waiting state
        while (true) {
            pthread_cond_wait(&cond_customers_waiting, &mutex);
            int nextCustomerInLine = customer_queue.front();
            if (customer_id == nextCustomerInLine) {
                break;
            }
        }
        customer_queue.pop();
        barber = findAvailableBarber();
    }

    barber->customer_id = customer_id;

    // wake up the barber just in case if he is sleeping
    log("customer", customer_id, "waking up B" + intToString(barber->id));
    pthread_cond_signal(&(barber->sleeping_cond));

    pthread_mutex_unlock(&mutex);
    return true;
}

void Shop::leaveShop(int customer_id) {
    pthread_mutex_lock(&mutex);

    Barber* barber = customerToBarber(customer_id);
    // Wait for service to be completed
    // barber not being woken up correctly
    log("customer", customer_id, "waiting for the hair-cut to finish");
    pthread_cond_wait(&(barber->working_cond), &mutex);
    log("customer", customer_id, "haircut finished");

    // Pay the barber and signal barber appropriately
    barber->paid = true;
    log("customer", customer_id, "paying B" + intToString(barber->id));
    pthread_cond_signal(&barber->paid_cond);
    log("customer", customer_id, "says good-bye to B" + intToString(barber->id));

    pthread_mutex_unlock(&mutex);
}


void Shop::acceptCustomer(int barber_id) {
    Barber* barber = barbers[barber_id];
    pthread_mutex_lock(&mutex);

    // If no customers then barber can sleep
    if (customer_queue.empty() && barber->customer_id == -1) {
        log("barber", barber_id, "sleeping because no customers");
        pthread_cond_wait(&(barber->sleeping_cond), &mutex);
        log("barber", barber_id, "woken up");
    }

   // check if the customer sat down.
    if (barber->customer_id == -1) {
        pthread_cond_wait(&barber->sleeping_cond, &mutex);
    }

    int customer_id = barber->customer_id;
    log("barber", 0, "starts a hair-cut service for C" + intToString(customer_id));
    pthread_mutex_unlock(&mutex);
}

void Shop::concludeCustomer(int barber_id) {
    Barber* barber = barbers[barber_id];
    pthread_mutex_lock(&mutex);

    int customer_id = barber->customer_id;
    barber->paid = false;

    log("barber", barber_id, "finished haircut for C" + intToString(customer_id));
    pthread_cond_signal(&(barber->working_cond));
    log("barber", barber_id, "waiting to be paid by C" + intToString(customer_id));
    while (!barber->paid) {
        pthread_cond_wait(&(barber->paid_cond), &mutex);
    }
    log("barber", barber_id, "paid by C" + intToString(customer_id));

    //Signal to customer to get next one
    barber->customer_id = -1;
    log("barber", barber_id, "ready to receive another customer");
    pthread_cond_signal(&cond_customers_waiting);

    pthread_mutex_unlock(&mutex);  // unlock
}

int Shop::waitingChairsAvailable() {
    return num_waiting_chairs - customer_queue.size();
}

Barber* Shop::findAvailableBarber() {
    for (int i = 0; i < num_barbers; i++) {
        if (barbers[i]->customer_id == -1) {
            return barbers[i];
        }
    }
    return nullptr;
}

Barber* Shop::customerToBarber(int customer_id) {
    for (int i = 0; i < num_barbers; i++) {
        if (barbers[i]->customer_id == customer_id) {
            return barbers[i];
        }
    }
    return nullptr;
}
void Shop::close() {
    for (int i = 0; i < num_barbers; i++) {
        delete barbers[i];
    }
    delete barbers;
}