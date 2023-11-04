#include "socketForPc.hpp"

void* TX_thread(void* arg){
    int servSock, clntSock;
    struct sockaddr_in servAddr, clntAddr;
    socklen_t clntAddrSize;
    int sockOption = 1;
    char idChk[ID_SIZE];
    char* pToken;
    //char* pArray[ARR_CNT] = {0};

    CLIENT_INFO* clientInfo = (CLIENT_INFO*) malloc( sizeof(CLIENT_INFO) );
    //FILE* fd = fopen("user.txt", "r");
    //if (fd ==nullptr) std::cout << " ERR in fopen" <<'\n';
    char clntId[20]; // id size
    strcpy(clntId, "Turtle1");
    int ret = 7;
    //ret = fscanf(fd, "%19s", clntId);
    std::cout << "clntId fscanf test : " << clntId << '\n';
    
    if (ret <= 0){
        std::cout << "err in read user" << '\n';
        exit(1);
    }
    clientInfo->fd = -1;
    strcpy(clientInfo->id, clntId);
    //fclose(fd);

    servSock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&servAddr, 0, sizeof(sockaddr_in));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(atoi("5000")); // PORT 5000
                                            
    setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, 
            (void*)&sockOption, sizeof(int));
    if ( bind(servSock,(struct sockaddr*)&servAddr, sizeof(sockaddr_in))
            == -1 ) {
        std::cout << "bind Err";
        exit(3);
    }
    std::cout << "opopop" << '\n';
    if ( listen(servSock, 7) == -1 ){
        std::cout << "listen Err";
        exit(3);
    }
    std::cout << "klklkl" << '\n';

    while (1) {
        clntAddrSize = sizeof(clntAddr);
        // wait until connection of  client
        clntSock = accept(
                servSock, (struct sockaddr*)&clntAddr, &clntAddrSize);
        if (clntSock < 0){
            std::cout << "accept Err";
            exit(3);
        } else std::cout<<"accepted" << '\n';

        // chk the REQUEST || MSG
        size_t strLen = 0;
        strLen = read(clntSock, idChk, sizeof(idChk));
        std::cout << "strLen in socket : " << strLen << '\n';
        std::string token(idChk);
        token = token.substr(0, static_cast<std::string::size_type>(strLen));
        pToken = const_cast<char*>(token.c_str());
        

        if (strLen > 0){
            // chk whethere Id is register Id in server as Turtle1
            if ( clientInfo->fd == -1 ){
                std::cout << "Login try" << '\n';
                if ( !strcmp(clientInfo->id, pToken) ){
                    clientInfo->fd = clntSock;
                    strcpy(clientInfo->ip, inet_ntoa(clntAddr.sin_addr));
                    std::cout << "Logged in..turtle" << inet_ntoa(clntAddr.sin_addr) << '\n';
                    // go loop to wait the POSITION DATA
                    int flag = 0;
                    std::string tmp = "";
                    while(1){
                        pthread_mutex_lock(&g_mutex);
                        if (tmp.compare(g_string) != 0){
                            tmp = g_string;
                            flag = 1;
                            std::cout << "differenct g_string!" << '\n';
                        }
                        pthread_mutex_unlock(&g_mutex);
                        
                        if (flag==1){
                            flag = 0;
                            std::cout << "prepare to send " << tmp << '\n';
                            if (tmp.compare("q")==0) shutdown(clntSock, SHUT_WR);
                            // SEND TO CLIENT
                            else write(clientInfo->fd, tmp.c_str(), tmp.length());
                            
                        }
                    }
                }
            } else {
                std::cout << "already full client" << '\n';
                shutdown(clntSock, SHUT_WR);
                break;
            }
        }
    }
    return NULL;
}



