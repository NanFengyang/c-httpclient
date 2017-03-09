#include <iostream>
#include <windows.h>
#include "../httpclient/http_request.h"
#include "../httpclient/http.h"
#include <cstring>
#include <string>  
#include <fstream>


class DelegateA : public Http::IProgressDelegate
{
public:
	virtual	void	dataWriteProgress(int write_length,int total_length)  {std::cout<<write_length<<"-------"<<total_length<<std::endl;}
	virtual	void	dataReadProgress(int read_length,int total_length) {std::cout<<read_length<<"-------"<<total_length<<std::endl;}
};

class DelegateB	: public Http::IAsyncHttpDelegate
{
public:
	virtual	void	onError(Http::HTTPERROR error_code,const Http::HttpRequest* request,const Http::HttpResponse* resposne)
	{
		std::cout<<"error code is     "<<error_code<<std::endl;
	}
	virtual	void	onSuccess(const Http::HttpRequest* request,const Http::HttpResponse* resposne)
	{
		std::cout<<"body is   "<<resposne->getBody()<<std::endl;
	}
	virtual	void	dataWriteProgress(int write_length,int total_length)  {std::cout<<write_length<<"-------"<<total_length<<std::endl;}
	virtual	void	dataReadProgress(int read_length,int total_length) {std::cout<<read_length<<"-------"<<total_length<<std::endl;}
};



std::string UTF8ToGBK(const std::string& strUTF8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);
	unsigned short * wszGBK = new unsigned short[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, (LPCCH)strUTF8.c_str(), -1, (LPWSTR)wszGBK, len);

	len = WideCharToMultiByte(CP_ACP, 0, (LPCWCH)wszGBK, -1, NULL, 0, NULL, NULL);
	char *szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, (LPCWCH)wszGBK, -1, szGBK, len, NULL, NULL);
	//strUTF8 = szGBK;  
	std::string strTemp(szGBK);
	delete[]szGBK;
	delete[]wszGBK;
	return strTemp;

}

//同步上传文件
void SyncUploadFile(const std::wstring& filepath)
{
	Http::HttpRequest	request("post", "http://192.168.103.243:8880/icr/recognize_vehicle_license");

	Http::HttpFileStream* file = new Http::HttpFileStream(filepath, "multipart/form-data");
	request.addFile("file", file);

	Http::HttpResponse	response;
	Http::HttpClient	client;
	DelegateA a;
	client.setProgressDelegate(&a);
	client.execute(&request, &response);
	std::cout << "download file successfully,filepath is:" << UTF8ToGBK(response.getBody()) << std::endl;
}

//同步下载文件
void SyncDownloadFile(const std::wstring& filepath)
{
	Http::HttpRequest	request("get","http://192.168.103.243:8880/icr/recognize_vehicle_license");
	request.saveToFile(filepath);
	Http::HttpResponse	response;
	Http::HttpClient	client;

	DelegateA a;
	client.setProgressDelegate(&a);
	client.execute(&request, &response);
	std::cout << "download file successfully,filepath is:" << UTF8ToGBK(response.getBody()) << std::endl;
}

//异步下载文件
void	AsyncDownloadFile()
{
	for (int i = 0; i < 1000; i++)
	{
		wchar_t buff[100]  = {0};
		swprintf_s(buff,100,L"D:\\httpclient_download\\%d.jpg",i);

		Http::HttpRequest*	request = new Http::HttpRequest("get","http://image.zcool.com.cn/1/24/m_1268212765379.jpg");
		request->saveToFile(buff);
		DelegateB* delegate = new DelegateB();
		Http::HttpPool::getInstance()->postRequest(request,delegate);
	}

}

void uploadFile()
{



}


int main()
{
	//未使用代理
	Http::ProxyConfig	empty_proxy;
	std::wstring	noproxy_filepath = L"C:\\Users\\Administrator\\Desktop\\云识别测试\\证件照\\QQ截图20170224100928.png";
	SyncUploadFile(noproxy_filepath);//同步

	////使用HTTP代理
	//Http::ProxyConfig	http_proxy(Http::PROXY_HTTP,"192.168.132.163",808,"proxy","test");
	//Http::HttpClient::setProxy(&http_proxy);
	//std::wstring	http_proxy_filepath = L"D:\\httpclient_download\\httpproxy.jpg";
	//SyncDownloadFile(http_proxy_filepath);


	////使用Socks4代理
	////Http::ProxyConfig	socks4_proxy(Http::PROXY_SOCK4,"192.168.132.163",1080);
	////Http::HttpClient::setProxy(&socks4_proxy);
	////std::wstring	sock4_proxy_filepath = L"D:\\httpclient_download\\socks4.jpg";
	////SyncDownloadFile(sock4_proxy_filepath);


	////使用Socks5代理
	//Http::ProxyConfig	socks5_proxy(Http::PROXY_SOCK5,"192.168.132.163",1080,"proxy","test");
	//Http::HttpClient::setProxy(&socks5_proxy);
	//std::wstring	socks5_proxy_filepath = L"D:\\httpclient_download\\socks5.jpg";
	//SyncDownloadFile(socks5_proxy_filepath);


	
	Http::HttpClient::setProxy(&empty_proxy);
	//AsyncDownloadFile();

	Sleep(5000000);
	return 0;
}