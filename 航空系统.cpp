#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>
#include <algorithm>
#include <limits>
#include <iomanip>
#include <stdio.h>
#include <stdexcept>
#include <regex>
#include <unordered_set>
#include <functional>


using namespace std;
/*
���²�����ǰ���ʵ��ʵ�ֵĹ���
*/
struct Flight {
    int flightID;
    string departureDate;
    string intlDome;
    string flightNo;
    int departureAirportID;
    int arrivalAirportID;
    string departureTime;
    string arrivalTime;
    int airplaneID;
    int airplaneModel;
    int airfares;
    int peakSeasonRates;
    int offSeasonRates;
    bool cheapest = false;
    bool shortestDuration = false;
    bool overnight = false;
    bool suspended = false;
    
};

struct FlightNode {
    Flight flight;
    FlightNode* next;

    FlightNode(const Flight& f) : flight(f), next(nullptr) {}
};

//��ȡ�ļ�����ʼ������ 
FlightNode* readFlightsFromCSV(const string& filename, int* entryNum) {
    ifstream file(filename);
   
    if (!file.is_open()) {
        cerr << "�޷����ļ�: " << filename << endl;
        return nullptr;
    }
    
    string line;
    getline(file, line);  // ���� CSV �ı�����
    
    FlightNode* head = nullptr;  // ��ʼΪ��
    FlightNode* tail = nullptr;  // ����׷������ĩβ�ڵ�
    
    while (getline(file, line)) {
        istringstream ss(line);
        Flight flight;
        (*entryNum)++;
        
        // ��ȡÿ���ֶΣ�ʹ��','�ָ�
        string token;
        if (getline(ss, token, ',')) flight.flightID = std::stoi(token);
        if (getline(ss, token, ',')) flight.departureDate = token;
        if (getline(ss, token, ',')) flight.intlDome = token;
        if (getline(ss, token, ',')) flight.flightNo = token;
        if (getline(ss, token, ',')) flight.departureAirportID = std::stoi(token);
        if (getline(ss, token, ',')) flight.arrivalAirportID = std::stoi(token);
        if (getline(ss, token, ',')) flight.departureTime = token;
        if (getline(ss, token, ',')) flight.arrivalTime = token;
        if (getline(ss, token, ',')) flight.airplaneID = std::stoi(token);
        if (getline(ss, token, ',')) flight.airplaneModel = std::stoi(token);
        if (getline(ss, token, ',')) flight.airfares = std::stoi(token);
        if (getline(ss, token, ',')) flight.peakSeasonRates = std::stoi(token);
        if (getline(ss, token, ',')) flight.offSeasonRates = std::stoi(token);

        // �����½ڵ㲢��������
        FlightNode* newNode = new FlightNode(flight);
        if (tail == nullptr) {  // ����Ϊ��
            head = newNode;
        } else {
            tail->next = newNode;
        }
        tail = newNode;
    }

    file.close();
    return head;  // ��������ͷ�ڵ�
}

double calculateFlightDuration(const std::string& departureTime, const std::string& arrivalTime,int ID) {
	
	double arr,dep;
	sscanf(departureTime.c_str(), "%lf", &dep);
    sscanf(arrivalTime.c_str(), "%lf", &arr);
    /*cout << ID << " " ;*/

    return arr - dep ; 
}

FlightNode* insertSortedByDepartureAirportID(FlightNode* head, FlightNode* newNode) {
    if (!head || (newNode->flight.departureAirportID > head->flight.departureAirportID)) {
        newNode->next = head;
        return newNode;
    }

    FlightNode* current = head;
    while (current->next && 
           ((newNode->flight.departureAirportID < current->next->flight.departureAirportID))) {
        current = current->next;
    }
    newNode->next = current->next;
    current->next = newNode;
    
    return head;
}

FlightNode* insertSortedByArrivalAirportID(FlightNode* head, FlightNode* newNode) {
    if (!head || (newNode->flight.departureAirportID > head->flight.departureAirportID) ||
        (newNode->flight.departureAirportID == head->flight.departureAirportID && 
         newNode->flight.arrivalAirportID > head->flight.arrivalAirportID) ) {
        newNode->next = head;
        return newNode;
    }

    FlightNode* current = head;
    while (current->next && 
           ((newNode->flight.departureAirportID < current->next->flight.departureAirportID) ||
            (newNode->flight.departureAirportID == current->next->flight.departureAirportID && 
             newNode->flight.arrivalAirportID < current->next->flight.arrivalAirportID) )) {
        current = current->next;
    }
    newNode->next = current->next;
    current->next = newNode;
    
    return head;
}

FlightNode* insertSortedByFlightDuration(FlightNode* head, FlightNode* newNode) {
    if (!head || (newNode->flight.departureAirportID > head->flight.departureAirportID) ||
        (newNode->flight.departureAirportID == head->flight.departureAirportID && 
         newNode->flight.arrivalAirportID > head->flight.arrivalAirportID) ||
        (newNode->flight.departureAirportID == head->flight.departureAirportID &&
         newNode->flight.arrivalAirportID == head->flight.arrivalAirportID &&
         calculateFlightDuration(newNode->flight.departureTime, newNode->flight.arrivalTime,newNode->flight.flightID) <= 
         calculateFlightDuration(head->flight.departureTime, head->flight.arrivalTime, head->flight.flightID))) {
        newNode->next = head;
        return newNode;
    }

    FlightNode* current = head;
    while (current->next && 
           ((newNode->flight.departureAirportID < current->next->flight.departureAirportID) ||
            (newNode->flight.departureAirportID == current->next->flight.departureAirportID && 
             newNode->flight.arrivalAirportID < current->next->flight.arrivalAirportID) ||
            (newNode->flight.departureAirportID == current->next->flight.departureAirportID &&
             newNode->flight.arrivalAirportID == current->next->flight.arrivalAirportID &&
             calculateFlightDuration(newNode->flight.departureTime, newNode->flight.arrivalTime,newNode->flight.flightID) >
             calculateFlightDuration(current->next->flight.departureTime, current->next->flight.arrivalTime, current->next->flight.flightID)))) {
        current = current->next;
    }
    newNode->next = current->next;
    current->next = newNode;
    
    return head;
}


FlightNode* sortFlightsByDepartureAirportID(FlightNode* head) {
    FlightNode* sorted = nullptr;
    FlightNode* current = head;

    while (current) {
        FlightNode* next = current->next;
        sorted = insertSortedByDepartureAirportID(sorted, current);
        current = next;
    }

    return sorted;
}

FlightNode* sortFlightsByArrivalAirportID(FlightNode* head) {
    FlightNode* sorted = nullptr;
    FlightNode* current = head;

    while (current) {
        FlightNode* next = current->next;
        sorted = insertSortedByArrivalAirportID(sorted, current);
        current = next;
    }

    return sorted;
}

FlightNode* sortFlightsByFlightDuration(FlightNode* head) {
    FlightNode* sorted = nullptr;
    FlightNode* current = head;

    while (current) {
        FlightNode* next = current->next;
        sorted = insertSortedByFlightDuration(sorted, current);
        current = next;
    }

    return sorted;
}


