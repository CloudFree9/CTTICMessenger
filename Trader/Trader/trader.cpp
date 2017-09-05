#include "trader.h"
#include "Logger.h"
#include <process.h>
#include <Windows.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <TlHelp32.h>
#include <list>

using namespace std;

#define strcpy strcpy_s
#define strncpy strncpy_s

tibemsConnectionFactory         TibStuff::m_factory			= NULL;
tibemsConnection                TibStuff::m_connection		= NULL;
tibemsSession                   TibStuff::m_session			= NULL;
tibemsMsgConsumer               TibStuff::m_msgConsumer		= NULL;
tibemsMsgProducer               TibStuff::m_msgProducer		= NULL;
tibemsDestination               TibStuff::m_destination		= NULL;
tibemsSSLParams                 TibStuff::m_sslParams		= NULL;
tibems_int                      TibStuff::m_receive			= 1;
tibemsErrorContext              TibStuff::m_errorContext	= NULL;

tibems_bool						TibStuff::m_useTopic		= TIBEMS_FALSE;
tibemsAcknowledgeMode			TibStuff::m_ackMode			= TIBEMS_AUTO_ACKNOWLEDGE;
Controller*						TibStuff::m_controller		= NULL;
// Controller*						CTTICClient::m_controller	= NULL;

char                            TibStuff::m_serverUrl[1024];
char                            TibStuff::m_userName[64];
char                            TibStuff::m_password[64];
char                            TibStuff::m_pk_password[64];
char                            TibStuff::m_producer_name[64];
char                            TibStuff::m_consumer_name[64];
char**                          TibStuff::m_data				= NULL;
tibems_int                      TibStuff::m_datac				= 0;   

HANDLE Controller::m_callback_mutext;
static void ShowPacket(IF2UnPacker *lpUnPacker);
static BOOL FindProcess(WCHAR *filename);
static void AssemblePacket(IF2UnPacker *lpUnPacker, char **ret);

TibStuff::TibStuff(Controller *controller) {
	setController(controller);
}

void TibStuff::fail(const char*message, tibemsErrorContext errorContext) {
    tibems_status               status = TIBEMS_OK;
    const char*                 str    = NULL;

    m_controller->GetLogger() << "ERROR: " << (char*)message << "\n";

    status = tibemsErrorContext_GetLastErrorString(errorContext, &str);
    m_controller->GetLogger() << "\nLast error message =\n" << (char *)str << "\n";
    status = tibemsErrorContext_GetLastErrorStackTrace(errorContext, &str);
    m_controller->GetLogger() << "\nStack trace = \n" << (char *)str << "\n";

    exit(TIBCO_FAIL);
}

void TibStuff::onException(tibemsConnection conn, tibems_status reason, void* closure)
{
    if (reason == TIBEMS_SERVER_NOT_CONNECTED) {
		m_controller->GetLogger() << "CONNECTION EXCEPTION: Server Disconnected\n";
        m_receive = 0;
    }
}

void TibStuff::setController(Controller *controller) {
	m_controller = controller;
}

Controller& TibStuff::getController() {
	return *m_controller;
}

TibProducer::TibProducer(Controller *controller): TibStuff(controller) {

}

unsigned __stdcall TibProducer::Run(TibProducer &prod) {
	m_controller->GetLogger() << "TibProducer thread started.\n";
    tibemsTextMsg               tMsg          = NULL;
	tibems_status               status       = TIBEMS_OK;
            
    /* publish messages */
	while (TRUE) {
		void *rmsg;
		WaitForSingleObject(m_controller->GetThreadSemaphore(TIB_PRODUCER_THREAD_NO), INFINITE);

		while (rmsg = m_controller->GetNextMsg('C')) {
			size_t len;
			memcpy(&len, rmsg, sizeof(len));
			char *msg = new char[len + 1];
			memcpy(msg, (void*)((long)rmsg + sizeof(len)), len);
			msg[len] = 0;
			free(rmsg);

			/* create the text message */
			status = tibemsTextMsg_Create(&tMsg);
			if (status != TIBEMS_OK) {
				fail("Error creating tibemsTextMsg", m_errorContext);
			}
        
			/* set the message text */
			status = tibemsTextMsg_SetText(tMsg, msg);
			if (status != TIBEMS_OK) {
				fail("Error setting tibemsTextMsg text", m_errorContext);
			}

			/* publish the message */
			status = tibemsMsgProducer_Send(m_msgProducer, tMsg);
			if (status != TIBEMS_OK) {
				fail("Error publishing tibemsTextMsg", m_errorContext);
			}
       
	       /* destroy the message */
			status = tibemsMsg_Destroy(tMsg);
			if (status != TIBEMS_OK) {
				fail("Error destroying tibemsTextMsg", m_errorContext);
			}
		}
	}
	return OK;
}

unsigned __stdcall TibProducer::ProducerEntryPoint(void * pInstance) {
	TibProducer &prod = *(TibProducer*)pInstance;
	prod.Run(prod);
	return OK;
}

void TibProducer::Init() {
	fillParams();
    tibems_status               status       = TIBEMS_OK;

    if (!m_producer_name) {
        m_controller->GetLogger() << "***Error: must specify destination name\n";
        exit(TIB_INVALID_DESTINATION);
    }

    m_controller->GetLogger() <<  "Publishing to destination '" << m_producer_name << "'\n";
    

    status = tibemsErrorContext_Create(&m_errorContext);

    if (status != TIBEMS_OK) {
        m_controller->GetLogger() <<  "ErrorContext create failed: " << (char *)tibemsStatus_GetText(status) << "\n";
        exit(TIB_CREATE_ERRORCONTEXT_FAIL);
    }

    m_factory = tibemsConnectionFactory_Create();
    if (!m_factory) {
        fail("Error creating tibemsConnectionFactory", m_errorContext);
    }
	
    status = tibemsConnectionFactory_SetServerURL(m_factory,m_serverUrl);
    if (status != TIBEMS_OK) {
        fail("Error setting server url", m_errorContext);
    }

    /* create the connection, use ssl if specified */
    if(m_sslParams) {
        status = tibemsConnectionFactory_SetSSLParams(m_factory, m_sslParams);
        if (status != TIBEMS_OK) {
            fail("Error setting ssl params", m_errorContext);
        }
        status = tibemsConnectionFactory_SetPkPassword(m_factory, m_pk_password);
        if (status != TIBEMS_OK) {
            fail("Error setting pk password", m_errorContext);
        }
    }

    status = tibemsConnectionFactory_CreateConnection(m_factory, &m_connection, m_userName, m_password);
    if (status != TIBEMS_OK) {
        fail("Error creating tibemsConnection", m_errorContext);
    }

    /* create the destination */
    if (m_useTopic)
        status = tibemsTopic_Create(&m_destination, m_producer_name);
    else
        status = tibemsQueue_Create(&m_destination, m_producer_name);
    if (status != TIBEMS_OK) {
        fail("Error creating tibemsDestination", m_errorContext);
    }

    /* create the session */
    status = tibemsConnection_CreateSession(m_connection, &m_session, TIBEMS_FALSE, TIBEMS_AUTO_ACKNOWLEDGE);
    if (status != TIBEMS_OK) {
        fail("Error creating tibemsSession", m_errorContext);
    }

    /* create the producer */
    status = tibemsSession_CreateProducer(m_session, &m_msgProducer, m_destination);
    if (status != TIBEMS_OK) {
        fail("Error creating tibemsMsgProducer", m_errorContext);
    }
}

