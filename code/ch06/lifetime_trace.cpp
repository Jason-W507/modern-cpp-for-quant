#include <iostream>
#include <string>
#include <utility>

class Trace final {
 public:
  explicit Trace(std::string label) : label_(std::move(label)) {
    std::cout << "construct=" << label_ << '\n';
  }

  Trace(const Trace& other) : label_(other.label_ + "-copy") {
    std::cout << "copy=" << other.label_ << "->" << label_ << '\n';
  }

  Trace(Trace&& other) noexcept : label_(std::move(other.label_)) {
    other.label_ = "moved-from";
    std::cout << "move=" << label_ << '\n';
  }

  ~Trace() { std::cout << "destroy=" << label_ << '\n'; }

  [[nodiscard]] const std::string& label() const { return label_; }

 private:
  std::string label_;
};

int main() {
  Trace original{"quote"};
  {
    Trace copied{original};
    Trace moved{std::move(original)};
    std::cout << "states=" << original.label() << ',' << copied.label() << ','
              << moved.label() << '\n';
  }
  std::cout << "after-inner=" << original.label() << '\n';
}