void printFlights(FlightNode* head) {
    FlightNode* current = head;
    while (current) {
        std::cout << "����ID: " << current->flight.flightID 
                  << "    ��������: " << current->flight.departureAirportID 
                  << "    �ִ����: " << current->flight.arrivalAirportID 
                  << "    ����ʱ��:" << current->flight.departureTime 
                  << "    �ִ�ʱ��:" << current->flight.arrivalTime 
                  
                  << std::endl;
        current = current->next;
    }
    
    cout << "�Ƿ�鿴��ϸ������Ϣ��(1��ʾ���ǡ���0��ʾ����):" << endl;
    int choice = 1;
    cin >> choice;
	if (choice == 1)
	{
		FlightNode* current = head;
	    while (current) {
	        std::cout << "����ID: " << current->flight.flightID 
	                  << "    ��������: " << current->flight.departureDate 
	                  << "    ��������: " << current->flight.intlDome
	                  << "    ������: " << current->flight.flightNo
	                  << "    ��������: " << current->flight.departureAirportID
	                  << "    �ִ����: " << current->flight.arrivalAirportID 
	                  << "    ����ʱ��:" << current->flight.departureTime 
	                  << "    �ִ�ʱ��:" << current->flight.arrivalTime 
	                  << "    �ɻ�ID:" << current->flight.airplaneID
	                  << "    �ɻ�����: " << current->flight.airplaneModel
	                  << "    ��������: " << current->flight.airfares
	                  << "    �����۸�: " << current->flight.peakSeasonRates
	                  << "    �����۸�: " << current->flight.offSeasonRates
	                  
	                  << std::endl;
	        current = current->next;
    	}
	}
    
}

bool isPrime(int num) {
    if (num < 2) return false;
    for (int i = 2; i * i <= num; ++i) {
        if (num % i == 0) return false;
    }
    return true;
}

void findFlightsWithPrimeFares(FlightNode* head) {
    cout << "��������Ϊ�����ĺ���ID: " << endl ;
    FlightNode* current = head;
    while(current != nullptr)
    {
    	if(isPrime(current->flight.airfares))
    	{
    		cout << current->flight.flightID << endl;
		}
		current = current->next;
	}
    cout << endl;
}

void findSaddlePoint(FlightNode* head, int departureID, int arrivalID) {
    // ������ά���󣨻������á������۸񡢵����۸�
    vector<vector<int>> matrix;
    FlightNode* current = head;
	while(current != nullptr) {
        if (current->flight.departureAirportID == departureID &&
            current->flight.arrivalAirportID == arrivalID) {
            matrix.push_back({
            	current->flight.flightID,
            	current->flight.airfares,
            	current->flight.peakSeasonRates,
            	current->flight.offSeasonRates
            });
        }
        current = current->next;
    }
    
    cout << "���ɵľ���:" << endl; 
	for (int i = 0; i < matrix.size(); i ++)
	{
		for(int j = 0; j < matrix[0].size(); j ++)
		{
			cout << matrix[i][j] << " ";
		}
		cout << endl;
	}
	
    // ��鰰��
    for (int i = 0; static_cast<unsigned int>(i) < matrix.size(); i++) {
        // �ҵ���ǰ�е����ֵ����������
        int rowMax = *max_element(matrix[i].begin()+1, matrix[i].end());
        int maxIndex = max_element(matrix[i].begin(), matrix[i].end()) - matrix[i].begin();

        // ���������ֵ�����У��Ƿ�Ϊ���е���Сֵ
        bool isSaddlePoint = true;
        for (int j = 0; static_cast<unsigned int>(j) < matrix.size(); j++) {
            if (matrix[j][maxIndex] < rowMax) {  // ����ȷ����������ֵ�����ڵ��ڵ�ǰ�����ֵ
                isSaddlePoint = false;
                break;
            }
        }

        // ����ҵ����㣬��ӡ��Ӧ����ID
        if (isSaddlePoint) {
            cout << "���㺽��ID: " << matrix[i][0] << endl;
            return;  // �ҵ�����󷵻�
        }
    }

    // ���û���ҵ����㣬��ӡ��ʾ��Ϣ
    cout << "û���ҵ����㡣" << endl;
}

FlightNode* createFlightListByAirplaneID(FlightNode* head, int airplaneID) {
    FlightNode* head0 = nullptr;
    FlightNode* tail = nullptr;
    FlightNode* current = head;
    
    // ���ȼ������ķɻ�ID�Ƿ�Ϸ�
    bool validID = false;
    while(current != nullptr) {
        if (current->flight.airplaneID == airplaneID) {
            validID = true;
            break;
        }
        current = current->next;
    }

    if (!validID) {
        cout << "����ķɻ�ID��Ч��" << endl;
        return nullptr;
    }

    // ����һ����ʱ�б����ڴ洢���ڸ÷ɻ�ID�ĺ���
    {
    	if (current->flight.airplaneID == airplaneID)
    	{
    		FlightNode* newNode = new FlightNode(current->flight);
			if (tail == nullptr) {  // ����Ϊ��
            head = newNode;
        } else {
            tail->next = newNode;
        }
        tail = newNode;
		}
		current = current->next;
	}
	return head0;
}


FlightNode* deleteFlightsByAirplaneModel(FlightNode* head, int airplaneModel, FlightNode*& deletedListHead) {
    deletedListHead = nullptr;
    FlightNode* tail = nullptr;
    FlightNode* current = head;
    FlightNode* previous = nullptr;
    
    cout << "ɾ���ĺ���ID��" << endl; 
    while(current != nullptr) {
        if (current->flight.airplaneModel == airplaneModel) {
            // �����½ڵ㲢���ӵ�����
            FlightNode* newNode = new FlightNode(current->flight);

            if (!deletedListHead) {
                deletedListHead = newNode;
                tail = newNode;
            } else {
                tail->next = newNode;
                tail = newNode;
            }
            
            cout << current->flight.flightID<< endl;

            if (previous) {
                previous->next = current->next; 
            } else {
                head = current->next; // ɾ������ͷ�ڵ�
            }
            
            // �ͷŵ�ǰ�ڵ��ڴ沢�ƶ�����һ���ڵ�
            FlightNode* temp = current;
            current = current->next; // �ƶ�����һ���ڵ�
            delete temp; // �ͷ��ڴ�
	        } else {
	            previous = current; // ����ǰһ���ڵ�
	            current = current->next;
	        }
    }
    return deletedListHead;

}

bool insertFlight(FlightNode*& head, const string& input) {
    istringstream ss(input);
    Flight flight;
    string token;

    // Count the number of fields
    int fieldCount = 0;

    // Parse input and check for validity
    while (getline(ss, token, ',')) {
        fieldCount++;
        switch (fieldCount) {
            case 1:
                flight.flightID = stoi(token);               
                break;
            case 2:
                flight.departureDate = token;
                break;
            case 3:
                flight.intlDome = token;  // intlDome is expected here
                break;
            case 4:
            	flight.flightNo = token;
            	break;
			case 5:
                flight.departureAirportID = stoi(token);
                break;
            case 6:
                flight.arrivalAirportID = stoi(token);
                break;
            case 7:
                flight.departureTime = token;
                break;
            case 8:
                flight.arrivalTime = token;
                break;
            case 9:
                flight.airplaneID = stoi(token);
                break;
            case 10:
                flight.airplaneModel = stoi(token);
                break;
            case 11:
                flight.airfares = stoi(token);
                break;
            	
            default:
                cout << "Input format error, please input again." << endl;
                return false;
        }

        
    }

    // Check if the expected number of fields was provided
    if (fieldCount != 11) {
        cout << "Input format error, please input again." << endl;
        return false;
    }

    // Check time validity
    if (flight.departureTime >= flight.arrivalTime) {
        cout << "Departure time is later than arrival time, please input again." << endl;
        return false;
    }

    // Add to the linked list
    FlightNode* newNode = new FlightNode(flight);
    if (head == nullptr) {
        head = newNode;
    } else {
        FlightNode* tail = head;
        while (tail->next) {
            tail = tail->next;
        }
        tail->next = newNode;
    }

    cout << "Insert successfully." << endl;
    return true;
}

