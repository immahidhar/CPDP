mycd, mypwd and myexit are implemented as internal commands. 

mytimeout is implemented as external command.

Here is how you call those commands - 
```
mycd [arg]
mypwd
myexit
mytimeout.x secs cmd args...
```

Limitation - Piping only works upto 2 pipes.


To compile all files, execute the following in terminal
>make

mytoolkit: To compile mytoolkit program, execute the following in terminal
>make mytoolkit

mytimeout: To compile mytimeout program, execute the following in terminal
>make mytmytimeoutime

To cleanup the compiled executables, execute the following in terminal
> make clean