
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

int state_tag(st_PATTERN *type, OFFSET *offset, char **data, char value)
{
	int		ret;
	int		state = STATE_TAG;

	ret = pattern_find_first(type, offset, data, value);
	if(ret < 0)
	{
		if(data == NULL) {
			switch(value)
			{
			case 0x0D:
				state = STATE_0D;
				break;
			case 0x0A:
				state = STATE_0A;
				break;
			default:
				state = STATE_VSKIP;
				break;
			}

		}
		else {
			switch(value)
			{
			case 0x20:
			case 0x3A:
				state = STATE_SSKIP;
				break;
			case 0x0D:
				state = STATE_0D;
				break;
			case 0x0A:
				state = STATE_0A;
				break;
			default:
				state = STATE_VSKIP;
				break;
			}
		}
	}

	return state;
}

int state_sskip(char value)
{
	int		state;

	switch(value)
	{
	case 0x20:
	case 0x3A:
		state = STATE_SSKIP;
		break;
	case 0x0D:
		state = STATE_0D;
		break;
	case 0x0A:
		state = STATE_0A;
		break;
	default:
		state = STATE_VALUE;
		break;
	}

	return state;
}

int state_vskip(char value)
{
	int		state;

	switch(value)
	{
	case 0x0D:
		state = STATE_0D;
		break;
	case 0x0A:
		state = STATE_0A;
		break;
	default:
		state = STATE_VSKIP;
		break;
	}

	return state;
}

int state_0d0a(int sts, char value)
{
	int		state;

	switch(sts)
	{
	case STATE_0D:
		switch(value)
		{
		case 0x0D:
			state = STATE_0D0D;
			break;
		case 0x0A:
			state = STATE_0D0A;
			break;
		default:
			state = STATE_TAG;
			break;
		}
		break;
	case STATE_0A:
		switch(value)
		{
		case 0x0D:
			state = STATE_0D;
			break;
		case 0x0A:
			state = STATE_0A0A;
			break;
		default:
			state = STATE_TAG;
			break;
		}
		break;
	case STATE_0D0A:
		switch(value)
		{
		case 0x0D:
			state = STATE_0D0A0D;
			break;
		case 0x0A:
			state = STATE_0A0A;
			break;
		default:
			state = STATE_TAG;
			break;
		}
		break;
	case STATE_0D0A0D:
		switch(value)
		{
		case 0x0D:
			state = STATE_0D0D;
			break;
		case 0x0A:
			state = STATE_0D0A0D0A;
			break;
		default:
			state = STATE_TAG;
			break;
		}
		break;
	default:
		state = STATE_TAG;
		break;
	}

	return state;
}

int state_value(st_PATTERN *p, st_PATTERN_BASE *pBASE, OFFSET *offset, char **data, char value)
{
	int		state;

	pattern_find_each(p, pBASE, offset, data, value);
	
	switch(value)
	{
	case 0x0D:
		state = STATE_0D;
		break;
	case 0x0A:
		state = STATE_0A;
		break;
	default:
		state = STATE_VALUE;
		break;
	}

	return state;
}

int test_search(st_PATTERN *type, st_PATTERN *p, char *input, int size, int *state)
{
	int			i;
	OFFSET		offset = 0;
	char		*data = NULL;
	char		buf[1024];
	int			len = 0;
	int			code = 0;
	int			tag = 0;
	int			sts = *state;
	st_PATTERN_BASE		stBASE;
	st_PATTERN_BASE		*pBASE = &stBASE;

	for(i = 0; i < size; i++)
	{
		switch(sts)
		{
		case STATE_TAG:
			sts = state_tag(type, &offset, &data, input[i]);		  
			if(sts == STATE_SSKIP && data != NULL) {
				tag = *((int *)data);
				// assign p (st_PATTERN)
				printf("tag=%d\n", tag);
			}
			break;
		case STATE_SSKIP:
			sts = state_sskip(input[i]);
			if(sts == STATE_VALUE) {
				offset = 0;
				data = NULL;
				len = 0;
				i--;
			}
			break;
		case STATE_VSKIP:
			sts = state_vskip(input[i]);
			break;
		case STATE_0D:
		case STATE_0A:
		case STATE_0D0A:
		case STATE_0D0A0D:
			sts = state_0d0a(sts, input[i]);
			if(sts == STATE_TAG) {
				offset = 0;
				data = NULL;
				i--;
			}
			else if(sts == STATE_0D0D || sts == STATE_0A0A || sts == STATE_0D0A0D0A) {
				*state = STATE_0D0A0D0A;
				return i;
			}
			break;
		case STATE_VALUE:
			sts = state_value(p, pBASE, &offset, &data, input[i]);
			if(sts == STATE_0D || sts == STATE_0A) {
				buf[len] = 0x00;
				printf("value=%d:%s\n", len, buf);
				if(data != NULL) {
					code = *((int *)data);
					printf("code=%d\n", code);
				}
			}
			else {
				buf[len++] = input[i];
			}
			break;
		default:
			sts = STATE_TAG;
			offset = 0;
			data = NULL;
			i--;
			break;
		}
	}

	*state = sts;

	return i;
}