TibProducer::~TibProducer() {

	tibemsDestination_Destroy(m_destination);
    tibemsConnection_Close(m_connection);
    if (m_sslParams)
    {
        tibemsSSLParams_Destroy(m_sslParams);
    }
    tibemsErrorContext_Close(m_errorContext);
}

TibConsumer::TibConsumer(Controller *controller): TibStuff(controller) {
}

void TibStuff::fillParams() {

	char * entry = (char *)m_controller->GetConfig().GetString("tibEMS", "serverUrl", DEFAULT_TIB_SERVER);
	strcpy(m_serverUrl, entry);

	entry = (char *)m_controller->GetConfig().GetString("tibEMS", "userName", DEFAULT_TIB_USER);
	strcpy(m_userName, entry);

	entry = (char *)m_controller->GetConfig().GetString("tibEMS", "password", DEFAULT_TIB_PWD);
	strcpy(m_password, entry);

	entry = (char *)m_controller->GetConfig().GetString("tibEMS", "pk_password", DEFAULT_TIB_PK_PWD);
	strcpy(m_pk_password, entry);

	entry = (char *)m_controller->GetConfig().GetString("tibEMS", "consumer_queue", DEFAULT_TIB_QUEUE_CONSUMER);
	strcpy(m_consumer_name, entry);

	entry = (char *)m_controller->GetConfig().GetString("tibEMS", "producer_queue", DEFAULT_TIB_QUEUE_PRODUCER);
	strcpy(m_producer_name, entry);

}

void TibConsumer::Init() {
	
	tibems_status               status      = TIBEMS_OK;

	fillParams();
    if (!m_consumer_name) {
        m_controller->GetLogger() << "***Error: must specify destination name\n";
        exit(TIB_INVALID_DESTINATION);
    }
    
    m_controller->GetLogger() << "Subscribing to destination: '" << m_consumer_name << "'\n\n";
    
    status = tibemsErrorContext_Create(&m_errorContext);

    if (status != TIBEMS_OK)
    {
        m_controller->GetLogger() << "ErrorContext create failed: " << (char*)tibemsStatus_GetText(status) << "\n";
        exit(TIB_CREATE_ERRORCONTEXT_FAIL);
    }

    m_factory = tibemsConnectionFactory_Create();
    if (!m_factory)
    {
        fail("Error creating tibemsConnectionFactory", m_errorContext);
    }

    status = tibemsConnectionFactory_SetServerURL(m_factory, m_serverUrl);
    if (status != TIBEMS_OK) 
    {
        fail("Error setting server url", m_errorContext);
    }

    /* create the connection, use ssl if specified */
    if(m_sslParams)
    {
        status = tibemsConnectionFactory_SetSSLParams(m_factory, m_sslParams);
        if (status != TIBEMS_OK) 
        {
            fail("Error setting ssl params", m_errorContext);
        }
        status = tibemsConnectionFactory_SetPkPassword(m_factory,m_pk_password);
        if (status != TIBEMS_OK) 
        {
            fail("Error setting pk password", m_errorContext);
        }
    }

    status = tibemsConnectionFactory_CreateConnection(m_factory, &m_connection, m_userName, m_password);
    if (status != TIBEMS_OK)
    {
        fail("Error creating tibemsConnection", m_errorContext);
    }

    /* set the exception listener */
    status = tibemsConnection_SetExceptionListener(m_connection, onException, NULL);
    if (status != TIBEMS_OK)
    {
        fail("Error setting exception listener", m_errorContext);
    }

    /* create the destination */
    if (m_useTopic)
        status = tibemsTopic_Create(&m_destination, m_consumer_name);
    else
        status = tibemsQueue_Create(&m_destination, m_consumer_name);
    if (status != TIBEMS_OK)
    {
        fail("Error creating tibemsDestination", m_errorContext);
    }
        
    /* create the session */
    status = tibemsConnection_CreateSession(m_connection, &m_session, TIBEMS_FALSE, m_ackMode);
    if (status != TIBEMS_OK)
    {
        fail("Error creating tibemsSession", m_errorContext);
    }
        
    /* create the consumer */
    status = tibemsSession_CreateConsumer(m_session, &m_msgConsumer, m_destination, NULL, TIBEMS_FALSE);
    if (status != TIBEMS_OK)
    {
        fail("Error creating tibemsMsgConsumer", m_errorContext);
    }
    
    /* start the connection */
    status = tibemsConnection_Start(m_connection);
    if (status != TIBEMS_OK)
    {
        fail("Error starting tibemsConnection", m_errorContext);
    }
}

unsigned __stdcall TibConsumer::Run(TibConsumer &consumer) {
	m_controller->GetLogger() << "TibConsumer thread started.\n";

    tibems_status               status      = TIBEMS_OK;
    tibemsMsg                   msg         = NULL;
    const char*                 txt         = NULL;
    tibemsMsgType               msgType     = TIBEMS_MESSAGE_UNKNOWN;
    char*                       msgTypeName = "UNKNOWN";
    
    /* read messages */
    while(m_receive) {
        /* receive the message */
        status = tibemsMsgConsumer_Receive(m_msgConsumer, &msg);
        if (status != TIBEMS_OK)
        {
            if (status == TIBEMS_INTR)
            {
                /* this means receive has been interrupted. This
                 * could happen if the connection has been terminated
                 * or the program closed the connection or the session.
                 * Since this program does not close anything, this 
                 * means the connection to the server is lost, it will
                 * be printed in the connection exception. So ignore it
                 * here.
                 */
                return TIB_INTERRUPTED;
            }
            fail("Error receiving message", m_errorContext);
        }
        if (!msg) {
            break;
		}

        /* acknowledge the message if necessary */
        if (m_ackMode == TIBEMS_CLIENT_ACKNOWLEDGE ||
            m_ackMode == TIBEMS_EXPLICIT_CLIENT_ACKNOWLEDGE ||
            m_ackMode == TIBEMS_EXPLICIT_CLIENT_DUPS_OK_ACKNOWLEDGE) {
            status = tibemsMsg_Acknowledge(msg);
            if (status != TIBEMS_OK) {
                fail("Error acknowledging message", m_errorContext);
            }
        }

        /* check message type */
        status = tibemsMsg_GetBodyType(msg, &msgType);
        if (status != TIBEMS_OK) {
            fail("Error getting message type", m_errorContext);
        }

        switch(msgType) {
            case TIBEMS_MESSAGE:
                msgTypeName = "MESSAGE";
                break;

            case TIBEMS_BYTES_MESSAGE:
                msgTypeName = "BYTES";
                break;

            case TIBEMS_OBJECT_MESSAGE:
                msgTypeName = "OBJECT";
                break;

            case TIBEMS_STREAM_MESSAGE:
                msgTypeName = "STREAM";
                break;

            case TIBEMS_MAP_MESSAGE:
                msgTypeName = "MAP";
                break;

            case TIBEMS_TEXT_MESSAGE:
                msgTypeName = "TEXT";
                break;

            default:
                msgTypeName = "UNKNOWN";
                break;
        }

        /* publish sample sends TEXT message, if received other
         * just print entire message
         */
        if (msgType != TIBEMS_TEXT_MESSAGE)
        {
            m_controller->GetLogger() << "Received " << msgTypeName << " message:\n";
            tibemsMsg_Print(msg);
        }
        else {
            /* get the message text */
            status = tibemsTextMsg_GetText(msg, &txt);
            if (status != TIBEMS_OK)
            {
                fail("Error getting tibemsTextMsg text", m_errorContext);
            }
			m_controller->SinkMsg('T', (void*)txt, (size_t)strlen(txt));
            m_controller->GetLogger() << "Received TEXT message: " << (char*)(txt ? txt : "<text is set to NULL>") << "\n";
			long count;
			ReleaseSemaphore(m_controller->GetThreadSemaphore(CTTICCLIENT_THREAD_NO), 1, &count);
        }

        /* destroy the message */
        status = tibemsMsg_Destroy(msg);
        if (status != TIBEMS_OK) {
            fail("Error destroying tibemsMsg", m_errorContext);
        }

    }
            
    /* destroy the destination */
    status = tibemsDestination_Destroy(m_destination);
    if (status != TIBEMS_OK) {
        fail("Error destroying tibemsDestination", m_errorContext);
    }

    /* close the connection */
    status = tibemsConnection_Close(m_connection);
    if (status != TIBEMS_OK) {
        fail("Error closing tibemsConnection", m_errorContext);
    }

    /* destroy the ssl params */
    if (m_sslParams) {
        tibemsSSLParams_Destroy(m_sslParams);
    }

    tibemsErrorContext_Close(m_errorContext);
	return OK;
}

