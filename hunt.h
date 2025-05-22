#ifndef HUNT_H
#define HUNT_H

//TODO: maybe define a hunt session class 

class Vault {
public:
    unsigned int Total = 0;
    void AddWinnings(int winnings);
};

class Wallet {
public:
    unsigned int Total = 0;
    void AddWinnings(int winnings);
    int SpendMoney(int amountToSpend);
};

#endif
