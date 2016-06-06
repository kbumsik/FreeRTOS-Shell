# FreeRTOS-Shell
Unix Bash style shell for FreeRTOS, providing Command Line Interface(CLI) for microcontrollers.

The essential idea of this project is to create pusudo-filesystem for FreeRTOS's tasks, to easily manage tasks in runtime. Each FreeRTOS tasks stored in the filesystem are given inodes, and running tasks are given TID (task ID), similar to PID(process ID) in UNIX. The user can manage tasks using inode and TID.

Note taht the root of pusudo-filesystem is stored in RAM, not FLASH or other external storage devices, since many embedded system using microcontroller are not capable of writing FLASH while running or running codes from external storage.

## TODOs
1. implement Shell.
2. implement killing and resuming tasks.
4. Add user tasks.
5. implement redireting stdout to IO devices, such as USART ,LCD, and SD card.
6. create memory block and memory allocation system for the root, to manage RAM strictly.
