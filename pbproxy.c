#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <fcntl.h>

struct ctr_state
{
    unsigned char ivec[AES_BLOCK_SIZE];
    unsigned int num;
    unsigned char ecount[AES_BLOCK_SIZE];
};

//struct to pass multiple arguments to a thread
// struct socket_info{
//     int sock1id;
//     int sock2id;
//     unsigned char* key;
// };

// struct connection_info {
// 	int sockfd;
// 	struct sockaddr_in ssh_addr;
// 	unsigned char *key;
// };

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int init_ctr(struct ctr_state *state, const unsigned char iv[16])
{
    /* aes_ctr128_encrypt requires 'num' and 'ecount' set to zero on the
    * first call. */
    state->num = 0;
    memset(state->ecount, 0, AES_BLOCK_SIZE);
 
    /* Initialise counter in 'ivec' to 0 */
    memset(state->ivec + 8, 0, 8);
 
    /* Copy IV into 'ivec' */
    memcpy(state->ivec, iv, 8);
}

// void *stdins_handler(void *temp){
//     int buff_read, buff_write;
//     unsigned char iv[AES_BLOCK_SIZE];
//     struct ctr_state state;
//     int set_key;
//     AES_KEY key_aes;
//     struct socket_info *info = (struct socket_info*)temp;
//     int newsock = info->sock1id;
//     int sshsock = info->sock2id;
//     unsigned char* enc_key = info->key;
//     char buffer[4096];
//     char cipher[4096];
//     bzero(buffer,4096);
//     bzero(cipher,4096);
//     // int buff_read;
//     while(1){
//         buff_read = read(sshsock,buffer,4096);
//         if(buff_read <= 0) error("Couldn't read from the sshsock");
//         if(strlen(buffer) > 0){
//             set_key = AES_set_encrypt_key(enc_key, 128, &key_aes); //set the encryption key
//             if(set_key < 0) error("Counldn't set the encrypted key");
//             if(!RAND_bytes(iv, AES_BLOCK_SIZE)) //creating the iv with random bytes
//             {
//                 fprintf(stderr, "Could not create random bytes.");
//                 exit(1);
//             }
//             init_ctr(&state, iv);
//             memcpy(cipher,iv,AES_BLOCK_SIZE);
//             AES_ctr128_encrypt(buffer, cipher + AES_BLOCK_SIZE, buff_read, &key_aes, state.ivec, state.ecount, &state.num);
//             buff_write = write(newsock,cipher,AES_BLOCK_SIZE+buff_read);
//             if(buff_write < 0) error("Couldn't write to the socket");
//             // encrypt(buffer,cipher,enc_key,buff_read);
//             // if(write(newsock,cipher,strlen(cipher)) <= 0) error("Couldn't write to the socket");
//         }
//     }
//     free(cipher);
// }

// void *sockets_handler(void *temp){
//     int buff_read, buff_write;
//     unsigned char iv[AES_BLOCK_SIZE];
//     struct ctr_state state;
//     int set_key;
//     AES_KEY key_aes;
//     struct socket_info *info = (struct socket_info*)temp;
//     int newsock = info->sock1id;
//     int sshsock = info->sock2id;
//     unsigned char* enc_key = info->key;
//     char buffer[4096];
//     char cipher[4096];
//     bzero(buffer,4096);
//     bzero(cipher,4096);
    
//     while(1){
//         buff_read = read(newsock,cipher,4096+AES_BLOCK_SIZE);
//         if(buff_read <= 0) error("Couldn't read from the socket in server");
//         if(strlen(cipher) > 0){
//             set_key = AES_set_encrypt_key(enc_key, 128, &key_aes); //set the encryption key
//             if(set_key < 0) error("Counldn't set the encrypted key");
//             memcpy(iv,cipher,AES_BLOCK_SIZE);
//             init_ctr(&state, iv);
//             AES_ctr128_encrypt(cipher + AES_BLOCK_SIZE, buffer, buff_read - AES_BLOCK_SIZE, &key_aes, state.ivec, state.ecount, &state.num);
//             buff_write = write(sshsock,buffer,buff_read-AES_BLOCK_SIZE);
//             if(buff_write < 0) error("Couldn't write to the sshsock");
//             // decrypt(cipher,buffer,enc_key, buff_read);
//             // if(write(sshsock,buffer,strlen(buffer)) <= 0) error("Couldn't write to the server");
//         }
//     }
//     free(buffer);
// }

