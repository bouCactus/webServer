#ifndef __HTTPCLIENT_H__
#define __HTTPCLIENT_H__

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "HttpMethodProcessor.hpp"
#include "confAST.hpp"

class HttpClient{
	HttpClient& operator=(const HttpClient& httpClient);
	HttpClient();

	public:
		// HttpRequest req;
		// HttpResponse res;

		HttpClient(servers_it &server, int serverSocket, int socket);
		HttpClient(const HttpClient& httpClient);
		~HttpClient();

		/**************************************/
		/***** Handle Request Functions *******/
		/**************************************/
		// void	processRequest(servers_it& conf_S);
		// void	sendit();
		// void	sendChunk();
		// void	sendBuffer();

		/**************************************/
		/*** Client Configuration Functions ***/
		/**************************************/
		Server*	getConfiguration();
		int		getServerSocket();
		int		getSocket();

		void	setConfiguration(Server* server);
		void	set_socket(int clientSocket);

		void	set_Reading_State(bool state);
		void	set_Writing_State(bool state);
		bool	ReadyTo_Read();
		bool	ReadyTo_Write();

	private:
		Server	*conf;
		int		_serverSocket;
		int		_socket;
		bool	_readyTo_Read;
		bool	_readyTo_Write;
};

#endif // __HTTPCLIENT_H__

/*

if (ready to read) {
	client.readRequest();
	client.parseRequest();
}

if (ready to write) {
	client.handlerRequest() {
		response = generateResponse();
		sendRequest(response) {
			openFile();
		}
	}
}

*/