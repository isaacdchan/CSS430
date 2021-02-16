1. Purpose
	- Simulate a simple many-to-one thread library and the scheduler that assigns time to each thread.
	- Implement saving and restoring each threads environment when context switching
		- Ex. currently on thread1. Thread1 becomes IO bound, save Thread1’s environment registers and stack, load Thread2
		- Use signal and alarm to guarantee a minimum execution time for each thread

2. User threads
	- Different from kernel threads and LWP. Only visible within each process
	- Each thread at minimum has it's own registers and stack, which are stored in a TCB (thread control block) data structure
		- Registers are stored in the jmp_env data structred. Saved and loaded using setjmp() and longjmp()
	- Each thread's stack is the stack space between the current SP and its BP

3. Saving an Execution Environment
	- Function calls add a new activation record to the top of the current stack
		- After curr function finishes, it uses the included stack return address to return control back to the caller
	- If funcA() calls funcB(), which calls funcC(), it is possible for funcC() to return directly to funcA(), rather than cascading back.
		- setjmp(): before calling funcB, save funcA's current CPU register contents to the heap
			- contents include PC, SP, BP
		- longjump(): after funcC finishes, load funcA's saved registers
	- funcA() can setjmp(), launch thread that runs funcB(), have threadB longjmp() back to the saved env, then funcA() can launch a thread that runs funcC()
		- funcX can each save their registers (PCx, SPx, BPx) in ther own envX 
	- Linux obscures SP and BP requiring ASM declrations to query and store
	- Every new thread will have a TCB object created with out. Everytime a thread is saved or restored it will use TCB

4. Uses cooperative scheduling - threads will run until they voluntary relinquish
	- However, each thread will have a minimum amount of execution time, when even if they yield, no context switch occurs
	- Every time the program switches threads, start a countdown that after five seconds, sets a global variable to true. Execution will switch to a different thread during yiel if the global variable is true
	