bool deleteFlight(FlightNode* head, int flightID) {
    FlightNode* current = head;
    FlightNode* previous = nullptr;

    while (current) {
        if (current->flight.flightID == flightID) {
            if (previous) {
                previous->next = current->next;
            } else {
                head = current->next; // ɾ��ͷ�ڵ�
            }
            delete current;
            cout << "Delete successfully." << endl;
            return true;
        }
        previous = current;
        current = current->next;
    }

    cout << "No such flight, please input again." << endl;
    return false;
}

bool updateFlight(FlightNode* head, int flightID, const string& input) {
    FlightNode* current = head;

    while (current) {
        if (current->flight.flightID == flightID) {
            // ����������
            istringstream ss(input);
            string token;
			
            // �����������ֶ�
            if (getline (ss,token,','));           
            if (getline(ss, token, ',')) current->flight.departureDate = token;
            if (getline(ss, token, ',')) current->flight.intlDome = token;
            if (getline(ss, token, ',')) current->flight.flightNo = token;
            if (getline(ss, token, ',')) current->flight.departureAirportID = stoi(token);
            if (getline(ss, token, ',')) current->flight.arrivalAirportID = stoi(token);
            if (getline(ss, token, ',')) current->flight.departureTime = token;
            if (getline(ss, token, ',')) current->flight.arrivalTime = token;
            if (getline(ss, token, ',')) current->flight.airplaneID = stoi(token);
            if (getline(ss, token, ',')) current->flight.airplaneModel = stoi(token);
            if (getline(ss, token, ',')) current->flight.airfares = stoi(token);

            current->flight.flightID = flightID;


            // ���Ϸ���
            if (current->flight.departureTime >= current->flight.arrivalTime) {
                cout << "Departure time is later than arrival time, please input again." << endl;
                return false;
            }

            cout << "Update successfully." << endl;
            return true;
        }
        current = current->next;
    }

    cout << "No such flight, please input again." << endl;
    return false;
}

void findCheapest(FlightNode* head) {
    if (head == nullptr) return; // Check for an empty list

    FlightNode* current = head;
    int lowestFare = head->flight.airfares; // Initialize lowestFare with the first flight's airfare

    // First pass: Find the lowest fare
    while (current != nullptr) {
        if (current->flight.airfares < lowestFare) {
            lowestFare = current->flight.airfares; // Update the lowest fare
        }
        current = current->next;
    }

    // Second pass: Reset and mark all flights with the lowest fare
    current = head;
    while (current != nullptr) {
        // Reset the cheapest flags for all flights
        current->flight.cheapest = 0; // Reset cheapest flags

        // Check and mark flights with the lowest fare
        if (current->flight.airfares == lowestFare) {
            current->flight.cheapest = 1; // Set the cheapest flag
        }

        current = current->next;
    }
}

int calculateDuration(const string& departure, const string& arrival) {
    std::tm depTime = {};
    std::tm arrTime = {};

    // Parse departure time
    sscanf(departure.c_str(), "%d/%d/%d %d:%d", &depTime.tm_mon, &depTime.tm_mday, &depTime.tm_year, &depTime.tm_hour, &depTime.tm_min);
    depTime.tm_year -= 1900; // Adjust year for std::tm
    depTime.tm_mon -= 1; // Adjust month for std::tm

    // Parse arrival time
    sscanf(arrival.c_str(), "%d/%d/%d %d:%d", &arrTime.tm_mon, &arrTime.tm_mday, &arrTime.tm_year, &arrTime.tm_hour, &arrTime.tm_min);
    arrTime.tm_year -= 1900; // Adjust year for std::tm
    arrTime.tm_mon -= 1; // Adjust month for std::tm

    // Convert to time_t for duration calculation
    time_t depEpoch = std::mktime(&depTime);
    time_t arrEpoch = std::mktime(&arrTime);

    // Calculate the duration in minutes
    double durationInSeconds = difftime(arrEpoch, depEpoch);
    return static_cast<int>(durationInSeconds / 60); // Convert seconds to minutes
}

void findShortestDuration(FlightNode* head) {
    if (head == nullptr) return;

    FlightNode* current = head;
    int shortestDuration = calculateDuration(head->flight.departureTime, head->flight.arrivalTime);
    
    // First pass: Find the shortest duration
    while (current != nullptr) {
        int currentDuration = calculateDuration(current->flight.departureTime, current->flight.arrivalTime);
        if (currentDuration < shortestDuration) {
            shortestDuration = currentDuration;
        }
        current = current->next;
    }

    // Second pass: Reset and mark all flights with the shortest duration
    current = head;
    while (current != nullptr) {
        // Reset the shortest duration flag for all flights
        current->flight.shortestDuration = false;

        // Check and mark flights with the shortest duration
        int currentDuration = calculateDuration(current->flight.departureTime, current->flight.arrivalTime);
        if (currentDuration == shortestDuration) {
            current->flight.shortestDuration = true;
        }

        current = current->next;
    }
}

bool isOvernight(const std::string& departure, const std::string& arrival) {
    // ��ȡ���ڲ��֣������ʽʼ��Ϊ "MM/DD/YYYY HH:MM"��
    std::string depDate = departure.substr(0, departure.find(' '));
    std::string arrDate = arrival.substr(0, arrival.find(' '));

    // ����������ں͵������ڲ�ͬ����Ϊ��ҹ����
    return depDate != arrDate;
}

void findOvernight(FlightNode* head) {
    if (head == nullptr) return;

    FlightNode* current = head;
    while (current != nullptr) {
        current->flight.overnight = isOvernight(current->flight.departureTime, current->flight.arrivalTime);
        current = current->next;
    }
}

void showFlights(FlightNode* head, int departureAirportID, int arrivalAirportID) {
    FlightNode* current = head;
    FlightNode* newHead = nullptr;
    FlightNode* newTail = nullptr;
    bool found = false;

    while (current != nullptr) {
        if (current->flight.departureAirportID == departureAirportID &&
            current->flight.arrivalAirportID == arrivalAirportID && current->flight.suspended == false) {
            FlightNode* newNode = new FlightNode(current->flight);
            if (newHead == nullptr) {
                newHead = newNode;
                newTail = newNode;
            } else {
                newTail->next = newNode;
                newTail = newNode;
            }
            found = true;
        }
        current = current->next;
    }

    if (newHead != nullptr) {
        findCheapest(newHead);
        findShortestDuration(newHead);
        findOvernight(newHead);
    }

    current = newHead;
    while (current != nullptr) {
        cout << "Flight ID: " << current->flight.flightID;
        if (current->flight.cheapest) cout << " (Cheapest)";
        if (current->flight.shortestDuration) cout << " (Shortest Duration)";
        if (current->flight.overnight) cout << " (+1 day)";
        cout << endl;
        current = current->next;
    }

    if (!found) {
        cout << "No direct flight." << endl;
    }

    current = newHead;
    while (current != nullptr) {
        FlightNode* temp = current;
        current = current->next;
        delete temp;
    }
}

void suspendFlights(FlightNode*& head, int airportID) {
    FlightNode* current = head;
    while (current != nullptr) {
        if (current->flight.departureAirportID == airportID || current->flight.arrivalAirportID == airportID) {
            current->flight.suspended = true;
        }
        current = current->next;
    }
    cout << "Suspend successfully." << endl;
}

void reactivateFlights(FlightNode*& head, int airportID) {
    FlightNode* current = head;
    while (current != nullptr) {
        if (current->flight.departureAirportID == airportID || current->flight.arrivalAirportID == airportID) {
            current->flight.suspended = false;
        }
        current = current->next;
    }
    cout << "Reactivate successfully." << endl;
}