unsigned __stdcall TibConsumer::ConsumerEntryPoint(void * pInstance) {
	TibConsumer &consumer = *(TibConsumer*)pInstance;
	consumer.Run(consumer);
	return 0;
}

CTTICClient::CTTICClient(Controller *controller) {
	m_controller = controller;
	m_connection = NULL;
}

void CTTICClient::Init() {
	m_controller->GetLogger() << "CTTICClient thread started.\n";



 	CConfigInterface *m_lpConfig = &(m_controller->GetConfig());
    m_connection = NewConnection(m_lpConfig);
    m_connection->AddRef();
	
    int ret = 0;

    // 初始化连接对象，返回0表示初始化成功，注意此时并没开始连接服务器,这里必须用Create2BizMsg，否则回调不成功
    if (0 == (ret = m_connection->Create2BizMsg(NULL))) {
		for (int i=0;i<3; i++) {
			ret = m_connection->Connect(DEFAULT_TIMEOUT);
			if (0 == ret) {
				break;
			}
		}
		if (0 != ret) {
			m_controller->GetLogger() << (char*)m_connection->GetErrorMsg(ret) << "\n";
			exit(CTTIC_CLIENT_CONNECTION_FAIL);
		}
	} else {
		m_controller->GetLogger() << (char*)m_connection->GetErrorMsg(ret) << "\n";
		exit(CTTIC_CLIENT_CREATE_CALLBACK_FAIL);
	}
	strncpy(m_info.identity_type, m_lpConfig->GetString("client", "identity_type", DEFAULT_IDENTITY_TYPE), sizeof(m_info.identity_type) - 1);
	strncpy(m_info.account_content, m_lpConfig->GetString("client", "account_content", DEFAULT_OP_ACCOUNT), sizeof(m_info.account_content) - 1);
	strncpy(m_info.account_content, m_lpConfig->GetString("client", "fund_account", DEFAULT_OP_ACCOUNT), sizeof(m_info.account_content) - 1);
	strncpy(m_info.op_account_content, m_lpConfig->GetString("client", "op_account_content", DEFAULT_OP_ACCOUNT), sizeof(m_info.op_account_content) - 1);
	strncpy(m_info.password, m_lpConfig->GetString("client", "password", DEFAULT_PASSWORD), sizeof(m_info.password) - 1);
	strncpy(m_info.op_entrust_way, m_lpConfig->GetString("client", "op_entrust_way", DEFAULT_OP_ENTRUST_WAY), sizeof(m_info.op_entrust_way) - 1);
	strncpy(m_info.input_content, m_lpConfig->GetString("client", "input_content", DEFAULT_INPUT_CONTENT), sizeof(m_info.input_content) - 1);
	strncpy(m_info.password_type, m_lpConfig->GetString("client", "password_type", DEFAULT_PASSWORD_TYPE), sizeof(m_info.password_type) - 1);
	strncpy(m_info.op_station, m_lpConfig->GetString("client", "op_station", "<ClientName> <IP> <MAC>"), sizeof(m_info.op_station) - 1);
	strncpy(m_info.request_num, m_lpConfig->GetString("client", "request_num", DEFAULT_REQUEST_NO), sizeof(m_info.request_num) - 1);
	strcpy(m_lorder_status, "1");
	strcpy(m_lorder_no, "1");
	Login();
//	SetupOptSubscription();
}

CTTICClient::~CTTICClient() {
	if (m_connection != NULL)	m_connection->Release();
}

BasicInfo CTTICClient::GetBasicInfo() {
	return m_info;
}

void CTTICClient::SetNecessaryOPParam(IF2Packer* pack) {
	pack->AddField("client_id");
	pack->AddField("fund_account");
	pack->AddField("sysnode_id");
	pack->AddField("identity_type");
	pack->AddField("op_branch_no");
	pack->AddField("branch_no");
	pack->AddField("op_station");
	pack->AddField("op_entrust_way");
	pack->AddField("password_type");
	pack->AddField("password");
	pack->AddField("asset_prop");
	pack->AddField("user_token");
	pack->AddField("request_num");
	pack->AddStr(m_info.client_id);
	pack->AddStr(m_info.account_content);
	pack->AddStr(m_info.sysnode_id);
	pack->AddStr(m_info.identity_type);
	pack->AddStr(m_info.branch_no);
	pack->AddStr(m_info.branch_no);
	pack->AddStr(m_info.op_station);
	pack->AddStr(m_info.op_entrust_way);
	pack->AddStr(m_info.password_type);
	pack->AddStr(m_info.password);
	pack->AddStr(m_info.asset_prop);
	pack->AddStr(m_info.user_token);
	pack->AddStr("500");
}


void CTTICClient::SetupOptSubscription() {
	ctsMdHandle ctsHandle ;
	int ret;
	int iRet = -100;
	for (int i=0; i<3; i++) {
		ret = frontConnect(&ctsHandle, DEFAULT_OPT_CONFIG_FILE);
		if (0 == ret) {
			iRet = SubscribeMd_Opt(ctsHandle,callBack_OptCode);
			if ( 0 <= iRet)	{
				break;
			}
		}
	}
	if (0 != ret || (0 > iRet && -1 != iRet)) {
		m_controller->GetLogger() << "期权行情订阅失败！\n" << GetErrorMsg(ctsHandle,iRet) << "\n";
		exit(CTTIC_CLIENT_OPT_SUBSCRIBE_FAIL);
	} else if (-1 == iRet) {
		m_controller->GetLogger() << "期权行情订阅失败！\n" << GetMCLastError(ctsHandle) << "\n";
		exit(CTTIC_CLIENT_OPT_SUBSCRIBE_FAIL);
	}
	m_controller->GetLogger() << "期权行情订阅成功\n";
}

