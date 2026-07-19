int main() {
  auto* price = new double{100.0};
  delete price;
  return *price > 0.0 ? 0 : 1;
}
