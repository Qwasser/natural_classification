//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // stricmp, strlen, strcpy, strrchr
 
#include "FileManager.h"

SEQStorage* FileManager::ReadCSVData(const char* name)
{
	FILE* stream = fopen(name, "r");
	if (stream != NULL)
	{
		int n_lines = CountLines(name);
		Sequence* objects = new Sequence[n_lines];
		
	    
		char line[1024];
		int n_seq = 0;
		while (fgets(line, 1024, stream))
		{
			Sequence obj;
			char* tmp = strdup(line); // malloc strlen+1 and copy
			// NOTE strtok clobbers tmp

			input_t token = strtok(line, ",");
			while (token != NULL)
			{
				obj.Append(token);
				token = strtok(NULL, ",\n");			
			}
			free(tmp);

			char seq_name[50];
			sprintf(seq_name, "seq name %d", n_seq);
			obj.SetName(std::string(seq_name));

			objects[n_seq++] = obj;
		}
		
		SEQStorage* data = new SEQStorage(objects, n_seq);
		delete[] objects;
		return data;
	}
	return 0;
}

#include <deque>
#include <wx/string.h>
#include <wx/xml/xml.h>

int count_childrens(wxXmlNode* root)
{
	int count = 0;
	wxXmlNode* leaf = root->GetChildren();
	while (leaf)
	{
		count++;
		leaf = leaf->GetNext();
	}
	return count;
}

wxXmlNode* get_last_node(wxXmlNode* node)
{
	wxXmlNode* current = node;
	wxXmlNode* next = node->GetNext();
	while (next)
	{
		current = next;
		next = next->GetNext();
	}
	return current;
}

void fill_leaf(wxXmlNode* leaf, wxString p1, wxString p2, wxString p3)
{
	wxXmlProperty* XmlTPos = new wxXmlProperty(wxT("pos"), p1);
	wxXmlProperty* XmlTSign = new wxXmlProperty(wxT("eq"), p2);
	wxXmlProperty* XmlTValue = new wxXmlProperty(wxT("val"), p3);
	
	leaf->AddProperty(XmlTPos);
	leaf->AddProperty(XmlTSign);
	leaf->AddProperty(XmlTValue);
}

void create_branch(std::deque<wxXmlNode*> &path, std::deque<wxXmlNode*>::iterator waypoint, Rule* set, unsigned long &rule_index, unsigned int chain_index)
{
	wxXmlNode* leaf;
	Rule* r = &set[rule_index];
	std::deque<wxXmlNode*>::iterator checkpoint = waypoint;

	if(waypoint == path.begin()) // root
		create_branch(path, ++waypoint, set, rule_index, 0);
	else if(--checkpoint == path.begin()) // decision
	{
		wxString t_pos = wxString::Format(wxT("%u"), r->getTTPos());
		wxString t_eq = r->getTTSign() == 1 ? wxT("+") : wxT("-");
		wxString t_val = wxString::Format(wxT("%i"), r->getTTValue());

		bool the_same = false;
		if(waypoint != path.end())
		{
			wxXmlNode* prev_leaf = *waypoint;
			the_same = t_pos.Cmp(prev_leaf->GetPropVal(wxT("pos"), wxT("0"))) == 0 // the same pos
				&& t_val.Cmp(prev_leaf->GetPropVal(wxT("val"), wxT("0"))) == 0 // the same value
				&& t_eq.Cmp(prev_leaf->GetPropVal(wxT("eq"), wxT("1"))) == 0; // the same sign
		}

		if(the_same)
			create_branch(path, ++waypoint, set, rule_index, 0);
		else
		{
			leaf = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("D"));
			fill_leaf(leaf, t_pos, t_eq, t_val);
			wxXmlNode* root = *checkpoint;
			if (count_childrens(root) < 1)
				root->AddChild(leaf);
			else
				root->InsertChildAfter(leaf, get_last_node(root->GetChildren()));
			path.erase(path.begin() + 1, path.end());
			path.push_back(leaf);
			create_branch(path, path.end(), set, rule_index, 0);
		}
	}	
	else if (chain_index < r->getRuleLength()) // sub root from path, add decision to length
	{
		wxString t_pos = wxString::Format(wxT("%u"), (*r)[chain_index].Shift);
		wxString t_eq = (*r)[chain_index].Sign == 1 ? wxT("+") : wxT("-");
		wxString t_val = wxString::Format(wxT("%i"), (*r)[chain_index].Value);
		
		bool the_same = false;
		if(waypoint != path.end())
		{
			wxXmlNode* prev_leaf = *waypoint;
			the_same = t_pos.Cmp(prev_leaf->GetPropVal(wxT("pos"), wxT("0"))) == 0 // the same pos
				&& t_val.Cmp(prev_leaf->GetPropVal(wxT("val"), wxT("0"))) == 0 // the same value
				&& t_eq.Cmp(prev_leaf->GetPropVal(wxT("eq"), wxT("1"))) == 0; // the same sign
		}
		
		chain_index++;
		if(the_same)
			create_branch(path, ++waypoint, set, rule_index, chain_index);
		else
		{
			leaf = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("P"));
			fill_leaf(leaf, t_pos, t_eq, t_val);
			wxXmlNode* root = *checkpoint;
			if (count_childrens(root) < 1)
				root->AddChild(leaf);
			else
				root->InsertChildAfter(leaf, get_last_node(root->GetChildren()));
			path.erase(path.begin() + 1 + chain_index, path.end()); // + root + decision - current = 2 - 1 = +1
			path.push_back(leaf);
			create_branch(path, path.end(), set, rule_index, chain_index);
		}
	}
	else
		++rule_index;
}