// void *stdin_handler(void *temp){
//     int buff_read, buff_write;
//     unsigned char iv[AES_BLOCK_SIZE];
//     struct ctr_state state;
//     int set_key;
//     AES_KEY key_aes;
//     struct socket_info *info = (struct socket_info*)temp;
//     int newsock = info->sock1id;
//     int sockfd = info->sock2id;
//     unsigned char* enc_key = info->key;
//     char buffer[4096];
//     char cipher[4096];
//     bzero(buffer,4096);
//     bzero(cipher,4096);
//     // int buff_read;
//     while(1){
//         buff_read = read(STDIN_FILENO,buffer,4096);
//         if(buff_read <= 0) error("Couldn't read from the stdin");
//         // if(!fgets(buffer,4096,stdin)) error("Couldn't read from stdin");
//         if(strlen(buffer) > 0){
//             set_key = AES_set_encrypt_key(enc_key, 128, &key_aes); //set the encryption key
//             if(set_key < 0) error("Counldn't set the encrypted key");
//             if(!RAND_bytes(iv, AES_BLOCK_SIZE)) //creating the iv with random bytes
//             {
//                 fprintf(stderr, "Could not create random bytes.");
//                 exit(1);
//             }
//             init_ctr(&state, iv);
//             memcpy(cipher,iv,AES_BLOCK_SIZE);
//             AES_ctr128_encrypt(buffer, cipher + AES_BLOCK_SIZE, buff_read, &key_aes, state.ivec, state.ecount, &state.num);
//             buff_write = write(sockfd,cipher,AES_BLOCK_SIZE + buff_read);
//             if(buff_write < 0) error("Couldn't write to the socket");
//             // encrypt(buffer,cipher,enc_key, strlen(buffer));
//             // if(write(sockfd,cipher,strlen(buffer)) <= 0) error("Couldn't write to the socket");
//         }
//     }
//     free(cipher);
// }

// void *socket_handler(void *temp){
//     unsigned char iv[AES_BLOCK_SIZE];
//     struct ctr_state state;
//     int set_key;
//     AES_KEY key_aes;
//     struct socket_info *info = (struct socket_info*)temp;
//     int newsock = info->sock1id;
//     int sockfd = info->sock2id;
//     unsigned char* enc_key = info->key;
//     char buffer[4096];
//     char cipher[4096];
//     bzero(buffer,4096);
//     bzero(cipher,4096);
//     int buff_read, buff_write;
//     while(1){
//         buff_read = read(sockfd,cipher,4096);
//         if(buff_read <= 0) error("Couldn't read from the socket in client");
//         if(strlen(cipher) > 0){
//             set_key = AES_set_encrypt_key(enc_key, 128, &key_aes); //set the encryption key
//             if(set_key < 0) error("Counldn't set the encrypted key");
//             memcpy(iv,cipher,AES_BLOCK_SIZE);
//             init_ctr(&state, iv);
//             AES_ctr128_encrypt(cipher + AES_BLOCK_SIZE, buffer, buff_read - AES_BLOCK_SIZE, &key_aes, state.ivec, state.ecount, &state.num);
//             buff_write = write(STDOUT_FILENO,buffer,buff_read-AES_BLOCK_SIZE);
//             if(buff_write < 0) error("Error writing to socket");
//             // decrypt(cipher,buffer,enc_key, buff_read);
//             // if(write(STDOUT_FILENO,buffer,strlen(cipher)) <= 0) error("Couldn't write to the stdout");
//         }
//     }
//     free(buffer);
// }

// void *server_handler(void *temp){
//     int n, sshfd,sockfd;
//     struct sockaddr_in sshserv_addr;
//     unsigned char* key;
//     unsigned char buffer[4096];
//     struct socket_info *temp1;
//     pthread_t stdin_thread, sock_thread;

//     bzero(buffer,4096);
//     printf("New thread\n");
//     struct connection_info *info = (struct connection_info*)temp;

//     sockfd = info->sockfd;
//     sshserv_addr = info->ssh_addr;
//     key = info->key;

//     sshfd = socket(AF_INET, SOCK_STREAM, 0);
	
