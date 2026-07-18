const double& price_reference() {
  const double price{100.0};
  return price;
}

int main() {
  return price_reference() > 0.0 ? 0 : 1;
}