void FileManager::WriteXMLRules(Rule* set, unsigned long length, const char* name)
{
	wxXmlDocument XmlDoc;
	wxXmlNode * XmlRules = new wxXmlNode(wxXML_ELEMENT_NODE ,wxT("Rules"));

	unsigned long i = 0;
	unsigned int j = 0;
	bool new_branch = true;
	std::deque<wxXmlNode*> path;
	path.push_back(XmlRules);

	while(i < length)
		create_branch(path, path.begin(), set, i, 0);
	
	XmlDoc.SetRoot(XmlRules);      
	XmlDoc.Save(wxString::FromUTF8(name), 4); // 4 for indentation
}

#include <fstream>
#include <algorithm>
int FileManager::CountLines(const char * file)
{
	std::ifstream inFile(file); 
	return std::count(std::istreambuf_iterator<char>(inFile), 
             std::istreambuf_iterator<char>(), '\n');
}

int FileManager::ReadPlainTextRules(const char* file, SEQStorage* data, RulesStorage* rules)
{
	if(data == NULL)
	{
		return 1; // no objects for rules, no codes
		//TODO: need something like checksum for rules
	}
	CodeTable* m_Code = CodeTable::Instance();
	
	FILE* r_txt = fopen( file, "r" );
	if (r_txt == NULL)
		return 2; // no file, probably

	// check file size
	fseek(r_txt, 0, SEEK_END); // seek to end of file
	double size = (double)ftell(r_txt); // get current file pointer
	fseek(r_txt, 0, SEEK_SET); // seek back to beginning of file
	// proceed with allocating memory and reading the file
	if( size/1048576. > 900. )
	{
		return 3; //Very big file
	}

	char ch;
	long pos=0, len=0;
	char val[2];
	float cp=0;
	RuleSection pred;
	bool notTheEnd;
	char line[MYMAXSTR];
	while (!feof(r_txt)) 
	{
		ch = fgetc (r_txt);
		if( ch == '#' )
			fgets(line, MYMAXSTR, r_txt);
		else if( ch == 'I' && fgetc (r_txt)=='F')
		{
			fscanf(r_txt, "(%i)", &len);
			Rule* pRule = new Rule(len);
			do{
				//fscanf(r_txt, " %i = %c ", &pos, &val);
				fscanf(r_txt, " %i =", &pos);
				ch = fgetc (r_txt);
				if( ch==' ' )
					pred.Sign = 1;
				else
				{
					pred.Sign = -1;
					fseek(r_txt, 2, SEEK_CUR);
				}
				fscanf(r_txt, " %1s ", &val);
				pred.Shift = pos-1;
				pred.Value = m_Code->getCode( val );
				pRule->AddSection(rules->GetPredicatePointer(pred.Shift, pred.Sign, pred.Value));
				
				ch = fgetc (r_txt);
				if( ch=='A' )//AND
				{
					fseek(r_txt, 2, SEEK_CUR);
					notTheEnd = true;
				}
				else if( ch=='T' )//THEN
				{
					fseek(r_txt, 3, SEEK_CUR);
					fscanf(r_txt, " %i =", &pos);
					ch = fgetc (r_txt);
					if( ch==' ' )
						pRule->setTTSign( 1 );
					else
					{
						pRule->setTTSign( -1 );
						fseek(r_txt, 2, SEEK_CUR);
					}
					fscanf(r_txt, " %1s", &val);
					pRule->setTTPos( pos-1 );
					pRule->setTTValue( m_Code->getCode( val ) );
					fscanf(r_txt, "(%f)", &cp);
					pRule->setCP( cp );
					if( pRule->getTTSign()==-1 ) pRule->setCP( -cp );
					fgetc (r_txt);
					/*if(cp<0.91)
					{
						notTheEnd = true;
						break;
					}*/
					
					notTheEnd = false;
				}
				else
				{
					//Error reading: incorrect structure of comment and data
					break;
				}
			} while( notTheEnd );
			if(notTheEnd)
				delete pRule;
			else
				rules->Add( pRule );
			
		}
		else if ( ch == -1 )
		{
			;
		}
		else
		{
			//Error reading: incorrect structure of comment and data
			break;
		}
	}
	fclose(r_txt);
	rules->MakePointersArray();

	return 0; // everything should be ok
}
