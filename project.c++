#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
using namespace std;


struct Node
{
    string name;
    string phone;
    string mail;
    Node* prev;
    Node* next;
};

Node* head = NULL;
Node* lastDeletedContact = NULL;


string countryCodes[4] = { "+91", "+1", "+44", "+61" };


string makeTextLowerCase(string text)
{
    for (int i = 0; i < text.length(); i++)
        if (text[i] >= 'A' && text[i] <= 'Z')
            text[i] += 32;
    return text;
}

void getLine(string& text)
{
    getline(cin, text);
    while (text.size() == 0)
        getline(cin, text);
}


string encryptText(string data)
{
    string result = "";
    for (int i = 0; i < data.length(); i++)
        result += char(data[i] + 3);
    return result;
}

string decryptText(string data)
{
    string result = "";
    for (int i = 0; i < data.length(); i++)
        result += char(data[i] - 3);
    return result;
}


bool isEmailCorrect(string email)
{
    string lower = makeTextLowerCase(email);
    return (lower.find("@gmail.com") != -1 || lower.find("@yahoo.com") != -1 ||
            lower.find("@outlook.com") != -1 || lower.find("@hotmail.com") != -1);
}

bool isPhoneNumberCorrect(string phone)
{
    if (phone.length() != 10) return false;
    for (char c : phone)
        if (!isdigit(c)) return false;
    return true;
}


Node* makeNode(string name, string phone, string mail)
{
    Node* n = new Node();
    n->name = name;
    n->phone = phone;
    n->mail = mail;
    n->prev = NULL;
    n->next = NULL;
    return n;
}


void saveToFile()
{
    ofstream file("contacts.txt");
    Node* t = head;
    while (t)
    {
        file << t->name << ","
             << encryptText(t->phone) << ","
             << encryptText(t->mail) << endl;
        t = t->next;
    }
    file.close();
}

void loadFromFile()
{
    ifstream file("contacts.txt");
    if (!file) return;

    string line;
    while (getline(file, line))
    {
        int p1 = line.find(",");
        int p2 = line.find(",", p1 + 1);

        string name = line.substr(0, p1);
        string phone = decryptText(line.substr(p1 + 1, p2 - p1 - 1));
        string mail = decryptText(line.substr(p2 + 1));

        Node* n = makeNode(name, phone, mail);
        if (!head) head = n;
        else
        {
            Node* t = head;
            while (t->next) t = t->next;
            t->next = n;
            n->prev = t;
        }
    }
    file.close();
}


string getCountryCode()
{
    int c;
    string code;
    cout << "1. India\n2. USA\n3. UK\n4. Australia\nChoose country: ";
    cin >> c;
    cin.ignore();
    if (c < 1 || c > 4) c = 1;

    cout << "Enter country code: ";
    getLine(code);
    while (code != countryCodes[c - 1])
    {
        cout << "Wrong code. Enter again: ";
        getLine(code);
    }
    return code;
}


bool isDuplicate(string phone, string email, Node* ignore = NULL)
{
    Node* t = head;
    while (t)
    {
        if (t != ignore)
        {
            if (t->phone == phone || makeTextLowerCase(t->mail) == makeTextLowerCase(email))
                return true;
        }
        t = t->next;
    }
    return false;
}


Node* searchContact(string name)
{
    Node* t = head;
    string key = makeTextLowerCase(name);
    while (t)
    {
        if (makeTextLowerCase(t->name) == key)
            return t;
        t = t->next;
    }
    return NULL;
}


void showContactsWithPagination()
{
    if (!head)
    {
        cout << "No contacts available.\n";
        return;
    }

    int pageSize = 20;
    int total = 0;
    Node* t = head;
    while (t) { total++; t = t->next; }

    int pages = total / pageSize + (total % pageSize != 0);
    int page = 1;

    while (true)
    {
        cout << "\nPage " << page << " of " << pages << endl;

        int skip = (page - 1) * pageSize;
        t = head;
        while (skip-- && t) t = t->next;

        int shown = 0;
        while (t && shown < pageSize)
        {
            cout << t->name << " | " << t->phone << " | " << t->mail << endl;
            t = t->next;
            shown++;
        }

        cout << "\n1.Next  2.Previous  3.Exit : ";
        int ch;
        cin >> ch;

        if (ch == 1 && page < pages) page++;
        else if (ch == 2 && page > 1) page--;
        else if (ch == 3) break;
        else cout << "Invalid.\n";
    }
}


void addContact()
{
    string name, phone, mail;
    cin.ignore();
    cout << "Enter name: "; getLine(name);

    string code = getCountryCode();
    cout << "Enter phone: "; getLine(phone);
    while (!isPhoneNumberCorrect(phone)) { cout << "Invalid phone. Try again: "; getLine(phone); }

    cout << "Enter email: "; getLine(mail);
    while (!isEmailCorrect(mail)) { cout << "Invalid email. Try again: "; getLine(mail); }

    string fullPhone = code + phone;

    while (isDuplicate(fullPhone, mail))
    {
        cout << "Duplicate found. Enter new phone & email.\n";
        getLine(phone);
        getLine(mail);
        fullPhone = code + phone;
    }

    Node* n = makeNode(name, fullPhone, mail);
    if (!head) head = n;
    else
    {
        Node* t = head;
        while (t->next) t = t->next;
        t->next = n;
        n->prev = t;
    }

    saveToFile();
    cout << "Contact added successfully.\n";
}


int main()
{
    loadFromFile();
    int choice;

    while (true)
    {
        cout << "\n1.Add\n2.Show\n3.Exit\nEnter: ";
        cin >> choice;

        if (choice == 1) addContact();
        else if (choice == 2) showContactsWithPagination();
        else if (choice == 3) break;
        else cout << "Invalid.\n";
    }

    return 0;
}