void __stdcall CTTICClient::callBack_OptCode(pCITICS_MD_OPTPRICE ctsOptCode) {

	std::stringstream ss;
	if (ctsOptCode != NULL) {
		ss	<< "exchange:"	<<	ctsOptCode->exchange_type	<< ","
			<< "contract:"	<<	ctsOptCode->option_code		<< ","
			<< "close:"		<<	std::setprecision(2)				<<	ctsOptCode->close_price			<<	","
			<< "open:"		<<	std::setprecision(2)				<<	ctsOptCode->open_price			<<	","
			<< "amount:"	<<	std::setprecision(2)				<<	ctsOptCode->business_balance	<<	","
			<< "high:"		<<	std::setprecision(2)				<<	ctsOptCode->high_price			<<	","
			<< "low:"		<<	std::setprecision(2)				<<	ctsOptCode->low_price			<<	","
			<< "last:"		<<	std::setprecision(2)				<<	ctsOptCode->last_price			<<	","
			<< "quant:"		<<	std::setprecision(2)				<<	ctsOptCode->business_balance	<<	","
			<< "buy1:"		<<	std::setprecision(2)				<<	ctsOptCode->opt_buy_price1		<<	","
			<< "buy2:"		<<	std::setprecision(2)				<<	ctsOptCode->opt_buy_price2		<<	","
			<< "buy3:"		<<	std::setprecision(2)				<<	ctsOptCode->opt_buy_price3		<<	","
			<< "buy4:"		<<	std::setprecision(2)				<<	ctsOptCode->opt_buy_price4		<<	","
			<< "buy5:"		<<	std::setprecision(2)				<<	ctsOptCode->opt_buy_price5		<<	","
			<< "sell1:"		<<	std::setprecision(2)				<<	ctsOptCode->opt_sale_price1		<<	","
			<< "sell2:"		<<	std::setprecision(2)				<<	ctsOptCode->opt_sale_price2		<<	","
			<< "sell3:"		<<	std::setprecision(2)				<<	ctsOptCode->opt_sale_price3		<<	","
			<< "sell4:"		<<	std::setprecision(2)				<<	ctsOptCode->opt_sale_price4		<<	","
			<< "sell5:"		<<	std::setprecision(2)				<<	ctsOptCode->opt_sale_price5		<<	","
			<< "buy_amt1:"	<<	ctsOptCode->buy_amount1		<<	","
			<< "buy_amt2:"	<<	ctsOptCode->buy_amount2		<<	","
			<< "buy_amt3:"	<<	ctsOptCode->buy_amount3		<<	","
			<< "buy_amt4:"	<<	ctsOptCode->buy_amount4		<<	","
			<< "buy_amt5:"	<<	ctsOptCode->buy_amount5		<<	","
			<< "sale_amt1:"	<<	ctsOptCode->sale_amount1	<<	","
			<< "sale_amt2:"	<<	ctsOptCode->sale_amount2	<<	","
			<< "sale_amt3:"	<<	ctsOptCode->sale_amount3	<<	","
			<< "sale_amt4:"	<<	ctsOptCode->sale_amount4	<<	","
			<< "sale_amt5:"	<<	ctsOptCode->sale_amount5	<<	","
			<< "pre_square:"<<	std::setprecision(2)				<<	ctsOptCode->pre_square_price	<<	","
			<< "square:"	<<	std::setprecision(2)				<<	ctsOptCode->square_price		<<	","
			<< "auction:"	<<	std::setprecision(2)				<<	ctsOptCode->auction_price		<<	","
			<< "auction_amt:"<<	std::setprecision(2)				<<	ctsOptCode->auction_amount		<<	","
			<< "undrop_amt:"<<	std::setprecision(2)				<<	ctsOptCode->undrop_amount		<<	","
			<< "optexch_status:"							<<	ctsOptCode->optexch_status		<<	","
			<< "restrict:"	<<	ctsOptCode->opt_open_restriction;
		
		std::string content;
		ss >> content;

		TibStuff::getController().SinkMsg('C', (char *)content.c_str(), content.length());
		TibStuff::getController().GetLogger() << content.c_str() << "\n";

		long count;
		ReleaseSemaphore(TibStuff::getController().GetThreadSemaphore(TIB_PRODUCER_THREAD_NO), 1, &count);
	}
}


int CTTICClient::Login() {
	
	int ret;
	IBizMessage* lpBizMessage = NewBizMessage();
	lpBizMessage->AddRef();

	lpBizMessage->SetFunction(IFUNC_USER_LOGIN);
	//请求类型
	lpBizMessage->SetPacketType(REQUEST_PACKET);
	//获取打包器
	IF2Packer* pack = NewPacker(2);
	pack->AddRef();
	pack->BeginPack();
	pack->AddField("identity_type");
	pack->AddField("password_type");
	pack->AddField("input_content");
	pack->AddField("op_entrust_way");
	pack->AddField("password");
	pack->AddField("account_content");
	pack->AddStr(m_info.identity_type);
	pack->AddStr(m_info.password_type);
	pack->AddStr(m_info.input_content);
	pack->AddStr(m_info.op_entrust_way);
	pack->AddStr(m_info.password);
	pack->AddStr(m_info.account_content);
	pack->EndPack();

	lpBizMessage->SetContent(pack->GetPackBuf(),pack->GetPackLen());

	void *data = NULL;
	int len = 0;
	ret = FunctionExecution(lpBizMessage, &data, &len);
	IF2UnPacker *lpUnPacker = NULL;

	if (NULL != data && 0 != len) {
		lpUnPacker = NewUnPacker(data, len);
	}

	if (ret == 0) {
		strcpy(m_info.client_id, lpUnPacker->GetStr("client_id"));
		strcpy(m_info.user_token, lpUnPacker->GetStr("user_token"));
		strcpy(m_info.branch_no, lpUnPacker->GetStr("branch_no"));
		strcpy(m_info.asset_prop, lpUnPacker->GetStr("asset_prop"));
		strcpy(m_info.sysnode_id, lpUnPacker->GetStr("sysnode_id"));
		m_systemNo = atoi(m_info.sysnode_id);
	}

	if (lpUnPacker) {
			lpUnPacker->AddRef();
			char *res = NULL;
			AssemblePacket(lpUnPacker, &res);
//			Put the message back to tibco queue
	#ifdef _DEBUG
			ShowPacket(lpUnPacker);
	#endif
			if (res) free(res);
			free(data);
			lpUnPacker->Release();
	}
	pack->FreeMem(pack->GetPackBuf());
	pack->Release();
	lpBizMessage->Release();
	if (ret) {
		exit(LOGIN_FAIL);
	}
	return ret;	
}

unsigned __stdcall CTTICClient::Run(CTTICClient &client) {

	TMsgHeader len;
	void *rmsg;

	while (TRUE) {
		WaitForSingleObject(m_controller->GetThreadSemaphore(CTTICCLIENT_THREAD_NO), INFINITE);
		while (rmsg = m_controller->GetNextMsg('T')) {
			memcpy(&len, rmsg, sizeof(len));
			char *msg = new char[len + 1];
			memcpy(msg, (void*)((long)rmsg + sizeof(len)), len);
			msg[len] = 0;
			free(rmsg);
			free(msg);
#ifdef _DEBUG
			Order order;
			strcpy(order.exchange_type, "1");//1表示上海A股，2表示深圳A股
			strcpy(order.option_code, "10000957");//股票代码
			strcpy(order.entrust_amount, "5");//数量
			strcpy(order.entrust_price, "0.01");//价格
			strcpy(order.entrust_bs, "1");//1买 2卖
			strcpy(order.entrust_prop, "0");// 普通买卖
			strcpy(order.batch_no, "0");//0表示单笔订单
			strcpy(order.entrust_oc, "O");

			QueryUserFund();
			PlaceOrder(order);
			GetOrderStatus(m_lorder_no);
			CancelOrder(m_lorder_no);
			GetOrderStatus(m_lorder_no);
			GetOrderStatus(m_lorder_no);
			GetOrderStatus(m_lorder_no);
			GetOrderStatus(m_lorder_no);
			QueryOptCode();
			GetDealStatus();
#endif
		}
 	}
	return 0;
}

