

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if !defined(CTS_MD_API_STRUCT_H)
#define CTS_MD_API_STRUCT_H

typedef void *ctsMdHandle;

typedef struct ctsMDStkPrice
{
	//交易类别：1-上海；2-深圳
	char exchange_type[4];
	//证券代码
	char stock_code[7];
	//最新价
	float last_price;
	//收盘价
	float close_price;
}CITICS_MD_STKPRICE, *pCITICS_MD_STKPRICE;

typedef struct ctsMDOptPrice
{
	//交易类别：1-上海；2-深圳
	char exchange_type[4];
	//期权合约编码
	char option_code[9];
	//期权合约简称（未启用）
	char option_name[33];
	//标的证券昨收盘价
	float close_price;
	//开盘价
	float open_price;
	//成交金额
	float business_balance;
	//最高价
	float high_price;
	//最低价
	float low_price;
	//最新价
	float last_price;
	//成交数量
	int business_amount;
	//买一价
	float opt_buy_price1;
	//买二价
	float opt_buy_price2;
	//买三价
	float opt_buy_price3;
	//买四价
	float opt_buy_price4;
	//买五价
	float opt_buy_price5;
	//卖一价
	float opt_sale_price1;
	//卖二价
	float opt_sale_price2;
	//卖三价
	float opt_sale_price3;
	//卖四价
	float opt_sale_price4;
	//卖五价
	float opt_sale_price5;
	//买一量
	int buy_amount1;
	//买二量
	int buy_amount2;
	//买三量
	int buy_amount3;
	//买四量
	int buy_amount4;
	//买五量
	int buy_amount5;
	//卖一量
	int sale_amount1;
	//卖二量
	int sale_amount2;
	//卖三量
	int sale_amount3;
	//卖四量
	int sale_amount4;
	//卖五量
	int sale_amount5;
	//交易指数（未启用）
	char exchange_index[32];
	//闭市索引（未启用）
	char close_exchange_index;
	//昨日结算价
	float pre_square_price;
	//结算价（未启用）
	float square_price;
	//动态参考价格
	float auction_price;
	//虚拟匹配数量
	float auction_amount;
	//未平仓数量
	float undrop_amount;
	//期权交易状态：S-启动（开始前）；C-集合竞价；T-连续交易；B-休市；E-闭市；U-收盘集合竞价；V-波动性中断；P-临时停牌
	char optexch_status;
	//期权实时开仓限制：0-限制买入开仓；1-限制卖出开仓；2-限制备兑开仓
	char opt_open_restriction[64];
}CITICS_MD_OPTPRICE, *pCITICS_MD_OPTPRICE;

#endif;