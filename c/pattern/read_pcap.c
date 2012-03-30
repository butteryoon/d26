/*
 * pattern.c
 *
 *  Created on: Mar 29, 2012
 *      Author: barca
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pcap.h>

#include "read_pcap.h"

char	*_pcap_file = NULL;
char	default_pcap_file[1024] = "/home/barca/Downloads/POOQ_TEST_20120222.pcap";
pcap_t	*pcap_handle = NULL;

static uint32_t detection_tick_resolution = 1000;
static int _pcap_datalink_type = 0;
static uint64_t raw_packet_count;

int		tcp_cnt = 0;
int		udp_cnt = 0;
int		etc_cnt = 0;
int		frag_cnt = 0;

int packet_processing(const uint64_t time, const struct iphdr *iph, uint16_t ipsize, uint16_t rawsize)
{
	uint8_t		*l4ptr = NULL;
	uint16_t	l4len = 0;
	uint8_t		l4protocol = 0;
	uint16_t	payload_len = 0;
	uint8_t		*payload = NULL;

	struct tcphdr		*tcp;
	struct udphdr		*udp;

//	printf("ip src=%u dst=%u version=%d protocol=%d\n", iph->saddr, iph->daddr, iph->version, iph->protocol);

	if(iph->version == 4 && (iph->frag_off & htons(0x1FFF)))
	{
		printf("fragmented ip packets\n");
		frag_cnt++;
		return 0;
	}

    l4ptr = (((uint8_t *) iph) + iph->ihl * 4);
    l4len = ntohs(iph->tot_len) - (iph->ihl * 4);
    l4protocol = iph->protocol;

    if(l4protocol == 6 && l4len >= 20)
    {
    	tcp = (struct tcphdr *)l4ptr;
        payload_len = l4len - tcp->doff * 4;
        payload = ((uint8_t *) tcp) + (tcp->doff * 4);
//        printf("i=%llu tcp src=%u:%u dst=%u:%u version=%d protocol=%d len=%d \n", raw_packet_count, iph->saddr, tcp->source, iph->daddr, tcp->dest, iph->version, iph->protocol, payload_len);
        tcp_cnt++;

        if(payload_len > 0 && payload[0] == 'G' && payload[1] == 'E' && payload[2] == 'T') printf("i=%llu data=%.*s\n", raw_packet_count, 100, payload);
    }
    else if(l4protocol == 17 && l4len >= 8)
    {
    	udp = (struct udphdr *)l4ptr;
    	payload_len = l4len - 8;
    	payload = ((uint8_t *)udp) + 8;
 //       printf("i=%llu udp src=%u:%u dst=%u:%u version=%d protocol=%d len=%d \n", raw_packet_count, iph->saddr, udp->source, iph->daddr, udp->dest, iph->version, iph->protocol, payload_len);
        udp_cnt++;
    }
    else
    {
    	printf("i=%llu ip src=%u dst=%u version=%d protocol=%d\n", raw_packet_count, iph->saddr, iph->daddr, iph->version, iph->protocol);
    	etc_cnt++;
    }

	return 0;
}
void pcap_packet_callback(u_char * args, const struct pcap_pkthdr *header, const u_char * packet)
{
    const struct ethhdr *ethernet = (struct ethhdr *) packet;
    struct iphdr *iph = (struct iphdr *) &packet[sizeof(struct ethhdr)];
    uint64_t time;
    static uint64_t lasttime = 0;
    uint16_t type;

    raw_packet_count++;

    time =
        ((uint64_t) header->ts.tv_sec) * detection_tick_resolution +
        header->ts.tv_usec / (1000000 / detection_tick_resolution);
    if (lasttime > time) {
        // printf("\nWARNING: timestamp bug in the pcap file (ts delta: %llu, repairing)\n", lasttime - time);
        time = lasttime;
    }
    lasttime = time;


    type = ethernet->h_proto;

    // just work on Ethernet packets that contain IP
    if (_pcap_datalink_type == DLT_EN10MB &&
        (type == htons(ETH_P_IP)
#ifdef IPOQUE_DETECTION_SUPPORT_IPV6
        || type == htons(ETH_P_IPV6)
#endif
        )
        && header->caplen >= sizeof(struct ethhdr)) {

        if (header->caplen < header->len) {
            static uint8_t cap_warning_used = 0;
            if (cap_warning_used == 0) {
                printf
                    ("\n\nWARNING: packet capture size is smaller than packet size, DETECTION MIGHT NOT WORK CORRECTLY OR EVEN CRASH\n\n");
                sleep(2);
                cap_warning_used = 1;
            }
        }

        if (header->len >= (sizeof(struct ethhdr) + sizeof(struct iphdr))) {
            // process the packet
            packet_processing(time, iph, header->len - sizeof(struct ethhdr), header->len);
        }
    }
}


int main(int argc, char **argv)
{
	int		opt;
	char	buffer[BUFSIZ];

	while((opt = getopt(argc, argv, "f:")) != EOF)
	{
		switch(opt)
		{
		case 'f':
			_pcap_file = optarg;
			break;
		}
	}

	if(_pcap_file == NULL)
	{
		_pcap_file = default_pcap_file;
	}

	pcap_handle = pcap_open_offline(_pcap_file, buffer);
	if(pcap_handle == NULL)
	{
		printf("ERROR: could not open pcap file: %s\n", buffer);
		exit(0);
	}

	_pcap_datalink_type = pcap_datalink(pcap_handle);

	pcap_loop(pcap_handle, -1, &pcap_packet_callback, NULL);

	pcap_close(pcap_handle);

	printf("packet count = %llu\n", raw_packet_count);
	printf("tcp=%d udp=%d etc=%d frag=%d\n", tcp_cnt, udp_cnt, etc_cnt, frag_cnt);
	return 0;
}
