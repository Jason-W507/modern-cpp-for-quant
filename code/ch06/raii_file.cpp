#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

int main() {
  std::string path{"ch06_raii_report.txt"};
  {
    std::ofstream output{path};
    output << "symbol,quantity\nAAPL,10\nMSFT,20\n";
    std::cout << "inside_open=" << static_cast<bool>(output) << '\n';
  }

  int rows{};
  {
    std::ifstream input{path};
    std::string line;
    std::getline(input, line);
    while (std::getline(input, line)) {
      ++rows;
    }
  }

  int removed = std::remove(path.c_str()) == 0;
  std::cout << "after_scope_rows=" << rows << " removed=" << removed << '\n';
}
