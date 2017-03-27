#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>  
#include <unistd.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <syslog.h>
#include <uv.h>
#include <signal.h>
#include "sglib.h"

uv_loop_t loop;

typedef struct node {
    char *data;
    char *key;
    size_t data_len;

    struct node *left;
    struct node *right;
    char colour_field;
} node;

static void wait_signal(uv_signal_t* handle, int signum)
{
    uv_stop(&loop);
    printf("\nStopping database\n");
}

#define COMPARATOR(x,y) (strcmp(x->key, y->key))

SGLIB_DEFINE_RBTREE_PROTOTYPES(node, left, right, colour_field, COMPARATOR);
SGLIB_DEFINE_RBTREE_FUNCTIONS(node, left, right, colour_field, COMPARATOR);

node *root;

static void on_alloc(uv_handle_t *handle, size_t size, uv_buf_t *buf) {
  buf->base = malloc(size);
  buf->len = size;
}

void on_send(uv_write_t* req, int status) {
    
}

static void send_reply(uv_stream_t* handle, char cmd, char* key, char *data, size_t data_len) {
    size_t key_len, total_len;
    char *buffer;
    uv_buf_t buf;
    uv_write_t req;
    key_len = strlen(key);
    total_len = key_len + data_len + 2; 
    buffer = malloc(total_len);
    buffer[0] = cmd;
    strcpy(&buffer[1], key);
    if(data_len > 0) {
        memcpy(&buffer[key_len+2], data, data_len);
    }
    buf = uv_buf_init(buffer, total_len);
    uv_write(&req, handle, &buf, 1, on_send);
}

static void on_recv(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf) {
    struct sglib_node_iterator it;
    node tmp, *link;
    size_t key_len;
    int result;

    if(nread < 0) {
        return;
    } else {
        tmp.key = &buf->base[1];
        key_len = strnlen(tmp.key, nread-1);
        if(key_len == nread-1) {
            
            goto out;
        }
        tmp.data = &buf->base[key_len+2];
        tmp.data_len = nread - key_len - 2;
        printf("got data (%s, %s)\n", tmp.key, tmp.data);

        switch(buf->base[0]) {
        case 'X':

            uv_stop(handle->loop);
            break;

        case 'G': 
            link = sglib_node_find_member(root, &tmp);
            if(link == NULL) {
                send_reply(handle, 'g', tmp.key, NULL, 0);
            } else {
                send_reply(handle, 'g', link->key, link->data, link->data_len);
            }
            break;

        case 'A':   
        case 'S':   
            link = sglib_node_find_member(root, &tmp);
            if(link == NULL) {
                link = malloc(sizeof(node));
                link->key = strdup(tmp.key);
                link->data_len = tmp.data_len;
                link->data = malloc(link->data_len);
                memcpy(link->data, &buf->base[key_len+2], link->data_len);
                sglib_node_add(&root, link);
            } else if(buf->base[0] == 'S') {
                link->data_len = tmp.data_len;
                link->data = realloc(link->data, link->data_len);
                memcpy(link->data, &buf->base[key_len+2], link->data_len);
            }
            send_reply(handle, tolower(buf->base[0]), link->key, link->data, link->data_len);
            break;

        case 'D':  
            result = sglib_node_delete_if_member(&root, &tmp, &link);
            if(result > 0) {
                send_reply(handle, 'd', link->key, link->data, link->data_len);
                free(link->key);
                free(link->data);
                free(link);
            } else {
                send_reply(handle, 'd', tmp.key, NULL, 0);
            }
            break;

        case 'Z':   
            for(link=sglib_node_it_init(&it, root); link != NULL; link=sglib_node_it_next(&it)) {
                free(link->key);
                free(link->data);
                free(link);
            }
         
            root = NULL;
            send_reply(handle, 'z', "", NULL, 0);

        case '_':  
        {
            char key[8];
            char value[8];
            size_t rss;
            int count = sglib_node_len(root);
            uv_resident_set_memory(&rss);
            printf("Total %d keys, using %zdkB RAM\n", count, rss/1024);

            sprintf(key, "%d", count);
            sprintf(value, "%zd", rss/1024);
            send_reply(handle, tolower(buf->base[0]), key, value, strlen(value));
            break;
        }

        default:
            printf("Unknown command: %c", buf->base[0]);
            break;
        }
    }

out:
    free(buf->base);
}

