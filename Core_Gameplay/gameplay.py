import random
import math
import Player
from itertools import combinations_with_replacement, combinations

class Value_Table:
    def __init__(self):
        self.prime_values = [2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41]
        self.rank_names = ['2', '3', '4', '5', '6', '7', '8', '9', '10', 'JACK', 'QUEEN', 'KING', 'ACE']
        self.suit_names = ['HEARTS', 'SPADES', 'DIAMONDS', 'CLUBS']
        self.suit_masks = [0x1000, 0x2000, 0x4000, 0x8000]
        self.deck = []
        for r in range(13):
            for s in range(4):
                # We build the 'Cactus Kev' integer for this card
                # This bit-smushing is the "PB&J" of poker AI
                card_int = (1 << (r + 16)) | self.suit_masks[s] | (r << 8) | self.prime_values[r]
                self.deck.append(card_int)

    def get_rank_index(self, card):
        # Extracts bits 8-11 (the r << 8 part)
        return (card >> 8) & 0xF

    def get_prime(self, card): 
        # Extracts the first 8 bits (the prime value)
        return card & 0xFF

    def get_suit_bits(self, card):
        # Extracts bits 12-15 (the suit mask)
        # Returns 0x1000, 0x2000, 0x4000, or 0x8000
        return card & 0xF000
    
    def get_suit_names(self, card):
        index = self.suit_masks.index(self.get_suit_bits(card))
        return self.suit_names[index]
    
    def get_rank_names(self, card):
        return self.rank_names[self.get_rank_index(card)]


class Deck:
    def __init__(self):
        self.vt = Value_Table()
        self._master_deck = list(self.vt.deck)
        self.cards = list(self._master_deck)

    def Shuffle(self):
        random.shuffle(self.cards)   

    def draw(self, n=1):
        drawn = self.cards[:n]
        self.cards = self.cards[n:]
        return drawn

    def print_deck(self):
        for card in self.cards:
            print(self.vt.get_rank_names(card), 'of', self.vt.get_suit_names(card))

    def reset(self):
        """Resets the deck without re-instantiating Value_Table."""
        self.cards = list(self._master_deck)

class Table:
    def __init__(self):
        self.deck = Deck()
        self.players = []
        self.community_cards = []

    def deal_flop(self):
        self.deck.Draw() # Burn card
        for _ in range(3):
            self.community_cards.append(self.deck.Draw())
    
    def deal_players(self):
        for _ in range(2):
            for player in self.players:
                player.addToHand(self.deck.Draw())


class PokerEvaluator:
    def __init__(self):
        self.vt = Value_Table()
        self.flush_lookup = {}
        self.unsuited_lookup = {}
        self.build_table(self)
    
    def build_table(self):
        all_rank_sets = list(combinations_with_replacement(range(13), 5))
        
        unsuited_results = []
        flush_results = []

        for hand in all_rank_sets:

            if any(hand.count(r) > 4 for r in set(hand)):
                continue

            prime_prod = 1
            for c in hand:
                prime_prod *= self.vt.prime_values[c]

            unsuited_strength = self.slow_evaluator(hand.sort(reverse=True), is_flush=False)
            unsuited_results.append({'prime': prime_prod, 'strength': unsuited_strength})

            if len(set(hand)) == 5:
                # We add it to a separate list to be ranked against other flushes
                flush_strength = self.slow_evaluator(hand, is_flush=True)
                flush_results.append({'prime': prime_prod, 'strength': flush_strength})
        
        all_hands = unsuited_results + flush_results

        # Sort all strength lookup so it can be ranked in order
        all_hands.sort(key=lambda x: x['strength'], reverse=True)
        
        for rank_id, hand in enumerate(all_hands, start=1):
            # If it's a flush, we need to know! 
            # You can use a bit-shift or a separate dict for flushes.
            if hand['strength'][0] in [9, 6]:
                self.flush_lookup[hand['prime']] = rank_id
            else:
                self.unsuited_lookup[hand['prime']] = rank_id

    # Function to evaluate and categorise hands by ranking
    def slow_evaluator(self, hand, is_flush=False):
        counts = self.get_counts(hand) # Returns how many of each card (e.g., {12:3, 7:2})
        sorted_ranks = sorted(counts.keys(), key=lambda r: (counts[r], r), reverse=True)
        val_counts = sorted(counts.values(), reverse=True)
        straight_high = self.is_straight(hand)

        if self.is_straight(hand) and is_flush: return (9, straight_high) # Straight Flush
        if 4 in counts.values(): return (8, sorted_ranks[0], sorted_ranks[1]) # 4 of a kind
        if 3 in counts.values() and 2 in counts.values(): return (7, sorted_ranks[0], sorted_ranks[1]) # Full House 
        if is_flush: return (6, sorted_ranks[0], sorted_ranks[1], sorted_ranks[2], sorted_ranks[3], sorted_ranks[4]) # Flush
        if self.is_straight(hand): return (5, straight_high)
        if 3 in counts.values(): return (4, sorted_ranks[0], sorted_ranks[1], sorted_ranks[2])
        if val_counts == [2,2,1]: return (3, sorted_ranks[0], sorted_ranks[1],sorted_ranks[2])
        if val_counts == [2,1,1,1]: return (2, sorted_ranks[0], sorted_ranks[1], sorted_ranks[2], sorted_ranks[3])
        return (1, sorted_ranks[0], sorted_ranks[1], sorted_ranks[2], sorted_ranks[3], sorted_ranks[4])

    def is_straight(self, hand):
        # OR the raw integers to combine their bitmasks (bits 16-28)

        combined_bits = (hand[0] | hand[1] | hand[2] | hand[3] | hand[4]) >> 16
        
        if combined_bits == 0x100F: return 3
        
        unique_ranks = sorted(list(set(hand)), reverse=True)   
        if len(unique_ranks) == 5:
        # If the gap between high and low is exactly 4, it's a straight
            if unique_ranks[0] - unique_ranks[4] == 4:
                return unique_ranks[0] # Returns the high card rank
        return None

    # Returns a dict of count for each card in hand
    def get_counts(self, hand):
        counts = {}
        for card_bits in hand:
            # Extract the rank (0-12) using your bitmask
            rank = self.vt.get_rank(card_bits)
            counts[rank] = counts.get(rank, 0) + 1
        return counts    


    # Finds Players Best Hand out of the 7 Available Cards and Returns it
    def evaluate_player(self, Board, Hand):
        # Combined List of Community and Player cards
        seven_cards = Hand + Board
        best_rank = 9999
        best_hand = None

        # Go through all combinations of card in 5 and find highest scoring combination
        for combo in combinations(seven_cards, 5):
            # Calculate unique prime product of combo
            prime_prod = math.prod([card & 0xFF for card in combo])

            # Check if the combo is a flush and use the right lookup depending on if it is or not
            if self.is_flush(combo):
                current_rank = self.flush_lookup[prime_prod]
            else:
                current_rank = self.unsuited_lookup[prime_prod]
            
            # Update best rank if the rank is higher
            if current_rank < best_rank:
                best_rank = current_rank
                best_hand = combo

                if best_rank == 1:
                    return best_rank, best_hand
                
        return best_rank, best_hand

    def is_flush(self, hand):
        return hand[0] &  hand[1] & hand[2] &  hand[3] & hand[4] &  0xF000 != 0 

