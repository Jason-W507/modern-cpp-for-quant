double fee_rate() {
  return 0.002;
}

int main() {
  return fee_rate() > 0.0 ? 0 : 1;
}