void test_search1(st_PATTERN *p, char *input, int size)
{
	int			i, ret;
	int			code;
	OFFSET		offset = 0;
	char		*data = NULL;
	st_PATTERN_BASE		stBASE;
	st_PATTERN_BASE		*pBASE = &stBASE;

	for(i = 0; i < size; i++)
	{
//printf("===> 1 offset=%ld data=%p input=%c\n", offset, data, input[i]);
		if((ret = pattern_find_each(p, pBASE, &offset, &data, input[i])) < 0)
		{
			if(data != NULL) 
			{
				code = *(int *)data;
printf("===> find code=%d\n", code);
			}
		}
//printf("===> 2 offset=%ld data=%p input=%c\n", offset, data, input[i]);
	}
}

#if 0
int main(int argc, char **argv)
{
	int				opt;
	st_PATTERN		*p;
	st_PATTERN		*type;

	int				state;
//	int				i;
	int				ret;
	int				len;
	int				data;
//	int				out_len;
//	char			*value;
	char			buf[1024];
//	char			out[BUFSIZ];

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

	if((type = pattern_init(8, 10240, 10240)) == NULL)
	{
		printf("pattern_init null\n");
		exit(0);
	}

	sprintf(buf, "daum.net");
	data = 10;
	if((ret = pattern_add(p, buf, strlen(buf), &data)) < 0)
	{
		printf("pattern_add ret=%d\n", ret);
		exit(0);
	}

	sprintf(buf, "no-cache");
	data = 20;
	if((ret = pattern_add(p, buf, strlen(buf), &data)) < 0)
	{
		printf("pattern_add ret=%d\n", ret);
		exit(0);
	}

	sprintf(buf, "facebook.com");
	data = 30;
	if((ret = pattern_add(p, buf, strlen(buf), &data)) < 0)
	{
		printf("pattern_add ret=%d\n", ret);
		exit(0);
	}

//pattern_reset(p);

	sprintf(buf, "google.com");
	data = 40;
	if((ret = pattern_add(p, buf, strlen(buf), &data)) < 0)
	{
		printf("pattern_add ret=%d\n", ret);
		exit(0);
	}

	sprintf(buf, "pooq.imbc.com");
	data = 50;
	if((ret = pattern_add(p, buf, strlen(buf), &data)) < 0)
	{
		printf("pattern_add ret=%d\n", ret);
		exit(0);
	}

	sprintf(buf, "imbc");
	data = 60;
	if((ret = pattern_add(p, buf, strlen(buf), &data)) < 0)
	{
		printf("pattern_add ret=%d\n", ret);
		exit(0);
	}

	sprintf(buf, "PooqiP");
	data = 70;
	if((ret = pattern_add(p, buf, strlen(buf), &data)) < 0)
	{
		printf("pattern_add ret=%d\n", ret);
		exit(0);
	}

	sprintf(buf, "keep-alive");
	data = 80;
	if((ret = pattern_add(p, buf, strlen(buf), &data)) < 0)
	{
		printf("pattern_add ret=%d\n", ret);
		exit(0);
	}

	sprintf(buf, "Host");
	data = 100;
	if((ret = pattern_add(type, buf, strlen(buf), &data)) < 0)
	{
		printf("pattern_add ret=%d\n", ret);
		exit(0);
	}

	sprintf(buf, "User-Agent");
	data = 110;
	if((ret = pattern_add(type, buf, strlen(buf), &data)) < 0)
	{
		printf("pattern_add ret=%d\n", ret);
		exit(0);
	}

	sprintf(buf, "Connection");
	data = 120;
	if((ret = pattern_add(type, buf, strlen(buf), &data)) < 0)
	{
		printf("pattern_add ret=%d\n", ret);
		exit(0);
	}

	sprintf(buf, "Content-Type");
	data = 130;
	if((ret = pattern_add(type, buf, strlen(buf), &data)) < 0)
	{
		printf("pattern_add ret=%d\n", ret);
		exit(0);
	}
	sprintf(buf, "Ho");
	data = 140;
	if((ret = pattern_add(type, buf, strlen(buf), &data)) < 0)
	{
		printf("pattern_add ret=%d\n", ret);
		exit(0);
	}


/*
	pattern_print(p, print_func);

	sprintf(buf, "abc");
	if((ret = pattern_del(p, buf, strlen(buf))) < 0)
	{
		printf("pattern_del ret=%d\n", ret);
		exit(0);
	}
*/

	pattern_print(p, print_func);
	pattern_print(type, print_func);

	printf("#################\n");

	len = 0;

#if 0
	for(i = 0; i < 10; i++)
	{
		buf[len++] = i + 1;
	}
#endif

	sprintf(&buf[len], 
		"GET /onair/TVplayerLive.ashx?channelid=0 HTTP/1.1\r\n"
		"Host: pooq.imbc.com\r\n"
		"x-Host: pooq.imbc.com111\r\n"
		"User-Agent: PooqiPhone/1.2 CFNetwork/548.0.4 Darwin/11.0.0\r\n"
		"Accept: */*\r\n"
		"Accept-Language: ko-kr\r\n"
		"Accept-Encoding: gzip, deflate\r\n"
		"Connection: keep-alive\r\n"
		"Content-Type: application/json; charset=UTF-8\r\n"
		"X-Content-Type-Options: nosniff\r\n"
		"\r\n"
		"X-AspNet-Version: 2.0.50727\r\n"
		"Cache-Control: no-cache\r\n"
		"Host: pooq.imbc.com222\r\n"
		"\r\n"
		);

//	len += strlen(&buf[len]);
	len = strlen(buf);

#if 0
	for(i = 0; i < 10; i++)
	{
		buf[len++] = i + 1;
	}
#endif

	buf[len] = 0x00;

#if 0
	sprintf(buf, "imbc");
	len = strlen(buf);
//	out_len = BUFSIZ;
	if((value = pattern_find(p, buf, len)) == NULL)
	{
		printf("===> not find str=\n%s\n", buf);
	}
	else
	{
//		printf("===> find data=%d out=%s\nstr=\n%s\n", *(int *)value, out, buf);
		printf("===> find data=%d str=\n%s\n", *(int *)value, buf);
	}
#endif

	state = STATE_TAG;
	ret = test_search(type, p, buf, len, &state);
//	test_search1(p, buf, len);

	printf("#####################\n");
	printf("ILEN=%d OLEN=%d STATE=%d DATA=\n%s", len, ret, state, buf);

	return 0;
}
#endif

