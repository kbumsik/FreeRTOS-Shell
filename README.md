# FreeRTOS-Shell
Unix Bash style shell for FreeRTOS, providing Command Line Interface(CLI) for microcontrollers.

The essential idea of this project is to create pusudo-filesystem for FreeRTOS's tasks, to easily manage tasks in runtime. Each FreeRTOS tasks stored in the filesystem are given inodes, and running tasks are given TID (task ID), similar to PID(process ID) in UNIX. The user can manage tasks using inode and TID.

Note taht the root of pusudo-filesystem is stored in RAM, not FLASH or other external storage devices, since many embedded system using microcontroller are not capable of writing FLASH while running or running codes from external storage.

## TODOs
1. implement `Ctrl+C` and `Ctrl+Z` for shell
1. Redirecting stdio to IO devices, such as USART ,LCD, and SD card.
2. create memory block and memory allocation system for the root, to manage RAM strictly.
3. moving to CMake project management.
4. make assert() for switching debugging option.
5. Add user command `fg` and `bg` for Shell.
