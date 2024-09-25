#include <iostream>
#define T 8 // size of the array in each Block Node
#define MAXSIZE 200
#define ASCI_SKIP 11
#define ASCI_SKIP2 12
#define ASCI_SKIP3 13
using namespace std;

// STRUCTURES
struct AttributeNode {
    char* name;
    char* value;
    AttributeNode* next;
};

struct SelectorNode {
    char* name;
    SelectorNode* next;
};

struct Block {
    SelectorNode* headSelector;
    SelectorNode* tailSelector;
    AttributeNode* headAttribute;
    AttributeNode* tailAttribute;
};

struct BlockNode {
    Block blocks[T];
    int occupied = 0;
    BlockNode* next;
    BlockNode* prev;
};

// FUNCTIONS

// compares two strings, whether they are the same
bool comparison(char* input, char* input2, int start, int end) {
    for (int i = 0; i < end - start; i++) {
        if (input[start + i] != input2[i]) return false;
    }
    if (input2[end - start] == '\0') return true;
    else if (input2[end - start] == ' ' && input2[end - start + 1] == '\0') return true;
    return false;
}

// reading line by line
int readLine(char* input, bool* event, int mode) {
    int i = 0;
    char c = getchar();
    bool name = false;
    while (c != '\n' && i < MAXSIZE - 1 && int(c) != EOF) {
        if (mode == 2) input[i++] = c;
        else if (c != '\t' && c != ASCI_SKIP && c != ASCI_SKIP2 && c != ASCI_SKIP3) {
            if (c == ' ' && i != 0) {
                if (name) input[i++] = c;
                else if (input[i] != ',') input[i++] = c;
            }
            else if (c == ' ' && i == 0) c = getchar();
            else input[i++] = c;
        }
        if (c == ':') name = true;
        else if (c == ';' && name) name = false;
        c = getchar();
    }
    input[i] = '\0';
    if (int(c) == EOF) *event = true;
    return i;
}

// printing some text to the console
void print(char* input) {
    int i = 0;
    while (input[i] != '\0') {
        cout << input[i++];
    }
}

// checking whether the text consists of digits or not
bool isnumeric(char* input, int start, int end) {
    for (int i = start; i < end; i++) {
        if (int(input[i]) < 48 || int(input[i]) > 57) return false;
    }
    return true;
}

// convert number stored as a text into actual integer data type
int TextToNumber(char* input, int start, int end) {
    int powerOfTen = 1, sum = 0;
    for (int i = end - 1; i >= start; i--) {
        sum += (int(input[i]) - 48) * powerOfTen;
        powerOfTen *= 10;
    }
    return sum;
}

// checking if an user typed '****' or '????'
bool CommSection(char* input) {
    if (input[0] == '*' || input[0] == '?') {
        char sign = input[0];
        for (int i = 1; i < 4; i++) {
            if (input[i] != sign) return false;
        }
        return true;
    }
    return false;
}

// Functions for Lists
void NewSelectorNode(SelectorNode*& head, SelectorNode*& tail, char* line, int start, int end) {
    SelectorNode* newNode = new SelectorNode;
    newNode->name = new char[end - start + 1];
    newNode->next = nullptr;
    for (int i = 0; i < end - start; i++) {
        newNode->name[i] = line[start + i];
    }
    newNode->name[end - start] = '\0';
    if (head == nullptr) {
        head = newNode;
        tail = newNode;
    }
    else {
        tail->next = newNode;
        tail = newNode;
    }
}

