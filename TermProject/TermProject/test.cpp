#define _CRT_SECURE_NO_WARNINGS
#include<fstream>
#include<iostream>
#include<string>
#include<vector>
#include <sstream>

using namespace std;

ifstream infile("test.txt");
ofstream writeFile;
vector<string> vec;
int currentLine = 0;
string errorMessage = " ";

void refreshVector() {
    //vec ���ΰ�ħ

    string reStr;
    for (vector<string>::iterator itr = vec.begin(); itr != vec.end(); itr++) {
        reStr += *itr;
    }

    istringstream ss(reStr);
    string stringBuffer;
    int size = 0;
    string temp;

    vec.clear();

    while (getline(ss, stringBuffer, ' ')) {
        size += stringBuffer.size() + 1;

        if (size <= 74) {
            temp += stringBuffer + ' ';
        }
        else {
            vec.push_back(temp);  
            size = stringBuffer.size();
            temp.clear();
            temp += stringBuffer + ' ';
        }

    }

}
bool checkMenu(string menu) {
    //(..) �� ����� ���Դ� �� Ȯ��
    if (menu.at(0) == 'n' || menu.at(0) == 'p' || menu.at(0) == 't') {
        //n�� p�� ��ȣ�� �ʿ����
        return true;
    }
    else if (menu.length() <= 3) {
        errorMessage = "Invalid Fortmat! Please check your input";
        return false;
    }

    if ((menu.at(1) == '(') && (menu.at(menu.length() - 1) == ')')) {
        return true;
    }

    errorMessage = "Invalid format! Please check your input!";
    return false;
}

class Output {
public:
    void outputFile() {
        for (int i = 0; i < 20; i++) {
            printf("%2d |  ", i + 1);
            if ((currentLine + i) >= vec.size())
            {
                cout << " " << endl;
            }
            else {
                cout << vec[currentLine+ i] << endl;
            }
            
        }
        this->outputLIne();

        this->outputMenu();

        this->outputConsoleMessage(errorMessage);
    }

    string inputMenu() {
        // �޴��Է¹ޱ�
        string menu;
        cout << "�Է�: ";
        cin >> menu;
        
        //���� ���
        this->outputLIne();

        //return input;
        return menu;
    }

    

private:
    void outputMenu() {
        //�޴����
        cout << "n: ����������, p: ����������, i : ����, d : ����, c: ����, s: ã��, t: ������ ����" << endl;

        //�������
        this->outputLIne();
    }

    void outputLIne() {
        //�������
        cout << "---------------------------------------------------------------------------------------" << endl;

    }

    void outputConsoleMessage(string message) {
        //�ָܼ޼��� ��� �� error�� �����ϸ� error���
        cout << "(�ָܼ޼���) " << message << endl;

        //error �ʱ�ȭ
        errorMessage = " ";

        //�������
        this->outputLIne();
    }
};

class Job {
public:
    static Job* menuToJob(string menu);
    virtual void execute() = 0;
    virtual void init() = 0;
    
};

class nextPage : public Job {
     void init(){
         currentLine += 20;
     }
     void execute() override{
         init();
        
        if (currentLine >= vec.size()) {
            //�������� ������ ���� ���
            errorMessage = "This is the last page!";
            currentLine -= 20;
        }
    }
};

class prevPage : public Job {
    void init() override {

    }

    void execute() override{
        if (currentLine == 0) {
            //�������� ù���� ���
            errorMessage = "This is the first page!";
        }
        else {
            currentLine -= 20;
            if (currentLine < 0) {
                //search�� currentLine -20�� 0���� ���� ���
                currentLine = 0;
            }
        }

        
    }
};

class Delete : public Job{
public:
    string jobMessage; //�Է�
    int line; //���° ��
    int deleteLocation; //��� �ܾ� ��
    bool isError = false; //�����߻��� �۾����� X

    Delete(string message) {
        jobMessage = message;
    }

