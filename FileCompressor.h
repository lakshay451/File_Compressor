#pragma once
#include<iostream>
#include<queue>
#include<vector>
#include<string>
#include<fstream>
#include<bitset>
using namespace std;


class FileCompressor
{
	class Node
	{
		char ch;
		int id;
		int frequency;
		string code;
		Node* left;
		Node* right;
		friend FileCompressor;

	public:
		Node(char character = ' ', int f = 0, string c = " ", Node* l = nullptr, Node* r = nullptr)
		{
			ch = character;
			frequency = f;
			code = c;
			left = l;
			right = r;
			id = 0;
		}
	};

	struct Comp
	{
		bool operator()(const Node* r, const Node* l)
		{
			return r->frequency > l->frequency;
		}
	};

	Node* Ascii_array[128];
	Node* root;
	priority_queue<Node*, vector<Node*>, Comp> pq;		//lowest frequency will be at top,pq to store frequencies

	char SearchCode(string s)
	{
		for (int i = 0; i < 128; i++)
		{
			if (s == Ascii_array[i]->code)
			{
				return Ascii_array[i]->ch;
			}
		}
	}

	char BitSetTochar(bitset<8> b)
	{
		char c;
		int ret = b.to_ullong();							
		c = char(ret);									
		//pack 8 bits into a single character and print it in a file to reduce size instead of printing 8 ascii bytes
		return c;
	}

	string decimal_to_binary(int in)
	{
		string temp = "";
		string result = "";
		while (in)
		{
			temp += ('0' + in % 2);
			in /= 2;
		}
		result.append(8 - temp.size(), '0');													
		//append '0' ahead to let the result become fixed length of 8
		for (int i = temp.size() - 1; i >= 0; i--)
		{
			result += temp[i];
		}
		return result;
	}

	void Traverse_Tree(string s)
	{
		ofstream fout("Decoded.txt");
		auto temp = root;
		for (int i = 0; i < s.size(); i++)
		{
			if (s[i] == '0')
				temp = temp->left;
			else if (s[i] == '1')
				temp = temp->right;

			if (temp->left == nullptr && temp->right == nullptr)
			{
				fout << temp->ch;
				temp = root;
			}
		}
	}

	void StoreCode_resursion(Node* r, string s)
	{
		if (r->left == nullptr && r->right == nullptr)
		{
			r->code = s;										//store the code when reach the node
			return;
		}
		if (r->left)											//0 to go left
			StoreCode_resursion(r->left, s + "0");
		if (r->right)											//1 to go right
			StoreCode_resursion(r->right, s + "1");

	}

public:

	FileCompressor()
	{
		for (int i = 0; i < 128; i++)
		{
			Ascii_array[i] = new Node();
			Ascii_array[i]->id = i;
		}
		root = nullptr;
	}

	void ReadFile()
	{
		ifstream fin("OriginalFile.txt");
		char input;
		while (fin.good())
		{
			input = fin.get();
			if (fin.eof())
				break;
			Ascii_array[input]->frequency++;					//Increase frequency of the node
			Ascii_array[input]->ch = input;						//use the ascii value as input of array
		}
		fin.close();

		for (int i = 0; i < 128; i++)
		{
			if (Ascii_array[i]->frequency)
				pq.push(Ascii_array[i]);
		}
	}

	void BuildTree()
	{
		priority_queue <Node*, vector<Node*>, Comp> tree(pq);				//BuildTree containing sum of all internal nodes as root
		while (tree.size() > 1)
		{
			auto min1 = tree.top();															
			tree.pop();
			auto min2 = tree.top();
			tree.pop();

			Node* insert = new Node(' ', min1->frequency + min2->frequency, " ", min1, min2);
			tree.push(insert);
		}
		root = tree.top();
	}

	void PrintCode()
	{
		for (int i = 0; i < 128; i++)							//Print all frequencies
		{
			if (Ascii_array[i]->frequency)
			{
				cout << Ascii_array[i]->ch << " : " << Ascii_array[i]->frequency << " : " << Ascii_array[i]->code << endl;
			}
		}
	}

	void StoreCode()
	{
		StoreCode_resursion(root, "");
		PrintCode();							
	}

	void WriteCode()
	{
		ifstream fin("OriginalFile.txt");
		ofstream fout("Encoded.txt", ios::binary);	//read in binary mode(removing this messes up encoding)

		bitset<8> Set;												
		int k = 7;
		string s = "";

		while (true)
		{
			char input = fin.get();
			if (fin.eof())
				break;
			s = s + Ascii_array[input]->code;	//store all code in a single string
		}

		while (s.size() % 8 != 0)
		{
			s = s + '0';				//make the string a multiple of 8 bit		

		}

		for (int i = 0; i < s.size(); i++)
		{
			Set.set(k, (int)s[i] - 48);											
			//Set the value in reverse as ulong converts correct when the bitset is stored in reverse
			k--;
			if (k < 0)
			{
				k = 7;				  //reset index
				fout << BitSetTochar(Set);
			}
		}
		fout.close();
		cout << s;


		
	}

	void Decode()
	{
		ifstream fin("Encoded.txt", ios::binary);		//read in binary mode(removing this messes up encoding)
		string s = "";
		int input;

		while (true)
		{
			input = fin.get();				//read ascii value
			if (fin.eof())
				break;
			s += decimal_to_binary(input);			//decode the binary value in a string
		}

		//cout << endl << endl << s;
		Traverse_Tree(s);					//decode the binary values
	}

	void Compress()
	{
		ReadFile();
		BuildTree();
		StoreCode();
		WriteCode();
		Decode();
	}
};