void NewAttrNode(AttributeNode*& head, AttributeNode*& tail, char* line, int start, int colon, int end) {
    // create new node
    AttributeNode* newNode = new AttributeNode;
    newNode->name = new char[colon - start + 1];
    newNode->value = new char[end - colon];
    newNode->next = nullptr;
    for (int i = 0; i < colon - start; i++) {
        newNode->name[i] = line[start + i];
    }
    newNode->name[colon - start] = '\0';
    for (int i = 0; i < end - colon - 1; i++) {
        newNode->value[i] = line[colon + i + 1];
    }
    newNode->value[end - colon - 1] = '\0';
    // check if list is empty
    if (head == nullptr) {
        head = newNode;
        tail = newNode;
    }
    else {
        tail->next = newNode;
        tail = newNode;
    }
}
void AddAttribute(BlockNode* tails, int j, char* input, int start, int colon, int end) {
    AttributeNode* attributes = tails->blocks[j].headAttribute;
    while (attributes != nullptr) {
        if (comparison(input, attributes->name, start, colon)) {
            attributes->value = new char[end - colon];
            for (int i = 0; i < end - colon - 1; i++) {
                attributes->value[i] = input[colon + i + 1];
            }
            attributes->value[end - colon - 1] = '\0';
            return;
        }
        attributes = attributes->next;
    }
    NewAttrNode(tails->blocks[j].headAttribute, tails->blocks[j].tailAttribute, input, start, colon, end);
}

void NewBlockNode(BlockNode** head, BlockNode** tail) {
    BlockNode* newNode = new BlockNode;
    newNode->next = nullptr;
    newNode->prev = *tail;
    for (int i = 0; i < 8; i++) {
        newNode->blocks[i].headSelector = nullptr;
        newNode->blocks[i].headAttribute = nullptr;
    }
    if (*tail != nullptr) {
        (*tail)->next = newNode;
    }
    else {
        *head = newNode;
    }
    *tail = newNode;
}
void deleteBlockList(BlockNode* head) {
    while (head != nullptr) {
        for (int i = 0; i < T; i++) {
            // Selectors
            SelectorNode* selNode = head->blocks[i].headSelector;
            while (selNode != nullptr) {
                SelectorNode* temp = selNode;
                selNode = selNode->next;
                delete[] temp->name;
                delete temp;
            }

            // Attributes
            AttributeNode* attrNode = head->blocks[i].headAttribute;
            while (attrNode != nullptr) {
                AttributeNode* temp = attrNode;
                attrNode = attrNode->next;
                delete[] temp->name;
                delete[] temp->value;
                delete temp;
            }
        }

        // deleting current node and moving to next one
        BlockNode* temp = head;
        head = head->next;
        delete temp;
    }
}

// FUNCTIONS FOR COMMANDS

// detecting each command (e.g. 1,S,? or 1.A,padding)
int whichCommand(char* input, int len, int* mode) {
    if (len == 1 && input[0] == '?') { *mode = 1; return 0; }                       // ?
    else {
        int comma = 0;
        for (int i = 0; i < len; i++) {
            if (input[i] == ',') {
                comma = i;
                if (input[comma + 2] != ',') return 0;
                break;
            }
        }
        if (isnumeric(input, 0, comma)) {
            if (input[comma + 1] == 'S') {
                if (input[comma + 3] == '?') *mode = 2;                              // i,S,j 
                else if (isnumeric(input, comma + 3, len)) *mode = 4;                // i,S,?
                else return 0;
            }
            else if (input[comma + 1] == 'A') {
                if (input[comma + 3] == '?') *mode = 3;                              // i,A,?
                else *mode = 5;                                                      // i,A,n
            }
            else if (input[comma + 1] == 'D') {
                if (input[comma + 3] == '*') *mode = 9;                              // i,D,*
                else *mode = 10;                                                     // i,D,n
            }
            else return 0;
        }
        else {
            if (input[comma + 1] == 'A' && input[comma + 3] == '?') *mode = 6;       // n,A,?
            else if (input[comma + 1] == 'S' && input[comma + 3] == '?') *mode = 7;  // z,S,?
            else if (input[comma + 1] == 'E') *mode = 8;                             // z,E,n
        }
        return comma;
    }
    return 0;
}

// execution of COMMAND 1 - "?"
int NumOfSections(BlockNode* head) {
    BlockNode* traverse = head;
    int counter = 0;
    while (traverse != nullptr) {
        counter += traverse->occupied;
        traverse = traverse->next;
    }
    return counter; 
}

