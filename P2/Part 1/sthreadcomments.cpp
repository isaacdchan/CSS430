#include <setjmp.h> // setjmp( )
#include <signal.h> // signal( )
#include <unistd.h> // sleep( ), alarm( )
#include <stdio.h>  // perror( )
#include <stdlib.h> // exit( )
#include <iostream> // cout
#include <string.h> // memcpy
#include <queue>    // queue

// setjmp(main_env) is a predefined env that restores the process to 
// some default values

// initializes the sthread scheduler
// must run before ANY threads are launched
// saves the env before the scheduler starts as main_env
#define scheduler_init() { \
   if (setjmp(main_env) == 0) { \
      scheduler(); \
   } \
}

// jumpstarts all sthreads after they've been launched
// saves the env before the scheduler starts as main_env
#define scheduler_start() { \
   if (setjmp(main_env) == 0) { \
      longjmp( scheduler_env, 1 ); \
   } \
}

// Captures the current thread's jmp_env and activation record into cur_tcb
// Helper function called from sthread_init( ) and sthread_yield( )
#define capture() { \
   register void *sp asm ("sp"); \
   register void *bp asm ("bp"); \
   cur_tcb->sp_ = sp; \
   cur_tcb->size_ = (int)((long long int)bp - (long long int)sp); \
   cur_tcb->stack_ = malloc(cur_tcb->size_); \
   memcpy(cur_tcb->stack_, sp, cur_tcb->size_); \
   thr_queue.push(cur_tcb); \
}

// called by each user thread to voluntarily yield the CPU
// only after an timer interrupt, calls capture() and goes back to the scheduler
// when the control comes back from the scheduler, retrieve this thread activation record from cur_tcb->stack.
#define sthread_yield() { \
   if (alarmed) { \
      alarmed = false; \
      if (setjmp(cur_tcb->env_) == 0) { \
         capture(); \
         longjmp(scheduler_env, 1); \
      } else { \
         memcpy(cur_tcb->sp_, cur_tcb->stack_, cur_tcb->size_); \
      } \
   } \
}

#define sthread_init() { \
   if (setjmp(cur_tcb->env_) == 0) { \
      capture(); \
      longjmp(main_env, 1); \
   } \
   memcpy(cur_tcb->sp_, cur_tcb->stack_, cur_tcb->size_); \
}

#define sthread_create(function, arguments) { \
   if (setjmp(main_env) == 0) { \
      func = &function; \
      args = arguments;	\
      thread_created = true; \
      cur_tcb = new TCB(); \
      longjmp(scheduler_env, 1); \
   } \
}

#define sthread_exit() { \
   if (cur_tcb->stack_ != NULL) { \
      free(cur_tcb->stack_); \
   } \
   longjmp(scheduler_env, 1); \
}

using namespace std;
static jmp_buf main_env;
static jmp_buf scheduler_env;
const int kTimeQuantum = 5;

// Thread control block
class TCB {
   public:
      TCB() : sp_(NULL), stack_(NULL), size_(0) {}
      jmp_buf env_;  // the execution environment captured by set_jmp()
      void* sp_;     // the stack pointer 
      void* stack_;  // the temporary space to maintain the latest stack contents
      int size_;     // the size of the stack contents
};

static TCB* cur_tcb;   // the TCB of the current thread in execution

// The queue of active threads
static queue<TCB*> thr_queue;

// Alarm caught to switch to the next thread
static bool alarmed = false;
static void sig_alarm(int signo) {
   alarmed = true;
}

// A function to be executed by a thread
void (*func)(void *);
void *args = NULL;
static bool thread_created = false;

static void scheduler() {
   // invoke scheduler
   if (setjmp(scheduler_env) == 0) {
      cout << 6 << cur_tcb->env_ << endl;

      // cerr << "scheduler: initialized" << endl;

      // set the sig_alarm() func to handle alarm signals
      if (signal(SIGALRM, sig_alarm) == SIG_ERR) {
         perror("signal function");
         exit(-1);
      }
      // load the main environment
      longjmp(main_env, 1);
   }

   // check if it was called from sthread_create()
   if ( thread_created == true ) {
      thread_created = false;
      (*func)(args);
   }

   // restore the next thread's environment
   if ((cur_tcb = thr_queue.front()) != NULL) {
      // cout << "Popped thread from queue:" << thr_queue.front() << endl;
      thr_queue.pop();

      // allocate a time quantum
      // set an alarm to go off in 5 seconds
      // cout << "Set alarm" << endl;
      alarm(kTimeQuantum);

      // return to the next thread's execution
      // cout << "Long jumping to:" << cur_tcb->env_ << endl;
      longjmp(cur_tcb->env_, 1);
   }

   // no threads to schedule, simply return
   // cerr << "scheduler: no more threads to schedule" << endl;
   longjmp(main_env, 2);
}
