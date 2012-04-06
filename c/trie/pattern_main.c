
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "pattern_api.h"

void print_func(st_PATTERN *p, char *data)
{
	int		*value;

	value = (int *)(data + sizeof(st_PATTERN_DATA));

	printf("data=%d\n", *value);
}

int main(int argc, char **argv)
{
	int				opt;
	st_PATTERN		*p;

	int				ret;
	int				data;
	char			*value;
	char			buf[BUFSIZ];

	while((opt = getopt(argc, argv, "f:p:")) != -1)
	{
		switch(opt)
		{
		case 'f':
			break;
		case 'p':
			break;
		}
	}

	if((p = pattern_init(8, 10240, 10240)) == NULL)
	{
		printf("pattern_init null\n");
		exit(0);
	}

	sprintf(buf, "abc");
	data = 10;
	if((ret = pattern_add(p, buf, strlen(buf), &data)) < 0)
	{
		printf("pattern_add ret=%d\n", ret);
		exit(0);
	}

	sprintf(buf, "abcd");
	data = 20;
	if((ret = pattern_add(p, buf, strlen(buf), &data)) < 0)
	{
		printf("pattern_add ret=%d\n", ret);
		exit(0);
	}

	sprintf(buf, "facebook");
	data = 30;
	if((ret = pattern_add(p, buf, strlen(buf), &data)) < 0)
	{
		printf("pattern_add ret=%d\n", ret);
		exit(0);
	}

//pattern_reset(p);

	sprintf(buf, "google");
	data = 40;
	if((ret = pattern_add(p, buf, strlen(buf), &data)) < 0)
	{
		printf("pattern_add ret=%d\n", ret);
		exit(0);
	}

	sprintf(buf, "ab");
	data = 50;
	if((ret = pattern_add(p, buf, strlen(buf), &data)) < 0)
	{
		printf("pattern_add ret=%d\n", ret);
		exit(0);
	}

	pattern_print(p, print_func);

	sprintf(buf, "abc");
//	sprintf(buf, "goo");
	if((ret = pattern_del(p, buf, strlen(buf))) < 0)
	{
		printf("pattern_del ret=%d\n", ret);
		exit(0);
	}

	pattern_print(p, print_func);

	printf("#################\n");
	sprintf(buf, "www.abc123.com");
	if((value = pattern_find(p, buf, strlen(buf))) == NULL)
	{
		printf("not find str=%s\n", buf);
	}
	else
	{
		printf("find str=%s data=%d\n", buf, *(int *)value);
	}

	return 0;
}

st_PATTERN *pattern_init(int dataSize, int dataCnt, int nodeCnt)
{
	int			size;
	st_PATTERN	*p;

	size = sizeof(st_PATTERN) + (sizeof(st_PATTERN_DATA) + dataSize) * nodeCnt + sizeof(st_PATTERN_NODE) * nodeCnt;

	printf("size=%d\n", size);

	if((p = malloc(size)) == NULL)
	{
		return NULL;
	}

	p->dataSize = sizeof(st_PATTERN_DATA) + dataSize;
	p->dataCnt = dataCnt;
	p->nodeSize = sizeof(st_PATTERN_NODE);
	p->nodeCnt = nodeCnt;
	p->dataStartOffset = sizeof(st_PATTERN);
	p->dataEndOffset = p->dataStartOffset + p->dataSize * p->dataCnt;
	p->dataFreeOffset = p->dataStartOffset + p->dataSize * 1;
	p->nodeStartOffset = p->dataEndOffset;
	p->nodeEndOffset = p->nodeStartOffset + p->nodeSize * nodeCnt;
	p->nodeFreeOffset = p->nodeStartOffset + p->nodeSize * 1;

	pattern_reset(p);

	return p;
}

