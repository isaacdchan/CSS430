# Documentation

### Barber Structs

Based on how much state information each Barber had, I thought it 
made sense to encapsulate this info into a struct.

Each struct stores the following info:

| var_name | Type | Description |
| ----------- | ----------- |----------- |
| barber_id | int | the index of the barber when initialized |
| customer_id | int | customer currently being serviced. -1 if sleeping/idle |
| working_cond | pthread_cond_t | signals when the barber has started/stopped working |
| paid_cond | pthread_cond_t | signals when a customer has paid the barber |
| sleeping_cond | pthread_cond_t | signals when a barber is sleeping <br > used by the customer to wake up the barber |

The `paid`, `sleeping`, and `working` flags were omitted. I found that
the thread conditions were sufficient.

### Shop Class
The Shop implementation is mostly unchanged. the only modifications were to integrate the barber structs.

### Pseudo Code
1. Parse user arguments and initialize Shop
   * Initialize barber array and thread conditions
2. Initialize barber threads
   * All barbers will begin in a sleeping state given no customers have been created yet
3. For each customer in `num_customers`
   * Wait a random interval of time
   * Enter the shop and check if the queue is empty and if a barber is available
       * If a barber is available:
           * Wake them and update their `customer_id`
           * Wait until they finish servicing
       * Else:
           * If the waiting queue is full:
               * leave shop and increment `num_rejections`
           * Else:
               * Go to the end of the queue
               * While waiting: if a barber signals that they are available and 
             if the customer is next in line - see `If barber is available:`
4. If a barber is woken up, check if the customer sat down and service them for `service_duration`
   * Signal the customer when done servicing and wait to be paid
5. When a customer is signaled that the service is over, pay the barber and leave the shop
6. When the barber is paid, set `customer_id = -1`
   * Check the queue. Sleep if queue is empty. Otherwise repeat step 4
7. When all customers have been serviced or turned away, delete all barbers and the barber array

# Analysis

One possible extension would be for each barber to keep track of 
customers serviced and how long they spent in a "sleep" state

One possible limitation is how each customer waits in the queue. Rather than
only the next in line customer thread being woken up when a barber finishes,
every customer thread has to be woken up because they observe the
same thread condition `barber_available`. They all check if it is their turn next and
if not - they go back to waiting.

Approximately 95 waiting chairs were required for 1 barber service all 200 customers  
* This result means that 95 waiting chairs are required for the barber to finish the first
105 customers before no more customers arrived
* The barber must finish `num_customers - (num_waiting_chairs * 2)` customers before the final customer arrives
* If there were 201 customers, there's a good chance that while the barber was finishing
customer 115, the final customer would have arrived, saw that all 95 chairs were occupied, and left

Approximately 5 barbers were required to service all 200 customers without a waiting chair
* This results means that 5 barbers (with this service time) are required to reach an equilibrium where any number
of customers can arrive with no waiting chairs and still be served
* This means that even if all 5 barbers are occupied, the probability that one of them will
finish before the next customer arrives is close to 100%


