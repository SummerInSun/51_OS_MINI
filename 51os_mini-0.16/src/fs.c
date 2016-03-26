#include "fs.h"

#ifdef FS_SELECT

/* ��ֵ�洢ָ���ַ����ָ��ָ��ǰ�ļ��� */
xdata unsigned int fs_struct_ptr;

/* ��ֵ�洢ָ���ַ����ָ��ָ���һ���յ��ڴ�飬�ӿ�ϵͳ�ڴ������� */
xdata unsigned int first_empty_page;

/* ������ʱ������������ʱ�ı���������Ϊ92�ֽ� */
xdata unsigned char erase_return_buff[FS_NEED_KEEP];

/* ���ı��ļ�����д�����ݵĻ�����,����Ϊ1K�ֽ� */
xdata unsigned char write_buff[1024];

/* 
 * ��ָ�����ڴ�������Ӧ���ֵ�node
 * ����find_file(0x5000, "dir1", 0); //��0x5000�ڴ�鴦��������Ϊ"dir1"���ļ�
 * ���ҵ��򷵻�����node�ĵ�ַ����û�ҵ��򷵻�0
 *     find_file(0x5000, "dir1", 1); //��0x5000�ڴ�鴦��������Ϊ"dir1"���ļ�
 * ���ҵ��򷵻��ļ��ĵ�ַ����û�ҵ��򷵻�0
 */
unsigned int find_file(unsigned int ptr, unsigned char *name, unsigned char flag)
{
	xdata unsigned int i,count = 0;
	xdata unsigned int next_ptr_mod = 0;
	xdata unsigned char name_mod[20];
	
	/* �������в���Ҫ���ҵ� */
	ptr = ptr + FS_FLAG_SIZE + FS_NAME_SIZE + FS_SIZE_SIZE + FS_PREV_SIZE + FS_NEXT_SIZE;
ENTER_CRITICAL;	//��Ҫ��ʱ��ִ�еĳ���Ҫ�Ƚ����ٽ��	
	/* ���ҷǿյ�node�ڵ� */
	for(i = 0; i < FS_NODE_SIZE; i += 2)
	{
		if(iap_read(ptr + i) == 0xFF)
		{
//			return 0;
//			printf("0xFF\r\n");
		}
		else
		{
			next_ptr_mod = iap_read(ptr + i + 1);	//�����ֽڷ���ptr_mod
			next_ptr_mod = next_ptr_mod << 8;					//����8λ������ptr_mod���ֽ�
			next_ptr_mod |= iap_read(ptr + i);		//�����ֽڷ���ptr_mod��8λ
			
			next_ptr_mod = next_ptr_mod + FS_FLAG_SIZE;		//�����ļ����͵Ķ�ȡ�����������			

			while(name_mod[count] = iap_read(next_ptr_mod + count))	//�ִ��ǿվ�һֱ��ȡ���ִ�ŵ����黺������
			{
				count ++;
			}
			name_mod[count] = '\0';	//�ַ������
			
			if(!os_strcmp("*", name))	/* Ҫ�ҵ����е��ִ� */
			{
				if(flag)
				{
					EXIT_CRITICAL;
					return (next_ptr_mod - FS_FLAG_SIZE);	//ֻҪ�ҵ���һ�����ǿյ�node�ͷ���
				}
				else
				{
					EXIT_CRITICAL;
					return i;	//ֻҪ�ҵ���һ�����ǿյ�node�ͷ���
				}
				
			}
			if(!os_strcmp(name_mod, name))	 /* �Ƿ��ҵ���Ӧ���ִ� */
			{
//				printf("find\r\n");
				if(flag)
				{
					EXIT_CRITICAL;
					return (next_ptr_mod - FS_FLAG_SIZE);
				}
				else
				{
					EXIT_CRITICAL;
					return i;	//����node�ĵ�ַ
				}
			}					

			count = 0;	
			next_ptr_mod = 0;	//���㣬�ȴ���һ�β���
		}
	}

	EXIT_CRITICAL;
	return 0;	//δ�ҵ�
}

