double fee_rate() {
  return 0.002;
}

int main() {
  if (fee_rate() > 0.0) {
    return 0;
  }
  return 1;
}
