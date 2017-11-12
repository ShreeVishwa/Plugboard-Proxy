Test Environment:
    Ubuntu 16.04.3 LTS (Linux 4.10.0-38-generic x86_64)

Compiler:
    gcc version 5.4.0

Commands to run the code and sample output:

    1) First you have to install libssl-dev in your system
       sudo apt-get install libssl-dev
    2) After installing you can use make file to compile the program
    3) Commands and output without ssh
       server: nc -l -p 5001
       pbproxy-server: ./pbproxy -l 5002 -k mykey.txt localhost 5001
       pbproxy-clinet: ./pbproxy -k mykey.txt localhost 5002

       output:
       server:
            hi
            how are you
            I am fine
            How are you
       pbproxy-server:
            listen_port = 5002
            key_file = key.txt
            Destination ip = localhost
            Destination port = 5001
            Waiting for connection request
            Got the connection
            Waiting for connection request
            connected to sshsock
            Message sent to ssh client
       pbproxy-client:
            listen_port = -1
            key_file = key.txt
            Destination ip = localhost
            Destination port = 5002
            Connected to the server
            hi
            Message sent to ssh client
            how are you
            Message sent to ssh client
            I am fine
            How are you

    4) Commands and output with ssh
       pbproxy-server: ./pbproxy -k mykey.txt -l 2233 localhost 22
       pbproxy-client: ssh -o "ProxyCommand ./pbproxy -k mykey.txt 172.24.226.165 2233" localhost

       output:
            vishwatej@localhost's password: 
            Welcome to Ubuntu 16.04.3 LTS (GNU/Linux 4.10.0-38-generic x86_64)

            * Documentation:  https://help.ubuntu.com
            * Management:     https://landscape.canonical.com
            * Support:        https://ubuntu.com/advantage

            153 packages can be updated.
            5 updates are security updates.

            Last login: Sat Nov 11 19:07:35 2017 from 127.0.0.1
            vishwatej@ubuntu:~$ pwd
            /home/vishwatej
            vishwatej@ubuntu:~$ cd Documents
            vishwatej@ubuntu:~/Documents$ logout
            Connection to localhost closed.

Description:

    First the listening port,the key file, the destination ip and the destination port are accepted from the 
    command line through getopt. The program throws an error if either of them is missing. If listen_port is -1
    then, client function is called else server function is called.

    Client:
        1) Client creates a socket for connecting to the server
        2) Gets the server address from the destination port and the destination ip address and connects
           to the server.
        3) After establishing the connection set the stdin and socket file descripters to non-blocking
        4) Client reads the data from the stdin, encrypts using AES-CTR encryption and a random iv and 
           then sends it to the server through the socket.
        5) Client reads the data from the socket and decrypts it using the random iv and sends it to the stdout
        6) The above two steps are repeated continously inside the while loop untill the connection is terminated.

    Server:
        1) Create a parent socket and set its structure with the listen port and then bind it to the server address
        2) Start listening for the connection and accept them if any are available.
        3) When the connection is successfull then the server creates another socket for the ssh as a seperate
           process using fork.
        4) The ssh is set up using the destination ip and destination port specified
        5) When the ssh connection is established, the server reads the data from the client decrypts it using
           the iv's and forwards it to the ssh.
        6) The server reads the data from the ssh and encrypts it using the AES-CTR encryption and the iv's and sends it to
           the client.
        7) The above two steps are repeated in an infinite while loop untill the connection is terminated.

References:
    1) http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html
    2) http://www.gurutechnologies.net/blog/aes-ctr-encryption-in-c/
    3) https://stackoverflow.com/questions/29441005/aes-ctr-encryption-and-decryption
    4) http://beej.us/guide/bgnet/output/html/multipage/setsockoptman.html
    5) http://man7.org/linux/man-pages/man2/fcntl.2.html
    6) https://linux.die.net/man/2/accept
    7) http://man7.org/linux/man-pages/man7/pthreads.7.html
    8) https://stackoverflow.com/questions/985051/what-is-the-purpose-of-fork
    9) http://www.amparo.net/ce155/thread-ex.html
    10) https://stackoverflow.com/questions/25061656/when-is-the-arg-for-f-getfl-fcntl-command-required
    11) https://www.cs.cmu.edu/afs/cs/academic/class/15213-f99/www/class26/tcpserver.c