char *pattern_get_node(st_PATTERN *p, char value)
{
	int				i, j;
	st_PATTERN_NODE	*node = NULL;

	for(i = 0; i < p->nodeCnt; i++)
	{
		node = (st_PATTERN_NODE *)pattern_ptr(p, p->nodeFreeOffset);

		p->nodeFreeOffset += p->nodeSize;
        if(p->nodeFreeOffset == p->nodeEndOffset) {
            p->nodeFreeOffset = p->nodeStartOffset + p->nodeSize;
        }

		if(node->isFree == 0)
		{
			node->value = value;
			node->cnt = 0;
			node->dataOffset = 0;
			for(j = 0; j < MAX_NODE_ARRAY; j++)
			{
				node->nextNodeOffset[j] = 0;
			}
			node->isFree = 1;
			break;
		}
	}

	return (char *)node;
}

void pattern_del_node(st_PATTERN *p, st_PATTERN_NODE *node)
{
	node->isFree = 0;
}

char *pattern_get_data(st_PATTERN *p)
{
	int				i;
	st_PATTERN_DATA	*data = NULL;

	for(i = 0; i < p->dataCnt; i++)
	{
		data = (st_PATTERN_DATA *)pattern_ptr(p, p->dataFreeOffset);

		p->dataFreeOffset += p->dataSize;
        if(p->dataFreeOffset == p->dataEndOffset) {
            p->dataFreeOffset = p->dataStartOffset + p->dataSize;
        }

		if(data->isFree == 0)
		{
			data->isFree = 1;
			break;
		}
	}

	return (char *)data;
}

void pattern_del_data(st_PATTERN *p, st_PATTERN_DATA *data)
{
	data->isFree = 0;
}

char *pattern_find_same(st_PATTERN *p, char *input, int size)
{
	int					i;
	int					c;
	OFFSET				offset;
	st_PATTERN_NODE		*pCur, *pNext;

	pCur = (st_PATTERN_NODE *)pattern_ptr(p, p->nodeStartOffset);
	for(i = 0; i < size; i++)
	{
		c = input[i];
		offset = pCur->nextNodeOffset[c];
		if(offset == 0)
		{
			return NULL;
		}

		pNext = (st_PATTERN_NODE *)pattern_ptr(p, offset);
		pCur = pNext;
	}

	return pattern_ptr(p, pCur->dataOffset);
}

int pattern_add(st_PATTERN *p, char *pattern, int size, void *data)
{
	int					c;
	int					i;
	OFFSET				offset;
	char				*pData;
	st_PATTERN_NODE		*pCur, *pNext;

	if(size > MAX_PATTERN_LEN) return -1;

	if((pData = pattern_find_same(p, pattern, size)) != NULL) {
		if(!memcmp(data, &pData[sizeof(st_PATTERN_DATA)], p->dataSize - sizeof(st_PATTERN_DATA))) {
			return 0;	
		}
		else {
			return -2;
		}
	}

	pCur = (st_PATTERN_NODE *)pattern_ptr(p, p->nodeStartOffset);

	for(i = 0; i < size; i++)
	{
		c = pattern[i];
		offset = pCur->nextNodeOffset[c];	
		if(offset == 0)
		{
			pNext = (st_PATTERN_NODE *)pattern_get_node(p, c);
			offset = pattern_offset(p, pNext);
			pCur->nextNodeOffset[c] = offset;
			pCur->cnt++;
		}
		else
		{
			pNext = (st_PATTERN_NODE *)pattern_ptr(p, offset);
		}

		if(pNext == NULL) return -3;
		pCur = pNext;
	}

	pData = pattern_get_data(p);
	if(pData == NULL) return -4;
	memcpy(&pData[sizeof(st_PATTERN_DATA)], data, p->dataSize);

	pCur->dataOffset = pattern_offset(p, pData);

	return 1;
}

void pattern_print_stack(st_PATTERN *p, OFFSET *stack, int cnt)
{
	int					i;
	OFFSET				offset;
	st_PATTERN_NODE		*node;

	printf("### STACK:%d ###\n", cnt);
	for(i = 0; i < cnt; i++)
	{
		offset = stack[i];
		node = (st_PATTERN_NODE *)pattern_ptr(p, offset);
		printf("idx=%d c=%c\n", i, node->value);
	}
	printf("################\n");
}