FlightNode* processingReviseCommand(FlightNode* head)
{
	string command;
	while (true) {
        cout << "���������" << endl;
        getline(cin, command);

        istringstream iss(command);
        string action;
        getline(iss, action, ' '); // ��ȡ����

        if (action == "insert") {
            string input;
            getline(iss, input); // ��ȡ��������
            insertFlight(head, input);
        } else if (action == "delete") {
            int flightID;
            iss >> flightID; // ��ȡ���� ID
            deleteFlight(head, flightID);
        } else if (action == "update") {
            int flightID;
            string input;
            iss >> flightID; // ��ȡ���� ID
            getline(iss, input); // ��ȡ��������
            updateFlight(head, flightID, input);
        } else if (action == "show") {
            int departureID, arrivalID;
            iss >> departureID >> arrivalID; // ��ȡ�𽵻��� ID
            showFlights(head, departureID, arrivalID);
        }else if (action == "Suspend") {
            int airportID;
            iss >> airportID;
            suspendFlights(head, airportID);
        } else if (action == "Reactivate") {
            int airportID;
            iss >> airportID;
            reactivateFlights(head, airportID);
        } else if (action == "show") {
            int airportID1, airportID2;
            iss >> airportID1 >> airportID2;
            showFlights(head, airportID1, airportID2);
        }else if (action == "exit") {
                break; // �˳�ѭ��
        } else {
            cout << "Invalid command." << endl;
        }
    }
    return head;
}

void processBatchCommands(FlightNode*& head, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Cannot open file: " << filename << endl;
        return;
    }

    string line;
    int successfulInserts = 0, failedInserts = 0;
    int successfulUpdates = 0, failedUpdates = 0;
    int successfulDeletes = 0;

    while (getline(file, line)) {
        istringstream ss(line);
        string action;
        getline(ss, action, ',');

        if (action == "insert") {
            string input;
            getline(ss, input); // get the rest of the line as input
            if (insertFlight(head, input)) {
                successfulInserts++;
            } else {
                failedInserts++;
            }
        } else if (action == "delete") {
            int flightID;
            ss >> flightID;
            if (deleteFlight(head, flightID)) {
                successfulDeletes++;
            }
        } else if (action == "update") {
            int flightID;
            string input;
            ss >> flightID;
            getline(ss, input); // get the rest of the line as input
            if (updateFlight(head, flightID, input)) {
                successfulUpdates++;
            } else {
                failedUpdates++;
            }
        }
    }

    file.close();
    cout << "Successfully deleted " << successfulDeletes << " record(s);" << endl;
    cout << "Successfully inserted " << successfulInserts << " record(s), failed to insert " << failedInserts << " record(s);" << endl;
    cout << "Successfully updated " << successfulUpdates << " record(s), failed to update " << failedUpdates << " record(s)." << endl;
}

void findMaxConnections(FlightNode* head, int startFlightID) {
    vector<vector<int>> longestPaths;
    vector<int> currentPath;
    unordered_set<int> visited;
    int maxFlightCount = 0;
    
    // Helper function to find paths recursively
    function<void(int)> dfs = [&](int flightID) {
        if (visited.count(flightID)) return;
        
        visited.insert(flightID);
        currentPath.push_back(flightID);

        // Check if this is a leaf node (no more flights to connect)
        bool hasNext = false;

        FlightNode* current = head;
        while (current != nullptr) {
            if (current->flight.flightID == flightID) {
                FlightNode* next = head;
                while (next != nullptr) {
                    if (next->flight.departureAirportID == current->flight.arrivalAirportID && 
                        !next->flight.suspended) {
                        // Check the transfer time constraint (30 minutes)
                        int duration = calculateDuration(current->flight.arrivalTime, next->flight.departureTime);
                        if (duration >= 30) {
                            hasNext = true; // We found a valid next flight
                            dfs(next->flight.flightID); // Recur with the next flight
                        }
                    }
                    next = next->next;
                }
                break;
            }
            current = current->next;
        }

        // If we reached the end of a path, check if it's the longest
        if (!hasNext) {
            int pathLength = currentPath.size();
            if (pathLength > maxFlightCount) {
                maxFlightCount = pathLength;
                longestPaths.clear(); // New max found, clear previous longest paths
                longestPaths.push_back(currentPath);
            } else if (pathLength == maxFlightCount) {
                longestPaths.push_back(currentPath); // Same length as max, add to longest paths
            }
        }

        currentPath.pop_back();
        visited.erase(flightID);
    };

    // Start DFS from the specified flight ID
    dfs(startFlightID);

    // Output the results
    cout << "The maximum number of flights is " << maxFlightCount  << ", all paths are as follow:" << endl;
    for (const auto& path : longestPaths) {
        for (size_t i = 0; i < path.size(); ++i) {
            cout << path[i];
            if (i < path.size() - 1) cout << " -> ";
        }
        cout << endl;
    }

    if (longestPaths.empty()) {
        cout << "No paths found." << endl;
    }
}

/*
������������ʵ��
*/

struct Airport {
    int id;
    char country[50];
    char province[50];
    char name[100];
};

#define MAX_AIRPORTS 100
#define MAX_CHILDREN 100  // ÿ���ڵ������ӽڵ���
#define MAX_NAME_LENGTH 100  // �����ַ�������󳤶�
#define MAX_RESULTS 1000

Airport airports[MAX_AIRPORTS];
int airportCount = 0;

// ��ȡ��������
void readAirportData(Airport* airports) {
    FILE* file = fopen("id2name.csv", "r");
    if (!file) return;

    char line[256];
    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file)) {
        airportCount++;
        int id;
        char info[150];
        sscanf(line, "%d,%[^\n]", &id, info);

        // Parse country, province, name
        char* token = strtok(info, "/");
        strcpy(airports[airportCount].country, token);

        token = strtok(NULL, "/");
        strcpy(airports[airportCount].province, token);

        token = strtok(NULL, "/");
        strcpy(airports[airportCount].name, token);

        airports[airportCount].id = id;
    }

    fclose(file);
}

// ���ڵ�ṹ
struct TreeNode {
    char name[MAX_NAME_LENGTH];            // �ڵ����ƣ����ҡ�ʡ�ݻ��������
    TreeNode* children[MAX_CHILDREN];      // �ӽڵ�����
    int childCount;                        // ��ǰ�ӽڵ�����

    // ���캯��
    TreeNode(const char* nodeName) {
        strncpy(name, nodeName, MAX_NAME_LENGTH);
        childCount = 0;
        for (int i = 0; i < MAX_CHILDREN; ++i) {
            children[i] = NULL;
        }
    }
};

// �ڽڵ���ӽڵ��в�������ΪnodeName�Ľڵ�
TreeNode* findChild(TreeNode* node, const char* nodeName) {
    for (int i = 0; i < node->childCount; ++i) {
        if (strcmp(node->children[i]->name, nodeName) == 0) {
            return node->children[i];
        }
    }
    return NULL;
}

// �����ӽڵ�
TreeNode* addChild(TreeNode* parent, const char* childName) {
    if (parent->childCount >= MAX_CHILDREN) {
        printf("�ӽڵ������������ơ�\n");
        return NULL;
    }
    TreeNode* child = new TreeNode(childName);
    parent->children[parent->childCount++] = child;
    return child;
}

// �ݹ�ɾ�������ͷ��ڴ�
void deleteTree(TreeNode* node) {
    for (int i = 0; i < node->childCount; ++i) {
        deleteTree(node->children[i]);
    }
    delete node;
}