/* 
 * �ռ���䣬����������Ŀռ��ַ
 * ����alloc(128 * 3, pos); ����һ��128*3�ֽڴ�С�Ŀռ䲢�ҷ��ؿռ����ڵ�λ�õ�pos��
 * �ú���������ص���0���ʾ�ռ䲻�㣬����ʧ��
 */
unsigned int alloc(unsigned int len, unsigned int *pos)
{
	xdata unsigned int i;
	xdata unsigned int temp;	
	xdata unsigned int find;

	/* ��ȷ��Ҫ����Ŀ�Ĵ�С��������512�ֽڶ��룬����Ҫ���������Ĳ��ֵ�512�ֽ� */
	if(i = len % FS_PAGE)
	{
		i = (len + FS_PAGE) / FS_PAGE;
	}
	else
	{
		i = len / FS_PAGE;
	}

	temp = i;

	/* ���ҿռ��еĿտ� */
	for(find = FS_BEGIN; (find < FS_END) && (i > 0); find += FS_PAGE)
	{
		if(iap_read(find) & 0x01) 	//Ϊ1��ʾ�˿��ǿ��еģ�δʹ�õ�
		{
			pos[temp-i] = find;	//���ҵ��Ŀ��еĿ����pos������
			i --;	//�ҵ�һ���յĿ飬i��ֵ��һ
		}
	}

	if(i > 0)
	{
		return 0;	//��ʾ�ռ䲻��
	}
	pos[temp] = '\0';	//���ַ������

	return temp;	//���ط���Ŀռ������
}

/* 
 * �ļ�ϵͳ��ʽ��
 * ����fs_format(); ��ʽ����0x5000��0xEFFF��40K�ĵ�ַ�ռ�
 */
void fs_format(void)
{
	xdata unsigned int addr;	

	/* ����ļ�ϵͳ�� */
	for(addr = FS_BEGIN; addr < FS_END; addr += 512)
	{
		iap_erase(addr);
	}

	/* ������Ŀ¼ */
	create_dir_file("/", FS_BEGIN, FS_BEGIN, ROOT_DIR);
	fs_struct_ptr = FS_BEGIN;
	first_empty_page = FS_BEGIN + FS_PAGE;	//ʵ��ûʹ��
}

/* 
 * �ļ�д����������Ҫд�ĵ�ַ�Լ�Ҫд�ĳ���
 * ����fs_write(0x5000, 20, "1234567"); ��0x5000��ַд��20�ֽڵ����ݣ�����Ϊ"1234567"
 */