// execution of COMMAND 2 AND 3 - "i,S,? / i,A,?"
int NumOfAttr(AttributeNode* head) {
    int counter = 0;
    AttributeNode* current = head;
    while (current != nullptr) {
        counter++;
        current = current->next;
    }
    return counter;
}

// counting selectors
int NumOfSel(SelectorNode* head) {
    int counter = 0;
    SelectorNode* current = head;
    while (current != nullptr) {
        counter++;
        current = current->next;
    }
    return counter;
}
int NumOfSomethingInSection(BlockNode* head, int i, int what) {
    BlockNode* current = head;
    int temp = i;
    while (current != nullptr) {
        if (temp > current->occupied) {
            temp -= current->occupied;
        }
        else {
            for (int j = 0; j < T; j++) {
                if (temp == 1 && current->blocks[j].headAttribute != nullptr) {
                    return (what == 1) ? NumOfSel(current->blocks[j].headSelector) : NumOfAttr(current->blocks[j].headAttribute);
                }
                if (temp != 1 && current->blocks[j].headAttribute != nullptr) {
                    temp = temp - 1;
                }
            }
            break;
        }
        current = current->next;
    }
    return -1;
}

// COMMAND 4 - "i,S,j"
void ithSel(SelectorNode* head, int i, char* input) {
    SelectorNode* current = head;
    int index = 1;
    while (current != nullptr && index < i) {
        current = current->next;
        index++;
    }
    if (current != nullptr && index == i) {
        print(input);
        cout << " == ";
        print(current->name);
        cout << endl;
    }
}
void ParticularSelInSection(BlockNode* head, int i, int k, char* input) {
    BlockNode* current = head;
    int temp = i;
    while (current != nullptr) {
        if (temp > current->occupied) {
            temp -= current->occupied;
        }
        else {
            for (int j = 0; j < T; j++) {
                if (temp == 1 && current->blocks[j].headAttribute != nullptr) {
                    ithSel(current->blocks[j].headSelector, k, input);
                    break;
                }
                if (temp != 1 && current->blocks[j].headAttribute != nullptr) {
                    temp = temp - 1;
                }
            }
            break;
        }
        current = current->next;
    }
}

// COMMAND 5 - "i,A,n"
void valueOfAttr(AttributeNode* head, char* input, int start, int end) {
    AttributeNode* current = head;
    while (current != nullptr) {
        if (comparison(input, current->name, start, end)) {
            print(input);
            cout << " ==";
            print(current->value);
            cout << endl;
            break;
        }
        current = current->next;
    }
}
void ValueOfAttrInSection(BlockNode* head, int i, char* input, int comma, int len) {

    BlockNode* current = head;
    int temp = i;
    while (current != nullptr) {
        if (temp > current->occupied) {
            temp -= current->occupied;
        }
        else {
            for (int j = 0; j < T; j++) {
                if (temp == 1 && current->blocks[j].headAttribute != nullptr) {
                    valueOfAttr(current->blocks[j].headAttribute, input, comma + 3, len);
                    break;
                }
                if (temp != 1 && current->blocks[j].headAttribute != nullptr) {
                    temp = temp - 1;
                }
            }
            break;
        }
        current = current->next;
    }
}

// COMMAND 6 and 7 - "n,A,?"/"z,S,?"
int OccurencesOfSth(BlockNode* head, char* input, int start, int end, int what) {
    BlockNode* current = head;
    int counter = 0;
    while (current != nullptr) {
        for (int i = 0; i < T; i++) {
            if (current->blocks[i].headAttribute != nullptr) {
                if (what == 1) {
                    AttributeNode* attr = current->blocks[i].headAttribute;
                    while (attr != nullptr) {
                        if (comparison(input, attr->name, start, end)) {
                            counter++;
                            break;
                        }
                        attr = attr->next;
                    }
                }
                else if (what == 2) {
                    SelectorNode* sel = current->blocks[i].headSelector;
                    while (sel != nullptr) {
                        if (comparison(input, sel->name, start, end)) {
                            counter++;
                            break;
                        }
                        sel = sel->next;
                    }
                }
            }

        }
        current = current->next;
    }
    return counter;
}