// ��ȡid2name�ļ���������
void buildTreeFromFile(TreeNode* root)
{
    FILE* file = fopen("id2name.csv", "r");
    if (!file) {
        printf("�޷����ļ� id2name.csv\n");
        return ;
    }

    char line[256];
    fgets(line, sizeof(line), file); // ����������

    while (fgets(line, sizeof(line), file)) {
        if (strlen(line) == 0) continue;

        // ȥ����ĩ�Ļ��з�
        line[strcspn(line, "\r\n")] = 0;

        // ����ID��Name
        char* idStr = strtok(line, ",");
        char* nameStr = strtok(NULL, ",");

        if (!idStr || !nameStr) {
            printf("���ݸ�ʽ����\n");
            continue;
        }

        // �ָ�NameΪ���ҡ�ʡ�ݡ�������
        char* country = strtok(nameStr, "/");
        char* province = strtok(NULL, "/");
        char* airportName = strtok(NULL, "/");

        if (!country || !province || !airportName) {
            cout << "Name�ֶθ�ʽ����" << endl;
            continue;
        }

        // �������ṹ
        // ���һ����ӹ��ҽڵ�
        TreeNode* countryNode = findChild(root, country);
        if (!countryNode) {
            countryNode = addChild(root, country);
        }

        // ���һ�����ʡ�ݽڵ�
        TreeNode* provinceNode = findChild(countryNode, province);
        if (!provinceNode) {
            provinceNode = addChild(countryNode, province);
        }

        // ���ӻ����ڵ�
        TreeNode* airportNode = findChild(provinceNode, airportName);
        if (!airportNode) {
            addChild(provinceNode, airportName);
        } else {
            printf("���棺�ظ��Ļ������� %s\n", airportName);
        }
    }

    fclose(file);
}

// �ַ���תСд
void toLowerCase(char* str) {
    for (; *str; ++str) {
        if (*str >= 'A' && *str <= 'Z') {
            *str = *str + ('a' - 'A');
        }
    }
}

// ����༭���루Levenshtein Distance��
int levenshteinDistance(const char* s, const char* t) {
    int n = strlen(s);
    int m = strlen(t);
    int d[MAX_NAME_LENGTH + 1][MAX_NAME_LENGTH + 1]; // ��������ַ������Ȳ����� MAX_NAME_LENGTH

    // ��ʼ��
    for (int i = 0; i <= n; ++i) d[i][0] = i;
    for (int j = 0; j <= m; ++j) d[0][j] = j;

    // ����
    for (int i = 1; i <= n; ++i) {
        char s_i = s[i - 1];
        for (int j = 1; j <= m; ++j) {
            char t_j = t[j - 1];
            int cost = (tolower(s_i) == tolower(t_j)) ? 0 : 1;
            int deletion = d[i - 1][j] + 1;
            int insertion = d[i][j - 1] + 1;
            int substitution = d[i - 1][j - 1] + cost;
            d[i][j] = deletion < insertion ? (deletion < substitution ? deletion : substitution) : (insertion < substitution ? insertion : substitution);
        }
    }

    return d[n][m];
}

// �������ƶȣ����ڱ༭������й�һ����
double calculateSimilarity(const char* str1, const char* str2) {
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    int maxLength = len1 > len2 ? len1 : len2;

    // ��������ַ������ǿյģ���Ϊ���ƶ�Ϊ 1
    if (maxLength == 0) return 1.0;

    int distance = levenshteinDistance(str1, str2);
    double similarity = 1.0 - (double)distance / maxLength;
    return similarity;
}

// ���ڴ�����������������Ľ��
struct Result {
    char path[3 * MAX_NAME_LENGTH];
    double similarity;
};



/*��������
����1������������ƥ��

����ʵ�֣�

����������ַ�����

1.������:�����ж������ַ����Ƿ�Ϊcountry��province�ڵ㣨�жϱ�׼Ϊ�༭����С��2����

1.1����ǣ����ж����й���ʡ������������country��province�����и��ӽڵ㣨��airportName���������ƶȴӴ�С�������

1.2������ǣ�����������и��ӽڵ��������ַ����������ƶȼ��㲢�����ƶȴӴ�С������������

*/

// ����������������ƥ��������ƣ�
void searchAirports(TreeNode* root, const char* query) {
    Result results[MAX_RESULTS];
    int resultCount = 0;

    // ת����ѯ�ַ���ΪСд
    char queryLower[MAX_NAME_LENGTH];
    strncpy(queryLower, query, MAX_NAME_LENGTH);
    toLowerCase(queryLower);

    struct StackNode {
        TreeNode* node;
        char path[3 * MAX_NAME_LENGTH];
    } stack[MAX_RESULTS];
    int stackSize = 0;

    // ��ʼ��ջ
    stack[0].node = root;
    strcpy(stack[0].path, "");
    stackSize++;

    // ��������/ʡ�ݻ���������������ַ��������ݿ��еĹ���/ʡ�ݵı༭����С��2��
    bool isCountry = false;
    bool isProvince = false;
    for (int i = 0; i < root->childCount; ++i) {
        TreeNode* countryNode = root->children[i];
        if (levenshteinDistance(queryLower, countryNode->name) < 2) {//�༭����С��4ʱ����ʵ�ʲ��Դ���Ϊ��Ҫ���й���/ʡ��ƥ��İ����Ƚ϶࣬���Ը�ΪС��2ʹ��׼�����ϸ�
            isCountry = true;
            for (int j = 0; j < countryNode->childCount; ++j) {
                TreeNode* provinceNode = countryNode->children[j];            
                for (int k = 0; k < provinceNode->childCount; ++k) {                  
                    TreeNode* airportNode = provinceNode->children[k];
                    strcpy(results[resultCount].path, provinceNode->name);
                    strcat(results[resultCount].path, "/");
                    strcat(results[resultCount].path, airportNode->name);
                    results[resultCount].similarity = calculateSimilarity(airportNode->name, query);
                    resultCount++;
                }
                
            }
        }

        for (int j = 0; j < countryNode->childCount; ++j) {
            TreeNode* provinceNode = countryNode->children[j];
            if (levenshteinDistance(queryLower, provinceNode->name) < 4) {
                isProvince = true;
                for (int k = 0; k < provinceNode->childCount; ++k) {
                    TreeNode* airportNode = provinceNode->children[k];
                    strcpy(results[resultCount].path, provinceNode->name);
                    strcat(results[resultCount].path, "/");
                    strcat(results[resultCount].path, airportNode->name);
                    results[resultCount].similarity = calculateSimilarity(airportNode->name, query);
                    resultCount++;
                }
            }
                
        }


    }

    //������������
    if (!(isCountry||isProvince)) {
        while (stackSize > 0) {
            // ����ջ��Ԫ��
            StackNode current = stack[--stackSize];
            TreeNode* node = current.node;
            char currentPath[3 * MAX_NAME_LENGTH];
            strcpy(currentPath, current.path);

            // ����·��
            if (strcmp(node->name, "World") != 0) {
                if (strlen(currentPath) > 0) {
                    strcat(currentPath, "/");
                }
                strcat(currentPath, node->name);
            }

            // �ж��Ƿ�ƥ��
            if (node->childCount == 0) {            
                    strcpy(results[resultCount].path, currentPath);
                    results[resultCount].similarity = calculateSimilarity(node->name, query);
                    resultCount++;
                
            } else {
                for (int i = 0; i < node->childCount; ++i) {
                    if (stackSize < MAX_RESULTS) {
                        stack[stackSize].node = node->children[i];
                        strcpy(stack[stackSize].path, currentPath);
                        stackSize++;
                    }
                }
            }
        }
    }

    // �����ƶ����򣨴Ӹߵ��ͣ�
    std::sort(results, results + resultCount, [](const Result& a, const Result& b) {
        return a.similarity > b.similarity;
    });

    // ������

    if (!(isCountry||isProvince))
    {
        cout << "���ƶ�ǰ10�����������" << endl;
        for (int i = 0;i<10;++i)
        {    
            printf("%s (���ƶ�: %.5f)\n", results[i].path, results[i].similarity);
        }
    }
    else{
    cout << "Ϊ�������˹���/ʡ������������Ϊƥ�䵽�Ĺ���/ʡ�ݵ����л�����" << endl;
    printf("���� '%s' �Ľ����\n", query);
    for (int i = 0; i < resultCount; ++i) {
        printf("%s (���ƶ�: %.5f)\n", results[i].path, results[i].similarity);
    }
    }
    
}

