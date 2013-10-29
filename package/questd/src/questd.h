#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#include <sys/sysinfo.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include <uci.h>

#define MAX_NETWORK	32
#define MAX_CLIENT	128
#define MAX_PORT	8

typedef struct {
	bool exists;
	bool is_lan;
        const char *name;
        const char *type;
        const char *proto;
        const char *ipaddr;
        const char *netmask;
        char ifname[128];
} Network;

typedef struct {
	bool exists;
	bool local;
	bool dhcp;
	char leaseno[24];
	char macaddr[24];
	char hostaddr[24];
	char hostname[32];
	char hwaddr[24];
	char network[32];
	char device[32];
	bool connected;
} Client;

typedef struct {
        unsigned long rx_bytes;
        unsigned long rx_packets;
        unsigned long rx_errors;
        unsigned long tx_bytes;
        unsigned long tx_packets;
        unsigned long tx_errors;
} Statistic;

typedef struct {
	char name[16];
	char device[32];
	Statistic stat;
	Client client[MAX_CLIENT];
} Port;

typedef struct {
	char name[64];
	const char *hardware;
	const char *model;
	const char *firmware;
	const char *brcmver;
	const char *socmod;
	const char *socrev;
	const char *cfever;
	const char *kernel;
	const char *basemac;
	const char *serialno;
	char uptime[64];
	unsigned int procs;
	unsigned int cpu;
} Router;

typedef struct {
	unsigned long total;
	unsigned long used;
	unsigned long free;
	unsigned long shared;
	unsigned long buffers;
} Memory;

typedef struct {
	const char *auth;
	const char *des;
	const char *wpa;
} Key;

typedef struct {
	bool wifi;
	bool adsl;
        bool vdsl;
        bool voice;
        bool dect;
        int vports;
	int eports;
} Spec;

typedef struct jiffy_counts_t {
	unsigned long long usr, nic, sys, idle;
	unsigned long long iowait, irq, softirq, steal;
	unsigned long long total;
	unsigned long long busy;
} jiffy_counts_t;

void recalc_sleep_time(bool calc, long dec);
void init_db_hw_config(void);
bool arping(char *target, char *device);
void remove_newline(char *buf);
void get_jif_val(jiffy_counts_t *p_jif);
void dump_keys(Key *keys);
void dump_specs(Spec *spec);
void dump_static_router_info(Router *router);
void dump_hostname(Router *router);
void dump_sysinfo(Router *router, Memory *memory);
void dump_cpuinfo(Router *router, jiffy_counts_t *prev_jif, jiffy_counts_t *cur_jif);
void get_port_info(Port *port);
