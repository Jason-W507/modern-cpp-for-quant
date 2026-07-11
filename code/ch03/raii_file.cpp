#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

class CsvInput final {
 public:
  explicit CsvInput(const std::string& path) : stream_(path) {
    if (!stream_) {
      throw std::runtime_error("cannot open CSV: " + path);
    }
  }

  [[nodiscard]] std::size_t data_rows() {
    std::size_t rows = 0;
    std::string line;
    std::getline(stream_, line);  // header
    while (std::getline(stream_, line)) {
      if (!line.empty()) {
        ++rows;
      }
    }
    return rows;
  }

 private:
  std::ifstream stream_;
};

int main() {
  const std::string path = "raii_example.csv";
  {
    std::ofstream output(path);
    output << "symbol,price,quantity\nAAPL,188.5,20\nMSFT,420.0,10\n";
  }  // output is flushed and closed here even if later work throws

  const auto rows = CsvInput(path).data_rows();
  std::cout << "rows=" << rows << '\n';
}