/*
����2����ͬʡ�ݺ����Ƽ�

����ʵ�ָ�������� ���� ���� �Ļ���ID��

1.��ԭʼ������ѯ����
�ܹ����Ȳ�ѯ����ֱ�ﺽ�࣬������е�ֱ�ﺽ��

2.û��ֱ�ﺽ��������
�����в�ѯ�������͵������������ͬʡ�ݵĻ�����Ȼ�������Щ�����������в��Һ��ಢ�����Ƽ���Ҫ������Ƽ��������ﵽ����ĺ��࣬Ȼ���������

3.��������������ȫ�޺���
*/

//��ѯֱ�ﺽ��
bool hasDirectFlight(int departureID, int arrivalID,FlightNode* head,FlightNode* &ans){
    // ������һ��ȫ�ֵĺ�������
    FlightNode* current = head;
    FlightNode* tail = nullptr;

    while (current != nullptr) {
        if (current->flight.departureAirportID == departureID && current->flight.arrivalAirportID == arrivalID) {
            FlightNode* newNode = new FlightNode(current->flight);
			if (tail == nullptr) {  // ����Ϊ��
            ans = newNode;
            } else {
                tail->next = newNode;
            }
            tail = newNode;	
        }
        current = current->next;
    }
    if(ans != nullptr)
    {
        return true;
    }
    return false;
}

//��ѯ��ͬ��ʡ�ݵĻ���
void findAirportsInSameProvince(int departureID, int arrivalID, std::vector<int>& departureProvinceAirports, std::vector<int>& arrivalProvinceAirports) {
    for (int i = 1; i <= airportCount; ++i) {
        if (strcmp(airports[i].province, airports[departureID].province) == 0 ) {
            departureProvinceAirports.push_back(i);
        }
        if (strcmp(airports[i].province, airports[arrivalID].province) == 0 ) {
            arrivalProvinceAirports.push_back(i);
        }
    }
}

//�Ƽ�����
void recommendFlights(int departureID, int arrivalID,FlightNode* head,FlightNode*&ans,bool &flag) {
    std::vector<int> departureProvinceAirports, arrivalProvinceAirports;
    readAirportData(airports);
    findAirportsInSameProvince(departureID, arrivalID, departureProvinceAirports, arrivalProvinceAirports);

    for (int depID : departureProvinceAirports) {
        for (int arrID : arrivalProvinceAirports) {
            FlightNode* ans = nullptr;
            if (hasDirectFlight(depID, arrID,head,ans)) {
                flag = 1;
                std::cout << "�Ƽ����ࣺ"<< depID<<"��"<< arrID<<endl;
                printFlights(ans);
                }
            }
        }
    }


//����2����������ʵ�ֿ�����
void searchFlights(int departureID, int arrivalID,FlightNode* head) {
    FlightNode* ans = nullptr;
    bool flag = 0;
    if (hasDirectFlight(departureID, arrivalID,head,ans)) {
        cout << "����ֱ�ﺽ��: " <<endl;
        printFlights(ans);

    } else {
        std::cout << "��ֱ�ﺽ�࣬����Ϊ���Ƽ�����..." << std::endl;
        recommendFlights(departureID, arrivalID,head,ans,flag);
    }
    if (ans == nullptr&&flag == 0)
    {
        cout <<"û���ҵ��κο�����ĺ���"<< endl;
    }
}

/*
�����Ǳ���ʵ������ݣ��æ�Ļ�����
*/

/*
�����Ǳ���ʵ����Ҫ�Ļ������ݽṹ�Ķ���
*/

typedef struct 
{
    int adjMatrix[MAX_AIRPORTS+1][MAX_AIRPORTS+1]; 
    int Flights[MAX_AIRPORTS+1]; 
} Graph;

typedef struct
{
    int departureCount = 0; // ��¼ÿ����������ɺ�����
    int arrivalCount = 0;   // ��¼ÿ�������Ľ��亽����
    int sum = 0;
    int ID;

} Counts;

/*
�����Ǳ���ʵ����Ҫ�õ��Ĺ��ܺ����Ķ���
*/

// ��ȡ������Ϣ���洢
void readFlightsFromCSVToGraph(const string& filename, Graph& graph) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "�޷����ļ�: " << filename << endl;
        return;
    }

    // ��ʼ���ڽӾ���
    for (int i = 0; i <= MAX_AIRPORTS; ++i) {
        for (int j = 0; j <= MAX_AIRPORTS; ++j) {
            graph.adjMatrix[i][j] = 0;
        }
    }

    string line;
    getline(file,line);
    while (getline(file, line)) {
        istringstream ss(line);
        Flight flight;

        // ��ȡÿ���ֶ�
        string token;
        if (getline(ss, token, ',')) flight.flightID = std::stoi(token);
        if (getline(ss, token, ',')) flight.departureDate = token;
        if (getline(ss, token, ',')) flight.intlDome = token;
        if (getline(ss, token, ',')) flight.flightNo = token;
        if (getline(ss, token, ',')) flight.departureAirportID = std::stoi(token);
        if (getline(ss, token, ',')) flight.arrivalAirportID = std::stoi(token);
        if (getline(ss, token, ',')) flight.departureTime = token;
        if (getline(ss, token, ',')) flight.arrivalTime = token;
        if (getline(ss, token, ',')) flight.airplaneID = std::stoi(token);
        if (getline(ss, token, ',')) flight.airplaneModel = std::stoi(token);
        if (getline(ss, token, ',')) flight.airfares = std::stoi(token);
        if (getline(ss, token, ',')) flight.peakSeasonRates = std::stoi(token);
        if (getline(ss, token, ',')) flight.offSeasonRates = std::stoi(token);

        // ��������Ϣ���ӵ�ͼ��
        int depAirportID = flight.departureAirportID;
        int arrAirportID = flight.arrivalAirportID;
        graph.adjMatrix[depAirportID][arrAirportID]++;
        graph.Flights[depAirportID]=1;
        graph.Flights[arrAirportID]=1;
    }

    file.close();
}

