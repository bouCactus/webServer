#ifndef __HTTPCLIENT_H__
#define __HTTPCLIENT_H__

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "HttpMethodProcessor.hpp"
#include "confAST.hpp"
#include <fstream>

class HttpClient{
	public:
		HttpRequest req;
		HttpResponse res;

		HttpClient(Server *server, int socket);
		HttpClient(const HttpClient& httpClient);
		HttpClient& operator=(const HttpClient& httpClient);
		~HttpClient();

		/**************************************/
		/***** Handle Request Functions *******/
		/**************************************/
		void	processRequest(servers_it& conf_S);
		void    sendResponse();
  		void    sendFileResponse(HttpResponse& res, int _socket);


		/**************************************/
		/*** Client Configuration Functions ***/
		/**************************************/
		Server*	getConfiguration() const;
		int		getSocket() const;

		// void	setConfiguration(Server& server);
		void	set_socket(int clientSocket);

        void	set_ToFinish(bool state);
        bool	sendIsFinished();

		// void	set_Reading_State(bool state);
		// void	set_Writing_State(bool state);
		// bool	ReadyTo_Read();
		// bool	ReadyTo_Write();

	private:
		Server	*conf;
		int		_socket;
		bool	_sendFinished;
		std::string _bufferRequest;
  		bool _writing;
  		bool _isHeaderSent;
  		std::streamsize _writingPos;
};

#endif // __HTTPCLIENT_H__

