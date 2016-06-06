# FreeRTOS-Shell
Unix Bash style shell for FreeRTOS, providing CLI for microcontrollers.
The essential idea of this project is to create pusudo-filesystem for FreeRTOS's tasks, to easily manage tasks in runtime. Each tasks stored in the filesystem are given inodes, and running tasks are given TID (task ID), similar to PID(process ID) in UNIX. The user can manage tasks using inode and TID.

Note taht the root of pusudo-filesystem is stored in RAM, not FLASH or other external storage devices, since many embedded system using microcontroller are not capable of writing FLASH while running or running code from external storage.
