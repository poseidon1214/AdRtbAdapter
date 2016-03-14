#include <stdio.h>
#include <stdlib.h>

#include "conhash.h"

struct node_s g_nodes[64];
struct conhash_s *conhash;

struct user_value_s{
	char stripaddr[16];
	char strport[10];
	void *conn; 
};

int main()
{
    int i;
    const struct node_s *node;
	struct user_value_s value1,value2,value3,value4;
	struct user_value_s *val;
    char str[128];
    long hashes[512];

    /* init conhash instance */
    conhash = conhash_init(NULL);
    if(conhash)
    {
        /* set nodes */
		strcpy(value1.stripaddr,"192.168.3.2031");
		strcpy(value1.strport,"16379");
        conhash_set_node(&g_nodes[0], "192.168.3.201 6379", 205,&value1);
		printf("node is %s\n",g_nodes[0].iden);

		strcpy(value2.stripaddr,"192.168.3.2032");
		strcpy(value2.strport,"16380");
        conhash_set_node(&g_nodes[1], "192.168.3.201 6380", 200,&value2);

		strcpy(value3.stripaddr,"192.168.3.201");
		strcpy(value3.strport,"16381");
        conhash_set_node(&g_nodes[2], "192.168.3.201 6381", 200, &value3);

		strcpy(value4.stripaddr,"192.168.3.201");
		strcpy(value4.strport,"16382");
        conhash_set_node(&g_nodes[3], "192.168.3.201 6382", 240, &value4);

        /* add nodes */
        conhash_add_node(conhash, &g_nodes[0]);
        conhash_add_node(conhash, &g_nodes[1]);
        conhash_add_node(conhash, &g_nodes[2]);
        conhash_add_node(conhash, &g_nodes[3]);

        printf("virtual nodes number %d\n", conhash_get_vnodes_num(conhash));
        printf("the hashing results--------------------------------------:\n");

        /* try object */
        for(i = 0; i < 10; i++)
        {
            sprintf(str, "James.km%03d", i);
            node = conhash_lookup(conhash, str);
            if(node) {
				val = (struct user_value_s*)node->value;
				//printf("[%16s] is in node: [%16s]\n","abcd", node->iden);
				printf("ip =%s,port = %s\n",val->stripaddr,val->strport);
			}
        }
		
	/*	printf("add one nodes -------------------------:\n");
		conhash_set_node(&g_nodes[4], "guanghui", 200);
		int ret;
		ret=conhash_add_node(conhash, &g_nodes[4]);
		if(ret==-1)
		{
			printf("add node failed\n");
		}		
	
		printf("virtual nodes number %d\n", conhash_get_vnodes_num(conhash));
        for(i = 0; i < 20; i++)
        {
            sprintf(str, "James.km%03d", i);
            node = conhash_lookup(conhash, str);
            if(node) printf("[%16s] is in node: [%16s]\n", str, node->iden);
        }

        conhash_get_vnodes(conhash, hashes, sizeof(hashes)/sizeof(hashes[0]));
       	printf("hash is %ld\n",hashes[1]);
		conhash_del_node(conhash, &g_nodes[2]);
        
		printf("remove node[%s], virtual nodes number %d\n", g_nodes[2].iden, conhash_get_vnodes_num(conhash));
        printf("the hashing results--------------------------------------:\n");
        for(i = 0; i < 20; i++)
        {
            sprintf(str, "James.km%03d", i);
            node = conhash_lookup(conhash, str);
            if(node) printf("[%16s] is in node: [%16s]\n", str, node->iden);
        }*/
    }
    conhash_fini(conhash);
    return 0;
}
