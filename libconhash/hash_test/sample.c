#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "conhash.h"

struct node_s g_nodes[128];
struct conhash_s *conhash;

struct user_value_s{
	char stripaddr[16];
	char strport[10];
	void *conn; 
};

int main(int argc,char **argv)
{
    int i = 0;
	FILE *fp = NULL;
    const struct node_s *node;
	struct user_value_s value;
	struct user_value_s *val;
    char str[128];
    long hashes[512];
	char linebuf[128];

	if(argc<3)
	{

		printf("useage:./hash_test.exe cfg_file key\n");
		return -1;
	}
	
	
    fp = fopen(argv[1],"r");
    if(fp == NULL)
    {
       printf("No Such File\n");
       return -1;
    }

    /* init conhash instance */
    conhash = conhash_init(NULL);
    if(conhash)
    {
        
		while(fgets(linebuf,sizeof(linebuf),fp)!= NULL)
		{
			linebuf[strlen(linebuf)-1] = '\0';
			//printf("linebuf=%s",linebuf);
			/* set nodes */
        	conhash_set_node(&g_nodes[i],linebuf,200,&value);
        	/* add nodes */
        	conhash_add_node(conhash, &g_nodes[i]);
			i++;
		}

	}    
        
	node = conhash_lookup(conhash, argv[2]);
    if(node) 
	{
		//val = (struct user_value_s*)node->value;
		printf("[ %s ] is in node: [ %s ]\n",argv[2], node->iden);
	}
	
	conhash_fini(conhash);
    return 0;
}




