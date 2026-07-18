#include <iostream>

double add_tick(double price) {
  price += 1.0;
  return price;
}

double observe_price(const double& price) {
  return price;
}

void charge_fee(double& cash, double fee) {
  cash -= fee;
}

int main() {
  const double original{100.0};
  const double copied_result{add_tick(original)};
  const double observed{observe_price(original)};

  double cash{1000.0};
  charge_fee(cash, 5.0);

  const double* price_view{&original};
  const double pointed{*price_view};
  const double* missing{nullptr};

  std::cout << "original=" << original << " copied_result=" << copied_result
            << " observed=" << observed << " cash=" << cash
            << " pointed=" << pointed << " null=" << (missing == nullptr)
            << '\n';
  return 0;
}