// 	if (connect(sshfd, (struct sockaddr *)&sshserv_addr, sizeof(sshserv_addr)) < 0) {
// 		printf("Connection to ssh failed!\n");
// 		pthread_exit(0);
// 	} else {
// 		printf("Connection to ssh established!\n");
//     }
//     temp1 = (struct socket_info*)malloc(sizeof(struct socket_info));
//     temp1->sock1id = sockfd;
//     temp1->sock2id = sshfd;
//     temp1->key = key;
//     if(pthread_create(&stdin_thread, NULL, stdins_handler, (void*)temp) < 0) error("Creating stdin thread failed");
//     if(pthread_create(&sock_thread, NULL, sockets_handler, (void*)temp) < 0) error("Creating socket thread failed");
//     pthread_join(stdin_thread, NULL);
//     pthread_join(sock_thread, NULL);
//     close(sockfd);
//     close(sshfd);
//     exit(0);
// }

void getClient(unsigned char* enc_key, const char* dest_ip, int dest_port){
    unsigned char iv[AES_BLOCK_SIZE];
    struct ctr_state state;
    int set_key;
    AES_KEY key_aes;
    int sockfd,flags;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int send_key;
    int n, key_write, buff_read, msg_write;
    char buffer[4096];
    fd_set fread;
    FILE *fp;
    int retval; 
    char cipher[4096];
    pthread_t stdin_thread, sock_thread;
    struct socket_info *temp = NULL;


    sockfd = socket(AF_INET, SOCK_STREAM, 0); //create a new socket
    if(sockfd < 0) {
        printf("Error opening the socket\n");
        if(sockfd == -1) close(sockfd);
        // if(enc_key) free(enc_key);
        exit(1);
    }
    // printf("Socket opnened\n");
    server = gethostbyname(dest_ip);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        if(sockfd == -1) close(sockfd);
        // if(enc_key) free(enc_key);
        exit(0);
    }
    // printf("server connected\n");
    bzero((char *) &serv_addr, sizeof(serv_addr)); //bezero sets all the values of the buffer to zero. frst argument is the buffer name and second argument is the size
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length); // copies from one string to another
    // printf("coppied correctly\n");
    serv_addr.sin_port = htons(dest_port);
    // printf("%d\n",dest_port);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) {
        // fprintf(stderr,"ERROR connecting\n");
        perror("Error: ");
        exit(1);
        // if(sockfd == -1) close(sockfd);
        // if(enc_key) free(enc_key);
    }
    printf("Connected to the server\n");
    // printf("%d\n",dest_port);
