# Project Background

### Problem
- Barbershop simulation
- N waiting chairs (w_chair)
- M barber_ids + M barber chairs (b_chair)

### Pseudocode
```
while true:
    if customer in w_chair_queue:
        if b_chair available:
            pop top of queue
            place customer in b_chair 
            wake barber
    if customer enters
        if queue empty:
            if b_chair available:
                place customer in b_chair 
                wake barber
            else: //all barber_ids working
                place customer in queue
        else: //other customers waiting ahead of you
            if queue full:
                send customer away
            else:
                push customer to end of queue
```

# Files
### driver.cpp
Creates the shop, the barber_ids and the clients
1. Receive command line args  

| argv Index | Name | Description |
|----------|----------|----------|
| 1 | num_barbers | # of b_chairs/barber_ids working in shop |
| 2 | num_chairs | # of w_chairs |
| 3 | num_customers | # of customers who will enter the shop |
| 4 | service_time | barber's turnover rate (microseconds) |

2. Initializes a shop instance (implemented in Shop)
3. spawn num_barbers barber threads. barber_ids are passed shop pointer, a unique id, service_time
4. Loops, spawning num_customers customer threads every random interval `(usleep(rand() % 1000))`
5. Waits until all customer threads have been serviced and terminated
6. Terminate all barber threads

   
### Notes
Keep track of the order of customers  
Barbers automatically go to sleep when they finish a customer. *Even if there are customers in w_chairs
  