#include "header.h"
#include "SearchEngine.h"
#include "InvIndex.h"
#include <WinSock2.h>
#pragma comment(lib,"ws2_32")


std::map<int, std::map<int, double, IntCmp>> docVec;

std::set<int> docList;

std::map<int, double> docModule;
std::map<std::string, int, StringCmp> word2num;
std::map<int, std::string, IntCmp> num2word;

std::map<std::string, std::vector<int>, StringCmp> soundMap;
std::map<int, WordItem, IntCmp> index;

int wordCnt = 0;
int IDF_FACTOR = 0;

char sendBuf[65535];
std::ostringstream ostr;

int main() {
	// init
	InvIndex * invIndex = new InvIndex();
	SearchEngine * NinetySix = new SearchEngine();

	std::cout << "Initalization Succesfully" << std::endl << std::endl;

	std::cout << "Binding Ports..." << std::endl;

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(1, 1);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		return 0;
	}

	if (LOBYTE(wsaData.wVersion) != 1 ||
		HIBYTE(wsaData.wVersion) != 1) {
		WSACleanup();
		return 0;
	}
	SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(19696);

	bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));

	listen(sockSrv, 5);

	SOCKADDR_IN addrClient;
	std::cout << "Service Start" << std::endl;
	int len = sizeof(SOCKADDR);
	while (1) {
		SOCKET sockConn = accept(sockSrv, (SOCKADDR*)&addrClient, &len);
		char recvBuf[50];
		memset(recvBuf, 0, sizeof(recvBuf));
		recv(sockConn, recvBuf, 50, 0);
		printf("key = %s\n", recvBuf);
		std::string s(recvBuf);
		ostr.str("");

		NinetySix->search(s);
		send(sockConn, ostr.str().c_str(), ostr.str().size() + 1, 0);
		std::cout << "SEND: " << ostr.str() << std::endl << std::endl;
		closesocket(sockConn);
	}

	system("pause");
}