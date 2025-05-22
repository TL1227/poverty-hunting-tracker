#include "hunt.h"

void Vault::AddWinnings(int winnings) {
    Total += ((winnings % 2) == 0) ? 0 : 1;
    Total += winnings / 2;
}

void Wallet::AddWinnings(int winnings) {
    Total += winnings / 2;
}

int Wallet::SpendMoney(int amountToSpend) {
    int remaining = Total - amountToSpend;

    if (remaining >= 0) {
        Total -= amountToSpend;
        return amountToSpend;
    } else {
        return 0;
    }
}