// COMMAND 8 - "z,E,n"
void ValueOfNthAttr(BlockNode* tail, char* input, int start, int comma, int len) {
    BlockNode* current = tail;
    while (current != nullptr) {
        for (int i = T - 1; i >= 0; i--) {
            SelectorNode* sel = current->blocks[i].headSelector;
            while (sel != nullptr) {
                if (comparison(input, sel->name, start, comma)) {
                    AttributeNode* attr = current->blocks[i].headAttribute;
                    while (attr != nullptr) {
                        if (comparison(input, attr->name, comma + 3, len)) {
                            print(input);
                            cout << " == ";
                            print(attr->value);
                            cout << endl;
                            return;
                        }
                        attr = attr->next;
                    }
                }
                sel = sel->next;
            }
        }
        current = current->prev;
    }
}

// COMMAND 9 - "i,D,*"
void DeletingSection(BlockNode* head, int ith, char* input, bool as_a_result) {
    BlockNode* current = head;
    int temp = ith;
    while (current != nullptr) {
        if (temp > current->occupied) {
            temp -= current->occupied;
        }
        else {
            for (int j = 0; j < T; j++) {
                if ((temp == 1 && as_a_result) || (temp == 1 && current->blocks[j].headAttribute != nullptr)) {
                    SelectorNode* selNode = current->blocks[j].headSelector;
                    while (selNode != nullptr) {
                        SelectorNode* temp = selNode;
                        selNode = selNode->next;
                        delete[] temp->name;
                        delete temp;
                    }

                    // Attributes
                    AttributeNode* attrNode = current->blocks[j].headAttribute;
                    while (attrNode != nullptr) {
                        AttributeNode* temp = attrNode;
                        attrNode = attrNode->next;
                        delete[] temp->name;
                        delete[] temp->value;
                        delete temp;
                    }
                    current->blocks[j].headSelector = nullptr;
                    current->blocks[j].headAttribute = nullptr;
                    if (!as_a_result) {
                        print(input);
                        cout << " == deleted" << endl;
                    }
                    current->occupied -= 1;
                    return;
                }
                if (temp != 1 && current->blocks[j].headAttribute != nullptr) {
                    temp = temp - 1;
                }
            }
            break;
        }
        current = current->next;
    }
}

// COMMAND 10 - "i,D,n"
void deleteAttributeNode(AttributeNode** head, char* input, int comma, int len) {
    AttributeNode* current = *head;
    AttributeNode* prev = nullptr;

    while (current != nullptr) {
        if (comparison(input, current->name, comma + 3, len)) {
            if (prev == nullptr) {
                *head = current->next;
            }
            else {
                prev->next = current->next;
            }
            delete[] current->name;
            delete[] current->value;
            delete current;
            print(input);
            cout << " == deleted" << endl;
            return;
        }
        // Move to the next node
        prev = current;
        current = current->next;
    }
}

void DeletingAttribute(BlockNode* head, int ith, char* input, int comma, int len) {
    BlockNode* current = head;
    int temp = ith;
    while (current != nullptr) {
        if (temp > current->occupied) {
            temp -= current->occupied;
        }
        else {
            for (int j = 0; j < T; j++) {
                if (temp == 1 && current->blocks[j].headAttribute != nullptr) {
                    deleteAttributeNode(&current->blocks[j].headAttribute, input, comma, len);
                    if (NumOfAttr(current->blocks[j].headAttribute) == 0) DeletingSection(head, TextToNumber(input, 0, comma), input, true);
                    break;
                }
                if (temp != 1 && current->blocks[j].headAttribute != nullptr) {
                    temp = temp - 1;
                }
            }
            break;
        }
        current = current->next;
    }
}

