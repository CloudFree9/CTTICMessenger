#ifndef __TRADER_H__
#define __TRADER_H__

#include "Logger.h"
#include "Constant.h"
#include <Windows.h>
#include "tibemsUtilities.h"
#include "t2sdk_interface.h"
#include <queue>
#include "CTSMdApiStruct.h"
#include "CTSMD.h"

class Controller;

class TibStuff {
public:
	TibStuff(Controller *controller);

protected:
	static Controller *m_controller;
	static tibemsConnectionFactory         m_factory;
	static tibemsConnection                m_connection;
	static tibemsSession                   m_session;
	static tibemsMsgConsumer               m_msgConsumer;
	static tibemsMsgProducer               m_msgProducer;
	static tibemsDestination               m_destination;
	static tibemsSSLParams                 m_sslParams;
	static tibems_int                      m_receive;
	static tibemsErrorContext              m_errorContext;
	static char**                          m_data;
	static tibems_int                      m_datac;    
	static char                            m_serverUrl[1024];
	static char                            m_userName[64];
	static char                            m_password[64];
	static char                            m_pk_password[64];
	static char                            m_producer_name[64];
	static char                            m_consumer_name[64];
	static tibems_bool                     m_useTopic;
	static tibemsAcknowledgeMode           m_ackMode;

public:
	static void fillParams();
	static void fail(const char*message, tibemsErrorContext errorContext);
	static void onException(tibemsConnection conn, tibems_status reason, void* closure);
	static void setController(Controller *controller);
	static Controller& getController();
};

class TibProducer: public TibStuff {
public:
	TibProducer(Controller *controller);
	~TibProducer();
	void Init();
	unsigned __stdcall Run(TibProducer &prod);
	static unsigned __stdcall ProducerEntryPoint(void * pInstance);

};

class TibConsumer: public TibStuff {

public:
	void Init();
	TibConsumer(Controller *controller);
	unsigned __stdcall Run(TibConsumer &consumer);
	static unsigned __stdcall ConsumerEntryPoint(void * pInstance);

};

typedef struct _BasciInfo {
	char password[32];
	char password_type[2];
	char input_content[2];
	char client_id[32];
	char account_content[32];
	char op_account_content[32];
	char user_token[64];
	char branch_no[8];
	char asset_prop[4];
	char sysnode_id[4];
	char entrust_no[16];
	char entrust_status[8];
	char op_entrust_way[4];
	char op_station[32];
	char identity_type[2];
	char request_num[50];
} BasicInfo;

typedef struct _Order {
	char exchange_type[2];
	char option_code[20];
	char entrust_amount[50];
	char entrust_price[50];
	char entrust_bs[2]; //1买 2卖
	char entrust_prop[2]; //"0" 普通买卖
	char batch_no[2]; // "0" 表示单笔订单
	char entrust_oc[2];
} Order;

class CTTICClient {

private:
	Controller *m_controller;
	CConnectionInterface *m_connection;
	BasicInfo m_info;
	char m_lorder_status[10];
	char m_lorder_no[20];
	int m_systemNo;

	int FunctionExecution(IBizMessage* lpBizMessage, void **lppBuffer, int *len);
	void SetupOptSubscription();

public:
	CTTICClient(Controller *controller);
	~CTTICClient();
	void Init();
	unsigned __stdcall Run(CTTICClient &client);
	static unsigned __stdcall ClientEntryPoint(void *pInstance);
	static void __stdcall callBack_OptCode(pCITICS_MD_OPTPRICE ctsOptCode);
	int Login();
	int PlaceOrder(Order &order);
	int CancelOrder(char *trust_no);
	int GetOrderStatus(char *trust_no="2");
	int GetDealStatus();
	int QueryUserInfo();
	int QueryUserFund();
	int QueryOptCode();
	BasicInfo GetBasicInfo();
	void SetNecessaryOPParam(IF2Packer* pack);

};

typedef std::queue<void *> MsgQueue;
typedef size_t TMsgHeader;

class Controller {
private:
	Logger m_logger;
	TibProducer *m_tibProducer;
	TibConsumer *m_tibConsumer;
	CTTICClient *m_CTTICClient;
	CConfigInterface *m_lpConfig;
	MsgQueue m_tibMsgBuf;   //  Used for intermediate message from Tibco message queue. CTTIC client will consume it.
	MsgQueue m_CTTICMsgBuf; //  Used for intermediate message from CTTIC message queue. Tibco producer will consume it.

	HANDLE m_semaphore[NUM_THREAD_SEMAPHORE];
	HANDLE m_threadHandlers[NUM_THREAD_SEMAPHORE];
	unsigned m_threadIDs[NUM_THREAD_SEMAPHORE];
public:
	static HANDLE m_callback_mutext;

public:	
	Controller();
	~Controller();
	void Init();
	void StartMonit();
	void Restart();
	void TearDown();

	TibProducer& GetProducer();
	TibConsumer& GetConsumer();
	CTTICClient& GetClient();
	CConfigInterface& GetConfig();
	Logger& GetLogger();
	HANDLE GetThreadSemaphore(int index);
	HANDLE GetCallbackMutex();

//	MsgQueue& getTibMsgQueue();
//	MsgQueue& getCTTICMsgQueue();
	void SinkMsg(char queueID, void *msg, size_t len);
	void* GetNextMsg(char queueID);
};

#endif