//     temp = (struct socket_info*)malloc(sizeof(struct socket_info));
//     temp->sock1id = 0;
//     temp->sock2id = sockfd;
//     temp->key = enc_key;
//     if(pthread_create(&stdin_thread, NULL, stdin_handler, (void*)temp) < 0) error("Creating stdin thread failed");
//     if(pthread_create(&sock_thread, NULL, socket_handler, (void*)temp) < 0) error("Creating socket thread failed");
//     pthread_join(stdin_thread, NULL);
//     pthread_join(sock_thread, NULL);
//     close(sockfd);
// }


    //sending the encrypted key to the server
    // bzero(buffer,4096);
    // memcpy(buffer,enc_key,sizeof(enc_key));
    // key_write = write(sockfd,buffer,sizeof(enc_key));
    // if(key_write < 0){
    //     printf("Cannot send the key to the server\n");
    //     exit(1);
    // }

    fcntl(STDIN_FILENO,F_SETFL,O_NONBLOCK);
    flags = fcntl(sockfd,F_GETFL);
    if(flags == -1) error("Error flag from sshsock. Closing the connection...");
    fcntl(sockfd,F_SETFL,flags|O_NONBLOCK);

    bzero(buffer,4096); //clear the buffer
    bzero(cipher,4096); //clear the encrypted buffer

    while(1){
        set_key = AES_set_encrypt_key(enc_key, 128, &key_aes); //set the encryption key
        if(set_key < 0) error("Counldn't set the encrypted key");
        //read the data continuously from the socket and decrypt it and write it to the stdout
        while((buff_read = read(sockfd, cipher, 4096)) > 0){
            memcpy(iv,cipher,AES_BLOCK_SIZE);
            init_ctr(&state, iv);
            //decrypt the data
            AES_ctr128_encrypt(cipher+AES_BLOCK_SIZE, buffer, buff_read-AES_BLOCK_SIZE, &key_aes, state.ivec, state.ecount, &state.num);
            write(STDOUT_FILENO, buffer, buff_read-AES_BLOCK_SIZE);
            if (buff_read < 4096)
                break;
        }
        //read the data continuously from the stdin and write it to the client
        while((buff_read = read(STDIN_FILENO, buffer, 4096)) >= 0){
            //generate a random iv
            if(!RAND_bytes(iv, AES_BLOCK_SIZE)) {
                fprintf(stderr, "Error generating random bytes.\n");
                exit(1);
            }
            init_ctr(&state, iv);
            //copy the iv into the cipher
            memcpy(cipher,iv,AES_BLOCK_SIZE);
            //encrypt the data
            AES_ctr128_encrypt(buffer, cipher + AES_BLOCK_SIZE, buff_read, &key_aes, state.ivec, state.ecount, &state.num);
            msg_write = write(sockfd,cipher,AES_BLOCK_SIZE + buff_read);
            if(msg_write < 0) error("Couldn't write to the socket");
            usleep(900);
            printf("Message sent to ssh client\n");
        }
    }
}
        // FD_ZERO(&fread);  //clear the socket set
        // FD_SET(STDIN_FILENO, &fread); // set the stdin file descriptor
        // FD_SET(sockfd, &fread); //set the master socket
        // //select function blocks untill an activity occurs
        // retval = select(sockfd+1, &fread, NULL, NULL, NULL); //wait for an activity on one of the sockets indefinitely because timeout is set to NULL
        // if(retval == -1){
        //     perror("Error in the select function");
        //     if(sockfd == -1) close(sockfd);
        //     exit(0);
        // }
        //if the std input is marked for read
        // if(FD_ISSET(STDIN_FILENO, &fread)){
        // while((buff_read = read(STDIN_FILENO,buffer,4096)) > 0){
        //     printf("In stdin\n");    
        //     // if(buff_read <= 0) error("Couldn't read from stdin\n");
        //     // if(fgets(buffer,4096,stdin) == NULL) error("Couldn't read from the stdin");
        //     if(strlen(buffer) > 0){
        //         encrypt(buffer,enc_buffer,enc_key,buff_read); //encrypt the message
        //         msg_write = write(sockfd,enc_buffer,sizeof(enc_buffer)); //send the encrypted message to the server
        //         // usleep(20000);
        //         if(msg_write <= 0) {
        //             perror("Couldn't send the message to the server");
        //             close(sockfd);
        //             exit(1);
        //         }
        //     }
        // }
        // else if(FD_ISSET(sockfd,&fread)){
    //     while((buff_read = read(sockfd,enc_buffer,4096)) > 0){
    //         printf("In socket\n");
    //         // bzero(buffer,4096); //clear the buffer
    //         // bzero(enc_buffer,4096); //clear the encrypted buffer
    //         // if(buff_read <= 0) error("Couldn't read from the socket");
    //         if(strlen(enc_buffer) > 0){
    //             decrypt(enc_buffer,buffer,enc_key,buff_read); //decrypt the message
    //             msg_write = write(STDOUT_FILENO,buffer,sizeof(buffer)); //send the encrypted message to the server
    //             // usleep(20000);
    //             if(msg_write <= 0) {
    //                 perror("Couldn't send the message to the server");
    //                 close(sockfd);
    //                 exit(1);
    //             }
    //         }
    //     }
    // }  
// }