void fs_write(unsigned char *name)
{
	xdata unsigned int reve_buf_len = 0;
	xdata unsigned char select_cmd = 0;
	xdata unsigned int pos_mod[2] = 0;
	xdata unsigned char pos_number = 0, i = 0;
	xdata unsigned int file_ptr = 0;
	xdata unsigned int file_number = 0;
	
	/* ���Ҫɾ���ļ��ĵ�ַ */
	if(file_ptr = find_file(fs_struct_ptr, name, 1))
	{
		/* ����Ǹ��ļ��У��˳� */
		if(iap_read(file_ptr) & 0x02)
		{
			printf("It is a floder\r\n");
			return;
		}
	}
	else
	{
		printf("No such file\r\n");
		return;
	}
	
	/* ����ļ������Ѿ��������ݣ���ô�Ͳ��༭ */
	if(find_file(file_ptr, "*", 1))
	{
		printf("There are texts in this file, please delete this file and create a new\r\n");
		return;
	}	
	
	while(1)
	{
		while(1)
		{				
			write_buff[reve_buf_len] = uart_reve_byte();	//���յ�һ���ַ�����ȡ�����ջ���������
			if(write_buff[reve_buf_len] == 0) 			//���û�н��յ��ִ���������ѯ
			{
				task_delay(2);							//��ʱ�����������ڣ��Ա������������ִ��
				TF1 = 1;	//�������һ���ж�
			}
			else
				break;
		}

		if(reve_buf_len >= 500)	//����ÿһ����Ҫ���б�ǣ�����Ҫ��2���ֽڣ���Ҫ�����ַ��жϳ��ޣ��ɴ��ټ���2���ֽ�
		{			
			printf("\r\nToo much words\r\n");		//�û�������ַ���������
			printf("Please save it now\r\n");
			goto wait_write_quit;		
		}	
		else
		{
			if(write_buff[reve_buf_len] == 0x1B)		//���յ�ESC����
			{
				write_buff[reve_buf_len] = '\0';
wait_write_quit:
				write_buff[reve_buf_len + 1] = '\0';
				printf("\r\n");
				while(1)
				{
					select_cmd = uart_wait_byte();
					putchar(select_cmd);
					if(select_cmd == 'w')
					{
//						printf(write_buff);
						goto write_buff2eeprom;	//�����д��Ļ�������д��ĳ������
					}
					else if(select_cmd == 'q')
					{
						printf("you quit\r\n");
						return;
					}
					else
					{
						printf("\b \b");	//һֱ�����˸�д�����˸��γɷ���������֪���˸�
					}
				}
			}
			else if(write_buff[reve_buf_len] == '\b')	  	//���յ��˸��
			{
				if(reve_buf_len > 0)
				{
					reve_buf_len --;
					printf("\b \b");
				}
			}
			else if(write_buff[reve_buf_len] == '\r')	  	//���յ����з�
			{
				printf("\r\n");
				reve_buf_len ++;
				write_buff[reve_buf_len] = '\n';
				reve_buf_len ++;
			}
			else
			{
				uart_send_byte(write_buff[reve_buf_len]);	//���ոս��յ���һ���ֽڲ��ҷ��͸����������ʾ
				reve_buf_len ++;							//�ִ�����ֵ��1
			}			
		}
	}

write_buff2eeprom:
ENTER_CRITICAL;	//��Ҫ��ʱ��ִ�еĳ���Ҫ�Ƚ����ٽ��
	if(pos_number = alloc(reve_buf_len, pos_mod))
	{
		/* �������в���Ҫ���õ� */
		file_ptr = file_ptr + FS_FLAG_SIZE + FS_NAME_SIZE + FS_SIZE_SIZE + FS_PREV_SIZE + FS_NEXT_SIZE;
		
		/* ���ҿյ�node�ڵ� */
		for(i = 0; i < FS_NODE_SIZE; i += 2)
		{
			if(iap_read(file_ptr + i) == 0xFF)
			{
				for(file_number = 0; file_number < pos_number; file_number ++)
				{
					iap_write(file_ptr + i + file_number * 2, pos_mod[file_number] % 256);		//��д�͵�ַ
					iap_write(file_ptr + i + 1 + file_number * 2, pos_mod[file_number] / 256);	//��д�ߵ�ַ
					/* ��ӡ�ļ����ڴ浱�е�λ�� */
					putchar(((pos_mod[file_number]) >> 12) + 48);
					putchar((((pos_mod[file_number]) >> 8) & 0x0F) + 48);
					putchar((((pos_mod[file_number]) >> 4) & 0x0F) + 48);
					putchar((((pos_mod[file_number]) >> 0) & 0x0F) + 48);
				}
				break;	//д���˳�
			}
		}

		i = pos_number; file_number = 0;
		while(i)
		{
			iap_write((pos_mod[pos_number-i])++, 0x00);
			iap_write((pos_mod[pos_number-i])++, 0x00);
			while(write_buff[file_number])
			{
				if(file_number > 508) 	//��ʵ�ϲ����ܴ���508
				{
					break;
				}
				iap_write((pos_mod[pos_number-i])++, write_buff[file_number]);
				file_number ++;
			}
			iap_write((pos_mod[pos_number-i])++, 0x00);	//�ļ��Ľ�βд����ַ��������ļ��Ľ���
			i --; 	//д���������һ
		}
		printf("\r\n");
	}
	else
	{
		printf("Alloc space failed\r\n");
	}
EXIT_CRITICAL;	//д���˳��ٽ��
}