int pattern_del(st_PATTERN *p, char *pattern, int size)
{
	int					i, cnt = 0;
	int					c, value;
	OFFSET				offset;
	OFFSET				stack[MAX_PATTERN_LEN];
	st_PATTERN_NODE		*pCur, *pNext, *pNode;

	pCur = (st_PATTERN_NODE *)pattern_ptr(p, p->nodeStartOffset);
	for(i = 0; i < size; i++)
	{
		c = pattern[i];
		offset = pCur->nextNodeOffset[c];
		if(offset == 0) return -1;
		pNext = (st_PATTERN_NODE *)pattern_ptr(p, offset);
		stack[cnt] = offset;	
		cnt++;
		pCur = pNext;
	}

//pattern_print_stack(p, stack, cnt);

	if(cnt > 0)
	{
		pNode = (st_PATTERN_NODE *)pattern_ptr(p, stack[cnt-1]);
		if(pNode->dataOffset > 0)
		{
			pattern_del_data(p, (st_PATTERN_DATA *)pattern_ptr(p, pNode->dataOffset));	
			pNode->dataOffset = 0;
		}
		value = -1;

		for(i = 0; i < size; i++)
		{
			cnt--;
			pNode = (st_PATTERN_NODE *)pattern_ptr(p, stack[cnt]);
			if(value >= 0) 
			{
				pNode->nextNodeOffset[value] = 0;
				pNode->cnt--;
			}

			if(pNode->cnt > 0 || pNode->dataOffset > 0)	break;

			value = pNode->value;	
			pattern_del_node(p, pNode);	
		}
	}

	return 1;
}

void pattern_reset(st_PATTERN *p)
{
	char		*tmp;

	tmp = pattern_ptr(p, p->dataStartOffset);
	if(tmp != NULL) memset(tmp, 0x00, (p->dataSize * p->dataCnt));
	tmp = pattern_ptr(p, p->nodeStartOffset);
	if(tmp != NULL) memset(tmp, 0x00, (p->nodeSize * p->nodeCnt));
}

char *pattern_find(st_PATTERN *p, char *input, int size)
{
	int					i;
	int					c;
	OFFSET				offset;
	char				*data = NULL;
	st_PATTERN_NODE		*pCur, *pNext;

	pCur = (st_PATTERN_NODE *)pattern_ptr(p, p->nodeStartOffset);
	for(i = 0; i < size; i++)
	{
		c = input[i];
		offset = pCur->nextNodeOffset[c];
		if(offset == 0)
		{
			if(data != NULL) {
				break;
			}
			else {
				offset = p->nodeStartOffset;
			}
		}

		pNext = (st_PATTERN_NODE *)pattern_ptr(p, offset);
		pCur = pNext;

		if(pCur->dataOffset > 0) data = pattern_ptr(p, pCur->dataOffset) + sizeof(st_PATTERN_DATA);
	}

printf("find loop cnt=%d:%d\n", i, size);
	return data;
}

void pattern_sub_print(st_PATTERN *p, st_PATTERN_NODE *node, char *value, int cnt, void (*print_func)(st_PATTERN *p, char *data))
{
	int			i;
	OFFSET		next;

//	printf("C=%c CNT=%d\n", node->value, node->cnt);
	
	if(node->dataOffset > 0)
	{
		printf("PATTERN=%.*s\n", cnt, value);
		if(print_func != NULL) {
			print_func(p, pattern_ptr(p, node->dataOffset));
		}
		else
		{

		}
	}

	for(i = 0; i < MAX_NODE_ARRAY; i++)
	{
		next = node->nextNodeOffset[i];
		if(next > 0)
		{
			value[cnt] = i;
			pattern_sub_print(p, (st_PATTERN_NODE *)pattern_ptr(p, next), value, cnt+1, print_func);
		}
	}
}

void pattern_print(st_PATTERN *p, void (*print_func)(st_PATTERN *p, char *data))
{
	int					cnt = 0;
	char				value[MAX_PATTERN_LEN];
	st_PATTERN_NODE		*pCur;

	pCur = (st_PATTERN_NODE *)pattern_ptr(p, p->nodeStartOffset);

	printf("### PRINT PATTERN ###\n");
	pattern_sub_print(p, pCur, value, cnt, print_func);
	printf("#####################\n");
}


