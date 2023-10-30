#include "socketForPc.hpp"

std::string g_string;
std::string g_endString;
pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

int main(){
    pthread_t t_id[1];

    pthread_create(t_id, NULL, TX_thread, NULL);
    pthread_detach(t_id[0]);


    int i = 3;
    std::string tmp = "tmp";
    while ( i != 0 ) {
        //std::cin >> tmp2;
        tmp = tmp + std::to_string(i);

        pthread_mutex_lock(&g_mutex);
        g_string = tmp;
        pthread_mutex_unlock(&g_mutex);
        i--;
    }

    std::cout << "end 3 times";

//TX thread
    
//cin integer 1. start ( go striaght )  - tx MAX_INT
//              2. stop and backward    - tx pos x,y


    return 0;
}
