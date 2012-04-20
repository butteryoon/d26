#ifndef __PATTERN_API_H__
#define __PATTERN_API_H__

#define OFFSET				long
#define MAX_NODE_ARRAY		256
#define MAX_PATTERN_LEN		1024

#define STATE_TAG			1
#define STATE_SSKIP			2
#define STATE_VSKIP			3
#define STATE_VALUE			4
#define STATE_0D			5
#define STATE_0D0A			6
#define STATE_0D0A0D	   	7
#define STATE_0D0A0D0A 		8
#define STATE_0D0D			9
#define STATE_0A			10
#define STATE_0A0A			11

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

typedef struct __pattern_base__
{
	int			base;
	char		stack[MAX_PATTERN_LEN];
} st_PATTERN_BASE;

#define pattern_offset(infoptr, ptr)  (OFFSET) ((char *) (ptr==NULL?0:(char *)ptr-(char *)infoptr))
#define pattern_ptr(infoptr, offset)  (char *) ((OFFSET) (offset==0?NULL:(OFFSET)offset+(char *)infoptr))

extern st_PATTERN *pattern_init(int dataSize, int dataCnt, int nodeCnt);
extern int pattern_add(st_PATTERN *p, char *pattern, int size, void *data);
extern int pattern_del(st_PATTERN *p, char *pattern, int size);
extern void pattern_reset(st_PATTERN *p);
extern char *pattern_find(st_PATTERN *p, char *input, int size);
extern void pattern_print(st_PATTERN *p, void (*print_func)(st_PATTERN *p, char *data));
extern int pattern_find_each(st_PATTERN *p, st_PATTERN_BASE *pBASE, OFFSET *root, char **data, char input);
extern int pattern_find_first(st_PATTERN *p, OFFSET *root, char **data, char input);


#endif
