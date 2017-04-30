#include <iostream>
#include <thread>
#include "BankAccount.h"

//
// A "Monitor" is an object that ensures safe concurrent access, providing mutual exclusion internally when needed.
// It also usually provides methods for waiting until certain conditions are met, such as the availability of a
// resource.
//


void child(BankAccount& forfun) {

  // I need money to spend
  std::cout << "Waiting for money to be put into account" << std::endl;
  forfun.WaitForMoney();

  for (int i=0; i<7; ++i) {
    if (forfun.WithdrawFunds(40)) {
      std::cout << "  Spent $40" << std::endl;
    } else {
      int balance = forfun.GetBalance();
      if (balance > 0) {
        std::cout << "  Uh oh, only " << balance << " left, withdrawing..." << std::endl;
        forfun.WithdrawFunds(balance);
      } else {
        std::cout << "  Out of money :(." << std::endl;
        return; // break out of loop
      }
    }
  }

  // end of the week, spend all my money
  int balance = forfun.GetBalance();
  forfun.WithdrawFunds(balance);
  std::cout << "  Spending all my extra money :), $" << balance << std::endl;
}

int main() {
  // BankAccount is a monitor class that provides safe access for depositing/withdrawing funds, and checking balances.
  BankAccount forschool{};

  // every week put 200 in
  for (int i=0; i<52; ++i) {
    std::thread bigspender(&child, std::ref(forschool));

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    std::cout << "Giving child $300 for school" << std::endl;
    forschool.DepositFunds(300);
    // wait until child is out of money or done for the week
    std::cout << "Waiting until child has spent all money" << std::endl;
    forschool.WaitForBankrupt();
    bigspender.join();  // wait for child
  }

  std::cout << "Done." << std::endl;
  return 0;
}