    void init() override{
        //ex) jobMessage = d(1,10) -> 1,10 
        jobMessage = jobMessage.substr(2, jobMessage.length() - 3);

        istringstream ss(jobMessage);
        string stringBuffer;
        vector<string> spl;
        spl.clear();

        while (getline(ss, stringBuffer, ','))
        {
            spl.push_back(stringBuffer);
        }

        try {
            if (spl.size() != 2) {
                //������ ��� �����߻�
                isError = true;
                errorMessage = "�Է¹��ڸ� Ȯ�����ּ���";
            }

            else {
                line = stoi(spl[0]);
                if ((line <= 0 || line > 20)) {
                    errorMessage = "Delete Job can work in 20 lines.";
                    isError = true;
                }
                deleteLocation = stoi(spl[1]);
            }
        }
        catch (invalid_argument e)
        {
            errorMessage = "Invald Input! Please Check your input";
            isError = true;
        }

    } 
    void execute() override{
        init();

        if (!isError) {
            //������ �߻����� �ʾ��� ���

            //���� -> �ܾ��
            string vectorStr = vec[currentLine + line - 1]; //�ش��������� line��°�� 
            istringstream ss(vectorStr);
            string stringBuffer;
            vector<string> words; //line��°�� �ܾ��
            words.clear();
            while (getline(ss, stringBuffer, ' '))
            {
                words.push_back(stringBuffer);
            }

            vectorStr.clear();

            //line��°�� ��ü
            int i = 1;
            for (vector<string>::iterator itr = words.begin(); itr != words.end(); itr++) {
                if (i != deleteLocation) {
                    vectorStr += (*itr + ' ');
                }
                
                i++;
            }

            //������ ' ' ����
            if (deleteLocation != words.size()) {
                vectorStr = vectorStr.substr(0, vectorStr.length() - 1);
            }

            //vec�� line��°�ٷ� ��ü
            vec[currentLine + line - 1] = vectorStr;

            refreshVector();
        }
    }
};

class Exchange : public Job {
public:
    string jobMessage;
    string findWord;
    string changeWord;
    bool is_Error = false;

    Exchange(string message) {
        jobMessage = message;
    }

    void init() override{
        //ex) jobMessage = c(hello, bye) -> hello,bye
        jobMessage = jobMessage.substr(2, jobMessage.length() - 3);

        istringstream ss(jobMessage);
        string stringBuffer;
        vector<string> spl;
        spl.clear();

        while (getline(ss, stringBuffer, ','))
        {
            spl.push_back(stringBuffer);
        }

            if (spl.size() != 2) {
                //������ ��� �����߻�
                is_Error = true;
                errorMessage = "Invald Input! Please Check your input";
            }

            else {
                findWord = spl[0];
                changeWord = spl[1];
            }
        }


    void execute() override{
        init();

        if (!is_Error) {
            vector<string> words;
            bool isFind = false;

            for (vector<string>::iterator itr = vec.begin(); itr != vec.end(); itr++) {
                //����ٿ� findWord�� �ִ� �� ã��
                istringstream ss(*itr);
                string stringBuffer;
                
                words.clear();

                while (getline(ss, stringBuffer, ' '))
                {
                    if (stringBuffer == findWord) {
                        words.push_back(changeWord);
                        isFind = true;
                    }
                    else {
                        words.push_back(stringBuffer);
                    }
                }
                
                if (isFind) {
                    string temp;
                    temp.clear();
                    for (vector<string>::iterator wordItr = words.begin(); wordItr != words.end(); wordItr++) {
                        temp += (*wordItr + ' ');
                    }
                    *itr = temp;
                    isFind = false;
                }
            }
        }
        refreshVector();
    }
};

class Insert : public Job {
public:
    string jobMessage; //�Է�
    int line; //���° ��
    int insertLocation; //��� �ܾ� ��
    string insertWord; //���Ե� �ܾ�
    bool isError = false; //�����߻��� �۾����� X

    Insert(string message) {
        jobMessage = message;
    }

    void init() override{
        //ex) jobMessage = i(1,10,hello) -> 1,10,hello 
        jobMessage = jobMessage.substr(2, jobMessage.length() - 3);

        istringstream ss(jobMessage);
        string stringBuffer;
        vector<string> spl;
        spl.clear();

        while (getline(ss, stringBuffer, ',' ))
        {
            spl.push_back(stringBuffer);
        }

        try {
            if (spl.size() != 3) {
                //������ ��� �����߻�
                isError = true;
                errorMessage = "�Է¹��ڸ� Ȯ�����ּ���";
            }

            else {
                 
                line = stoi(spl[0]);
                insertLocation= stoi(spl[1]);
                insertWord = spl[2];

                if (insertWord.size() >= 75) {
                    isError = true;
                    errorMessage = "Insert Word is too long! Please Check your input";
                }
            }
        }
        catch (invalid_argument&)
        {
            errorMessage = "Invaild Input! Please Check your input";
            isError = true;
        }        
    }