/* 
 * �ļ�������������Ҫ���ĵ�ַ�Լ�Ҫ���ĳ���
 * ����fs_read(0x5000, 20, str); ��0x5000��ַ��ʼ��20�ֽڳ������ݷ���str����
 */
void fs_read(unsigned char *name)
{
	xdata unsigned int ptr_mod;
	xdata unsigned int i = 0;
	xdata unsigned char str_mod = 0;
	xdata unsigned int next_ptr_mod = 0;

	ptr_mod = fs_struct_ptr; 
	while(1)
	{
		/* ���Ҳ������ı��ļ��ĵ�ַ */
		if(ptr_mod = find_file(ptr_mod, name, 1))
		{
			/* �ж��Ƿ����ı��ļ� */
			if(iap_read(ptr_mod) & 0x02)
			{
				printf("It not a text file\r\n");
				return;
			}
			else
			{	
				ptr_mod = ptr_mod + FS_FLAG_SIZE + FS_NAME_SIZE + FS_SIZE_SIZE + FS_PREV_SIZE + FS_NEXT_SIZE;
				/* ���Ҳ��Ҵ�ӡ�ı� */
				for(i = 0; i < FS_NODE_SIZE; i += 2)
				{
					if(iap_read(ptr_mod + i) == 0xFF)
					{
						return;	//����FF˵���ı��Ѿ��������
					}
					else
					{
						next_ptr_mod = iap_read(ptr_mod + i + 1);	//�����ֽڷ���ptr_mod
						next_ptr_mod = next_ptr_mod << 8;					//����8λ������ptr_mod���ֽ�
						next_ptr_mod |= iap_read(ptr_mod + i);		//�����ֽڷ���ptr_mod��8λ
						
						ENTER_CRITICAL;	//��Ҫ��ʱ��ִ�еĳ���Ҫ�Ƚ����ٽ��
						ptr_mod = ptr_mod + FS_FLAG_SIZE; //�����ļ����͵Ķ�ȡ���ı��ļ�����洢��һ�����ı�
						while(str_mod = iap_read(ptr_mod))
						{
							putchar(str_mod);	//��ӡ�������е��ı��ļ���ֱ�������ı�������Ϊֹ
							ptr_mod ++;
						}

						EXIT_CRITICAL;	//д���˳��ٽ��			
					}
				}//End for
			}
		}
		else
		{
			printf("No such file\r\n");
			return;
		}
	}
}

/* 
 * �½��ļ��л����ļ�
 * ����create_dir("/", 0x5000, 0x5000, NORMAL_DIR_TYPE);	
 * ��0x5000������һ���µ��ļ��У�����Ϊ"/"����һ���ṹ���ַΪ0x5000��Ҳ�����Լ�,���ļ������������ļ�ϵͳ�ĸ�Ŀ¼
 * 
 * 
 */
