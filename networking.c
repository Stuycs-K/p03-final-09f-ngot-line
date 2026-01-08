int client_tcp_handshake(char * server_address) {
    struct addrinfo hints, *results;
    
    //hints for getaddrinfo
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP

  
    // PORT is defined in networking.h
    int status = getaddrinfo(server_address, PORT, &hints, &results);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    // made socket using the info from getaddres
    int server_socket = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
    err(server_socket, "socket error");

    // connects to serve
    // start of tcp hs
    int c = connect(server_socket, results->ai_addr, results->ai_addrlen);
    err(c, "connect error");

    
    freeaddrinfo(results);

    return server_socket;
}