unsigned __stdcall CTTICClient::ClientEntryPoint(void * pInstance) {
	CTTICClient &client = *(CTTICClient *)pInstance;
	client.Run(client);
	return 0;
}

int CTTICClient::FunctionExecution(IBizMessage* lpBizMessage, void **lppBuffer, int *len) {

	int ret = 0;
	int successful = 0;
	void * lpBuffer = NULL;
	*lppBuffer = NULL;

	cout << "Connect status:" << m_connection->GetStatus() << "\n";
	if ((ret = m_connection->SendBizMsg(lpBizMessage)) < 0) {
		m_controller->GetLogger() << m_connection->GetErrorMsg(ret) << "\n";
	} else {
		IBizMessage* lpBizMessageRecv = NULL;
		ret = m_connection->RecvBizMsg(ret,&lpBizMessageRecv, 10000);  

		if (ret == 0) {
			ret = lpBizMessageRecv->GetReturnCode();
			if(ret==1 || ret==-1) { 
				m_controller->GetLogger()	<< "errorNo:" 
											<< lpBizMessageRecv->GetErrorNo() 
											<< ",errorInfo:" 
											<< lpBizMessageRecv->GetErrorInfo() 
											<< "\n";
			} else if(ret==0) { 
				m_controller->GetLogger() <<  "业务操作成功\n";

			} else { 
				m_controller->GetLogger() << "业务操作失败\n"
											<< lpBizMessageRecv->GetErrorNo() 
											<< ",errorInfo:" 
											<< lpBizMessageRecv->GetErrorInfo() 
											<< "\n";
			}
			int iLen = 0;
			const void *lpTemp = lpBizMessageRecv->GetContent(iLen);
			lpBuffer = malloc(iLen);
			memcpy(lpBuffer, lpTemp, iLen);
			*lppBuffer = lpBuffer;
			*len = iLen;
		} else {
			m_controller->GetLogger() << m_connection->GetErrorMsg(ret) << "\n";
		}
		if (lpBizMessageRecv) {
			lpBizMessageRecv->AddRef();
			lpBizMessageRecv->Release();
		}
	}
	return ret;
}

int CTTICClient::PlaceOrder(Order &order) {
	int ret;
	IBizMessage* lpBizMessage = NewBizMessage();
	lpBizMessage->AddRef();

	lpBizMessage->SetFunction(IFUNC_PLACE_OPT_ORDER);
	lpBizMessage->SetPacketType(REQUEST_PACKET);

	IF2Packer* pack = NewPacker(2);
	pack->AddRef();
	pack->BeginPack();

	pack->AddField("client_id");
	pack->AddField("option_account");
	pack->AddField("fund_account");
	pack->AddField("sysnode_id");
	pack->AddField("identity_type");
	pack->AddField("op_branch_no");
	pack->AddField("branch_no");
	pack->AddField("op_station");
	pack->AddField("op_entrust_way");
	pack->AddField("password_type");
	pack->AddField("password");
	pack->AddField("asset_prop");
	pack->AddField("user_token");
	pack->AddField("request_num");

	pack->AddField("exchange_type");
	pack->AddField("option_code");
	pack->AddField("entrust_amount");
	pack->AddField("opt_entrust_price");
	pack->AddField("entrust_bs");
	pack->AddField("entrust_oc");
	pack->AddField("entrust_prop");
	pack->AddField("batch_no");
	
	pack->AddStr(m_info.client_id);
	pack->AddStr("9899012971");
	pack->AddStr("9899012971");
	pack->AddStr(m_info.sysnode_id);
	pack->AddStr("2");
	pack->AddStr(m_info.branch_no);
	pack->AddStr(m_info.branch_no);
	pack->AddStr("<ClientName> <IP> <MAC>");
	pack->AddStr("5");
	pack->AddStr("2");
	pack->AddStr("111111");
	pack->AddStr(m_info.asset_prop);
	pack->AddStr(m_info.user_token);
	pack->AddStr("500");

	pack->AddStr(order.exchange_type);
	pack->AddStr(order.option_code);
	pack->AddStr(order.entrust_amount);
	pack->AddStr(order.entrust_price);
	pack->AddStr(order.entrust_bs);
	pack->AddStr(order.entrust_oc);
	pack->AddStr(order.entrust_prop);
	pack->AddStr(order.batch_no);

	pack->EndPack();

	lpBizMessage->SetContent(pack->GetPackBuf(),pack->GetPackLen());
	lpBizMessage->SetSystemNo(m_systemNo);

	void *data = NULL;
	int len = 0;
	ret = FunctionExecution(lpBizMessage, &data, &len);
	IF2UnPacker *lpUnPacker = NULL;

	if (ret == 0) {
		if (NULL != data && 0 != len) {
			lpUnPacker = NewUnPacker(data, len);
			strcpy(m_lorder_no, lpUnPacker->GetStr("entrust_no"));
		}

		//释放资源
		if (lpUnPacker) {
			lpUnPacker->AddRef();
			char *res = NULL;
			AssemblePacket(lpUnPacker, &res);
//			Put the message back to tibco queue
	#ifdef _DEBUG
			ShowPacket(lpUnPacker);
	#endif
			if (res) free(res);
			free(data);
			lpUnPacker->Release();
		}
	}

	pack->FreeMem(pack->GetPackBuf());
	pack->Release();
	lpBizMessage->Release();

	return ret;
}

int CTTICClient::QueryUserInfo() {
	int ret;
	IBizMessage* lpBizMessage = NewBizMessage();
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction(IFUNC_QUERY_USER_INFO);
	//请求类型
	lpBizMessage->SetPacketType(REQUEST_PACKET);
	//获取打包器
	IF2Packer* pack = NewPacker(2);
	pack->AddRef();
	pack->BeginPack();

	pack->EndPack();

	lpBizMessage->SetSystemNo(m_systemNo);
	lpBizMessage->SetContent(pack->GetPackBuf(),pack->GetPackLen());

	void *data = NULL;
	int len = 0;
	ret = FunctionExecution(lpBizMessage, &data, &len);
	IF2UnPacker *lpUnPacker = NULL;

	if (ret == 0) {
		if (NULL != data && 0 != len) {
			lpUnPacker = NewUnPacker(data, len);
		}

		//释放资源
		if (lpUnPacker) {
			lpUnPacker->AddRef();
			char *res = NULL;
			AssemblePacket(lpUnPacker, &res);
//			Put the message back to tibco queue
	#ifdef _DEBUG
			ShowPacket(lpUnPacker);
	#endif
			if (res) free(res);
			free(data);
			lpUnPacker->Release();
		}
	}

				//释放资源
	pack->FreeMem(pack->GetPackBuf());
	pack->Release();
	lpBizMessage->Release();

	return ret;
}

