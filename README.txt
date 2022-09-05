CIS 3110 Operating Systems A1
Olivia Biancucci

DESCRIPTION + INSTALLATION:
My program simulates a UNIX shell in C that supports different commands. After downloading the "myShell.c" and "makefile" files, type "make" in the terminal to compile and then "./myShell" to execute.

FUNCTIONS:
-"exit" terminates the shell
-a command with or without arguments, such as ls or ls -lt
-adding an "&" at the end of a command will execute it in the background
-adding a ">" plus a file name after a command will redirect the output to a file
-adding a "<" plus a file name after a command will redirect the input from a file
-adding a "|" in between two commands will pipe the output of the first command to the input of the second command
-"history" will print out all the previous input to the shell (this can also be redirected to a file)
-"cd" plus a directory will change the working directory

ASSUMPTIONS: 
My program assumes that the character limit of a command is 256 characters. It also assumes that a file name will not be longer than 50 characters.
