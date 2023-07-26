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
		
		HttpClient(servers_it& server, int socket);
		HttpClient(const HttpClient& httpClient);
		HttpClient& operator=(const HttpClient& httpClient);
		~HttpClient();

		/**************************************/
		/***** Handle Request Functions *******/
		/**************************************/
		void	processRequest(servers_it& conf_S);
		int    sendResponse();
  		int    sendFileResponse(HttpResponse& res, int _socket);


		/**************************************/
		/*** Client Configuration Functions ***/
		/**************************************/
		servers_it  getConfiguration() const;
		int		getSocket() const;

		// void	setConfiguration(Server& server);
		void	set_socket(int clientSocket);

        void	setRespondComplete(bool state);
        bool	isRespondComplete();
		bool 	isRequestComplete();
		void    setRequestComplete(bool state);
		// void	set_Reading_State(bool state);
		// void	set_Writing_State(bool state);
		// bool	ReadyTo_Read();
		// bool	ReadyTo_Write();

	private:
		servers_it		_conf;
		int				_socket;
		bool			_isRespondComplete;
		std::string 	_bufferRequest;
  		bool 			_writing;
  		bool 			_isHeaderSent;
  		std::streamsize _writingPos;
		bool 			_isRequestComplete;
};

int sendHeader(HttpResponse& res,int  _socket /*_socket to not complicated things */);

#endif // __HTTPCLIENT_H__