    void execute() override{
        //�ʱ�ȭ
        init();

        if (!isError) {
            //������ �߻����� �ʾ��� ���

            //���� -> �ܾ��
            string vectorStr = vec[currentLine + line - 1]; //�ش��������� line��°�� 
            istringstream ss(vectorStr);
            string stringBuffer;
            vector<string> words; //line��°�� �ܾ��
            words.clear();
            while (getline(ss, stringBuffer, ' ' ))
            {
                words.push_back(stringBuffer);
            }

            //�ܾü
            if (insertLocation > words.size()) {
                errorMessage = to_string(insertLocation) + "th word doesn't existed!";
                return;
            }
            words[insertLocation - 1] += (' ' + insertWord);

            //line��°�� ��ü
            vectorStr.clear();
            for (vector<string>::iterator itr = words.begin(); itr != words.end(); itr++) {
                vectorStr += (*itr + ' ');
            }
            
            //vec�� line��°�ٷ� ��ü
            vec[currentLine + line - 1] = vectorStr;
            
            refreshVector();
        }
    }
};
class Search : public Job {    
public:
    string jobMessage; //�Է� ex) s(hello)
    string findWord; //ã�ƾ��Ҵܾ�
    bool is_Error = false;

    Search(string message) {
        jobMessage = message;
    }

    void init() override{
        //ex s(hello) -> hello
        findWord = jobMessage.substr(2, jobMessage.length() - 3);

        if (findWord == " " ) {
            //�����̸� Eroor
            is_Error = true;
            errorMessage = "Invalid Input";
        }
    }

    void execute() override{
        init();

        if (!is_Error) {
            int findLine = -1;
            int current = 0;

            for (vector<string>::iterator itr = vec.begin(); itr != vec.end(); itr++) {
                //����ٿ� findWord�� �ִ� �� ã��
                istringstream ss(*itr);
                string stringBuffer;
                
                while (getline(ss, stringBuffer, ' '))
                {
                    if (stringBuffer == findWord) {
                        findLine = current;
                    }
                }

                current++;

                if (findLine != -1) {
                    //findWord�� ã���� for�� Ż��
                    break;
                }
            }

            if (findLine == -1) {
                //findWord�� �� ã��
                errorMessage = findWord + " does not exist in file";
            }
            else {
                //findWord�� ã��
                currentLine = findLine;
            }
        }

    }
};

class Store : public Job {
public:
    string fileString;

    Store(){}
    
    void init() override {
        //vec -> string
        for (vector<string>::iterator itr = vec.begin(); itr != vec.end(); itr++) {
            fileString += *itr;
        }
    }
    void execute() override{
        init();

        writeFile.open("test.txt");

        if (writeFile.is_open()) {
            writeFile.write(fileString.c_str(), fileString.size());
        }

        writeFile.close();
    }
};

Job* Job::menuToJob(string message) {
    char menu = message.at(0);
    Job* job = NULL;
    switch (menu)
    {
    case 'n':
        job = new nextPage();
        break;
    case 'p':
        job = new prevPage();
        break;
    case 'i':
        job = new Insert(message);
        break;
    case 'd':
        job = new Delete(message);
        break;
    case 'c':
        job = new Exchange(message);
        break;
    case 's':
        job = new Search(message);
        break;
    case 't':
        job = new Store();
        break;
    default:
        errorMessage = "Invalid Command! Please Check jobMenu";
        break;
    }

    return job;
}

int main() {

    string fileStr;

    if (infile.is_open()) {
            getline(infile, fileStr);

            istringstream ss(fileStr);
            string stringBuffer;

            vec.clear();
            
            int size = 0;
            string temp;
            while (getline(ss, stringBuffer, ' ')) {
                size += stringBuffer.size()+ 1;

                if (size <= 74) {
                    temp += stringBuffer + ' ';
                }
                else {
                 
                    vec.push_back(temp);
                    size = stringBuffer.size();
                    temp.clear();
                    temp += stringBuffer + ' ';
                }              
            }          
    }
    infile.close();

    Output output;
    output.outputFile();
    string menu = output.inputMenu();
    Job* jobs = Job::menuToJob(menu);
    if (jobs == NULL || !checkMenu(menu)) {
        while (jobs == NULL || !checkMenu(menu)) {
            output.outputFile();
            menu = output.inputMenu();
            jobs = Job::menuToJob(menu);
        }
    }
        jobs->execute();
        while (menu.at(0) != 't') {
            output.outputFile();
            menu = output.inputMenu();
            jobs = Job::menuToJob(menu);
            if (jobs == NULL || !checkMenu(menu)) {
                continue;
            }
            jobs->execute();
        }
    return 0;
}