// ���㺽���Ƿ��ڹ涨ʱ�����
bool ifInDuration(const string& time , const string& aimtime1,const string& aimtime2) {
    std::tm Time = {};
    std::tm aimTime = {};    
    std::tm aimTime2 = {};
    // cout << departure <<" " << aimDeparture1 << " " << aimDeparture2 << " " << arrival << " " << aimArrival1 << " " << aimArrival2 << endl;

    // ��������ʱ��
    sscanf(time.c_str(), "%d/%d/%d %d:%d", &Time.tm_mon, &Time.tm_mday, &Time.tm_year, &Time.tm_hour, &Time.tm_min);
    Time.tm_year -= 1900; 
    Time.tm_mon -= 1;

    // ����Ŀ������ʱ��1
    sscanf(aimtime1.c_str(), "%d/%d/%d %d:%d", &aimTime.tm_mon, &aimTime.tm_mday, &aimTime.tm_year, &aimTime.tm_hour, &aimTime.tm_min);
    aimTime.tm_year -= 1900;
    aimTime.tm_mon -= 1; 

    // ����Ŀ������ʱ��2
    sscanf(aimtime2.c_str(), "%d/%d/%d %d:%d", &aimTime2.tm_mon, &aimTime2.tm_mday, &aimTime2.tm_year, &aimTime2.tm_hour, &aimTime2.tm_min);
    aimTime2.tm_year -= 1900;
    aimTime2.tm_mon -= 1; 

    // ת���� time_t 
    time_t Epoch = std::mktime(&Time);
    time_t aimEpoch = std::mktime(&aimTime);
    time_t aimEpoch2 = std::mktime(&aimTime2);
    // cout <<depEpoch<< " " << aimDepEpoch << " " << aimDepEpoch2 << " " << arrEpoch << " " << aimArrEpoch << " " << aimArrEpoch2 << endl;

    bool check = (Epoch >= aimEpoch && Epoch <= aimEpoch2) ;
    // cout << check << endl;
    return check;

    
}

// ͳ������ʱ�εĺ���
void calculateFlightsAccordingToTimeDep(FlightNode* head,Graph graph,Counts count[])
{
    // ��ȡʱ��
    string TimeStr,TimeStr2;
    cout << "���������ʱ�䷶Χ(��ʽ: MM/DD/YYYY HH:MM, ���и���)��" << endl;
    getchar();
    getline(cin, TimeStr); 
    getline(cin, TimeStr2);
    // cout << arrTimeStr << " " << arrTimeStr2 << " " << depTimeStr << " " << depTimeStr2 << endl;
        
    if (head == nullptr) 
    {
        cout << "LinkList is NULL."  << endl;
        return;
    }
    
    FlightNode* current = head;
    int suspendCount = 0,notSuspendCount = 0;
    while (current != nullptr) {
        // cout << current->flight.flightID << " ";
        bool notSuspend = ifInDuration(current->flight.departureTime, TimeStr,TimeStr2);                
        if (!notSuspend) {
            graph.adjMatrix[current->flight.departureAirportID][current->flight.arrivalAirportID]--;
            suspendCount++;
            // cout << "suspend:" << current->flight.flightID << endl;
        }
        else
        {
            cout << "not suspend:" << current->flight.flightID << endl;
            notSuspendCount++;
        }
        current = current->next;
    }   
 
    //cout << "suspendCount:" << suspendCount <<endl;
    //cout << "notsuspendCount:" << notSuspendCount<<endl;

    for (int i = 0; i <= MAX_AIRPORTS; ++i) {
        for (int j = 0; j <= MAX_AIRPORTS; ++j) {
            if (graph.adjMatrix[i][j] != 0) {
                count[i].departureCount += graph.adjMatrix[i][j]; // ͳ����ɺ���
                // count[j].arrivalCount += graph.adjMatrix[i][j];   // ͳ�ƽ��亽��
                // std::cout << count[i].departureCount << " " << count[j].arrivalCount << endl;
            }
        }
    }

}

// ͳ�Ƶִ�ʱ�εĺ���
void calculateFlightsAccordingToTimeArr(FlightNode* head,Graph graph,Counts count[])
{
    // ��ȡʱ��
    // ���ж�����ʱ��͵ִ�ʱ���������������Ҫ�ֱ��жϣ����������жϵĴ����кܴ���ظ��ԣ����԰�������switch��������һ����������
    string TimeStr,TimeStr2;
    cout << "������ִﷶΧʱ��(��ʽ: MM/DD/YYYY HH:MM, ���и���)��" << endl;
    getline(cin, TimeStr); 
    getline(cin, TimeStr2);
    cout << TimeStr  << " " << TimeStr2 << " "<< endl;

    if (head == nullptr) 
    {
        cout << "LinkList is NULL."  << endl;
        return;
    }
    
    FlightNode* current = head;
    int suspendCount = 0,notSuspendCount = 0;
    while (current != nullptr) {
        // cout << current->flight.flightID << " ";
        bool notSuspend = ifInDuration(current->flight.arrivalTime, TimeStr,TimeStr2);                
        if (!notSuspend) {
            graph.adjMatrix[current->flight.departureAirportID][current->flight.arrivalAirportID]--;
            suspendCount++;
            // cout << "suspend:" << current->flight.flightID << endl;
        }
        else
        {
            cout << "not suspend:" << current->flight.flightID << endl;
            notSuspendCount++;
        }
        current = current->next;
    }
       
    //cout << "suspendCount:" << suspendCount <<endl;
    cout << "notsuspendCount:" << notSuspendCount<<endl;

    for (int i = 0; i <= MAX_AIRPORTS; ++i) {
        for (int j = 0; j <= MAX_AIRPORTS; ++j) {
            if (graph.adjMatrix[i][j] != 0) {
                // count[i].departureCount += graph.adjMatrix[i][j]; // ͳ����ɺ���
                count[j].arrivalCount += graph.adjMatrix[i][j];   // ͳ�ƽ��亽��
                // std::cout << count[i].departureCount << " " << count[j].arrivalCount << endl;
            }
        }
    }

}

// �����æ����
void calculateBusiestAirports(Graph graph,FlightNode* head) {
        
        // int flightNum = 0;
        Counts count[MAX_AIRPORTS+1];

        // ͳ�ƺ�����
        for (int i = 0; i <= MAX_AIRPORTS; ++i) {
            for (int j = 0; j <= MAX_AIRPORTS; ++j) {
                if (graph.adjMatrix[i][j] != 0) {
                    count[i].departureCount += graph.adjMatrix[i][j]; // ͳ����ɺ���
                    count[j].arrivalCount += graph.adjMatrix[i][j];   // ͳ�ƽ��亽��
                    // std::cout << count[i].departureCount << " " << count[j].arrivalCount << endl;
                }
            }
        }

        for (int i = 0;i<=MAX_AIRPORTS;i++)
        {
            count[i].sum = count[i].arrivalCount + count[i].departureCount;
            count[i].ID = i;
        }


        for (int i = 0; i < MAX_AIRPORTS; i++) 
        {
            // �ҵ����Ԫ�ص�����
            int maxIndex = i;
            for (int j = i + 1; j < MAX_AIRPORTS; j++) {
                if (count[j].sum > count[maxIndex].sum) {
                    maxIndex = j;
                }
            }
            // ������ǰλ�ú���СԪ�ص�λ��
            if (maxIndex != i) {
                Counts temp = count[i];
                count[i] = count[maxIndex];
                count[maxIndex] = temp;
            }
        }

    // �����æ�Ļ���ǰ5��
    for(int i = 0;i<5;i++)
    {
        cout << "��" << i+1 << "��æ�Ļ�����" << count[i].ID << endl;
        cout << "�ܺ�������" << count[i].sum << "     ������������" << count[i].departureCount << "     �ִﺽ������" << count[i].arrivalCount << endl;
        cout << endl;
    }

}

