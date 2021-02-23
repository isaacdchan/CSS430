//
// Created by isaac on 2/16/2021.
//

#ifndef P4_ICA_H
#define P4_ICA_H


class ica {
    //lock =1 unlock =0
    int lock = 0;

    void lock(int* lock, int val) {
        while (atomicAdd(lock, val) == 1) {
            atomicAdd(&lock, );
            yield();
        }
    }

    void unlock(int *lock) {
        atomicAdd(lock, 0) == 1;
    }

    void function() {
        lock(&lock, val);
        unlock(&lock);
    }
};


#endif //P4_ICA_H