void serialize(FILE* file)
{
    node* link;
    struct sglib_node_iterator it;

    printf("saving to file\n");

    for(link=sglib_node_it_init(&it, root); link != NULL; link=sglib_node_it_next(&it)) {
        fprintf(file, "%s\x01%s\n", link->key, link->data);

        free(link->key);
        free(link->data);
        free(link);
    }
}

void deserialize(FILE* file)
{
    char buffer[2048];
    char *key, *value;
    node* link;

    printf("loading from file\n");

    while (fgets(buffer, 2048, file))
    {
        buffer[strlen(buffer) - 1] = '\0';

        key = strtok(buffer, "\x01");
        value = strtok(NULL, "\x01");

        link = malloc(sizeof(node));
        link->key = strdup(key);
        link->data_len = strlen(value);
        link->data = malloc(link->data_len);
        memcpy(link->data, value, link->data_len);
        sglib_node_add(&root, link);
    }
}

void on_new_connection(uv_stream_t *server, int status)
{
    printf("new connection\n");

    if (status == -1) {
        return;
    }

    uv_buf_t* buf = malloc(sizeof(uv_buf_t));
    buf->len = 0;
    uv_tcp_t* client = malloc(sizeof(uv_tcp_t));
    client->data = buf;
    uv_tcp_init(&loop, client);
    if (uv_accept(server, (uv_stream_t*) client) == 0) {
        uv_read_start((uv_stream_t*) client, on_alloc, on_recv);
    } else {
        uv_close((uv_handle_t*) client, NULL);
        }
}

int main(int argc, char **argv) {

    uv_signal_t sig;
    uv_tcp_t skt;
    
    struct sockaddr_in addr;
    char *host = "0.0.0.0";
    int port = 6347;
    int verbose = 0;
    int opt;
    char* filename = NULL;
    FILE* file;

    root = NULL;

    while ((opt = getopt(argc, argv, "a:f:p:v")) != -1) {
        switch (opt) {

        case 'a':

            host = optarg;
            break;

        case 'f':

            filename = optarg;
        case 'p':

            port = atoi(optarg);
            break;

        case 'v':

            verbose += 1;
            break;

        default:
            fprintf(stderr, "Usage: %s [-a bindto] [-p port]\n", argv[0]);
            exit(-1);
        }
    }

    if (filename)
    {
        file = fopen(filename, "r");
        deserialize(file);
        fclose(file);
    }
    else if ((file = fopen("store.db", "r")) != NULL)
    {
        deserialize(file);
        fclose(file);
    }

    uv_ip4_addr(host, port, &addr);

    if(verbose > 0) {
        int i, c;
        uv_cpu_info_t *cpus;

        uv_cpu_info(&cpus, &c);

        for(i=0; i<c; i++) {
            printf("CPU %d: %s (%d MHz)\n", i, cpus[i].model, cpus[i].speed);
        }
        uv_free_cpu_info(cpus, c);
    }

    uv_loop_init(&loop);
    uv_tcp_init(&loop, &skt);    
    uv_tcp_bind(&skt, (const struct sockaddr *)&addr, 0);
    int r = uv_listen((uv_stream_t*) &skt, 128, on_new_connection);   

    if(verbose > 0) {
    
        printf("Listening [%s:%d]\n", host, port);
    }
    if (r) {
        fprintf(stderr, "Listen error\n");  
        return 1;
    }
    uv_signal_init(&loop, &sig);
    uv_signal_start(&sig, wait_signal, SIGINT);
    uv_run(&loop, UV_RUN_DEFAULT);
    uv_loop_close(&loop);

    file = fopen("store.db", "w");
    serialize(file);
    fclose(file);
}

