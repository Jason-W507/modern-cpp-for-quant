int main() {
  int rounded_price{100.5};  // intentional: list initialization rejects narrowing
  return rounded_price;
}
