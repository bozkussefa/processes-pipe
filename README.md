**INTRODUCTION**

To hide the effects of interrupts, operating systems provide a conceptual model consisting of
sequential processes running in parallel. Processes can be created and terminated dynamically. Each
process has its own address space. Processes can communicate with each other using interprocess
communication primitives, such as semaphores, monitor, pipes or messages. These primitives are
used to ensure that no two processes are ever in their critical regions at the same time, a situation
that leads to chaos. A process can be running, runnable (ready), or blocked and can change state
when it or another process executes one of the interprocess communication primitives

A pipe is a form of redirection that is used in Linux and other Unix-like operating systems to send the
output of one program to another program for further processing.

**EXPERIEMENT**

The aim of this experiment is to make you familiar with some concepts of processes and interprocess
communication and have practical experience on interprocess communication programming using
UNIX and POSIX facilities

You will implement a multi process encryption application that communicates via pipes. The main
application will fork next child after it created its two pipelines, on for next child and one for last
child. The child processes will communicate with pipes, which are defined before fork() operations.
Each process will process the data provided from the previous child process and pass the result to
the next child process. In this context, each process will have 2 pipe, one for reading and one for
writing.

The main process reads a plain text from the file named plain.txt and a 128-bit key from the file
named key.txt. A key must be 128-bit length but plain text may be longer than 128-bit, but it
contains exactly fixed-size blocks, e.g. 64 bytes of plain text consists of 8 blocks.
After reading both key and plain data, the main process first writes key file and after 128-bit plain
text blocks by dividing plain data to the next-child pipe. Then this process starts to read encrypted
blocks from the last child process and writes them to encrypt.txt. All these three files will be in
binary format.
The second process applies XOR operation. After reading plain block from main process pipe and
applying XOR between the same length plain and key blocks, this process writes the intermediate
result to the next child pipe.
The third process applies permutation. In this step, data of length 8 bytes is divided into two blocks
as the first 4 and last 4 bytes, and the byte data having the same indices as the two blocks are
replaced with each other. For example, the byte at the 0th position replaces the byte at the 4th
indent and the 1st byte and the 5th indent. After permutation, the result block is written to nextchild
pipe.
The fourth process applies substitution. In this step, each byte of the block is replaced with a
substitution value which is provided by a substitution-box. This substitution-box will be given in the
context of the assignment. After substitution, the result block is written to next-child pipe.
In addition to all this scenario, each process writes to the intermediate results to a file after all data
has been processed. The name of this file is in the format “process- <id> .txt”. In this
context, you are expected to develop an approach for triggering the writing all blocks received to the
file that data blocks sent over the pipe are completed. At the end of this writing process can
terminate itself