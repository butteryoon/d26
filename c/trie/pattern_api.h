#ifndef __PATTERN_API_H__
#define __PATTERN_API_H__

#define OFFSET				long
#define MAX_NODE_ARRAY		256
#define MAX_PATTERN_LEN		1024

typedef struct __pattern_node__
{
	char		isFree;
	char		value;
	char		cnt;
	OFFSET		dataOffset;
	OFFSET		nextNodeOffset[MAX_NODE_ARRAY];
} st_PATTERN_NODE;

typedef struct __pattern_data__
{
	char		isFree;
} st_PATTERN_DATA;

typedef struct __pattern__
{
	int			dataSize;
	int			dataCnt;
	int			nodeSize;
	int			nodeCnt;
	OFFSET		dataStartOffset;	
	OFFSET		dataEndOffset;	
	OFFSET		dataFreeOffset;	
	OFFSET		nodeStartOffset;
	OFFSET		nodeEndOffset;
	OFFSET		nodeFreeOffset;
} st_PATTERN;

#define pattern_offset(infoptr, ptr)  (OFFSET) ((char *) (ptr==NULL?0:(char *)ptr-(char *)infoptr))
#define pattern_ptr(infoptr, offset)  (char *) ((OFFSET) (offset==0?NULL:(OFFSET)offset+(char *)infoptr))

extern st_PATTERN *pattern_init(int dataSize, int dataCnt, int nodeCnt);
extern int pattern_add(st_PATTERN *p, char *pattern, int size, void *data);
extern int pattern_del(st_PATTERN *p, char *pattern, int size);
extern void pattern_reset(st_PATTERN *p);
extern char *pattern_find(st_PATTERN *p, char *input, int size);
extern void pattern_print(st_PATTERN *p, void (*print_func)(st_PATTERN *p, char *data));


#endif
