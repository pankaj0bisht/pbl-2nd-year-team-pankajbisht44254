#include <iostream>
#include <csignal>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cstdarg>
#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
  typedef SOCKET socket_t;
#else
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <sys/time.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <unistd.h>
  #include <strings.h>
  typedef int socket_t;
  #define INVALID_SOCKET (-1)
  #define SOCKET_ERROR (-1)
#endif
using namespace std;
const int NAME_LEN   = 16;
const int MAX_BLOCKS = 10;

inline void copyStr(char* destination, const char* source, int size) {
    strncpy(destination, source, size - 1);
    destination[size - 1] = '\0';
}

template <class T>
void genericSwap(T& first, T& second) {
    T temp = first;
    first  = second;
    second = temp;
}

class ParkingException {
    char message[100];
public:
    ParkingException(const char* msg) { copyStr(message, msg, 100); }
    const char* what() const { return message; }
};