int CTTICClient::QueryUserFund() {
	int ret=0;
	IBizMessage* lpBizMessage = NewBizMessage();
	lpBizMessage->AddRef();

	lpBizMessage->SetFunction(IFUNC_QUERY_OPT_FUND_INFO);
	//请求类型
	lpBizMessage->SetPacketType(REQUEST_PACKET);
	//获取打包器
	IF2Packer* pack = NewPacker(2);
	pack->BeginPack();
	pack->AddField("op_branch_no");
	pack->AddField("op_entrust_way");
	pack->AddField("op_station");
	pack->AddField("branch_no");
	pack->AddField("client_id");
	pack->AddField("fund_account");
	pack->AddField("password");
	pack->AddField("password_type");
	pack->AddField("user_token");
	pack->AddField("sysnode_id");

	pack->AddStr(m_info.branch_no);
	pack->AddStr(m_info.op_entrust_way);
	pack->AddStr(m_info.op_station);
	pack->AddStr(m_info.branch_no);
	pack->AddStr(m_info.client_id);
	pack->AddStr(m_info.account_content);
	pack->AddStr(m_info.password);
	pack->AddStr(m_info.password_type);
	pack->AddStr(m_info.user_token);
	pack->AddStr(m_info.sysnode_id);

	pack->EndPack();
	lpBizMessage->SetContent(pack->GetPackBuf(),pack->GetPackLen());
	lpBizMessage->SetSystemNo(m_systemNo);

	void *data = NULL;
	int len = 0;
	ret = FunctionExecution(lpBizMessage, &data, &len);
	IF2UnPacker *lpUnPacker = NULL;

	if (ret == 0) {
		if (NULL != data && 0 != len) {
			lpUnPacker = NewUnPacker(data, len);
		}

		//释放资源
		if (lpUnPacker) {
			lpUnPacker->AddRef();
			char *res = NULL;
			AssemblePacket(lpUnPacker, &res);
//			Put the message back to tibco queue
	#ifdef _DEBUG
			ShowPacket(lpUnPacker);
	#endif
			if (res) free(res);
			free(data);
			lpUnPacker->Release();
		}
	}

	//释放资源
	pack->FreeMem(pack->GetPackBuf());
	pack->Release();
	lpBizMessage->Release();

	return ret;
}

int CTTICClient::CancelOrder(char *trust_no) {
	int ret=0;
	IBizMessage* lpBizMessage = NewBizMessage();
	lpBizMessage->AddRef();

	lpBizMessage->SetFunction(IFUNC_CANCEL_OPT_ORDER);
	//请求类型
	lpBizMessage->SetPacketType(REQUEST_PACKET);
	//获取打包器
	IF2Packer* pack = NewPacker(2);
	pack->BeginPack();
	pack->AddField("op_branch_no");
	pack->AddField("op_entrust_way");
	pack->AddField("op_station");
	pack->AddField("branch_no");
	pack->AddField("client_id");
	pack->AddField("fund_account");
	pack->AddField("password");
	pack->AddField("password_type");
	pack->AddField("user_token");
	pack->AddField("asset_prop");
	pack->AddField("exchange_type");
	pack->AddField("entrust_no");
	pack->AddStr(m_info.branch_no);
	pack->AddStr(m_info.op_entrust_way);
	pack->AddStr(m_info.op_station);
	pack->AddStr(m_info.branch_no);
	pack->AddStr(m_info.client_id);
	pack->AddStr(m_info.account_content);
	pack->AddStr(m_info.password);
	pack->AddStr(m_info.password_type);
	pack->AddStr(m_info.user_token);
	pack->AddStr(m_info.asset_prop);
	pack->AddStr("1");
	pack->AddStr(trust_no);
	pack->EndPack();
	lpBizMessage->SetContent(pack->GetPackBuf(),pack->GetPackLen());
	lpBizMessage->SetSystemNo(m_systemNo);

	void *data = NULL;
	int len = 0;
	ret = FunctionExecution(lpBizMessage, &data, &len);
	IF2UnPacker *lpUnPacker = NULL;

	if (ret == 0) {
		if (NULL != data && 0 != len) {
			lpUnPacker = NewUnPacker(data, len);
		}

		//释放资源
		if (lpUnPacker) {
			lpUnPacker->AddRef();
			char *res = NULL;
			AssemblePacket(lpUnPacker, &res);
//			Put the message back to tibco queue
	#ifdef _DEBUG
			ShowPacket(lpUnPacker);
	#endif
			if (res) free(res);
			free(data);
			lpUnPacker->Release();
		}
	}
	//释放资源
	pack->FreeMem(pack->GetPackBuf());
	pack->Release();
	lpBizMessage->Release();

	return ret;
}

int CTTICClient::QueryOptCode() {
	int ret;
	char *ret_status = NULL;
	IBizMessage* lpBizMessage = NewBizMessage();
	lpBizMessage->AddRef();

	lpBizMessage->SetFunction(IFUNC_QUERY_OPT_CODE);
	//请求类型
	lpBizMessage->SetPacketType(REQUEST_PACKET);
	//获取打包器
	IF2Packer* pack = NewPacker(2);
	pack->BeginPack();
	pack->AddField("op_branch_no");
	pack->AddField("op_entrust_way");
	pack->AddField("op_station");
	pack->AddField("request_num");
	
	pack->AddStr(m_info.branch_no);
	pack->AddStr("5");
	pack->AddStr("<ClientName> <IP> <MAC>");
	pack->AddStr("10");
	pack->EndPack();

	lpBizMessage->SetSystemNo(m_systemNo);
	lpBizMessage->SetContent(pack->GetPackBuf(),pack->GetPackLen());

	void *data = NULL;
	int len = 0;
	ret = FunctionExecution(lpBizMessage, &data, &len);
	IF2UnPacker *lpUnPacker = NULL;

	if (ret == 0) {
		if (NULL != data && 0 != len) {
			lpUnPacker = NewUnPacker(data, len);
		}

		//释放资源
		if (lpUnPacker) {
			lpUnPacker->AddRef();
			char *res = NULL;
			AssemblePacket(lpUnPacker, &res);
//			Put the message back to tibco queue
	#ifdef _DEBUG
			ShowPacket(lpUnPacker);
	#endif
			if (res) free(res);
			free(data);
			lpUnPacker->Release();
		}
	}

	//释放资源
	pack->FreeMem(pack->GetPackBuf());
	pack->Release();
	lpBizMessage->Release();

	return ret;
}

