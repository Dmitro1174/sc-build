
#include "../include/termcolor-helper.hpp"

using namespace std;

std::string operator""_ff(const char* fstr, size_t sz)	{

	for(size_t pos = 0; pos < sz; pos++)	{
		if(fstr[pos] == '^')	{	// foreground control character !
			if(not is_last(pos, sz))	{
				pos += 1;
				switch(fstr[pos])	{
					case '^':	// output ^ itself
						cout << '^'; break;
					case ';':	// reset
						cout << termcolor::reset; break;
					case 'r':	
						cout << termcolor::red; break;
					case 'w':	// white
						cout << termcolor::white; break;
					case 'b':
						cout << termcolor::blue; break;
					case 'y':
						cout << termcolor::yellow; break;
					case 'g':
						cout << termcolor::green; break;
					case 'd':
						cout << termcolor::dark; break; 
					case 'c':
						cout << termcolor::cyan; break; 
					case 'm':
						cout << termcolor::magenta; break; 
					case 'u':
						cout << termcolor::underline; break; 
					default:
						cout << fstr[pos];
						break;				
				} 	// switch...
			}	// if(not last_pos...)
		} else if(fstr[pos] == '~')	{	// background control character
			if(not is_last(pos, sz))	{
				pos += 1;
				switch(fstr[pos])	{
					case '~':	// output ^ itself
						cout << '^'; break;
					case ';':	// reset
						cout << termcolor::reset; break;
					case 'r':	
						cout << termcolor::on_red; break;
					case 'w':	// white
						cout << termcolor::on_white; break;
					case 'b':
						cout << termcolor::on_blue; break;
					case 'y':
						cout << termcolor::on_yellow; break;
					case 'g':
						cout << termcolor::on_green; break;
					case 'd':
						cout << termcolor::on_grey; break; 
					case 'c':
						cout << termcolor::on_cyan; break; 
					case 'm':
						cout << termcolor::on_magenta; break; 
					case 'u':
						cout << termcolor::underline; break; 
					default:
						cout << fstr[pos];
						break;				
				} 	// switch...
			}	// if(not last_pos...)
		} else 
				cout << fstr[pos];
		}	// for...

	// done.
	return "";

}	// operator"ff()

// eof

