#include <iostream>
#include <map>
int main()
{
std::string key = "a";
    std::map<std::string, std::string> _M;
    _M.insert(std::pair<std::string, std::string> ("a", "A"));
    _M.insert(std::pair<std::string, std::string> ("b", "B"));
    _M.insert(std::pair<std::string, std::string> ("d", "D"));
    _M.insert(std::pair<std::string, std::string> ("e", "E"));
    



    std::map<std::string, std::string>::iterator it = _M.lower_bound(key);
    if(it != _M.end() && it->first == key)
        std::cout << it->first << "***" << it->second ;
    else
     std::cout << "xxxx\n";

}