int main(int argc, char **argv)
{
	int				opt;
	st_PATTERN		*p;

//	int				state;
//	int				i;
	int				ret;
	int				len;
	int				data;
//	int				out_len;
//	char			*value;
	char			buf[1024];
//	char			out[BUFSIZ];

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

	sprintf(buf, "m.stom.videofarm.daum.net");
	data = 10;
	if((ret = pattern_add(p, buf, strlen(buf), &data)) < 0)
	{
		printf("pattern_add ret=%d\n", ret);
		exit(0);
	}

	sprintf(buf, "story.kakao.co.kr");
	data = 20;
	if((ret = pattern_add(p, buf, strlen(buf), &data)) < 0)
	{
		printf("pattern_add ret=%d\n", ret);
		exit(0);
	}

	sprintf(buf, "tory.kakao.co.kr");
	data = 30;
	if((ret = pattern_add(p, buf, strlen(buf), &data)) < 0)
	{
		printf("pattern_add ret=%d\n", ret);
		exit(0);
	}

	pattern_print(p, print_func);

	printf("#################\n");

	len = 0;

	sprintf(&buf[len], 
		"up-m.story.kakao.co.kr\r\n"
#if 0
		"GET /onair/TVplayerLive.ashx?channelid=0 HTTP/1.1\r\n"
		"Host: up-m.story.kakao.co.kr\r\n"
		"x-Host: pooq.imbc.com111\r\n"
		"User-Agent: PooqiPhone/1.2 CFNetwork/548.0.4 Darwin/11.0.0\r\n"
		"Accept: */*\r\n"
		"Accept-Language: ko-kr\r\n"
		"Accept-Encoding: gzip, deflate\r\n"
		"Connection: keep-alive\r\n"
		"Content-Type: application/json; charset=UTF-8\r\n"
		"X-Content-Type-Options: nosniff\r\n"
		"\r\n"
		"X-AspNet-Version: 2.0.50727\r\n"
		"Cache-Control: no-cache\r\n"
		"Host: pooq.imbc.com222\r\n"
		"\r\n"
#endif
		);

	len = strlen(buf);
	buf[len] = 0x00;

	printf("input str=\n%s\n", buf);
	test_search1(p, buf, len);

#if 0
	if((value = pattern_find(p, buf, len)) == NULL)
	{
		printf("===> not find str=\n%s\n", buf);
	}
	else
	{
		printf("===> find data=%d str=\n%s\n", *(int *)value, buf);
	}
#endif

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
	int					base = 0;
	int					cnt = 0;
	OFFSET				offset;
	char				*data = NULL;
	st_PATTERN_NODE		*pCur, *pNext;

	pCur = (st_PATTERN_NODE *)pattern_ptr(p, p->nodeStartOffset);
	for(i = 0; i < size; i++)
	{
		cnt++;
		c = input[i];
		offset = pCur->nextNodeOffset[c];
		if(offset == 0)
		{
			if(data != NULL) {
				break;
			}
			else {
				offset = p->nodeStartOffset;
printf("reset idx=%d:%c base=%d\n", i, c, base);
				if(base > 0) i = base;
				base = 0;
			}
		}
		else if(base == 0) 
		{
			base = i + 1;
printf("find idx=%d:%c base=%d\n", i, c, base);
		}

		pNext = (st_PATTERN_NODE *)pattern_ptr(p, offset);
		pCur = pNext;

		if(pCur->dataOffset > 0) data = pattern_ptr(p, pCur->dataOffset) + sizeof(st_PATTERN_DATA);
	}

printf("===> find loop cnt=%d:%d\n", cnt, size);
	return data;
}

