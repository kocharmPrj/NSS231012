#include "socketForPc.hpp"

int TX_thread(){
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    int clnt_adr_sz;
    int sock_option  = 1;
    pthread_t t_id[BOT_USER] = {0};
    int str_len = 0;
    int i=0;
    char idpasswd[(ID_SIZE*2)+3];
    char *pToken;
    char *pArray[ARR_CNT]={0};
    char msg[BUF_SIZE];

    if(argc != 2) {
        printf("Usage : %s <port>\n",argv[0]);
        exit(1);
    }
    //+
    FILE *idFd = fopen("idpasswd.txt","r");

    char id[ID_SIZE];
    char pw[ID_SIZE];

    CLIENT_INFO *client_info = (CLIENT_INFO *)calloc(sizeof(CLIENT_INFO),       MAX_CLNT);
    int ret;
    do {
        ret = fscanf(idFd,"%s %s",id,pw);
        if(ret <= 0)
            break;
        client_info[i].fd=-1;
        strcpy(client_info[i].id,id);
        strcpy(client_info[i].pw,pw);

        i++;
        if(i > MAX_CLNT)
        {
            printf("error client_info pull(MAX:%d)\n",MAX_CLNT);
            exit(2);
        }
    } while(1);
    fclose(idFd);

    fputs("IoT Server Start!!\n",stdout);

    if(pthread_mutex_init(&g_mutex, NULL) != 0)
        error_handling("mutex init error");

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family=AF_INET;
    serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_adr.sin_port=htons(atoi(argv[1]));

    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (void*)&sock_option,        sizeof(sock_option));
    if(bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr))==-1)
        error_handling("bind() error");

    if(listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    while(1) {
        clnt_adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr,             &clnt_adr_sz);
        if(clnt_cnt >= MAX_CLNT)
        {
            printf("socket full\n");
            shutdown(clnt_sock,SHUT_WR);
            continue;
        }
        else if(clnt_sock < 0)
        {
            perror("accept()");
            continue;
        }

        str_len = read(clnt_sock, idpasswd, sizeof(idpasswd));
        idpasswd[str_len] = '\0';

        if(str_len > 0)
        {
            i=0;
            pToken = strtok(idpasswd,"[:]");

            while(pToken != NULL)
            {
                pArray[i] =  pToken;
                if(i++ >= ARR_CNT)
                    break;
                pToken = strtok(NULL,"[:]");
            }
            for(i=0;i<MAX_CLNT;i++)
            {
                if(!strcmp(client_info[i].id,pArray[0]))
                {
                    if(client_info[i].fd != -1)
                    {
                        sprintf(msg,"[%s] Already logged!\n",pArray[0]);
                        write(clnt_sock, msg,strlen(msg));
                        shutdown(clnt_sock,SHUT_WR);
#if 1   //for MCU
                        shutdown(client_info[i].fd,SHUT_WR);
                        pthread_mutex_lock(&g_mutex);
                        client_info[i].fd = -1;
                        pthread_mutex_unlock(&g_mutex);
#endif
                        break;
                    }
                    if(!strcmp(client_info[i].pw,pArray[1]))
                    {

                        strcpy(client_info[i].ip,inet_ntoa(clnt_adr.sin_addr));
                        pthread_mutex_lock(&g_mutex);
                        client_info[i].index = i;
                        client_info[i].fd = clnt_sock;
                        clnt_cnt++;
                        pthread_mutex_unlock(&g_mutex);
                        sprintf(msg,"[%s] New connected! (ip:%s,fd:%d,sockcnt:  %d)\n",pArray[0],inet_ntoa(clnt_adr.sin_addr),clnt_sock,clnt_cnt);
                        write(clnt_sock, msg,strlen(msg));

                        pthread_create(t_id+i, NULL, rcv_msg, (void *           )(client_info + i));
                        pthread_detach(t_id[i]);
                        break;
                    }
                }
            }
            if(i == MAX_CLNT)
            {
                sprintf(msg,"[%s] Authentication Error!\n",pArray[0]);
                write(clnt_sock, msg,strlen(msg));
                shutdown(clnt_sock,SHUT_WR);
            }
        }
        else
            shutdown(clnt_sock,SHUT_WR);

    }
    return 0;
}


void send_msg(MSG_INFO * msg_info, CLIENT_INFO * first_client_info)
{
    int i=0;

    if(!strcmp(msg_info->to,"ALLMSG"))
    {
        for(i=0;i<MAX_CLNT;i++)
            if((first_client_info+i)->fd != -1)
                write((first_client_info+i)->fd, msg_info->msg, msg_info->len);
    }
    else if(!strcmp(msg_info->to,"IDLIST"))
    {
        char* idlist = (char *)malloc(ID_SIZE * MAX_CLNT);
        msg_info->msg[strlen(msg_info->msg) - 1] = '\0';
        strcpy(idlist,msg_info->msg);

        for(i=0;i<MAX_CLNT;i++)
        {
            if((first_client_info+i)->fd != -1)
            {
                strcat(idlist,(first_client_info+i)->id);
                strcat(idlist," ");
            }
        }
        strcat(idlist,"\n");
        write(msg_info->fd, idlist, strlen(idlist));
        free(idlist);
    }

    else if(!strcmp(msg_info->to,"GETTIME"))
    {
        sleep(1);
        getlocaltime(msg_info->msg);
        write(msg_info->fd, msg_info->msg, strlen(msg_info->msg));
    }else {
        for(i=0;i<MAX_CLNT;i++)
            if((first_client_info+i)->fd != -1)
                if(!strcmp(msg_info->to,(first_client_info+i)->id))
                    write((first_client_info+i)->fd, msg_info->msg, msg_info-   >len);
    }
}

void error_handling(char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}

void  getlocaltime(char * buf)
{
    struct tm *t;
    time_t tt;
    char wday[7][4] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
    tt = time(NULL);
    if(errno == EFAULT)
        perror("time()");
    t = localtime(&tt);
    sprintf(buf,"[GETTIME]%02d.%02d.%02d %02d:%02d:%02d %s",t->tm_year+1900-    2000,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec,wday[t->tm_wday]);
    return;
}