void getServer(int listen_port,unsigned char* enc_key, const char* dest_ip, int dest_port){
    // printf("%d\n",dest_ip);

    int sockfd, newsockfd, clilen, sshsock, flags, pid;
    char buffer[4096], enc_buffer[4096];
    struct sockaddr_in serv_addr, cli_addr;
    int n, optval, read_key, res, retval,key_write, buff_read, msg_write;
    struct hostent *hostp, *server;
    char* hostaddrp;
    unsigned char* key;
    struct sockaddr_in sshserv_addr;
    fd_set fread;
    pthread_t stdin_thread, sock_thread;
    pthread_t thread;
    struct socket_info *temp;
    struct connection_info *info;
    unsigned char iv[AES_BLOCK_SIZE];
    struct ctr_state state;
    int set_key;
    AES_KEY key_aes;
    char buffer_data[4096];
    char cipher[4096];
    bzero(buffer_data,4096);
    bzero(cipher,4096);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){ 
       perror("ERROR opening socket");
       exit(1);
    }
    optval = 1;
    //lets us rerun the server immediately after we kill it and also eliminates binding error
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int)) < 0) error("Couldn't set up the socket\n");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    //get the server by resolving the destination ip
    server = gethostbyname(dest_ip);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        if(sshsock == -1) close(sshsock);
        exit(1);
    }
    // deatials about the listening port
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htons(INADDR_ANY);
    serv_addr.sin_port = htons(listen_port);

    // details about the destination port
    sshserv_addr.sin_family = AF_INET;
    sshserv_addr.sin_port = htons(dest_port);
    sshserv_addr.sin_addr.s_addr = ((struct in_addr*)(server->h_addr))->s_addr;
    
    //bind the listening port and start listening
    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){ 
             perror("ERROR on binding");
             exit(1);
             }
    listen(sockfd,10);
    clilen = sizeof(cli_addr);
    while(1){
        // info = (struct connection_info*)malloc(sizeof(struct connection_info));
        // waiting for the connection request
        printf("Waiting for connection request\n");
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) error("ERROR on accept");
        printf("Got the connection\n");
        // info->sockfd = newsockfd;
		// info->ssh_addr = sshserv_addr;
        // info->key = enc_key;
        // if(newsockfd > 0) {
        //     pthread_create(&thread,NULL,server_handler,(void*)info);
        //     pthread_detach(thread);
        // }
        //lets you know who sent the message
        // hostp = gethostbyaddr((const char *)&cli_addr.sin_addr.s_addr,sizeof(cli_addr.sin_addr.s_addr), AF_INET);
        // if (hostp == NULL) error("ERROR on gethostbyaddr");
        // printf("Got the hostname\n");
        // hostaddrp = inet_ntoa(cli_addr.sin_addr);
        // if (hostaddrp == NULL) error("ERROR on inet_ntoa\n");
        
        //create a seperate process from the main process so forward the information through the ssh
        pid = fork();
        if(pid == 0){
            close(sockfd);
            //socket for forwarding the information to and from the ssh
            sshsock = socket(AF_INET, SOCK_STREAM, 0);
            if (sshsock < 0){ 
            perror("ERROR opening socket");
            exit(1);
            }
            // server = gethostbyname(dest_ip);
            // if (server == NULL) {
            //     fprintf(stderr,"ERROR, no such host\n");
            //     if(sshsock == -1) close(sshsock);
            //     exit(1);
            // }
            // printf("Got the host name\n");
            // bzero((char *) &sshserv_addr, sizeof(sshserv_addr)); //bezero sets all the values of the buffer to zero. frst argument is the buffer name and second argument is the size
            // sshserv_addr.sin_family = AF_INET;
            // bcopy((char *)server->h_addr, (char *)&sshserv_addr.sin_addr.s_addr,server->h_length); // copies from one string to another
            // sshserv_addr.sin_port = htons(dest_port);
            // printf("coppied correctly\n");
            if (connect(sshsock,(struct sockaddr *)&sshserv_addr,sizeof(sshserv_addr)) < 0) {
                fprintf(stderr,"ERROR connecting ssh\n");
                exit(1);
                // if(sockfd == -1) close(sockfd);
                // if(enc_key) free(enc_key);
            }
            printf("connected to sshsock\n");
            // temp = (struct socket_info*)malloc(sizeof(struct socket_info));
            // temp->sock1id = newsockfd;
            // temp->sock2id = sockfd;
            // temp->key = enc_key;
            // if(pthread_create(&stdin_thread, NULL, stdins_handler, (void*)temp) < 0) error("Creating stdin thread failed");
            // if(pthread_create(&sock_thread, NULL, sockets_handler, (void*)temp) < 0) error("Creating socket thread failed");
            // pthread_join(stdin_thread, NULL);
            // pthread_join(sock_thread, NULL);
            // close(sshsock);
            // close(newsockfd);
            // exit(0);

            //check the flags and set the fd's to be non-blocking
            flags = fcntl(newsockfd,F_GETFL);
            if(flags == -1) error("Error flag from newsock. Closing the connection...");
            fcntl(newsockfd,F_SETFL,flags|O_NONBLOCK);
            flags = fcntl(sshsock,F_GETFL);
            if(flags == -1) error("Error flag from sshsock. Closing the connection...");
            fcntl(sshsock,F_SETFL,flags|O_NONBLOCK);
            while(1){
                set_key = AES_set_encrypt_key(enc_key, 128, &key_aes); //set the encryption key
                if(set_key < 0) error("Counldn't set the encrypted key");
                //read the data from the socket and decrypt it
                while((buff_read = read(newsockfd, cipher, 4096)) > 0){
                    memcpy(iv,cipher,AES_BLOCK_SIZE);
                    init_ctr(&state, iv);
                    AES_ctr128_encrypt(cipher+AES_BLOCK_SIZE, buffer, buff_read-AES_BLOCK_SIZE, &key_aes, state.ivec, state.ecount, &state.num);
                    write(sshsock, buffer, buff_read-AES_BLOCK_SIZE);
                    if (buff_read < 4096)
                        break;
                }
                //read the data from the sshsocket and encrypt it
                while((buff_read = read(sshsock, buffer, 4096)) >= 0){
                    if(!RAND_bytes(iv, AES_BLOCK_SIZE)) {
                        fprintf(stderr, "Error generating random bytes.\n");
                        exit(1);
                    }
                    init_ctr(&state, iv);
                    memcpy(cipher,iv,AES_BLOCK_SIZE);
                    AES_ctr128_encrypt(buffer, cipher + AES_BLOCK_SIZE, buff_read, &key_aes, state.ivec, state.ecount, &state.num);
                    msg_write = write(newsockfd,cipher,AES_BLOCK_SIZE + buff_read);
                    if(msg_write < 0) error("Couldn't write to the socket");
                    usleep(900);
                    printf("Message sent to ssh client\n");
                }
            }
            exit(0);
        } else if(pid < 0) error("Error on fork");
        else close(newsockfd);
    }
}


