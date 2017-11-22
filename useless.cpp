#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <cstring>
//#include <cerrno>

using namespace std;

struct delayedCommand {
    unsigned time;
    string command;
};


bool timeCmp (delayedCommand a, delayedCommand b) {
    return a.time < b.time;
}

char** genArgv (string lineToParce)
{
    char** result;
    unsigned numOfWords = 1;
    unsigned spacePos, pos = 0;

    while (true)
    {
        spacePos = static_cast<unsigned int>(lineToParce.find(' ', pos));
        numOfWords++;
        if (spacePos > lineToParce.length())
            break;
        pos = spacePos+1;
    }
    result = new char* [numOfWords+1];

    pos=0;
    unsigned i = 0;
    while ((spacePos = static_cast<unsigned int>(lineToParce.find(' ', pos))) < lineToParce.length())
    {
        string tmp = lineToParce.substr(pos, spacePos-pos);
        *(result+i) = new char [tmp.length()+1];
        strcpy(*(result+i), tmp.c_str());
        pos = spacePos+1;
        i++;
    }
    string tmp = lineToParce.substr(pos, lineToParce.length());
    *(result+i) = new char [tmp.length()+1];
    strcpy(*(result+i), tmp.c_str());
    *(result+i+1) = {0};
    return result;
}

char* getPath (const string &parcingLine)
{
    auto spacePos = static_cast<unsigned int>(parcingLine.find(' ', 0));
    if (spacePos < parcingLine.length())
    {
        string tmp = parcingLine.substr(0, spacePos);
        auto * result = new char [tmp.length()+1];
        strcpy(result, tmp.c_str());
        return result;
    } else
    {
        auto * result = new char [parcingLine.length()+1];
        strcpy(result, parcingLine.c_str());
        return result;
    }

}

int main () {
    ifstream input;
    input.open ("/home/michael/CLionProjects/IT_homeworks/useless/useless_input.txt", ifstream::in);
    if (!input.is_open()) {
        cerr << "Couldn't open file" << endl;
        exit (-1);
    }

    size_t numOfStructures = 10;
    vector<delayedCommand> dComm(numOfStructures);
    size_t numOfReadLines = 0;
    string line;
    while (!input.eof()) {
        getline(input, line);
        if (line.length() == 0)
            break;

        numOfReadLines++;
        if (numOfReadLines == numOfStructures) {
            numOfStructures *= 2;
            dComm.resize(numOfStructures);
        }
        dComm[numOfReadLines-1].time = (unsigned) stoi (line.substr(0, line.find(' ')), nullptr);
        dComm[numOfReadLines-1].command = line.substr(line.find(' ', 0)+1, line.length());
    }
    input.close();

    sort (dComm.begin(), dComm.begin()+numOfReadLines, timeCmp);

    unsigned timeWorking=0;
    for(unsigned i=0; i<numOfReadLines; ++i) {
        sleep(dComm[i].time-timeWorking);
        timeWorking = dComm[i].time;

        pid_t result;
        if ((result = fork()) < 0) {
            cerr << "Couldn't start another process" << endl;
            exit (-1);
        } else
            if (result == 0) {
                char** argv = genArgv(dComm[i].command);
                char* path = getPath(dComm[i].command);
                execv(path, argv);
                cerr << "Couldn't perfom a task:" << dComm[i].command << endl;
                exit(-1);
            }
    }

    return 0;
}