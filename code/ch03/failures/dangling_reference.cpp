const double& price_reference() {
  const double price{100.0};
  return price;
}

int main() {
  if (price_reference() > 0.0) {
    return 0;
  }
  return 1;
}