int pattern_find_each(st_PATTERN *p, st_PATTERN_BASE *pBASE, OFFSET *root, char **data, char input)
{
	int					flag = 1;
	int					c;
	int					i, cnt;
	OFFSET				offset;
	st_PATTERN_NODE		*pPrev, *pCur;

	offset = *root;
	if(offset < 0) return -1;
	else if(offset == 0) offset = p->nodeStartOffset;

	if(offset == p->nodeStartOffset) pBASE->base = 0;

	pPrev = (st_PATTERN_NODE *)pattern_ptr(p, offset);

	c = input;
	offset = pPrev->nextNodeOffset[c];
	if(offset == 0)
	{
		if(*data != NULL) {
			flag = -1;
			offset = -1;
		}
		else {
			offset = p->nodeStartOffset;

			if(pBASE->base > 0) {
				pBASE->stack[pBASE->base++] = c;
				cnt = pBASE->base;
				for(i = 1; i < cnt; i++)
				{
					pattern_find_each(p, pBASE, &offset, data, pBASE->stack[i]);
				}
			}
		}
		pCur = NULL;
	}
	else
	{
		pCur = (st_PATTERN_NODE *)pattern_ptr(p, offset);
		pBASE->stack[pBASE->base++] = c;
	}

	if((pCur != NULL) && (pCur->dataOffset > 0)) *data = pattern_ptr(p, pCur->dataOffset) + sizeof(st_PATTERN_DATA);
	*root = offset;

	return flag;
}

int pattern_find_first(st_PATTERN *p, OFFSET *root, char **data, char input)
{
	int					flag = 1;
	int					c;
	OFFSET				offset;
	st_PATTERN_NODE		*pPrev, *pCur;

	offset = *root;
	if(offset <= 0) offset = p->nodeStartOffset;

	pPrev = (st_PATTERN_NODE *)pattern_ptr(p, offset);

	c = input;
	offset = pPrev->nextNodeOffset[c];
	if(offset == 0)
	{
		flag = -1;
		pCur = NULL;
	}
	else
	{
		pCur = (st_PATTERN_NODE *)pattern_ptr(p, offset);
	}

	if((pCur != NULL) && (pCur->dataOffset > 0)) *data = pattern_ptr(p, pCur->dataOffset) + sizeof(st_PATTERN_DATA);
	*root = offset;

	return flag;
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


