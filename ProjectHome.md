This project contains source code related to the Critterbot Project at the University of Alberta's RLAI lab.

Code in the firmware/avr directory is for the ATMega 168 controllers that run the motor control loops and the power monitoring and battery charging systems.

Code in the firmware/arm directory is for the ARM sensor processor.  This includes a simple cooperative multitasking operating system, HAL components, and sensor interface components.

Code in the sim directory is the current development branch for the as-yet unnamed simulator that is being developed for the Critterbot Project.  This is written in Java and will probably eventually branch into it's own repository.