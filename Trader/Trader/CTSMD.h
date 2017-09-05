
#include"CTSMdApiStruct.h"

#if defined(ISLIB) && defined(WIN32)
#ifdef LIB_QUERY_API_EXPORT
#define QUERY_API_EXPORT __declspec(dllexport)
#else
#define QUERY_API_EXPORT __declspec(dllimport)
#endif
#else
#define QUERY_API_EXPORT __declspec(dllimport)
#endif



typedef void (__stdcall *TCallBack_StkPrice)(pCITICS_MD_STKPRICE pCtsStkPrice);
typedef void (__stdcall *TCallBack_OptCode)(pCITICS_MD_OPTPRICE pCtsOptPrice);

extern "C" QUERY_API_EXPORT int __stdcall SubscribeMd_Opt(ctsMdHandle ctsHandle,TCallBack_OptCode callBackOpt);
extern "C" QUERY_API_EXPORT int __stdcall SubscribeMd_Stk(ctsMdHandle ctsHandle,TCallBack_StkPrice callBackStk);
extern "C" QUERY_API_EXPORT int __stdcall frontConnect(ctsMdHandle* ctsHandle,char* cfgFileName);
extern "C" QUERY_API_EXPORT char* __stdcall GetErrorMsg(ctsMdHandle ctsHandle,int iRet);
extern "C" QUERY_API_EXPORT char* __stdcall GetMCLastError(ctsMdHandle ctsHandle);