int main(int argc, char **argv){
    int c;
    int listen_port = -1;
    char* key_file;
    char* destination_ip;
    char* destination_port;
    FILE *fp;
    int i=0, k;
    unsigned char* enc_key;
    unsigned char c1,c2,sum;
    char key[1000];
    char ch;

    while((c = getopt (argc, argv, "l:k:")) != -1){
        switch (c)
        {
		case 'l':
			listen_port = atoi(optarg);
            break;
		case 'k':
			key_file = optarg;
            break;
        case '?':
            if(optopt == 'l') {
                printf("Please enter a port number to listen\n");
                return 1;
            }
            else if(optopt == 'k'){
                printf("Please specify the argument for the key file\n");
                return 1;
            }
            else {
                printf("Unknown argument\n");
                return 1;
            }
        }
    }
    destination_ip = argv[optind];
    optind++;
    destination_port = argv[optind];
    // if(listen_port == -1) printf("Yes\n");
    printf("listen_port = %d\n",listen_port);
    printf("key_file = %s\n",key_file);
    printf("Destination ip = %s\n",destination_ip);
    printf("Destination port = %s\n",destination_port);

    if(listen_port == 0) {
        printf("Please enter a istening port to listen\n");
        return 1;
    }

    if(strlen(key_file) == 0){
        printf("Please enter a key file\n");
        return 1;
    }

    if(key_file == NULL){
        printf("Please enter the location of the key file\n");
        return 1;
    }

    fp = fopen(key_file,"r");
    if(!fp){
        printf("Error opening the file\n");
        return 1;
    }

    ch = fgetc(fp);
    while(ch!=EOF && ch!='\n'){
        key[i] = ch;
        i++;
        ch = fgetc(fp);
    }
    
    key[i] = '\0';
    // for(k=0;k<i;k++) printf("%c",key[k]);
    // printf("\n");
    enc_key = (unsigned char*)key;
    // printf("%s\n",enc_key);
    fclose(fp);
    
    if(destination_ip == NULL){
        printf("Please provide the destination_ip\n");
        return 1;
    }

    if(destination_port == NULL){
        printf("Please provide the destination_port number\n");
        return 1;
    }

    if(listen_port == -1){
        getClient(enc_key,destination_ip,atoi(destination_port));
        // printf("Yes\n");
    } 
    else getServer(listen_port,enc_key,destination_ip,atoi(destination_port));


    return 0;
}