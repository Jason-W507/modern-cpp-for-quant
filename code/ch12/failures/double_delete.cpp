int main() {
  auto* price = new double{100.0};
  delete price;
  delete price;
}
