#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <tmf_cap.h>
#include <define.h>
#include <logutil.h>

int				file_fd = -1, idx_fd = -1;
unsigned int	cur_time = 0;
struct tm		cur_tm;

#define PCAP_FILEHEADER_SIZE		24
char pcap_file_header[PCAP_FILEHEADER_SIZE] =
	{	0xD4, 0xC3, 0xB2, 0xA1,
		0x02, 0x00, 0x04, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0xFF, 0xFF, 0x00, 0x00,
		0x01, 0x00, 0x00, 0x00	};

typedef struct __pcap_data_header__
{
	unsigned int	sec;
	unsigned int	msec;
	unsigned int	datalen1;
	unsigned int	datalen2;
} pcap_data_header;
#define PCAP_DATAHEADER_SIZE		sizeof(pcap_data_header)

#define SAVE_INTERVAL				(60 * 5)
#define PKT_PATH					"/LQMS/PKT_TEMP"

int write_pkt_pcap(Capture_Header_Msg *header, char *data)
{
	unsigned int		len = 0;

	pcap_data_header	pcapdataheader;	
	pcap_data_header	*pcap = &pcapdataheader;	

	pcap->sec = header->curtime;
	pcap->msec = header->ucurtime;
	pcap->datalen1 = header->datalen;
	pcap->datalen2 = header->datalen;

	if((len = write(file_fd, pcap, PCAP_DATAHEADER_SIZE)) != PCAP_DATAHEADER_SIZE)
	{
		dAppLog(LOG_CRI, "F=%s:%s.%d PKT_SAVE write PCAP DATA HEADER", __FILE__, __FUNCTION__, __LINE__);
		return -1;
	}

	if((len = write(file_fd, data, pcap->datalen1)) != pcap->datalen1)
	{
		dAppLog(LOG_CRI, "F=%s:%s.%d PKT_SAVE write PCAP DATA", __FILE__, __FUNCTION__, __LINE__);
		return -2;
	}

	return 0;
}

void close_file_pcap()
{
	close(file_fd);
	cur_time = -1;
	memset(&cur_tm, 0x00, sizeof(struct tm));
}

int open_new_file_pcap(Capture_Header_Msg *header)
{
	unsigned int	len = 0;
	time_t	curtime = header->curtime / SAVE_INTERVAL * SAVE_INTERVAL;
	char	filename[BUFSIZ];
	struct tm	*tm = &cur_tm;

	if(file_fd > 0)	close_file_pcap();

	localtime_r(&curtime, tm);

	sprintf(filename, "%s/TEST_%04d%02d%02d%02d%02d.cap", 
				PKT_PATH, 
				tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min);

	if((file_fd = open(filename, O_CREAT|O_EXCL|O_RDWR, 0777)) == -1) 
	{
		if(errno == EEXIST) {
			dAppLog(LOG_CRI, "F=%s:%s.%d PKT_SAVE exist filename=%s", __FILE__, __FUNCTION__, __LINE__, filename);
			cur_time = curtime;
			if((file_fd = open(filename, O_CREAT|O_APPEND|O_RDWR, 0777)) == -1) 
			{
				dAppLog(LOG_CRI, "F=%s:%s.%d PKT_SAVE exist fail to open filename=%s", __FILE__, __FUNCTION__, __LINE__, filename);
				return -1;
			}
			return 0;
		}
		else 
		{
			dAppLog(LOG_CRI, "F=%s:%s.%d PKT_SAVE fail to open filename=%s", __FILE__, __FUNCTION__, __LINE__, filename);
			return -2;
		}
	}

	cur_time = curtime;
	if((len = write(file_fd, pcap_file_header, PCAP_FILEHEADER_SIZE)) != PCAP_FILEHEADER_SIZE) {
		dAppLog(LOG_CRI, "F=%s:%s.%d PKT_SAVE fail to write pcap file header filename=%s", __FILE__, __FUNCTION__, __LINE__, filename);
		close_file_pcap();
		return -3;
	}

	return 1;
}

int save_pkt(Capture_Header_Msg *header, char *data)
{
	if(((header->curtime / SAVE_INTERVAL) * SAVE_INTERVAL) != cur_time)
	{
		if(open_new_file_pcap(header) < 0)
		{
			dAppLog(LOG_CRI, "F=%s:%s.%d PKT_SAVE open_new_file_pcap", __FILE__, __FUNCTION__, __LINE__);
			return -1;
		}
	}
	else if(file_fd < 0)
	{
		if(open_new_file_pcap(header) < 0)
		{
			dAppLog(LOG_CRI, "F=%s:%s.%d PKT_SAVE open_new_file_pcap", __FILE__, __FUNCTION__, __LINE__);
			return -2;
		}
	}

	if(write_pkt_pcap(header, data) < 0)
	{
		dAppLog(LOG_CRI, "F=%s:%s.%d PKT_SAVE write_pkt_pcap", __FILE__, __FUNCTION__, __LINE__);
		return -3;
	}

	return 0;
}
