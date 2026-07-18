double fee_rate();  // declaration without a definition

int main() {
  if (fee_rate() > 0.0) {
    return 0;
  }
  return 1;
}
