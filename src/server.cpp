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
using namespace std;

static int newsockfd;


struct node {
	string careername;
	int startPay;
	int endPay;
	node *next = NULL;
};

//linked list to store career information
class list {
	private: node *head, *tail;

	public:	list() {
		head = NULL;
		tail = NULL;
	}

	//add to list
	void addtoList(string cN, string sP, string eP) {
		node *A = new node;
		A->careername = cN;
		A->startPay = stoi(sP);
		A->endPay = stoi(eP);

		if (head == NULL) {
			head = A;
			tail = A;
		}
		else {
			tail->next = A;
			tail = A;
		}
	}

	//linear search for career name
	bool careerSearch(string name, int &sP, int &eP) {
		node *temp = new node;
		temp = head;

		while (temp != NULL) {
			if (name.compare(temp->careername) == 0) {
				sP = temp->startPay;
				eP = temp->endPay;
				return true;
			}
			temp = temp->next;
		}

		return false;
	}
	//display function (used for testing)
	void display() {
		node *temp = new node;
		temp = head;

		while (temp != NULL) {
			cout << temp->careername << " " << temp->startPay << " " << temp->endPay << endl;
			cout << endl;
			temp = temp->next;
		}
	}
};

list careers;

//functions to read and respond to the client
void clientFunc() {
	char buffer[256];
	bzero(buffer, 256);
	int n = 0;

	while (true) {
		bzero(buffer, 256);
		n = read(newsockfd, buffer, 255);
		if (n < 0) {
			cout << "Error reading from socket\n";
			close(newsockfd);
			break;
		}

		string container = string(buffer);
		if (container.empty())
			break;
		container.pop_back();

		int a = -1, b = -1;
		careers.careerSearch(container, a, b);
		sprintf(buffer, "%d %d", a, b);

		n = write(newsockfd, buffer, 255);
		if (n < 0) {
			cout << "Error writing to socket";
			close(newsockfd);
			break;
		}
	}
	close(newsockfd);
}

//socket code
void socketOpen() {
	int sockfd, portno = -1;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;

	cout << "Enter server port number: ";
	cin >> portno;

	if (portno == -1) {
		cout << "Port number not entered\n";
		exit(1);
	}

	if ((portno < 3000) || (portno > 65535)) {
		cout << "Port number must be between 3000 and 65535" << endl;
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		cout << "Error opening socket";
		exit(1);
	}

	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		cout << "Error on binding\n";
		exit(1);
	}


	//loop to listen for connections
	listen(sockfd, 3);
	while (true) {
		socklen_t len = sizeof(cli_addr);
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0)
		{
			cout << "Error on accept";
			exit(1);
		}
		clientFunc();
	}

}

int main() {

	//read file
	ifstream myFile;
	string fileName = "";
	cout << "Enter file name: ";
	getline(cin, fileName);
	myFile.open(fileName);

	if (!myFile) {
		cout << "File not found\n";
		exit(1);
	}

	stringstream bufferSS;
	string bufferHolder;
	string token;
	string careerName;
	string startP;
	string endP;
	
	//parse file and add CAREER, START PAY, and END PAY to list
	while (getline(myFile, bufferHolder)) {
		bufferSS << bufferHolder;

		getline(bufferSS, token, '\t');
		careerName = token;
		getline(bufferSS, token, '\t');
		startP = token;
		getline(bufferSS, token, '\t');
		endP = token;

		bufferSS.clear();
		careers.addtoList(careerName, startP, endP);
	}
	myFile.close();
	socketOpen();

	return 0;
}

