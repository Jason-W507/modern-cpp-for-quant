#include <iostream>
#include <memory>
#include <string>
#include <utility>

struct Ledger final {
  std::string name;
};

int main() {
  Ledger direct{"direct"};
  Ledger copied{direct};
  copied.name = "copy";

  auto unique = std::make_unique<Ledger>(Ledger{"unique"});
  auto transferred = std::move(unique);

  auto shared = std::make_shared<Ledger>(Ledger{"shared"});
  std::weak_ptr<Ledger> observer{shared};

  std::cout << "direct=" << direct.name << " copied=" << copied.name
            << " unique_empty=" << (unique == nullptr)
            << " unique_owner=" << transferred->name
            << " shared_owners=" << shared.use_count()
            << " weak_expired=" << observer.expired();

  shared.reset();
  std::cout << " weak_after_reset=" << observer.expired() << '\n';
}
