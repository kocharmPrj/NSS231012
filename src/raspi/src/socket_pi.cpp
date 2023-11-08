#include "socketForRaspi.hpp"

static const char* CP = "10.10.141.16";
const char* NAME = "Turtle1";
int XToken = 0;
int YToken = 0; 

void* RX_thread(void* arg){
    int sock;
    struct sockaddr_in servAddr;

	sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1){
        std::cout <<"socket() error";
		exit(1);
	}

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family=AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(CP); 
	servAddr.sin_port = htons(5000);
    if(connect(sock, (struct sockaddr*)&servAddr, sizeof(servAddr)) == -1){
		std::cout << "connect() err";
		exit(1);
	} else std::cout << "connected" << '\n';

	write(sock, NAME, strlen(NAME));
	char msg[100];
	char* Xtmp = nullptr;
	char* Ytmp = nullptr;
	int strLen=0;
	int strLenZeroFlag = 0;
    while(1) {
        memset(msg,0x0,sizeof(msg));
        strLen = read(sock, msg, sizeof(msg));
        if(strLen <= 0)
        {
			if (strLenZeroFlag==0) {
				strLenZeroFlag = 1;
				std::cout << "strlen under 0" << '\n';
			}
        } else {
			std::cout << "get" << '\n';
        	msg[strLen] = ' ';
			Xtmp = strtok(msg, " ");
			Ytmp = strtok(NULL, " ");
		}			

		if ( (strLen>0) && !(Xtmp == nullptr || Ytmp == nullptr) ){
			pthread_mutex_lock(&g_mutex);
			XToken = atoi(Xtmp); Xtmp = nullptr;
			YToken = atoi(Ytmp); Ytmp = nullptr;
			std::cout << "XandY : " << XToken << " " << YToken << '\n';
			pthread_mutex_unlock(&g_mutex);
		}
	}			
    close(sock);
}

