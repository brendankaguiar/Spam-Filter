#include <iostream>
#include <string>
#include <fstream>
#include <vector>
const int COLS = 916;
const int ROWS = 3902;
static int HAM_CT = 0;
static int SPAM_CT = 0;
struct DOCUMENT{
	std::string Class;
	std::string Text[175];
	void setClass(std::string parser)
	{
		Class = parser;
		if (Class == "ham")
			HAM_CT++;
		else
			SPAM_CT++;
	}
	void setText(std::string parser, int i)
	{
		Text[i] = parser;
	}
};

int load_docs(std::string filename, DOCUMENT DOCS[]);

int main() {
	//std::vector <std::string> testDoc[ROWS];
	DOCUMENT* myDoc = new DOCUMENT[3902];
	int val = load_docs("spam.csv", myDoc);
	if (val == 0)
		std::cout << "Document Successfully loaded\n";
	float P_of_HAM = (float)HAM_CT / (HAM_CT + (float)SPAM_CT);
	float P_of_SPAM = 1 - P_of_HAM;

	return 0;
}


int load_docs(std::string filename, DOCUMENT DOCS[])
{
	std::ifstream bfile(filename.c_str());
	int l = 0;//index for COlS
	if (bfile.is_open())
	{
		std::string parser;
		for (int i = 0; i < 3863; i++)// For Each document
		{
			std::string scanner;
			getline(bfile, parser, ',');
			DOCS[i].setClass(parser);//Set the class of the current document
			do//For Each word in document
			{
				if (bfile.peek() != '"')
				{
					parser.clear();
					scanner.clear();
					while (bfile.peek() != '\n' && bfile.peek() != ' ')
						parser.push_back(bfile.get());
					int garbage = bfile.get();
					if (parser.size() >= 3)
					{
						for (int k = parser.size(); k > parser.size() - 3; k--)//check if tail of parser is ',,,'
						{
							scanner.push_back(parser[k - 1]);//feed scanner last 3 characters
						}
						if (scanner == ",,,")//If scanner discovers terminal characters
							parser.erase(parser.size() - 3, 3);//erase terminal characters
					}

					DOCS[i].setText(parser, l);//add word to document vector
					l++;
				}
				else
					int garbage = bfile.get();//collect error prone symbols
			} while(scanner != ",,,");//until terminal characters found
			l = 0;//reset COLS for next Text document
		}

		bfile.close();
	}
	else
	{
		printf("unable to open file: [%s]\n", filename.c_str());
		return -1;
	}
	return 0;
}