unsigned char create_dir_file(unsigned char *name, unsigned int addr, unsigned int prev_addr, unsigned char flag)
{
	xdata unsigned int i = 0;	
	
	/* ˵���Ѿ�����һ��ͬ���ļ� */
	if(find_file(prev_addr, name, 1))
	{
		return 1;
	}

	/* ������ȣ��ͱ������Ǹ��ļ� */
	if(addr != prev_addr)
	{
		/* �������в���Ҫ���õ� */
		prev_addr = prev_addr + FS_FLAG_SIZE + FS_NAME_SIZE + FS_SIZE_SIZE + FS_PREV_SIZE + FS_NEXT_SIZE;
		
		/* ���ҿյ�node�ڵ� */
		for(i = 0; i < FS_NODE_SIZE; i += 2)
		{
			if(iap_read(prev_addr + i) == 0xFF)
			{
				iap_write(prev_addr + i, addr % 256);		//��д�͵�ַ
				iap_write(prev_addr + i + 1, addr / 256);	//��д�ߵ�ַ
//				fs_struct_ptr = addr;
				i = 0;
				prev_addr = prev_addr - FS_FLAG_SIZE - FS_NAME_SIZE - FS_SIZE_SIZE - FS_PREV_SIZE - FS_NEXT_SIZE;
				goto create_new;
			}
		}

		return 1; 	//����1��ʾ����ʧ�ܣ�û���㹻�Ŀռ���
	}

create_new:	
	iap_write(addr, flag);
	iap_write(addr + 1, 0x00);
	addr += FS_FLAG_SIZE;	//����flag
	
	while(*name)
	{
		iap_write(addr+i, *name++);
		i ++;
	}
	iap_write(addr+i, '\0');	//�ڽ�β����ִ�������
	i = 0;

	addr += FS_NAME_SIZE;		//д���ļ��е�����
	addr += FS_SIZE_SIZE;		//�����ļ���Сѡ��

	/* д��һ���ṹ���ַ����д�͵�ַ��д�ߵ�ַ������������һ���ļ��� */
	iap_write(addr, prev_addr % 256);
	iap_write(addr + 1, prev_addr / 256);
	addr += FS_PREV_SIZE;		//д��˽ṹ�����һ���ṹ���ַ

/*	������ղ���Ҫ������˫���������ʽ���������Ķ���ע�͵��ˣ�һ���µ��ļ���û����node�ڵ��	
//	 �����½��Ľṹ��ʼ�ղ������̾�����һ���ṹ�� 
	iap_write(addr, 0x00);
	iap_write(addr + 1, 0x00);
	addr += FS_NEXT_SIZE;		//д��˽ṹ�����һ���ṹ���ַ
*/

	return 0;
}

/* 
 * �г���ѡ�е��ļ�������������ļ�
 * ����list_dir(0x5000); �г�0x5000������������е��ļ�
 * 
 */
void list_file(unsigned int cur_f_ptr, unsigned char l_flag)
{
	xdata unsigned int i = 0;
	xdata unsigned int ptr_mod = 0;
	xdata unsigned char name_mod = 0;	
	
	/* �������в���Ҫ���ҵ� */
	cur_f_ptr = cur_f_ptr + FS_FLAG_SIZE + FS_NAME_SIZE + FS_SIZE_SIZE + FS_PREV_SIZE + FS_NEXT_SIZE;
	
	/* ���ҷǿյ�node�ڵ� */
	for(i = 0; i < FS_NODE_SIZE; i += 2)
	{
		if(iap_read(cur_f_ptr + i) == 0xFF)
		{
//			return;
		}
		else
		{
			ptr_mod = iap_read(cur_f_ptr + i + 1);	//�����ֽڷ���ptr_mod
			ptr_mod = ptr_mod << 8;					//����8λ������ptr_mod���ֽ�
			ptr_mod |= iap_read(cur_f_ptr + i);		//�����ֽڷ���ptr_mod��8λ

			if(l_flag)
			{
				if(iap_read(ptr_mod) & 0x02)
				{
					printf("�ļ���\t");	//�������ļ���
				}
				else
				{
					printf("�ı�\t");	//�������ı��ļ�
				}
				if(iap_read(ptr_mod) & 0x20)
				{
					printf("�ɶ�\t");	//�����ǿɶ���
				}
				else
				{
					printf("���ɶ�\t");	//�����ǲ��ɶ���
				}
				if(iap_read(ptr_mod) & 0x10)
				{
					printf("��д\t");	//�����ǿ�д��
				}
				else
				{
					printf("����д\t");	//�����ǲ���д��
				}
				if(iap_read(ptr_mod) & 0x08)
				{
					printf("��ɾ\t");	//�����ǿ�ɾ����
				}
				else
				{
					printf("����ɾ\t");	//�����ǲ���ɾ��
				}
				putchar((ptr_mod >> 12) + 48);
				putchar(((ptr_mod >> 8) & 0x0F) + 48);
				putchar(((ptr_mod >> 4) & 0x0F) + 48);
				putchar(((ptr_mod >> 0) & 0x0F) + 48);

				printf("\t\t");
			}

			ptr_mod = ptr_mod + FS_FLAG_SIZE;		//�����ļ����͵Ķ�ȡ			
			while(name_mod = iap_read(ptr_mod++))	//�ִ��ǿվ�һֱ��ȡ���ִ�ŵ����黺������
			{
				putchar(name_mod);			//��ӡ�������ַ���
			}					

			name_mod = 0;	//���㣬�ȴ���һ�β���
			if(l_flag)
			{
				printf("\r\n");	//ֱ�ӻ���
			}
			else
			{
				printf("\t\t");	//������һ�����봦
			}
		}
	}
}

