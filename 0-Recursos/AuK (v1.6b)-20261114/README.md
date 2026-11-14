# AuK
Real-time microkernel

Another Real Time Microkernel (for dsPIC33E and dsPIC33F families).
Another microkernel (AuK) provides the ability to develop software systems based on the paradigm 
of cooperating processes. Processes are activated using the priority-based scheduling (PS) policy. 
Processes can be synchronized using semaphores and mutexes. The Immediate Ceiling Priority Protocol 
(ICPP) locking policy is used when permanent shared resources (mutexes in AuK) are used.

To prepare a new project with AμK, it is necessary to place all AμK source files (AuK_config.h, 
AuK_structures.h, AuK.h and AuK.c) into the working directory. It is also necessary to select a 
Hardware Abstraction Layer (HAL) file to place in the same directory. Currently it is possible to work 
with devices of two dsPIC33 families and then two HAL files are offered (dsPIC33E.s and dsPIC33F.s).

By default, AμK is configured in the AuK_config.h file to work with a maximum of 10 processes and 
a maximum memory size of 3000 words (6000 bytes) to be used for the processes stack. If these values 
are not sufficient for your project, they can be changed in AuK_config.h.

The main file of the new project using AμK must contain the AμK.h file. It is recommended to create 
a function to configure the clock signal and a specific routine for each peripheral device to be used 
in the project. Before using AμK, it is mandatory to run the init_AuK service. This service is used 
to declare the instruction cycle frequency (Fcyc) in Hz and the period to execute the scheduler in seconds.  
After that, it is necessary to create all the processes of the system, all the semaphores and mutexes 
needed and finally, to run AμK, it is necessary to execute the sart_AuK service.

There is a detailed description of each AμK service in the file AuK.h.

This documentation is under development and will be updated in the near future.
