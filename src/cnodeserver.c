/*
  cnodeserver.c

  Adaptation of the Erlang C Node server code, some refactoring for clarity and modernity.
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "erl_interface.h"
#include "ei.h"

#define BUFSIZE 1000

ETERM* dispatchMessage( const char* func, ETERM* args) {
  if (strncmp(func, "foo", 3) == 0) {
    return erl_format("{cnode, ~i}", foo(ERL_INT_VALUE(args)));
  } else if (strncmp(func, "bar", 3) == 0) {
    return erl_format("{cnode, ~i}", bar(ERL_INT_VALUE(args)));
  } else {
    return NULL;
  }
}

int main(int argc, char **argv) {
  int port;                                /* Listen port number */
  int listen;                              /* Listen socket */
  int fd;                                  /* fd to Erlang node */
  ErlConnect conn;                         /* Connection data */

  int loop = 1;                            /* Loop flag */
  int got;                                 /* Result of receive */
  unsigned char buf[BUFSIZE];              /* Buffer for incoming message */
  ErlMessage emsg;                         /* Incoming message */

  ETERM *fromp, *tuplep, *fnp, *argp, *resp;
  int res;

  long allocated, freed;

  port = atoi(argv[1]);

  erl_init(NULL, 0);

  if (erl_connect_init(1, "secretcookie", 0) == -1)
    erl_err_quit("erl_connect_init");

  /* Make a listen socket */  
  if ((listen = my_listen(port)) <= 0) {
    erl_err_quit("my_listen");
  } 
  if (erl_publish(port) == -1) {
    erl_err_quit("erl_publish");
  }
  if ((fd = erl_accept(listen, &conn)) == ERL_ERROR) {
    erl_err_quit("erl_accept");
  }
    
  fprintf(stderr, "Connected to %s\n\r", conn.nodename);

  while (loop) {

    got = erl_receive_msg(fd, buf, BUFSIZE, &emsg);
    switch (got){
      case ERL_TICK: printf("got erl tick\n"); break;
      case ERL_ERROR: printf("got erl error\n");
                      loop = 0;
                      break;
      default: if (emsg.type == ERL_REG_SEND) {
                 printf("got registered message\n");
                 fromp = erl_element(2, emsg.msg);
                 tuplep = erl_element(3, emsg.msg);
                 fnp = erl_element(1, tuplep);
                 argp = erl_element(2, tuplep);

                 resp = dispatchMessage(ERL_ATOM_PTR(fnp), argp);
                 erl_send(fd, fromp, resp);
                 erl_free_compound(resp);
                 erl_free_compound(emsg.from);
                 erl_free_compound(emsg.msg);
               }
               break;
    }
  }
  return 0;
}

int my_listen(int port) {
  int listen_fd;
  struct sockaddr_in addr;
  int on = 1;

  if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    return (-1);

  setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(listen_fd, (struct sockaddr*) &addr, sizeof(addr)) < 0)
    return (-1);

  listen(listen_fd, 5);
  return listen_fd;
}
