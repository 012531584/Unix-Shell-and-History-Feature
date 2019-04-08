# Unix-Shell-and-History-Feature

In this project, we will develop a simple UNIX shell by designing a C program to serve as a shell interface which are able to execute the following four actions: (1) accept user commands (2) execute each command in a separate process (3) provide the user a prompt after which the next command is entered (4) provide a history feature that allows user to access the most recently en-tered commands. One approach of implementing a shell interface is to create a separate child pro-cess that performs the command after the parent process first read what the user enters on the command line. Except where otherwise provided, the parent process waits for the child to exit be-fore continuing. Nevertheless, UNIX shells normally let the child process to run in the back-ground, or run at the same time, by specifying the ampersand (&) at the end of the command. The user’s command is executed by using one of the system calls in the exec() family, such as ex-ecvp(), and the child process is created by using the fork() system call. 


![image](https://user-images.githubusercontent.com/43554096/55717274-d4728500-59ad-11e9-98f4-b2cb4c43a000.png)
