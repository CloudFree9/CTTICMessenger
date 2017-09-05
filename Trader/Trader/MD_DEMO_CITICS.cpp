#include "CTSMdApiStruct.h"
#include "CTSMD.h"
#include <iostream>
using namespace std;

// #pragma comment(lib, "CTSMD.lib")

void __stdcall callBack_Stkprice(pCITICS_MD_STKPRICE ctsStkPrice)
{
  if (ctsStkPrice != NULL) 
  {
	  cout<<ctsStkPrice->stock_code<<"最新价："<<ctsStkPrice->last_price<<endl;
  }
}

void __stdcall callBack_OptCode(pCITICS_MD_OPTPRICE ctsOptCode)
{
  if (ctsOptCode != NULL) 
  {
	  cout<<ctsOptCode->option_code<<"最新价："<<ctsOptCode->last_price<<endl;
  }
}

int main1(int argc, char* argv[])
{
	ctsMdHandle ctsHandle ;
	int ret = frontConnect(&ctsHandle,"t2sdk.ini");
	if (0 == ret)
	{
		int iRet = SubscribeMd_Opt(ctsHandle,callBack_OptCode);
		if ( iRet >= 0)
		{
			printf("期权行情订阅成功\n");
		}
		else if (iRet == -1)
		{
			printf("%s\n",GetMCLastError(ctsHandle));
		}
		else
		{
			printf("%s\n",GetErrorMsg(ctsHandle,iRet));
		}
		iRet = SubscribeMd_Stk(ctsHandle,callBack_Stkprice);
		if (iRet >= 0)
		{
			printf("股票行情订阅成功\n");
		}
		else if (iRet == -1)
		{
			printf("%s\n",GetMCLastError(ctsHandle));
		}
		else
		{
			printf("%s\n",GetErrorMsg(ctsHandle,iRet));
		}

	}
	else
	{
		printf("%s\n",GetErrorMsg(ctsHandle,ret));
	}
	getchar();
	return 0;
}