// �����æ�Ļ�����ָ��ʱ��Σ�
void calculateBusiestAirportsByTime(Graph graph,FlightNode* head) {

    // int flightNum = 0;
    Counts count[MAX_AIRPORTS+1];
    Graph graph2 = graph;

    // ͳ����������
    calculateFlightsAccordingToTimeDep(head,graph,count);

    // ͳ�Ƶִ����
    calculateFlightsAccordingToTimeArr(head,graph2,count);

    for (int i = 0;i<=MAX_AIRPORTS;i++)
    {
        count[i].sum = count[i].arrivalCount + count[i].departureCount;
        count[i].ID = i;
    }


    for (int i = 0; i < MAX_AIRPORTS; i++) 
    {
        // �ҵ����Ԫ�ص�����
        int maxIndex = i;
        for (int j = i + 1; j < MAX_AIRPORTS; j++) {
            if (count[j].sum > count[maxIndex].sum) {
                maxIndex = j;
            }
        }
        // ������ǰλ�ú���СԪ�ص�λ��
        if (maxIndex != i) {
            Counts temp = count[i];
            count[i] = count[maxIndex];
            count[maxIndex] = temp;
        }
    }

    // �����æ�Ļ���ǰ5��
    for(int i = 0;i<5;i++)
    {
        cout << "��" << i+1 << "��æ�Ļ�����" << count[i].ID << endl;
        cout << "�ܺ�������" << count[i].sum << "     ������������" << count[i].departureCount << "     �ִﺽ������" << count[i].arrivalCount << endl;
        cout << endl;
    }

}

//�û����ܽ���չʾ 
void displayMenu() {
    cout << "��ӭ�����񺽶�Ʊϵͳ!" << endl;
    cout << "1. ��������" << endl;
    cout << "2. ���ݲ�ѯ" << endl; 
    cout << "3. ���ݷɻ�����ɾ������,������ɾ��������" << endl;
    cout << "4. ���ӡ�ɾ�����޸ĺͲ�ѯ������Ŀ" << endl;
    cout << "5. �������ļ��������ӡ�ɾ�����޸ĺ�����Ϣ" << endl;
    cout << "6. ��ͣ�ͻָ�����" << endl;
    cout << "7. ������γ˻�˳���" << endl;
    cout << "8. �������ƽ�������" << endl;
    cout << "9. ��ͬʡ�ݻ����Ƽ�" << endl;
    cout << "10.�æ����ͳ��" << endl;
    cout << "11.�˳�" << endl;
    cout << "����������ѡ��: ";
}

//�û����ܽ���չʾ 
void displaySubMenu1()
{
	cout << "1.����������ID������������" << endl; 
	cout << "2.���ִ����ID��������������1�Ļ����ϣ�" << endl;
	cout << "3.����������ID��������������1 2 �Ļ����ϣ�" << endl;
}

//�û����ܽ���չʾ 
void displaySubMenu2()
{
	cout << "1.���һ�������Ϊ���������к���" << endl; 
	cout << "2.���Ұ���" << endl;
	cout << "3.���ݷɻ�ID������������" << endl;
}

//�û����ܽ���չʾ 
void displaySubMenu4()
{
	cout << "1.����" << endl; 
	cout << "2.ɾ��" << endl;
	cout << "3.�޸�" << endl;
}

void displaySubMenu10()
{
	cout << "1.ȫʱ���æ����ͳ��" << endl; 
	cout << "2.ָ��ʱ���æ����ͳ��" << endl;

}


int main() {
    bool dataLoaded = false;

    // �������� 
    int entryNum = 0;
    FlightNode *head = readFlightsFromCSV("flight_data.csv",  &entryNum);
    
    if (entryNum != 0) {
    	/*FlightNode *head = readFlightsFromCSV("lab1_data.csv",  &i);*/
        dataLoaded = true;
        
        cout << "�������ݳɹ�����, �ܺ�������: " << entryNum << endl;
    } else {
        cerr << "�������ݼ���ʧ��!" << endl;
    }

    while (true) {
        displayMenu();
        int choice;
        cin >> choice;

        if (cin.fail() || choice < 1 || choice > 11) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "��Чѡ�����������롣" << endl;
            continue;
        }

        // �ⲿ��case�Ƚ϶����Ƚϳ�����Ҫһ���Կ����switch�������ܵĽṹ�����Ʋ�����һ���û����ܽ���չʾ������
        switch (choice) {
            case 1: {
            	displaySubMenu1(); 
            	int choice1;
            	cin >> choice1;
            	switch(choice1)
            	{
            		case 1:
            			sortFlightsByDepartureAirportID(head);               
                		printFlights(head);
                		break;
            		case 2:
            			sortFlightsByArrivalAirportID(head);               
                		printFlights(head);
                		break;
            		case 3:
            			sortFlightsByFlightDuration(head);               
                		printFlights(head);
                		break;
				}
                break;
            }
            case 2:
                if (dataLoaded) {
                	displaySubMenu2(); 
            		int choice1;
            		cin >> choice1;
            		switch(choice1)
            		{
            		case 1:
            			findFlightsWithPrimeFares(head);
                		break;
            		case 2:
            			int departureID, arrivalID;
                    	cout << "��������������ID�͵ִ����ID: ";
                    	cin >> departureID >> arrivalID;
                    	findSaddlePoint(head, departureID, arrivalID);
                		break;
            		case 3:
            			int airplaneID;
                    	cout << "������ɻ�ID: ";
                    	cin >> airplaneID;
                    	FlightNode* flightListByAirplane = createFlightListByAirplaneID(head, airplaneID);
                    	printFlights(flightListByAirplane);
                		break;
					}
				}
            case 3: {
                if (dataLoaded) {
                    int airplaneModel;
                    cout << "������ɻ�������ɾ����Ӧ����: ";
                    cin >> airplaneModel;
                    FlightNode* deletedListHead = nullptr;
                    deleteFlightsByAirplaneModel(head, airplaneModel, deletedListHead);
                    head = deletedListHead;
                    printFlights(head);
                    }
                else {
                    cout << "���ȼ��غ������ݡ�" << endl;
                }
                break;
            }
            case 4:
			{
				if(dataLoaded)
				{
					processingReviseCommand(head);
										
				}
				break;
			 } 
			case 5:
			{
				if(dataLoaded)
				{
					string filename;
                    cout << "Enter filename for batch processing: ";
                    cin >> filename;
                    processBatchCommands(head, filename);
                    break;
										
				}
				break;
			 } 
            case 6:
            {
                if(dataLoaded)
				{
					processingReviseCommand(head);
										
				}
				break;
            }
            case 7:
            {
                int startFlightID;
                cout << "Enter the starting flight ID: ";
                cin >> startFlightID;
                findMaxConnections(head, startFlightID);
                break;
            }
            case 8:
            {
                TreeNode* root = new TreeNode("World");
                buildTreeFromFile(root);
                string keywords;
                cout << "��������Ҫ���ҵĻ�����"<< endl;
                cout << "(ע�����ʡ��/����ƥ���ʱ�����ƶ����ֻ��Ϊ��ͳһ��ʽҪ��ʵ��û������): " << endl;
                getchar();
                getline(cin, keywords); // ʹ�� getline ��ȡһ����
                const char* keyword = keywords.c_str();
                searchAirports(root, keyword);
                delete root; // �ͷ��ڴ�
                break;
            }
            case 9:
            {
                cout << "��������Ҫ��ѯ�ĳ����ʹﵽ�Ļ�����ID ���ո��������"<< endl;
                int departureID,arrivalID;
                cin >> departureID >> arrivalID;
                searchFlights(departureID,arrivalID,head); 
                break;
            }
            case 10:
            {
                Graph graph;
                string filename = "flight_data.csv";
                readFlightsFromCSVToGraph(filename, graph);
                displaySubMenu10();
                int choice10;
                cin >> choice10;
                switch (choice10)
                {
                    case 1:
                    {
                        calculateBusiestAirports(graph,head);
                        break;
                    }
                    case 2:
                    {
                        calculateBusiestAirportsByTime(graph,head);
                        break;

                    }
                }
                break;

            }
			case 11:
            {
                cout << "�˳�����" << endl;
                return 0;
            }

        }
    }
}






