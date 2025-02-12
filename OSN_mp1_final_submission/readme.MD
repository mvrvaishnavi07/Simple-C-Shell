
# spec 1

- for entire prompt i gave pink color onlyy

# spec 2

- MAX ip len is assumed to be 4096 chars
- 

# spec 3
-  assuming  4096 characters as limit to the number of characters that will be included in the directory path name
- for printing absolute path is used always
- I have checked all the testcases that mentioned in doc all those are working fine , I am not handling edge cases 

# spec 4

- assuming 4096 as max path len
- I have checked all the testcases that mentioned in doc all those are working fine , I am not handling edge cases 
- when -l is given color coding is given for its description
- 1024 maximum entries to be present in a directory
- reveal with file will work only if -l flag is given in cmd

# spec 5

- iam storing error cmds too (it includes "\n" too)
- for command_log file i have given abs path of shell home directory
- in tokenise if home_dir is null then it is log execute
- I have checked all the testcases that mentioned in doc all those are working fine , I am not handling edge cases

# spec 6
- in background if some error command is run , then it also prints the pid (as it is not mentioned to handle this explicitily)
- if & is given at end of my custom command errors are not printed , they ignore &
- background process interrupts and prints completion msg
- for background process exit status im not printing process name im printing its pid
- for sleep > 2 s the prompt may remain same for next also
- I have checked all the testcases that mentioned in doc all those are working fine , I am not handling edge cases

# spec 7
- I have checked all the testcases that mentioned in doc all those are working fine , I am not handling edge cases
- max path : 4096

# spec 8
- I have checked all the testcases that mentioned in doc all those are working fine , I am not handling edge cases
- i am printing absolute paths

# spec 10 , 11, 12
- I have checked all the testcases that mentioned in doc all those are working fine , I am not handling edge cases
- works correctly only for system commands
- I/O redirection is used only at the end of the command and all its parameters.

# spec 13
- I have checked all the testcases that mentioned in doc all those are working fine , I am not handling edge cases
- max process - 4096
- status code of 'T' implies stopped. Everything else can be considered to be running.

# spec 14
- I have checked all the testcases that mentioned in doc all those are working fine , I am not handling edge cases
- in vs code some errors are being shown but it runs without any errors
- when ctrl + d is pressed shell just exit and not print anything
- for ctrl + z , ctrl + c , enter is to be given

# spec 15
- I have checked all the testcases that mentioned in doc all those are working fine , I am not handling edge cases

# spec 16
- for neonate x\n must be given
- ip format is followed exactly
- I have checked all the testcases that mentioned in doc all those are working fine , I am not handling edge cases

# spec 17
- I have checked all the testcases that mentioned in doc all those are working fine , I am not handling edge cases
-html tags are not removed as we can print the html tags as well if there are any.
- initail buffer size : 4096


# general
- almost im assuming all ips are of max of size = 4096
- lexicographical acc to command name here
- some debug statements may be present











