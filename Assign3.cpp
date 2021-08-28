#include <cstdlib>
#include <iostream>
#include <fstream>
#include <locale>
#include <sstream>
#include <string>

using namespace std;

struct Node
{
    int data;
    Node *next;
};

class Queue
{
    private:
        Node *front, *rear;
        int queueLength;

    public:
        Queue();
        ~Queue();
        void Join(int newItem);
        void Leave();
        int Front();
        bool isEmpty();
        int Length();
};

Queue::Queue()
{
    front = NULL;
    rear = NULL;
    queueLength = 0;
}

Queue::~Queue()
{
}

// joins element to queue
void Queue::Join(int newItem)
{
    Node *temp;
    temp = new Node;
    temp->data = newItem;
    temp->next = NULL;
    if (rear != NULL)
    {
        rear->next = temp;
    }
    rear = temp;
    if (front == NULL)
    {
        front = temp;
    }
    queueLength++;
}

//removes element from queue
void Queue::Leave()
{
    Node *temp;
    if (front == NULL)
    {
        return;
    }
    temp = front;
    front = front->next;
    if (front == NULL)
    {
        rear = NULL;
    }
    delete temp;
    queueLength--;
}

//recovers data at front of queue
int Queue::Front()
{
    return front->data;
}

bool Queue::isEmpty()
{
    if (front == NULL)
    {
        return true;
    }
    return false;
}

//returns length of queue
int Queue::Length()
{
    return queueLength;
}

#define TIMEDELAY 3
#define N 128
int OutQueues_current[N];
int Congestion_Size[N];

//arrays of queues
Queue InputQueues[N];
Queue OutputQueues[N];

void init_simulation()
{
    for (int a = 0; a < N; a++)
    {
        OutQueues_current[a] = 0;
        Congestion_Size[a] = 0;
    }
}

int sum_elements_array(int array[])
{
    int sum = 0;
    for (int a = 0; a < N; a++)
    {
        sum = sum + array[a];
    }
    return sum;
}

int number_of_ports = 0;

main(int argc, char **argv)
{
    //gets arguments from command line (name of the simulation text file)
    int portnumber = 0;
    string expression;
    string geninput;
    ifstream input_file;
    if (argc != 2)
    {
        cout << "Enter simulation file name: " << endl << argv[1] << endl;
        exit(0);
    }
    input_file.open(argv[1]);
    if (input_file.is_open() == false)
    {
        cout << "Could not read the file." << endl << argv[1] << endl;
        exit(0);
    }
    string token;

    //read the file, print the input ports contents
    while (!input_file.eof())
    {
        getline(input_file, expression);
        stringstream line(expression);
        if (input_file.eof())
            break;
        if (expression[0] == '#')
            continue;   //jump line, this is a line of comments
        if (expression[0] == 'P')
        {
            getline(line, token, ' ');
            getline(line, token, ' ');
            number_of_ports = atoi(token.c_str());
            cout << "Found the number of ports to be " << number_of_ports << endl;
            continue;//got the number of ports
        }
        //stores data in InputQueues
        portnumber++;   //get data for the next port

        while (getline(line, token, ' '))
        {
            int destination;
            destination = atoi(token.c_str());  //use this one if your compiler is not C++11
            if (destination < 1 || destination > number_of_ports || number_of_ports < portnumber)
            {
                cout << "ERROR in the format of the text file." << endl;
                exit(0);
            }
            InputQueues[portnumber - 1].Join(destination);
        }
        int sumofinputpackets = InputQueues[portnumber - 1].Length();
    }

    init_simulation();
    unsigned long int clock = 0;
    unsigned long int currentsum = 99999999;
    //sum_elements_array(OutQueues_current);
    portnumber = 0;
    int toTransfer = 0;


    while (currentsum > 0)  //one clock cycle
    {
        if (!InputQueues[portnumber].isEmpty())
        {
            toTransfer = InputQueues[portnumber].Front();
            OutputQueues[toTransfer - 1].Join(toTransfer);
            InputQueues[portnumber].Leave();
            OutQueues_current[toTransfer - 1] = OutputQueues[toTransfer - 1].Length();
        }
        portnumber++;
        if (portnumber >= number_of_ports)
        {
            portnumber = 0;
        }
        clock++;

        if (clock % (TIMEDELAY * number_of_ports) == 0 && clock != 0)
        {
            //Delete 1 packet from each queue and count number of packets again
            for (int a = 0; a < number_of_ports; a++)
            {
                if (OutputQueues[a].isEmpty() == false)
                {
                    OutputQueues[a].Leave();
                    OutQueues_current[a] = OutputQueues[a].Length();
                }
            }
        }
        // include the queue updates for the simulation
        currentsum = sum_elements_array(OutQueues_current);
        //compute the current state of the output queues
        if (currentsum > sum_elements_array(Congestion_Size))
        {
            for (int a = 0; a < number_of_ports; a++)
            {
                Congestion_Size[a] = OutQueues_current[a];
            }
        }
    }
    for (int a = 0; a < number_of_ports; a++)
    {
        cout << "output port " << a + 1 << ": " << Congestion_Size[a] << " packets" << endl;
    }
}
