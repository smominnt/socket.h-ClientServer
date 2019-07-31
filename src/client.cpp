#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <strings.h>
#include <unistd.h>
#include <cstring>

using namespace std;

int sockfd, portno, n;
struct sockaddr_in serv_addr;
struct hostent *server;
char host[255];

//socket connection code
void connectSocket() {
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		cout << "Error whilst opening socket\n";
		exit(1);
	}

	server = gethostbyname(host);
	if (server == NULL) {
		cout << "Host does not exist\n";
		exit(1);
	}

	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);

	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		cout << "Error on connecting\n";
		exit(1);
	}
}

int main()
{

	cout << "Enter server host name: ";
	cin >> host;
	cout << "Enter server port number: ";
	cin >> portno;
	//port ranges
	if ((portno < 3000) || (portno > 65535)) {
		cout << "Port number must be between 3000 and 65535" << endl;
		exit(1);
	}

	//attempt connection and close
	connectSocket();
	close(sockfd);
	
	while ((getchar()) != '\n');

	char buffer[256];
	stringstream recvbuffer;
	char careerEntry[256];
	string token;
	string payS;
	string payE;

	//loop to send requests until empty string is entered
	while (true) {
		cout << "Enter a college major: ";
		bzero(buffer, 256);
		fgets(buffer, 255, stdin);
		connectSocket();		//requests a new connection
		strcpy(careerEntry, buffer);
		careerEntry[strlen(careerEntry) - 1] = '\0';

		if (buffer[0] == '\n' && buffer[1] == '\0') //check to see if empty string is entered
			break;

		n = write(sockfd, buffer, strlen(buffer));
		if (n < 0) {
			cout << "Error writing to socket\n";
			exit(1);
		}


		bzero(buffer, 256);
		n = read(sockfd, buffer, 255);
		if (n < 0) {
			cout << "Error reading from socket\n";
			exit(1);
		}

		//put received data into stringstream and parse it
		recvbuffer << buffer;
		getline(recvbuffer, token, ' ');
		payS = token;
		getline(recvbuffer, token, ' ');
		payE = token;

		recvbuffer.clear();
		
		if (!payS.compare("-1") == 0) {	//if pay received does not return a -1
			cout << "The average early career pay for a " << careerEntry << " major is $" << payS << endl;
			cout << "The corresponding mid-career pay is $" << payE << endl;
			cout << endl;
		}
		else {
			cout << "That major is not in the table\n";
			cout << endl;
		}

		close(sockfd);		//closes connection after request completed
	}

	return 0;
}
