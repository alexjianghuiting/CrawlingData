#include <WinSock2.h>
#include <string>
#pragma comment(lib. "ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <fstream>
/**
 * Parsing URL
 * @param url : the website waiting for parsing
 * @param host : OUT host name
 * @param resource : OUT resource name
 * @return false : fails to parse the url
 **/
bool ParseUrl(const std::string &strUrl, std::string &strHost, std::string &strResource) {
    bool bRet = false;
    do {
        const int MAXRESOURCESIZE = 2000;
        const int MINHOSTSIZE = 5;
        const std::string strHttp = "http://";
        const char *szPos = strstr(strUrl.c_str(), strHttp.c_str());
        if (szPos == nullptr) {
            szPos = strUrl.c_str();
        } else {
            szPos = strHttp.length() + strUrl.c_str();
        }
        char szHost[MAXBYTE] = {0}; //新建非指针
        char szResource[MAXRESOURCESIZE] = {0};
        sscanf(szPos, "%[^/]%s", szHost, szResource);
        if (strlen(szHost) < MINHOSTSIZE) {
            break;
        }
        strHost = szHost;
        if (strlen(szResource) != 0) {
            strResource = szResource;
        }
        bRet = true;
        return bRet;
    } while (false);
    return bRet;
}
int _tmain(int argc, _TCHAR*argv[])
{
    char *pBuffer = nullptr;
    bool bRet = false;
    using std::string;
    SOCKET socketServer = INVALID_SOCKET;
    do
    {
        WSADATA wsaData;
        if(WSAStartup(MAKEWORD(2,2) != 0, &wsaData))
        break;
        if(LOBYTE(wsaData.wVersion != 2) || HIBYTE(wsaData.wVersion) != 2)
        break;
        string url = "www.wmpic.me/meinv";
        string strHost, strResource;
        ParseUrl(url, strHost, strResource);
        hostent *hServer = gethostbyname(strHost.c_str());
        if(hServer == nullptr) {
            break;
        }
        sockaddr_in addrServer = {0};
        addrServer.sin_family = AF_INET;
        addrServer.sin_port = htons(80);
        memcpy(&(addrServer.sin_addr), hServer->h_addr, 4);
        socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (INVALID_SOCKET == socketServer)
        {
            break;
        }
        if( connect(socketServer, (SOCKADDR*)&addrServer, sizeof(addrServer)) == SOCKET_ERROR)
        {
            break;
        }
        std::string strRequest = "GET" + strResource + "HTTP/1.1\r\nHost:"
        + strHost + "\r\nConnection:Close\r\n\r\n";
        if(send(socketServer, strRequest.c_str(), strRequest.length(), 0) == SOCKET_ERROR)
        {
            break;
        }
        const int MAXBUFFERSIZE = 1*1024*1024;
        pBuffer = (char*)malloc(MAXBUFFERSIZE);
        ZeroMemory(pBuffer, MAXBUFFERSIZE);
        int nRecvSize = 1;
        int nOffset = 0, nReallocCount = 2;
        while(nRecvSize > 0){
            nRecvSize = recv(socketServer, pBuffer + nOffset, MAXBUFFERSIZE - nOffset, 0);
            nOffset += nRecvSize;
            if (MAXBUFFERSIZE - nOffset < 100){
                pBuffer = (char*)realloc(pBuffer, MAXBUFFERSIZE*2);
                nReallocCount++;
            }
        }
        std::ofstream outPutStream("Buffer.txt", std::ios::app);
        outPutStream << pBuffer;
        outPutStream.close();
    } while (false);
    if(socketServer != INVALID_SOCKET){
        closesocket(socketServer);
    }
    WSACleanup();
    system("pause");
    return 0;
    
}