/* 
 * �л��ļ��е�Ŀ¼
 * ����change_dir("test"); �л�����ǰĿ¼�µ�testĿ¼��ȥ
 * 
 */
unsigned char change_dir(unsigned char *name)
{
	xdata unsigned int ptr_mod = 0;
	xdata unsigned int prev_ptr_mod = 0;
	
	ptr_mod = fs_struct_ptr;
	
	if(!os_strcmp("..", name))	//������һ���ļ���
	{
		goto exit2last_dir;
	}
	else if(!os_strcmp("../", name))  //������һ���ļ���
	{
		goto exit2last_dir;
	}
	else if(!os_strcmp("/", name))	 //���ظ�Ŀ¼
	{
		fs_struct_ptr = FS_BEGIN;
		return 0;
	}
	else
	{
		/* ����ҵ��Ļ��ͽ����ļ��е��л� */
		if(ptr_mod = find_file(ptr_mod, name, 1))
		{
			if(iap_read(ptr_mod) & 0x02)
			{
				fs_struct_ptr = ptr_mod;
				return 0;
			}
			else
			{
				printf("Not a dir, but a text file\r\n");
				return 1;
			}
			
		}		
		printf("No such dir\r\n");
		return 1;
	}

exit2last_dir:
	ptr_mod = ptr_mod + FS_FLAG_SIZE + FS_NAME_SIZE + FS_SIZE_SIZE;
	prev_ptr_mod = iap_read(ptr_mod + 1);
	prev_ptr_mod = prev_ptr_mod << 8;
	prev_ptr_mod |= iap_read(ptr_mod);
	fs_struct_ptr = prev_ptr_mod;

return 0;
}

/* 
 * �ļ���ɾ��
 * ����delete_file("123"); //ɾ����ǰ�ļ���������Ϊ123���ļ�
 * 
 */
