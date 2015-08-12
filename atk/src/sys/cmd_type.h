
#ifndef __CMD_TYPE_H__
#define __CMD_TYPE_H__

#define __CMD_SEND_MAX__ 16
#define __CMD_RECV_MAX__ 32

typedef struct
{
	unsigned char cmd_pipe;
	unsigned char cmd_type;
	unsigned char cmd_len;
	unsigned char cmd_stat;
	unsigned char *cmd_send;
	unsigned char cmd_resp[16];
}__cmd_configure;

#define __MAX_SYS_CMD__ 	8
typedef struct
{
	__cmd_configure cmd_cfg;
	unsigned char cmd_send[__CMD_SEND_MAX__];
	unsigned char cmd_resp[__CMD_RECV_MAX__];
}__cmd_type;

#define CMD_PIPE_SHIFT	24
#define CMD_PIPE_MASK	(0xFF << CMD_PIPE_SHIFT)

#define CMD_TYPE_SHIFT	16
#define CMD_TYPE_MASK	(0xFF << CMD_TYPE_SHIFT)

#define CMD_LEN_SHIFT	8
#define CMD_LEN_MASK	(0xFF << CMD_LEN_SHIFT)

#define CMD_PIPE_TRANS	0x01
#define CMD_PIPE_ATK	0x02

#endif
