
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h> //arp -> ip 주소 -> mac 주소
#include <netdb.h> //DNS 조회함수

//HTTP GET 요청을 처리하는 함수
int http_get(const char *url);
//TCP 상태확인을 처리하는 함수
int tcp_check(const char *host, int port);


int main(int argc, int *argv[]) {
	//명령어 인자가 충분하지 않을 경우 사용법 출력
	if(argc < 2) {
		printf("Usage: %s <command> <args>\n", argv[0]);
		printf("Commands:\n");
		printf("  http-get <url>\n");
		printf("  tcp-check <host> <port>\n");
		return 1;
	}

	//사용자가 http-get 명령을 입력
	if(strcmp(argv[1], "http-get") == 0) {
		if(argc != 3) { //인자가 부족
			printf("Usage: %s http-get <url>\n", argv[0]);
			return 1;
		}
		return http_get(argv[2]);// 함수 호출
		
	}
	// 사용자가 tcp-check 명령을 입력
	else if(strcmp(argv[1], "tcp-check") == 0)
	{
		if(argc != 4) {//인자 부족
			printf("Usage: %s tcp-check <host> <port> \n", argv[0]);
			return 1;
		}
		return tcp_check(argv[2], atoi(argv[3]));//함수 호출
	}
	else 
	{
		printf("Unknown command: %s\n", argv[1]);
		return 1;
	}

	return 0;
}

int http_get(const char *url) {
	struct addrinfo hints, *res; //DNS 조회결과 구조체
	int sockfd;
	char request[1024];	//http 요청 메시지
	char response[4096];	//서버 응답 저장
	
	// 1. DNS 조회를 위한 힌트 초기화
	memset(&hints, 0, sizeof(hints)); 
	hints.ai_family = AF_INET; //IPv4 사용
	hints.ai_socktype = SOCK_STREAM; //tcp 소켓 
					
	

	// 2. URL 을 IP주소로 변환 (80 포트는 기본) 
	if(getaddrinfo(url, "80", &hints, &res) != 0) {
		perror("getaddrinfo");
		return 1;
	}

	
	// 3. 소켓 생성
	sockfd = socket(res->ai_family, res->ai_socktype,res->ai_protocol);
	if(sockfd < 0) //실패
	{
		perror("socket");
		freeaddrinfo(res);
		return 1;
	}


	// 4. 서버 연결
	if(connect(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
		perror("connect");
		close(sockfd);
		freeaddrinfo(res); //DNS 결과 메모리해제
		return 1;
	}


	// 5. GET 요청 메시지 작성
	sprintf(request,
			"GET / HTTP/1.1\r\n"
			"Host : %s \r\n"
			"Connection: close\r\n\r\n",
			url);

	//요청 전송
	send(sockfd, request, strlen(request), 0);

	// 응답 수신
	while(recv(sockfd, response, sizeof(response) -1 , 0)) {
			printf("%s", response);
	}

	close(sockfd);
	freeaddrinfo(res);
	return 0;
}


int tcp_check(const char *host, int port){
	struct addrinfo hints, *res;
	int sockfd;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;


	//DNS 조회
	if(getaddrinfo(host, NULL, &hints, &res) != 0) {
		perror("getaddrinfo");
		return 1;
	}

	//포트 설정 (사용자 입력)
	((struct sockaddr_in *)res->ai_addr)->sin_port = htons(port); // 네트워크 바이트 순서로 변환
								   //소켓 생성
	 sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	 if(sockfd < 0) {
		 perror("socket");
		 freeaddrinfo(res);
		 return 1;
	 }

	 // 5. 서버 연결 시도
	 if(connect(sockfd, res->ai_addr, res->ai_addrlen)== 0){
		printf("Server is up!\n");
	 }
	 else{
		 printf("Server is down!\n");
	 }

	 close(sockfd);
	 freeaddrinfo(res);
	 return 0;
}



