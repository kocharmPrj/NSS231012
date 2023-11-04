#include "socketForPc.hpp"

std::string g_string;
std::string g_endString;
pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

int main(){
    pthread_t t_id[1];

    pthread_create(t_id, NULL, TX_thread, NULL);
    pthread_detach(t_id[0]);


    // FOR TEST
    std::time_t base = time(NULL);
    while (true){
        if ( (time(NULL)-base) > 4){
            pthread_mutex_lock(&g_mutex);
            g_string = "30 9";
            pthread_mutex_unlock(&g_mutex);
            break;
        }
    }


    
//cin integer 1. start ( go striaght )  - tx MAX_INT
//              2. stop and backward    - tx pos x,y
    pthread_testcancel(); // or use a different function


    return 0;
}
