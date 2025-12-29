import random
import Gameplay

class Player:
    def __init__(self):
        self.hand = Hand()
        self.chips = 0
    def addToHand(self, card):
        self.hand.addCard(card)
        

class Hand:
    def __init__(self):
        self.hand = []
        self.vt = Gameplay.Value_Table()
    
    def addCard(self, card):
        self.hand.append(card)
    
    def getHandValue(self):
        return sum(self.vt.getCardValue(c.rank) for c in self.cards)