int CTTICClient::GetDealStatus() {
	int ret;
	char *ret_status = NULL;
	IBizMessage* lpBizMessage = NewBizMessage();
	lpBizMessage->AddRef();

	lpBizMessage->SetFunction(IFUNC_QUERY_OPT_DEAL);
	//请求类型
	lpBizMessage->SetPacketType(REQUEST_PACKET);
	//获取打包器
	IF2Packer* pack = NewPacker(2);
	pack->AddRef();

	pack->BeginPack();
	pack->AddField("op_branch_no");
	pack->AddField("op_entrust_way");
	pack->AddField("op_station");
	pack->AddField("branch_no");
	pack->AddField("client_id");
	pack->AddField("fund_account");
	pack->AddField("password");
	pack->AddField("password_type");
	pack->AddField("user_token");
	pack->AddField("asset_prop");
	pack->AddField("option_code");
	pack->AddField("request_num");
	
	pack->AddStr(m_info.branch_no);
	pack->AddStr("5");
	pack->AddStr("<ClientName> <IP> <MAC>");
	pack->AddStr(m_info.branch_no);
	pack->AddStr(m_info.client_id);
	pack->AddStr(m_info.account_content);
	pack->AddStr(m_info.password);
	pack->AddStr(m_info.password_type);
	pack->AddStr(m_info.user_token);
	pack->AddStr(m_info.asset_prop);
	pack->AddStr("10000957");
	pack->AddStr("10");
	pack->EndPack();

	lpBizMessage->SetSystemNo(m_systemNo);
	lpBizMessage->SetContent(pack->GetPackBuf(),pack->GetPackLen());

	void *data = NULL;
	int len = 0;
	ret = FunctionExecution(lpBizMessage, &data, &len);
	IF2UnPacker *lpUnPacker = NULL;

	if (ret == 0) {
		if (NULL != data && 0 != len) {
			lpUnPacker = NewUnPacker(data, len);
		}

		//释放资源
		if (lpUnPacker) {
			lpUnPacker->AddRef();
			char *res = NULL;
			AssemblePacket(lpUnPacker, &res);
//			Put the message back to tibco queue
	#ifdef _DEBUG
			ShowPacket(lpUnPacker);
	#endif
			if (res) free(res);
			free(data);
			lpUnPacker->Release();
		}
	}

	lpBizMessage->Release();
	pack->FreeMem(pack->GetPackBuf());
	pack->Release();

	return ret;
}

int CTTICClient::GetOrderStatus(char *trust_no) {
	int ret;
	char *ret_status = NULL;
	IBizMessage* lpBizMessage = NewBizMessage();
	lpBizMessage->AddRef();

	lpBizMessage->SetFunction(IFUNC_QUERY_OPT_ORDER);
	//请求类型
	lpBizMessage->SetPacketType(REQUEST_PACKET);
	//获取打包器
	IF2Packer* pack = NewPacker(2);
	pack->BeginPack();
	pack->AddField("op_branch_no");
	pack->AddField("op_entrust_way");
	pack->AddField("op_station");
	pack->AddField("branch_no");
	pack->AddField("client_id");
	pack->AddField("fund_account");
	pack->AddField("password");
	pack->AddField("password_type");
	pack->AddField("user_token");
	pack->AddField("asset_prop");
	pack->AddField("locate_entrust_no");
	
	pack->AddStr(m_info.branch_no);
	pack->AddStr("5");
	pack->AddStr("<ClientName> <IP> <MAC>");
	pack->AddStr(m_info.branch_no);
	pack->AddStr(m_info.client_id);
	pack->AddStr("9899012971");
	pack->AddStr("111111");
	pack->AddStr("2");
	pack->AddStr(m_info.user_token);
	pack->AddStr(m_info.asset_prop);
	pack->AddStr(trust_no);
	pack->EndPack();

	lpBizMessage->SetSystemNo(m_systemNo);
	lpBizMessage->SetContent(pack->GetPackBuf(),pack->GetPackLen());

	void *data = NULL;
	int len = 0;
	ret = FunctionExecution(lpBizMessage, &data, &len);
	IF2UnPacker *lpUnPacker = NULL;

	if (ret == 0) {
		if (NULL != data && 0 != len) {
			lpUnPacker = NewUnPacker(data, len);
		}

		//释放资源
		if (lpUnPacker) {
			lpUnPacker->AddRef();
	#ifdef _DEBUG
			ShowPacket(lpUnPacker);
	#endif
			ShowPacket(lpUnPacker);
			char *ret;
			AssemblePacket(lpUnPacker,&ret);
			cout << ret << "\n";
			free(data);
			free(ret);
			lpUnPacker->Release();
		}
	}

	pack->FreeMem(pack->GetPackBuf());
	pack->Release();
	lpBizMessage->Release();

	return ret;
}

TibProducer& Controller::GetProducer() {
	return *m_tibProducer;
}

TibConsumer& Controller::GetConsumer() {
	return *m_tibConsumer;
}

CTTICClient& Controller::GetClient() {
	return *m_CTTICClient;
}

CConfigInterface& Controller::GetConfig() {
	return *m_lpConfig;
}

Logger& Controller::GetLogger() {
	return m_logger;
}

Controller::Controller() {
	m_tibProducer = new TibProducer(this);
	m_tibConsumer = new TibConsumer(this);
	m_CTTICClient = new CTTICClient(this);
	m_lpConfig = NULL;
}

Controller::~Controller() {
	if (m_tibProducer != NULL) delete m_tibProducer;
	if (m_tibConsumer != NULL) delete m_tibConsumer;
	if (m_CTTICClient != NULL) delete m_CTTICClient;
	if (m_lpConfig != NULL) m_lpConfig->Release();
	while (!m_tibMsgBuf.empty()) {
		free(m_tibMsgBuf.front());
		m_tibMsgBuf.pop();
	}

	for (int i=0;i<NUM_THREAD_SEMAPHORE; i++) {
		CloseHandle(m_threadHandlers[i]);
		CloseHandle(m_semaphore[i]);
	}
}

void Controller::Init() {

	m_lpConfig = NewConfig();
    m_lpConfig->AddRef();
	m_lpConfig->Load(DEFAULT_OPT_CONFIG_FILE);

	if (!FindProcess(L"tibemsd.exe")) {
		m_logger << "Tibco EMS server is not started, please run it before start me!\n";
		exit(1);
	}

	m_tibProducer->Init();
	m_tibConsumer->Init();
	m_CTTICClient->Init();

	m_threadHandlers[TIB_PRODUCER_THREAD_NO] = (HANDLE)_beginthreadex( NULL,         // security  
                                   0,            // stack size  
                                   TibProducer::ProducerEntryPoint,  
								   m_tibProducer,           // arg list  
                                   CREATE_SUSPENDED,  // so we can later call ResumeThread()  
                                   &m_threadIDs[TIB_PRODUCER_THREAD_NO] );  
    m_threadHandlers[TIB_CONSUMER_THREAD_NO] = (HANDLE)_beginthreadex( NULL,         // security  
                                   0,            // stack size  
                                   TibConsumer::ConsumerEntryPoint,  
								   m_tibConsumer,           // arg list  
                                   CREATE_SUSPENDED,  // so we can later call ResumeThread()  
                                   &m_threadIDs[TIB_CONSUMER_THREAD_NO] );  
    m_threadHandlers[CTTICCLIENT_THREAD_NO] = (HANDLE)_beginthreadex( NULL,         // security  
                                   0,            // stack size  
                                   CTTICClient::ClientEntryPoint,  
								   m_CTTICClient,           // arg list  
                                   CREATE_SUSPENDED,  // so we can later call ResumeThread()  
                                   &m_threadIDs[CTTICCLIENT_THREAD_NO] );  
  
	for (int i=0;i<NUM_THREAD_SEMAPHORE; i++) {
		if ( m_threadHandlers[i] == 0 )  {
			m_logger << "Failed to create thread of" << THREAD_NAMES[i] << "\n";  
			exit(CREATE_THREAD_PRODUCER_FAIL + i);
		}
		m_semaphore[i] = CreateSemaphore(NULL, 0, 1, (LPCWSTR)THREAD_NAMES[i]);
		if ( m_semaphore[i] == 0 )  {
			m_logger << "Failed to create inter-threads semaphore for " << THREAD_NAMES[i] << "\n";  
			exit(CREATE_THREAD_SEMAPHORE_FAIL);
		}

		DWORD   dwExitCode;  
		GetExitCodeThread( m_threadHandlers[i], &dwExitCode );  // should be STILL_ACTIVE = 0x00000103 = 259  
		GetLogger() << "TibProducer thread initial status code: " << dwExitCode << "\n";

	}

	m_callback_mutext = CreateMutex(NULL, FALSE, (LPCWSTR)"Callback Mutex");
	if (m_callback_mutext == 0) {
			m_logger << "Failed to create callback mutext\n";  
			exit(CREATE_CALLBACK_MUTEX_FAIL);
	}

}

