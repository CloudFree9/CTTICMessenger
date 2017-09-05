

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if !defined(CTS_MD_API_STRUCT_H)
#define CTS_MD_API_STRUCT_H

typedef void *ctsMdHandle;

typedef struct ctsMDStkPrice
{
	//�������1-�Ϻ���2-����
	char exchange_type[4];
	//֤ȯ����
	char stock_code[7];
	//���¼�
	float last_price;
	//���̼�
	float close_price;
}CITICS_MD_STKPRICE, *pCITICS_MD_STKPRICE;

typedef struct ctsMDOptPrice
{
	//�������1-�Ϻ���2-����
	char exchange_type[4];
	//��Ȩ��Լ����
	char option_code[9];
	//��Ȩ��Լ��ƣ�δ���ã�
	char option_name[33];
	//���֤ȯ�����̼�
	float close_price;
	//���̼�
	float open_price;
	//�ɽ����
	float business_balance;
	//��߼�
	float high_price;
	//��ͼ�
	float low_price;
	//���¼�
	float last_price;
	//�ɽ�����
	int business_amount;
	//��һ��
	float opt_buy_price1;
	//�����
	float opt_buy_price2;
	//������
	float opt_buy_price3;
	//���ļ�
	float opt_buy_price4;
	//�����
	float opt_buy_price5;
	//��һ��
	float opt_sale_price1;
	//������
	float opt_sale_price2;
	//������
	float opt_sale_price3;
	//���ļ�
	float opt_sale_price4;
	//�����
	float opt_sale_price5;
	//��һ��
	int buy_amount1;
	//�����
	int buy_amount2;
	//������
	int buy_amount3;
	//������
	int buy_amount4;
	//������
	int buy_amount5;
	//��һ��
	int sale_amount1;
	//������
	int sale_amount2;
	//������
	int sale_amount3;
	//������
	int sale_amount4;
	//������
	int sale_amount5;
	//����ָ����δ���ã�
	char exchange_index[32];
	//����������δ���ã�
	char close_exchange_index;
	//���ս����
	float pre_square_price;
	//����ۣ�δ���ã�
	float square_price;
	//��̬�ο��۸�
	float auction_price;
	//����ƥ������
	float auction_amount;
	//δƽ������
	float undrop_amount;
	//��Ȩ����״̬��S-��������ʼǰ����C-���Ͼ��ۣ�T-�������ף�B-���У�E-���У�U-���̼��Ͼ��ۣ�V-�������жϣ�P-��ʱͣ��
	char optexch_status;
	//��Ȩʵʱ�������ƣ�0-�������뿪�֣�1-�����������֣�2-���Ʊ��ҿ���
	char opt_open_restriction[64];
}CITICS_MD_OPTPRICE, *pCITICS_MD_OPTPRICE;

#endif;