int main() {
    BlockNode* BlockHead = nullptr;
    BlockNode* BlockTail = nullptr;
    NewBlockNode(&BlockHead, &BlockTail);
    int index = 0, mode = 0; // 0 - Selectors ; 1 - Attributes ; 2 - Commands
    bool end = false;
    while (!end) {
        char line[MAXSIZE];
        int len;
        len = readLine(line, &end,mode);
        if (len == 4 && CommSection(line)) {
            if (line[0] == '?' && mode == 0) mode = 2;
            else if (line[0] == '*' && mode == 2) mode = 0;
            continue;
        }
        else if (len == 1 && line[0] == '{') { 
            mode = 1; 
            continue;
        }
        else if (len == 1 && line[0] == '}') {
            mode = 0;
            BlockTail->occupied += 1;
            index++;
            if (index == T) {
                index = 0;  NewBlockNode(&BlockHead, &BlockTail);
            }
            continue;
        }
        // Allocating Selectors
        int start = 0;
        int separator = 0;
        if (mode == 0) {
            for (int i = 0; i < len; i++) {
                if (line[i] == ',' || line[i] == '{') {
                    separator = i;
                    NewSelectorNode(BlockTail->blocks[index].headSelector, BlockTail->blocks[index].tailSelector, line, start, separator);
                    start = separator + 1;
                    if (line[i] == '{') {
                        mode = 1;
                        break;
                    }
                }

            }
            if (mode == 0 && (separator < len - 1)) NewSelectorNode(BlockTail->blocks[index].headSelector, BlockTail->blocks[index].tailSelector, line, start, len);
        }
        // Allocating Attributes
        if (mode == 1) {
            int semicolon = 0;
            for (int i = start; i < len; i++) {
                if (line[i] == ':') {
                    separator = i;
                }
                else if (line[i] == ';') {
                    semicolon = i;
                    AddAttribute(BlockTail, index, line, start, separator, semicolon);
                    start = semicolon + 1;
                }
                else if (line[i] == '}') {
                    mode = 0;
                    BlockTail->occupied += 1;
                    index++;
                    if (index == 8) {
                        index = 0;  NewBlockNode(&BlockHead, &BlockTail);
                    }
                    break;
                }
            }
        }
        // Command Section
        if (mode == 2) {
            int command = 0;
            int comma = whichCommand(line, len, &command);
            if (command == 1) {
                print(line);
                cout << " == " << NumOfSections(BlockHead) << endl;
            }
            else if (command == 2) {
                int l = NumOfSomethingInSection(BlockHead, TextToNumber(line, 0, comma), 1);
                if (l != -1) {
                    print(line); cout << " == " << l << endl;
                }
            }
            else if (command == 3) {
                int l = NumOfSomethingInSection(BlockHead, TextToNumber(line, 0, comma), 2);
                if (l != -1) {
                    print(line); cout << " == " << l << endl;
                }
            }
            else if (command == 4) {
                ParticularSelInSection(BlockHead, TextToNumber(line, 0, comma), TextToNumber(line, comma + 3, len), line);
            }
            else if (command == 5) {
                ValueOfAttrInSection(BlockHead, TextToNumber(line, 0, comma), line, comma, len);
            }
            else if (command == 6) {
                print(line);
                cout << " == ";
                cout << OccurencesOfSth(BlockHead, line, 0, comma, 1) << endl;
            }
            else if (command == 7) {
                print(line);
                cout << " == ";
                cout << OccurencesOfSth(BlockHead, line, 0, comma, 2) << endl;
            }
            else if (command == 8) {
                ValueOfNthAttr(BlockTail, line, 0, comma, len);
            }
            else if (command == 9) {
                DeletingSection(BlockHead, TextToNumber(line, 0, comma), line, false);
            }
            else if (command == 10) {
                DeletingAttribute(BlockHead, TextToNumber(line, 0, comma), line, comma, len);
            }
        }
    }
    deleteBlockList(BlockHead);
    return 0;
}