unsigned char delete_file(unsigned char *name)
{
	xdata unsigned int ptr_mod = 0;
	xdata unsigned int next_file_ptr = 0;
	xdata unsigned int file_ptr = 0;
	xdata unsigned int i = 0;
	xdata unsigned int count = 0;
	
	ptr_mod = fs_struct_ptr;
	
	if(ptr_mod = find_file(ptr_mod, name, 1))
	{
		
		if(!(iap_read(ptr_mod) & 0x08))
		{
			printf("Permition denied\r\n");
			return 1;
		}
		else
		{
			/* �ж��Ƿ����ļ��� */
			if(iap_read(ptr_mod) & 0x02)
			{
				if(find_file(ptr_mod, "*", 1)) 	//������ļ��еĻ��������Ƿ����ļ�
				{
					printf("There are files in this folder, please delete them first\r\n");
					return 1;	//���ļ�������ɾ������
				}
				else
				{
					file_ptr = ptr_mod;
					ptr_mod = find_file(fs_struct_ptr, name, 0);	//�ҵ�����ṹ������Ӧ��node�ڵ��λ��
					/* ��ԭ�����ļ���־�������� */
					for(i = 0; i < FS_NEED_KEEP; i ++)
					{
						if(i == (ptr_mod + 28))
						{
							/* ���ո�ɾ�����ļ�node�ڵ���� */
							erase_return_buff[i] = 0xFF;
							i ++;
							erase_return_buff[i] = 0xFF;
						}
						else
						{
							erase_return_buff[i] = iap_read(fs_struct_ptr + i);
						}
					}					
					iap_erase(fs_struct_ptr);  //������ǰ�ļ������������
					for(i = 0; i < FS_NEED_KEEP; i ++)
					{
						iap_write(fs_struct_ptr + i, erase_return_buff[i]);
					}

					printf("\r\n");

					iap_erase(file_ptr);	//�����ļ�������

					return 0;
				}
			}
			else	//���ļ�����һ���ı��ļ�
			{
				ptr_mod = ptr_mod + FS_FLAG_SIZE + FS_NAME_SIZE + FS_SIZE_SIZE + FS_PREV_SIZE + FS_NEXT_SIZE;
				/* ���Ҳ���ɾ���ļ��� */
				for(i = 0; i < FS_NODE_SIZE; i += 2)
				{
					if(iap_read(ptr_mod + i) == 0xFF)
					{
						break;	//����FF˵���ı��Ѿ��������
					}
					else
					{
						next_file_ptr = iap_read(ptr_mod + i + 1);	//�����ֽڷ���ptr_mod
						next_file_ptr = next_file_ptr << 8;					//����8λ������ptr_mod���ֽ�
						next_file_ptr |= iap_read(ptr_mod + i);		//�����ֽڷ���ptr_mod��8λ
						
						ENTER_CRITICAL;	//��Ҫ��ʱ��ִ�еĳ���Ҫ�Ƚ����ٽ��
//						putchar((next_file_ptr >> 12) + 48);
//						putchar(((next_file_ptr >> 8) & 0x0F) + 48);
//						putchar(((next_file_ptr >> 4) & 0x0F) + 48);
//						putchar(((next_file_ptr >> 0) & 0x0F) + 48);
						iap_erase(next_file_ptr);	//�����ļ���
						EXIT_CRITICAL;	//д���˳��ٽ��			
					}
				}//End for

				ENTER_CRITICAL;	//��Ҫ��ʱ��ִ�еĳ���Ҫ�Ƚ����ٽ��
				file_ptr = find_file(fs_struct_ptr, name, 1);	//�ҵ�����ṹ������Ӧ���ļ���λ��

				ptr_mod = find_file(fs_struct_ptr, name, 0);	//�ҵ�����ṹ������Ӧ��node�ڵ��λ��
//				putchar((ptr_mod >> 12) + 48);
//				putchar(((ptr_mod >> 8) & 0x0F) + 48);
//				putchar(((ptr_mod >> 4) & 0x0F) + 48);
//				putchar(((ptr_mod >> 0) & 0x0F) + 48);
				/* ��ԭ�����ļ���־�������� */
				for(i = 0; i < FS_NEED_KEEP; i ++)
				{
					if(i == (ptr_mod + 28))
					{
						/* ���ո�ɾ�����ļ�node�ڵ���� */
						erase_return_buff[i] = 0xFF;
						i ++;
						erase_return_buff[i] = 0xFF;
					}
					else
					{
						erase_return_buff[i] = iap_read(fs_struct_ptr + i);
					}
				}					
				iap_erase(fs_struct_ptr);  //������ǰ�ļ������������
				for(i = 0; i < FS_NEED_KEEP; i ++)
				{
					iap_write(fs_struct_ptr + i, erase_return_buff[i]);
				}

//				putchar((file_ptr >> 12) + 48);
//				putchar(((file_ptr >> 8) & 0x0F) + 48);
//				putchar(((file_ptr >> 4) & 0x0F) + 48);
//				putchar(((file_ptr >> 0) & 0x0F) + 48);
				iap_erase(file_ptr);	//ɾ�����ļ��ڵ�ǰ�ļ����µļ�¼
				EXIT_CRITICAL;	//д���˳��ٽ��
			}
			return 0;			
		}		
	}

	printf("No such file or dir\r\n");
	return 1;
}
#endif