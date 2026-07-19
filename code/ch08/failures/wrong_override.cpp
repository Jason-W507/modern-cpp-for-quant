struct Strategy {
  virtual ~Strategy() = default;
  virtual int decide(const double& price) const = 0;
};

struct WrongStrategy final : Strategy {
  int decide(double& price) const override { return price < 100.0; }
};

int main() { WrongStrategy strategy; }
