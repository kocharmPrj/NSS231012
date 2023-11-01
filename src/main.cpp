#include "socketForPc.hpp"
#include <ctime>

std::string g_string;
std::string g_endString;
pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

int main(){
    pthread_t t_id[1];

    pthread_create(t_id, NULL, TX_thread, NULL);
    pthread_detach(t_id[0]);


    std::cout<< '1' << '\n';
    int i = 3;
    std::string tmp = "tmp";
    while ( i > 0 ) {
        std::cout << "test: " << i << '\n';
        //std::cin >> tmp2;
        tmp = tmp + std::to_string(i);

        pthread_mutex_lock(&g_mutex);
        g_string = tmp;
        pthread_mutex_unlock(&g_mutex);
        i--;
        std::cout << "test end : " << i << '\n';
    }
    std::cout << "fu" << '\n';
    
    std::time_t base = time(NULL);
    while ( time(NULL)-base < 3 ){
        pthread_mutex_lock(&g_mutex);
        g_string = "3 3";
        pthread_mutex_unlock(&g_mutex);
    }


    


//TX thread
    
//cin integer 1. start ( go striaght )  - tx MAX_INT
//              2. stop and backward    - tx pos x,y


    return 0;
}