void Controller::StartMonit(){
	for (int i=0; i<NUM_THREAD_SEMAPHORE; i++) {
		if (ResumeThread(m_threadHandlers[i]) == -1) {
			DWORD lastErr = GetLastError();
			m_logger << "Failed to resume thread[" << i << "], Windows error code: " << lastErr << "\n";
			exit(RESUME_THREAD_PRODUCER_FAIL + i);
		}
	}
	WaitForMultipleObjects(NUM_THREAD_SEMAPHORE, m_threadHandlers, FALSE, INFINITE);
}

void Controller::Restart() {
//	tearDown();
	Init();
}

void Controller::TearDown() {
	// WaitForMultipleObjects(NUM_THREAD_SEMAPHORE, m_threadHandlers, 1, INFINITE);
}

HANDLE Controller::GetThreadSemaphore(int index) {
	return m_semaphore[index];
}

HANDLE Controller::GetCallbackMutex() {
	return m_callback_mutext;
}

void Controller::SinkMsg(char queueID, void *msg, size_t len) {

	if (queueID != 'T' && queueID != 'C') {
		return;
	}

	TMsgHeader length = len;

	void *content = malloc(length + sizeof(length));
	if (!content) {
		m_logger << "Failed on allocating memory for addMsgToQueue()\n";
		ExitThread(1);
	}
	memcpy(content, &length, sizeof(length));
	memcpy((void*)((long)content + sizeof(length)), msg, length);
	MsgQueue& target = queueID == 'T' ? m_tibMsgBuf : m_CTTICMsgBuf;
	target.push(content);
}

void* Controller::GetNextMsg(char queueID) {

	if (queueID != 'T' && queueID != 'C') {
		return NULL;
	}

	MsgQueue& target = queueID == 'T' ? m_tibMsgBuf : m_CTTICMsgBuf;

	void *next = NULL;
	if (!target.empty()) {
		next =  target.front();
		target.pop();
	}
	return next;
}

int main(int argc, char ** argv) {
	Controller controller;
	controller.Init();
	controller.StartMonit();
	return 0;
}

static void ShowPacket(IF2UnPacker *lpUnPacker)
{
    int i = 0, t = 0, j = 0, k = 0;

    for (i = 0; i < lpUnPacker->GetDatasetCount(); ++i)
    {
        // 设置当前结果集
        lpUnPacker->SetCurrentDatasetByIndex(i);

        // 打印字段
        for (t = 0; t < lpUnPacker->GetColCount(); ++t)
        {
            printf("%20s", lpUnPacker->GetColName(t));
        }

        putchar('\n');

        // 打印所有记录
        for (j = 0; j < (int)lpUnPacker->GetRowCount(); ++j)
        {
            // 打印每条记录
            for (k = 0; k < lpUnPacker->GetColCount(); ++k)
            {
                switch (lpUnPacker->GetColType(k))
                {
                case 'I':
                    printf("%20d", lpUnPacker->GetIntByIndex(k));
                    break;

                case 'C':
                    printf("%20c", lpUnPacker->GetCharByIndex(k));
                    break;

                case 'S':
                    printf("%20s", lpUnPacker->GetStrByIndex(k));
                    break;

                case 'F':
                    printf("%20f", lpUnPacker->GetDoubleByIndex(k));
                    break;

                case 'R':
                    {
                        int nLength = 0;
                        void *lpData = lpUnPacker->GetRawByIndex(k, &nLength);

                        // 对2进制数据进行处理
                        break;
                    }

                default:
                    // 未知数据类型
                    printf("未知数据类型。\n");
                    break;
                }
            }

            putchar('\n');

            lpUnPacker->Next();
        }

        putchar('\n');
    }
}

static void AssemblePacket(IF2UnPacker *lpUnPacker, char **ret) {

    int i = 0, t = 0, j = 0, k = 0;
	list<char*> buffer;
	list<char*>::iterator it;
	stringstream ss;

    for (i = 0; i < lpUnPacker->GetDatasetCount(); ++i)
    {
        // 设置当前结果集
        lpUnPacker->SetCurrentDatasetByIndex(i);
		if (i>0) {
			ss << '\x01';
		}

		for (t = 0; t < lpUnPacker->GetColCount(); ++t) {
			buffer.push_back((char *)lpUnPacker->GetColName(t));
		}

        for (j = 0; j < (int)lpUnPacker->GetRowCount(); ++j)
        {
			it = buffer.begin();

			if (j>0) {
				ss << '\x02';
			}

			for (k = 0; k < lpUnPacker->GetColCount(); ++k)
            {
				if (k>0) {
					ss << ",";
				}

                switch (lpUnPacker->GetColType(k))
                {
                case 'I':
                    ss << *it << ":" << lpUnPacker->GetIntByIndex(k);
                    break;

                case 'C':
                    ss << *it << ":" << lpUnPacker->GetCharByIndex(k);
                    break;

                case 'S':
                    ss << *it << ":" << lpUnPacker->GetStrByIndex(k);
                    break;

                case 'F':
					char temp[100];
					sprintf(temp, "%.2f", lpUnPacker->GetDoubleByIndex(k));
                    ss << *it << ":" << temp;
                    break;

                case 'R':
                    {
                        int nLength = 0;
                        void *lpData = lpUnPacker->GetRawByIndex(k, &nLength);

                        // 对2进制数据进行处理
                        break;
                    }

                default:
                    // 未知数据类型
                    // printf("未知数据类型。\n");
                    break;
                }
				it++;
            }
            lpUnPacker->Next();
        }
    }

	string content;
	ss >> content;
	char *p, *q;
	q = (char*)content.c_str();
	int l = strlen(q);
	p = (char*)malloc(l + 1);
	memcpy(p, q, l);
	p[l] = 0;
	*ret = p;
}

static BOOL FindProcess(WCHAR *filename)  
{  
    int i=0;  
    PROCESSENTRY32 pe32;  
    pe32.dwSize = sizeof(pe32);

	WCHAR *pfile;

    HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);  

    BOOL bMore = ::Process32First(hProcessSnap, &pe32);  
    while(bMore)  
    {  
        if(wcscmp(filename,pe32.szExeFile)==0)  
        {  
            i=1;
			break;
        }  
        bMore = ::Process32Next(hProcessSnap, &pe32);  
    }  
    if(i>=1){           //大于1，排除自身  
        return TRUE;  
    }else{  
        return FALSE;  
    }  
}

