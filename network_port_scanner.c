// Basic Network Port Scanner (TCP/UDP)
// Final Combined Project by Team T16 (Sasken Internship 2025)
// Includes: TCP Scanner, UDP Scanner, Multithreading, Logging

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <getopt.h>

// ---------------------- TCP SCANNER ----------------------
int is_tcp_port_open(const char *ip, int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) return 0;

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &addr.sin_addr);

    int result = connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));
    close(sockfd);
    return result == 0;
}

// ---------------------- UDP SCANNER ----------------------
int is_udp_port_responsive(const char *ip, int port) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) return 0;

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &addr.sin_addr);

    char *msg = "Hello";
    sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *)&addr, sizeof(addr));

    struct timeval tv = {1, 0}; // 1 second timeout
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    char buffer[1024];
    socklen_t len = sizeof(addr);
    int resp = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&addr, &len);

    close(sockfd);
    return resp >= 0;
}

// ---------------------- LOGGING ----------------------
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

void log_result(const char *protocol, int port, const char *status) {
    pthread_mutex_lock(&log_mutex);
    FILE *logfile = fopen("scan_results.txt", "a");
    if (logfile) {
        time_t now = time(NULL);
        fprintf(logfile, "[%s] Port %d: %s [%s]", protocol, port, status, ctime(&now));
        fclose(logfile);
    }
    pthread_mutex_unlock(&log_mutex);
}

// ---------------------- THREADING ----------------------
struct scan_args {
    const char *ip;
    int port;
    int protocol; // 0: TCP, 1: UDP
};

void *threaded_scan(void *arg) {
    struct scan_args *args = (struct scan_args *)arg;
    if (args->protocol == 0) {
        if (is_tcp_port_open(args->ip, args->port)) {
            printf("[TCP] Port %d is OPEN\n", args->port);
            log_result("TCP", args->port, "OPEN");
        }
    } else {
        if (is_udp_port_responsive(args->ip, args->port)) {
            printf("[UDP] Port %d is RESPONSIVE\n", args->port);
            log_result("UDP", args->port, "RESPONSIVE");
        }
    }
    free(arg);
    return NULL;
}

void launch_threads(const char *ip, int start_port, int end_port, int protocol) {
    for (int port = start_port; port <= end_port; port++) {
        pthread_t tid;
        struct scan_args *args = malloc(sizeof(struct scan_args));
        args->ip = ip;
        args->port = port;
        args->protocol = protocol;
        pthread_create(&tid, NULL, threaded_scan, args);
        pthread_detach(tid);
    }
    sleep(2); // Allow time for threads to complete
}

// ---------------------- MAIN FUNCTION ----------------------
void usage(const char *prog) {
    printf("Usage: %s -i <IP> -s <start_port> -e <end_port> [-t | -u]\n", prog);
}

int main(int argc, char *argv[]) {
    char *ip = NULL;
    int start_port = 0, end_port = 0;
    int use_tcp = 0, use_udp = 0;
    int opt;

    while ((opt = getopt(argc, argv, "i:s:e:tu")) != -1) {
        switch (opt) {
            case 'i': ip = optarg; break;
            case 's': start_port = atoi(optarg); break;
            case 'e': end_port = atoi(optarg); break;
            case 't': use_tcp = 1; break;
            case 'u': use_udp = 1; break;
            default: usage(argv[0]); return 1;
        }
    }

    if (!ip || start_port <= 0 || end_port <= 0 || (!use_tcp && !use_udp)) {
        usage(argv[0]);
        return 1;
    }

    printf("Starting scan on %s from port %d to %d...\n", ip, start_port, end_port);
    if (use_tcp) {
        printf("Running TCP scan...\n");
        launch_threads(ip, start_port, end_port, 0);
    }
    if (use_udp) {
        printf("Running UDP scan...\n");
        launch_threads(ip, start_port, end_port, 1);
    }

    printf("Scan complete. Check 'scan_results.txt' for logs.\n");
    return 0;
}
